#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "SurfaceImage.hpp"
#include "ui_ToolboxTiles.h"

#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QSet>
#include <QWheelEvent>
#include <QtMath>
#include <algorithm>
#include <cmath>

namespace {
constexpr int TileColumns = 20;
constexpr int TileRows = 24;
constexpr int TileSize = 16;
constexpr int TileGap = 1;
constexpr qreal MaxCellSize = TileSize * 8;
const QRect TileGrid(0, 0, TileColumns, TileRows);

qreal FitCellSize(const QSize& size) {
    return std::min(qreal(size.width()) / TileColumns, qreal(size.height()) / TileRows);
}

int TileID(const QPoint& cell) {
    return cell.y() * TileColumns + cell.x();
}

QRect SelectionRect(const QPoint& first, const QPoint& last) {
    return QRect(QPoint(std::min(first.x(), last.x()), std::min(first.y(), last.y())),
                 QPoint(std::max(first.x(), last.x()), std::max(first.y(), last.y())));
}
}

class cTilePalette : public QWidget {
    QScrollArea* mScrollArea;
    QLabel* mStatus;
    QImage mImage;
    QSet<int> mSelected;
    QPoint mBrushOrigin;
    QPoint mPressedCell;
    QPoint mHoveredCell;
    QRect mMarquee;
    qreal mZoomCellSize = 0;
    bool mSelecting = false;

    QRectF GridRect() const;
    QPoint CellAt(const QPointF& position) const;
    void UpdateStatus();
    void CommitSelection();

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void leaveEvent(QEvent*) override;
    void wheelEvent(QWheelEvent*) override;

public:
    cTilePalette(QScrollArea* scrollArea, QLabel* status);
    void SetImage(const QImage& image);
    void ClearSelection();
};

cToolboxTiles::cToolboxTiles(QWidget* parent) : QWidget(parent) {
    Ui_ToolboxTiles ui;
    ui.setupUi(this);
    mPalette = new cTilePalette(ui.mScrollArea, ui.mCurrentTile);
    ui.mScrollArea->setWidget(mPalette);
    ui.mScrollArea->setFrameShape(QFrame::NoFrame);
    ui.mCurrentTile->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    connect(ui.mClearTiles, &QPushButton::clicked, this, []() { g_OFED->CursorReset(); });
}

void cToolboxTiles::RenderTiles() {
    cSurface surface(TileColumns * TileSize, TileRows * TileSize);
    surface.clearBuffer();
    for (int tile = 0; tile < TileColumns * TileRows; ++tile)
        g_Fodder->mGraphics->Map_Tile_Draw(&surface, tile, tile % TileColumns, tile / TileColumns, 0);

    g_Fodder->mGraphics->PaletteSet(&surface);
    surface.surfaceSetToPaletteNew();
    surface.draw(0, 0);
    mPalette->SetImage(ImageFromSDLSurface(*surface.GetSurface()));
}

void cToolboxTiles::ClearSelection() {
    mPalette->ClearSelection();
}

int cToolboxTiles::WidthForFittedTiles() const {
    return qCeil(qreal(mPalette->height()) * TileColumns / TileRows) + width() - mPalette->width();
}

cTilePalette::cTilePalette(QScrollArea* scrollArea, QLabel* status)
    : mScrollArea(scrollArea), mStatus(status), mHoveredCell(-1, -1) {
    setMouseTracking(true);
    setToolTip(tr("Scroll to zoom\nClick to select or deselect a tile\nShift-drag to select a rectangle\nShift-click to add or remove a tile"));
    UpdateStatus();
}

void cTilePalette::SetImage(const QImage& image) {
    mImage = image;
    update();
}

void cTilePalette::ClearSelection() {
    mSelecting = false;
    mMarquee = QRect();
    mSelected.clear();
    UpdateStatus();
    update();
}

QRectF cTilePalette::GridRect() const {
    const qreal cell = mZoomCellSize > 0 ? mZoomCellSize : FitCellSize(size());
    return QRectF(0, 0, cell * TileColumns, cell * TileRows);
}

QPoint cTilePalette::CellAt(const QPointF& position) const {
    const QRectF grid = GridRect();
    if (grid.isEmpty() || (!mSelecting && !grid.contains(position)))
        return QPoint(-1, -1);

    const QPointF offset = position - grid.topLeft();
    return QPoint(std::clamp(int(offset.x() * TileColumns / grid.width()), 0, TileColumns - 1),
                  std::clamp(int(offset.y() * TileRows / grid.height()), 0, TileRows - 1));
}

void cTilePalette::UpdateStatus() {
    QString status = tr("%1 selected").arg(mMarquee.isValid() ? mMarquee.width() * mMarquee.height()
                                                              : mSelected.size());
    if (TileGrid.contains(mHoveredCell))
        status += tr("  |  Tile %1").arg(TileID(mHoveredCell));
    mStatus->setText(status);
}

void cTilePalette::CommitSelection() {
    // Stamp each tile at its original offset, including gaps left by Shift-click.
    sTiles brush;
    for (int tile = 0; tile < TileColumns * TileRows; ++tile) {
        if (mSelected.contains(tile))
            brush.mTiles.emplace_back(tile % TileColumns - mBrushOrigin.x(),
                                      tile / TileColumns - mBrushOrigin.y(), tile);
    }
    g_OFED->setCursorTiles(brush, true);
    UpdateStatus();
    update();
}

void cTilePalette::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    const QRectF grid = GridRect();
    painter.fillRect(rect(), QColor(30, 34, 38));
    if (mImage.isNull() || grid.isEmpty()) return;

    const qreal cell = grid.width() / TileColumns;
    for (int y = 0; y < TileRows; ++y) {
        const int top = qRound(y * cell);
        const int height = qRound((y + 1) * cell) - top - TileGap;
        for (int x = 0; x < TileColumns; ++x) {
            const int left = qRound(x * cell);
            const int width = qRound((x + 1) * cell) - left - TileGap;
            painter.drawImage(QRect(left, top, width, height), mImage,
                              QRect(x * TileSize, y * TileSize, TileSize, TileSize));
        }
    }

    const auto cellRect = [&](const QRect& cells) {
        return QRectF(cells.x() * cell, cells.y() * cell, cells.width() * cell, cells.height() * cell);
    };

    painter.setPen(QColor(255, 220, 90));
    const QColor fill(255, 220, 90, 70);
    if (mMarquee.isValid()) {
        const QRectF box = cellRect(mMarquee);
        painter.fillRect(box, fill);
        painter.drawRect(box.adjusted(0.5, 0.5, -0.5, -0.5));
    } else {
        for (int tile : mSelected) {
            const QRectF box = cellRect(QRect(tile % TileColumns, tile / TileColumns, 1, 1));
            painter.fillRect(box, fill);
            painter.drawRect(box.adjusted(0.5, 0.5, -0.5, -0.5));
        }
    }
}

void cTilePalette::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton || mImage.isNull()) return;
    const QPoint cell = CellAt(event->position());
    if (!TileGrid.contains(cell)) return;

    mHoveredCell = cell;
    if (event->modifiers().testFlag(Qt::ShiftModifier)) {
        mPressedCell = cell;
        mSelecting = true;
        UpdateStatus();
    } else {
        if (mSelected.size() == 1 && mSelected.contains(TileID(cell))) {
            mSelected.clear();
        } else {
            mSelected = {TileID(cell)};
            mBrushOrigin = cell;
        }
        CommitSelection();
    }
}

void cTilePalette::mouseMoveEvent(QMouseEvent* event) {
    mHoveredCell = CellAt(event->position());
    if (mSelecting && (mHoveredCell != mPressedCell || mMarquee.isValid())) {
        mMarquee = SelectionRect(mPressedCell, mHoveredCell);
        update();
    }
    UpdateStatus();
}

void cTilePalette::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton || !mSelecting) return;

    const QPoint cell = CellAt(event->position());
    if (cell != mPressedCell || mMarquee.isValid()) {
        const QRect selection = SelectionRect(mPressedCell, cell);
        mSelected.clear();
        mBrushOrigin = selection.topLeft();
        for (int y = selection.top(); y <= selection.bottom(); ++y)
            for (int x = selection.left(); x <= selection.right(); ++x)
                mSelected.insert(TileID(QPoint(x, y)));
    } else if (!mSelected.remove(TileID(cell))) {
        if (mSelected.isEmpty()) mBrushOrigin = cell;
        mBrushOrigin = QPoint(std::min(mBrushOrigin.x(), cell.x()), std::min(mBrushOrigin.y(), cell.y()));
        mSelected.insert(TileID(cell));
    }
    mSelecting = false;
    mMarquee = QRect();
    mHoveredCell = CellAt(event->position());
    CommitSelection();
}

void cTilePalette::leaveEvent(QEvent*) {
    mHoveredCell = QPoint(-1, -1);
    UpdateStatus();
}

void cTilePalette::wheelEvent(QWheelEvent* event) {
    const qreal steps = event->angleDelta().y() ? event->angleDelta().y() / 120.0
                                               : event->pixelDelta().y() / 40.0;
    const auto grid = GridRect();
    if (mImage.isNull() || grid.isEmpty() || !steps) {
        event->ignore();
        return;
    }
    event->accept();

    const qreal oldCell = grid.width() / TileColumns;
    const qreal fit = FitCellSize(mScrollArea->maximumViewportSize());
    const qreal newCell = std::clamp(oldCell * std::pow(1.25, steps), fit, std::max(fit, MaxCellSize));
    if (qFuzzyCompare(newCell, oldCell)) return;

    auto* viewport = mScrollArea->viewport();
    const QPointF pointer = mapTo(viewport, event->position());
    const QPointF tileAnchor(std::clamp(event->position().x() / oldCell, qreal(0), qreal(TileColumns)),
                             std::clamp(event->position().y() / oldCell, qreal(0), qreal(TileRows)));
    mZoomCellSize = qFuzzyCompare(newCell, fit) ? 0 : newCell;
    const QSize minimum = mZoomCellSize > 0
        ? QSize(qCeil(newCell * TileColumns), qCeil(newCell * TileRows))
        : QSize(0, 0);
    setMinimumSize(minimum);
    resize(viewport->size().expandedTo(minimum));
    const QPointF scroll = tileAnchor * newCell - pointer;
    mScrollArea->horizontalScrollBar()->setValue(qRound(scroll.x()));
    mScrollArea->verticalScrollBar()->setValue(qRound(scroll.y()));

    mHoveredCell = CellAt(mapFrom(viewport, pointer));
    UpdateStatus();
    update();
}
