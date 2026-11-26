/*
 *  Open Fodder
 *  ------------------------
 *
 *  Copyright (C) 2008-2017 Robert Crossfield
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "SurfaceImage.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QStatusBar>
#include <QLineF>

namespace {
bool mapReady() {
    return g_Fodder && g_Fodder->mMapLoaded && g_Fodder->mGraphics && g_Fodder->mMap;
}
QImage surfaceImage(cSurface& surface) {
    surface.surfaceSetToPaletteNew();
    surface.draw(0, 0);
    return ImageFromSDLSurface(*surface.GetSurface());
}
}

cWindowQT::cWindowQT(QWidget* parent) : QWidget(parent), cWindow() {
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(240, 180);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setToolTip(tr("Mouse wheel: zoom at cursor\nMiddle drag: pan\nLeft click/drag: place selected tiles or sprites\nRight click: advance simulation"));
    connect(&mRedrawTimer, &QTimer::timeout, this, [this]() {
        if (!mapReady()) return;
        g_Fodder->Mission_Sprites_Handle();
        g_Fodder->Phase_Loop_Interrupt();
        g_Fodder->mGraphics->MapTiles_Draw();
        g_Fodder->Sprites_Draw();
        g_Fodder->Video_Sleep();
    });
    mRedrawTimer.start(20);
}
void cWindowQT::EventCheck() {}
bool cWindowQT::InitWindow(const std::string&) { return true; }
void cWindowQT::PositionWindow() {}
void cWindowQT::RenderShrunk(cSurface*) { update(); }
void cWindowQT::RenderAt(cSurface*) { update(); }
void cWindowQT::FrameEnd() { update(); }

int cWindowQT::TileAt(int x, int y) const {
    if (!mapReady() || x < 0 || y < 0 || x >= g_Fodder->mMapLoaded->getWidth() || y >= g_Fodder->mMapLoaded->getHeight()) return -1;
    const size_t offset = 0x60 + 2 * (size_t(y) * g_Fodder->mMapLoaded->getWidth() + x);
    if (offset + 2 > g_Fodder->mMap->size()) return -1;
    const int tile = readLE<uint16>(g_Fodder->mMap->data() + offset) & 0x1FF;
    return tile < 480 ? tile : 0;
}
void cWindowQT::SetTileAt(int x, int y, int tile) {
    if (TileAt(x, y) < 0 || tile < 0 || tile >= 480) return;
    const size_t offset = 0x60 + 2 * (size_t(y) * g_Fodder->mMapLoaded->getWidth() + x);
    writeLEWord(g_Fodder->mMap->data() + offset, uint16(tile));
    if (!mTerrain.isNull()) {
        QPainter painter(&mTerrain);
        painter.drawImage(QRect(x * 16, y * 16, 16, 16), mTileAtlas,
                          QRect((tile % 20) * 16, (tile / 20) * 16, 16, 16));
    }
}
void cWindowQT::RefreshMap(bool resetView) {
    if (!mapReady()) return;
    cSurface atlas(320, 384);
    atlas.clearBuffer();
    for (int tile = 0; tile < 480; ++tile)
        g_Fodder->mGraphics->Map_Tile_Draw(&atlas, tile, tile % 20, tile / 20, 0);
    g_Fodder->mGraphics->PaletteSet(&atlas);
    mTileAtlas = surfaceImage(atlas);
    mSpritePreviews.clear();
    const int w = g_Fodder->mMapLoaded->getWidth(), h = g_Fodder->mMapLoaded->getHeight();
    // Cache ordinary maps; very large maps draw only visible tiles instead.
    mTerrain = (int64(w) * h <= 65536) ? QImage(w * 16, h * 16, QImage::Format_RGB32) : QImage();
    if (!mTerrain.isNull()) {
        QPainter painter(&mTerrain);
        for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
            const int tile = TileAt(x, y);
            if (tile >= 0) painter.drawImage(QRect(x * 16, y * 16, 16, 16), mTileAtlas,
                                           QRect(tile % 20 * 16, tile / 20 * 16, 16, 16));
        }
    }
    if (resetView) mViewOrigin = QPointF();
    mLastPaintTile = QPoint(-1, -1);
    ClampView();
    emit mapEdited();
    emit viewChanged();
    update();
}
const cWindowQT::sSpritePreview& cWindowQT::SpritePreview(int type, int sheetIndex, int frame) {
    const auto key = std::make_tuple(type, sheetIndex, frame);
    auto existing = mSpritePreviews.find(key);
    if (existing != mSpritePreviews.end()) return existing->second;
    sSpritePreview preview;
    if (type >= 0 && type < 118 && type != eSprite_Null && (sheetIndex >= 0 || g_SpriteAnim[type] >= 0)) {
        const int anim = sheetIndex >= 0 ? sheetIndex : g_SpriteAnim[type];
        auto sheet = g_Fodder->Sprite_Get_Sheet(anim, frame);
        int w = sheet->mColCount, h = sheet->mRowCount;
        if (g_Fodder->mVersionCurrent->isAmiga()) w = (w - 1) * 16;
        if (w > 0 && h > 0) {
            cSurface surface(w, h);
            surface.clearBuffer();
            g_Fodder->mGraphics->PaletteSet(&surface);
            sSprite sprite;
            sprite.mPosX = g_Fodder->mMapTile_MovedHorizontal * 16 + g_Fodder->mMapTile_ColumnOffset - sheet->mModX;
            sprite.mPosY = g_Fodder->mMapTile_MovedVertical * 16 + g_Fodder->mMapTile_RowOffset + h - 16 - sheet->mModY;
            sprite.mHeight = sprite.mRowsToSkip = 0;
            g_Fodder->Sprite_Draw_Frame(&sprite, anim, frame, &surface);
            preview.image = surfaceImage(surface).convertToFormat(QImage::Format_ARGB32);
            const auto indexed = surface.GetSurfaceBuffer();
            for (int y = 0; y < h; ++y) {
                auto pixels = reinterpret_cast<QRgb*>(preview.image.scanLine(y));
                for (int x = 0; x < w; ++x) if (!indexed[y * w + x]) pixels[x] = 0;
            }
            // The full-map canvas starts at column zero; the old 16-pixel
            // cropped viewport inset must not be subtracted from sprite X.
            preview.offset = QPoint(sheet->mModX, sheet->mModY - h);
        }
    }
    return mSpritePreviews.emplace(key, std::move(preview)).first->second;
}
void cWindowQT::DrawTerrain(QPainter& painter, const QRectF& visible) {
    if (!mTerrain.isNull()) { painter.drawImage(QPoint(0, 0), mTerrain); return; }
    int left = std::max(0, int(std::floor(visible.left() / 16)));
    int top = std::max(0, int(std::floor(visible.top() / 16)));
    int right = std::min(g_Fodder->mMapLoaded->getWidth(), int(std::ceil(visible.right() / 16)));
    int bottom = std::min(g_Fodder->mMapLoaded->getHeight(), int(std::ceil(visible.bottom() / 16)));
    for (int y = top; y < bottom; ++y) for (int x = left; x < right; ++x) {
        int tile = TileAt(x, y);
        if (tile >= 0) painter.drawImage(QRect(x * 16, y * 16, 16, 16), mTileAtlas,
                                       QRect(tile % 20 * 16, tile / 20 * 16, 16, 16));
    }
}
void cWindowQT::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(30, 34, 38));
    if (!mapReady() || mTileAtlas.isNull()) return;
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.scale(mZoom, mZoom);
    painter.translate(-mViewOrigin);
    DrawTerrain(painter, Viewport());
    std::vector<const sSprite*> sprites;
    for (const auto& sprite : g_Fodder->mSprites)
        if (sprite.mPosX != -32768 && sprite.mPosX != -1) sprites.push_back(&sprite);
    std::stable_sort(sprites.begin(), sprites.end(), [](const sSprite* a, const sSprite* b) {
        return std::make_pair(a->mDrawOrder, a->mPosY) < std::make_pair(b->mDrawOrder, b->mPosY);
    });
    for (const auto* sprite : sprites) {
        const auto& preview = SpritePreview(sprite->mSpriteType, sprite->mSheetIndex, sprite->mFrameIndex);
        const QPoint point = QPoint(sprite->mPosX, sprite->mPosY - sprite->mHeight) + preview.offset;
        if (Viewport().intersects(QRectF(point, preview.image.size()))) painter.drawImage(point, preview.image);
    }
    if (mHasMouse && !mPanning && g_OFED) {
        const QPointF world = MapAt(mLastMouse);
        const int x = int(std::floor(world.x() / 16)), y = int(std::floor(world.y() / 16));
        if (TileAt(x, y) >= 0) {
            painter.setOpacity(0.55);
            painter.save();
            painter.setClipRect(QRect(0, 0, g_Fodder->mMapLoaded->getWidthPixels(),
                                      g_Fodder->mMapLoaded->getHeightPixels()), Qt::IntersectClip);
            for (const auto& tile : g_OFED->GetCursorRangeTiles().mTiles) {
                if (tile.mTileID < 480) painter.drawImage(QRect((x + tile.mX) * 16, (y + tile.mY) * 16, 16, 16), mTileAtlas,
                    QRect(tile.mTileID % 20 * 16, tile.mTileID / 20 * 16, 16, 16));
            }
            painter.restore();
            if (g_OFED->GetCursorSpriteID() >= 0) painter.drawImage(world, SpritePreview(g_OFED->GetCursorSpriteID()).image);
            painter.setOpacity(1);
            painter.setPen(QPen(QColor(255, 220, 90), 1 / mZoom));
            painter.drawRect(QRectF(x * 16, y * 16, 16, 16));
        }
    }
}
void cWindowQT::ClampView() {
    if (!mapReady()) return;
    const double w = g_Fodder->mMapLoaded->getWidthPixels(), h = g_Fodder->mMapLoaded->getHeightPixels();
    const double vw = width() / mZoom, vh = height() / mZoom;
    mViewOrigin.setX(vw >= w ? (w - vw) / 2 : std::max(0.0, std::min(mViewOrigin.x(), w - vw)));
    mViewOrigin.setY(vh >= h ? (h - vh) / 2 : std::max(0.0, std::min(mViewOrigin.y(), h - vh)));
    CameraTilesUpdate();
}
void cWindowQT::SetViewZoom(double zoom, const QPointF& anchor) {
    if (!mapReady()) return;
    const QPointF world = MapAt(anchor);
    const double fit = std::min(double(width()) / g_Fodder->mMapLoaded->getWidthPixels(),
                                double(height()) / g_Fodder->mMapLoaded->getHeightPixels());
    mZoom = std::max(std::min(0.125, fit), std::min(8.0, zoom));
    mScaleWidth = mScaleHeight = mZoom;
    mViewOrigin = world - anchor / mZoom;
    ClampView();
    emit viewChanged();
    update();
}
void cWindowQT::CenterOnMap(const QPointF& point) {
    mViewOrigin = point - QPointF(width() / (2 * mZoom), height() / (2 * mZoom));
    ClampView();
    emit viewChanged();
    update();
}
void cWindowQT::FitMap() {
    if (!mapReady()) return;
    SetViewZoom(std::min(double(width()) / g_Fodder->mMapLoaded->getWidthPixels(),
                         double(height()) / g_Fodder->mMapLoaded->getHeightPixels()), rect().center());
    CenterOnMap(QPointF(g_Fodder->mMapLoaded->getWidthPixels() / 2.0, g_Fodder->mMapLoaded->getHeightPixels() / 2.0));
}
void cWindowQT::CameraSetTiles() { CameraTilesUpdate(); }
void cWindowQT::CameraTilesUpdate() {
    if (!mapReady()) return;
    mScaleWidth = mScaleHeight = mZoom;
    mCameraTilesX = int16(std::min(g_Fodder->mMapLoaded->getWidth(), int(std::ceil(width() / mZoom / 16))));
    mCameraTilesY = int16(std::min(g_Fodder->mMapLoaded->getHeight(), int(std::ceil(height() / mZoom / 16))));
}
void cWindowQT::CameraUpdate() { update(); }
void cWindowQT::resizeEvent(QResizeEvent*) { ClampView(); emit viewChanged(); }
void cWindowQT::wheelEvent(QWheelEvent* event) {
    double steps = event->angleDelta().y() / 120.0;
    if (steps == 0) steps = event->pixelDelta().y() / 120.0;
    SetViewZoom(mZoom * std::pow(1.25, steps), event->position());
    event->accept();
}
void cWindowQT::enterEvent(QEnterEvent*) { mHasMouse = true; }
void cWindowQT::leaveEvent(QEvent*) { mHasMouse = false; update(); }

void cWindowQT::PaintAt(const QPointF& position, bool first) {
    if (!mapReady() || !g_OFED) return;
    const QPointF world = MapAt(position);
    const QPoint tile(int(std::floor(world.x() / 16)), int(std::floor(world.y() / 16)));
    if (TileAt(tile.x(), tile.y()) < 0 || (!first && tile == mLastPaintTile)) return;
    mLastPaintTile = tile;
    const auto brush = g_OFED->GetCursorRangeTiles();
    bool edited = false;
    if (!brush.mTiles.empty() && (first || !brush.mOnce)) {
        for (const auto& cell : brush.mTiles) SetTileAt(tile.x() + cell.mX, tile.y() + cell.mY, cell.mTileID);
        for (const auto& sprite : brush.mSprites) {
            auto sheet = g_Fodder->Sprite_Get_Sheet(g_SpriteAnim[sprite.mSpriteID], 0);
            g_Fodder->Sprite_Add(sprite.mSpriteID, tile.x() * 16 + sprite.mX - sheet->mModX,
                                (tile.y() + 1) * 16 + sprite.mY + sheet->mModY);
        }
        edited = true;
        if (brush.mOnce) g_OFED->CursorReset();
    } else if (first && g_OFED->GetCursorSpriteID() >= 0) {
        int type = g_OFED->GetCursorSpriteID();
        const auto& preview = SpritePreview(type);
        g_Fodder->Sprite_Add(type, int(world.x()) - preview.offset.x(), int(world.y()) - preview.offset.y());
        edited = true;
    }
    if (edited) { g_OFED->Sprite_AddNew(); update(); }
}
void cWindowQT::mousePressEvent(QMouseEvent* event) {
    setFocus();
    mLastMouse = event->position();
    if (event->button() == Qt::MiddleButton) { mPanning = true; setCursor(Qt::ClosedHandCursor); }
    else if (event->button() == Qt::LeftButton) { mPainting = true; PaintAt(event->position(), true); }
    else if (event->button() == Qt::RightButton && mapReady()) {
        g_Fodder->Sprite_Handle_Loop(); RefreshMap(false);
    }
}
void cWindowQT::mouseMoveEvent(QMouseEvent* event) {
    const QPointF position = event->position();
    if (mPanning) {
        mViewOrigin -= (position - mLastMouse) / mZoom;
        ClampView(); emit viewChanged();
    } else if (mPainting) {
        const int steps = std::max(1, int(QLineF(mLastMouse, position).length() / std::max(1.0, 8 * mZoom)));
        for (int i = 1; i <= steps; ++i) PaintAt(mLastMouse + (position - mLastMouse) * (double(i) / steps), false);
    }
    mLastMouse = position;
    if (mapReady() && g_OFED) {
        auto world = MapAt(position);
        g_OFED->statusBar()->showMessage(tr("Tile %1, %2  |  Wheel to zoom · Middle drag to pan").arg(int(std::floor(world.x() / 16))).arg(int(std::floor(world.y() / 16))));
    }
    update();
}
void cWindowQT::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) { mPanning = false; unsetCursor(); }
    if (event->button() == Qt::LeftButton) { mPainting = false; mLastPaintTile = QPoint(-1, -1); }
    update();
}

QImage cWindowQT::OverviewImage() {
    if (!mapReady() || mTileAtlas.isNull()) return QImage();
    const QSize world(g_Fodder->mMapLoaded->getWidthPixels(), g_Fodder->mMapLoaded->getHeightPixels());
    const QSize size = world.scaled(512, 512, Qt::KeepAspectRatio);
    QImage image(size, QImage::Format_RGB32);
    image.fill(Qt::black);
    QPainter painter(&image);
    painter.scale(double(size.width()) / world.width(), double(size.height()) / world.height());
    DrawTerrain(painter, QRectF(QPointF(), world));
    return image;
}
