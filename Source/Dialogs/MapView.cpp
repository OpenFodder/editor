#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include <QPainter>
#include <QMouseEvent>

cMapView::cMapView(QWidget* parent) : QWidget(parent) {
    setMinimumSize(160, 140);
    setCursor(Qt::PointingHandCursor);
    setToolTip(tr("Click or drag to move around the map. The outline shows the visible area."));
    mRefreshTimer.setSingleShot(true);
    mRefreshTimer.setInterval(150);
    connect(&mRefreshTimer, &QTimer::timeout, this, &cMapView::RenderTiles);
    auto window = std::dynamic_pointer_cast<cWindowQT>(g_Window);
    connect(window.get(), &cWindowQT::viewChanged, this, [this]() { update(); });
    connect(window.get(), &cWindowQT::mapEdited, this, [this]() {
        if (!mRefreshTimer.isActive()) mRefreshTimer.start();
    });
    RenderTiles();
}
QRectF cMapView::ImageRect() const {
    if (mImage.isNull()) return QRectF();
    QSizeF size = mImage.size();
    size.scale(QSizeF(std::max(1, width() - 16), std::max(1, height() - 16)), Qt::KeepAspectRatio);
    return QRectF(QPointF((width() - size.width()) / 2, (height() - size.height()) / 2), size);
}
void cMapView::RenderTiles() {
    auto window = std::dynamic_pointer_cast<cWindowQT>(g_Window);
    if (window) mImage = window->OverviewImage();
    update();
}
void cMapView::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(30, 34, 38));
    if (mImage.isNull() || !g_Fodder->mMapLoaded) return;
    const auto target = ImageRect();
    painter.drawImage(target, mImage);
    const double sx = target.width() / g_Fodder->mMapLoaded->getWidthPixels();
    const double sy = target.height() / g_Fodder->mMapLoaded->getHeightPixels();
    painter.save();
    painter.setClipRect(target);
    for (const auto& sprite : g_Fodder->mSprites) {
        if (sprite.mPosX == -32768 || sprite.mPosX == -1) continue;
        painter.setPen(sprite.mSpriteType == eSprite_Player ? QColor(255, 230, 100) : QColor(255, 130, 100));
        painter.drawPoint(target.topLeft() + QPointF(sprite.mPosX * sx, sprite.mPosY * sy));
    }
    const auto view = std::dynamic_pointer_cast<cWindowQT>(g_Window)->Viewport();
    QRectF visible(target.left() + view.left() * sx, target.top() + view.top() * sy, view.width() * sx, view.height() * sy);
    visible = visible.intersected(target.adjusted(1, 1, -1, -1));
    painter.setBrush(QColor(100, 210, 255, 30));
    painter.setPen(QPen(QColor(100, 210, 255), 2));
    painter.drawRect(visible);
    painter.restore();
}
void cMapView::Navigate(const QPointF& position) {
    const auto target = ImageRect();
    if (target.isEmpty() || !target.contains(position)) return;
    const auto point = position - target.topLeft();
    std::dynamic_pointer_cast<cWindowQT>(g_Window)->CenterOnMap(QPointF(
        point.x() / target.width() * g_Fodder->mMapLoaded->getWidthPixels(),
        point.y() / target.height() * g_Fodder->mMapLoaded->getHeightPixels()));
}
void cMapView::mousePressEvent(QMouseEvent* event) { if (event->button() == Qt::LeftButton) Navigate(event->position()); }
void cMapView::mouseMoveEvent(QMouseEvent* event) { if (event->buttons() & Qt::LeftButton) Navigate(event->position()); }
