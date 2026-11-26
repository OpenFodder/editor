#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "SurfaceImage.hpp"
#include "ui_ToolboxTiles.h"

#include <QMouseEvent>
#include <QPainter>
#include <QSet>
#include <algorithm>

namespace {
constexpr int TileColumns = 20;
constexpr int TileRows = 24;
constexpr int TileSize = 16;
constexpr int CellSize = TileSize + 2;
const QRect TileGrid(0, 0, TileColumns, TileRows);

int TileID(const QPoint& cell) {
    return cell.y() * TileColumns + cell.x();
}

QRect SelectionRect(const QPoint& first, const QPoint& last) {
    return QRect(QPoint(std::min(first.x(), last.x()), std::min(first.y(), last.y())),
                 QPoint(std::max(first.x(), last.x()), std::max(first.y(), last.y())));
}
}

class cTilePalette : public QWidget {
    QLabel* mStatus;
    QImage mImage;
    QSet<int> mSelected;
    QPoint mBrushOrigin;
    QPoint mPressedCell;
    QPoint mHoveredCell;
    QRect mMarquee;
    int mZoomPercent = 0;
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

public:
    explicit cTilePalette(QLabel* status);
    void SetImage(const QImage& image);
    void SetZoom(int percent);
    void ClearSelection();
};

cToolboxTiles::cToolboxTiles(QWidget* parent) : QWidget(parent) {
    Ui_ToolboxTiles ui;
    ui.setupUi(this);
    mPalette = new cTilePalette(ui.mCurrentTile);
    ui.mScrollArea->setWidget(mPalette);
    ui.mZoom->addItem(tr("Fit"), 0);
    for (int percent : {100, 150, 200, 300, 400})
        ui.mZoom->addItem(tr("%1%").arg(percent), percent);
    connect(ui.mZoom, &QComboBox::currentIndexChanged, this, [this, zoom = ui.mZoom](int index) {
        mPalette->SetZoom(zoom->itemData(index).toInt());
    });
}

void cToolboxTiles::RenderTiles() {
    cSurface surface(TileColumns * CellSize, TileRows * CellSize);
    surface.clearBuffer();
    for (int tile = 0; tile < TileColumns * TileRows; ++tile)
        g_Fodder->mGraphics->Map_Tile_Draw(&surface, tile, tile % TileColumns, tile / TileColumns, 2);

    g_Fodder->mGraphics->PaletteSet(&surface);
    surface.surfaceSetToPaletteNew();
    surface.draw(0, 0);
    mPalette->SetImage(ImageFromSDLSurface(*surface.GetSurface()));
}

void cToolboxTiles::ClearSelection() {
    mPalette->ClearSelection();
}

cTilePalette::cTilePalette(QLabel* status) : mStatus(status), mHoveredCell(-1, -1) {
    setMouseTracking(true);
    setToolTip(tr("Click to select a tile\nShift-drag to select a rectangle\nShift-click to add or remove a tile"));
    UpdateStatus();
}

void cTilePalette::SetImage(const QImage& image) {
    mImage = image;
    update();
}

void cTilePalette::SetZoom(int percent) {
    mZoomPercent = percent;
    const int cell = (TileSize * percent / 100) + 2;
    setMinimumSize(percent ? QSize(cell * TileColumns, cell * TileRows) : QSize(0, 0));
    mHoveredCell = QPoint(-1, -1);
    UpdateStatus();
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
    const qreal cell = mZoomPercent
        ? (TileSize * mZoomPercent / 100) + 2
        : std::min(qreal(width()) / TileColumns, qreal(height()) / TileRows);
    const QSizeF size(cell * TileColumns, cell * TileRows);
    return QRectF(QPointF((width() - size.width()) / 2, (height() - size.height()) / 2), size);
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
    painter.fillRect(rect(), palette().color(QPalette::Dark));
    if (mImage.isNull()) return;

    const QRectF grid = GridRect();
    const qreal cell = grid.width() / TileColumns;
    painter.drawImage(grid, mImage);
    const QColor outline(255, 220, 90);
    const auto cellRect = [&](const QRect& cells) {
        return QRectF(grid.x() + cells.x() * cell, grid.y() + cells.y() * cell,
                      cells.width() * cell, cells.height() * cell);
    };

    painter.setPen(outline);
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
        mSelected = {TileID(cell)};
        mBrushOrigin = cell;
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
