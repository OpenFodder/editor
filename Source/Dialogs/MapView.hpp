#pragma once
#include <QWidget>
#include <QImage>
#include <QRectF>
#include <QTimer>

class cMapView : public QWidget {
    QImage mImage;
    QTimer mRefreshTimer;
    QRectF ImageRect() const;
    void Navigate(const QPointF& position);
protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
public:
    explicit cMapView(QWidget* parent = nullptr);
    void RenderTiles();
};
