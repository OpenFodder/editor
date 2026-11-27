#pragma once
#include "stdafx_ofed.hpp"
#include <QPointF>
#include <QRectF>
#include <map>
#include <tuple>

class QPainter;
struct sTiles;

class cWindowQT : public QWidget, public cWindow {
    Q_OBJECT
    QTimer mRedrawTimer;
    QImage mTileAtlas;
    QImage mTerrain;
    struct sSpritePreview { QImage image; QPoint offset; };
    std::map<std::tuple<int, int, int>, sSpritePreview> mSpritePreviews;
    QPointF mViewOrigin;
    QPointF mLastMouse;
    QPoint mLastPaintTile = QPoint(-1, -1);
    double mZoom = 2.0;
    bool mFitMap = true;
    bool mPanning = false;
    bool mPainting = false;
    bool mHasMouse = false;
    int16 mCameraTilesX = 0, mCameraTilesY = 0;

    double FitZoom() const;
    void ClampView();
    void PaintAt(const QPointF& position, bool first);
    int TileAt(int x, int y) const;
    void SetTileAt(int x, int y, int tile);
    const sSpritePreview& SpritePreview(int type, int sheet = -1, int frame = 0);
    void DrawTerrain(QPainter& painter, const QRectF& visible);
    void DrawBrush(QPainter& painter, const QPoint& tile, const sTiles& brush);

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void enterEvent(QEnterEvent*) override;
    void leaveEvent(QEvent*) override;

public:
    double mScaleWidth = 2.0, mScaleHeight = 2.0;
    explicit cWindowQT(QWidget* parent);
    void EventCheck() override;
    void FrameEnd() override;
    bool InitWindow(const std::string&) override;
    void PositionWindow() override;
    void RenderAt(cSurface*) override;
    void RenderShrunk(cSurface*) override;
    void CameraSetTiles();
    void CameraTilesUpdate();
    void CameraUpdate();
    int16 getCameraTilesX() const { return mCameraTilesX; }
    int16 getCameraTilesY() const { return mCameraTilesY; }

    void RefreshMap(bool resetView = true);
    QImage OverviewImage();
    QImage SpritePaletteImage(int type);
    QImage StructureThumbnail(const sTiles& brush);
    QPointF MapAt(const QPointF& position) const { return mViewOrigin + position / mZoom; }
    QRectF Viewport() const { return QRectF(mViewOrigin, QSizeF(width() / mZoom, height() / mZoom)); }
    double Zoom() const { return mZoom; }
    bool IsFittingMap() const { return mFitMap; }
    void SetViewZoom(double zoom, const QPointF& anchor);
    void CenterOnMap(const QPointF& point);
    void FitMap();

signals:
    void viewChanged();
    void mapEdited();
};
