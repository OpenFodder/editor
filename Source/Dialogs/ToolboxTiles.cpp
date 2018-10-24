#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "ui_ToolboxTiles.h"

#include <qpixmap.h>
#include <qpainter.h>
#include <qevent.h>

cToolboxTiles::cToolboxTiles( QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

	mTileSurface = new cSurface(360, 432);

	mUi = new Ui_ToolboxTiles();
	mUi->setupUi(this);

	RenderTiles();
}

cToolboxTiles::~cToolboxTiles() {

	delete mTileSurface;
}

void cToolboxTiles::RenderTiles() {
	size_t width = this->width();
	size_t height = this->height();

	int32 X = 0, Y = 0;
	
	mTileSurface->clearBuffer();

	// Loop each tile
	for (int16 TileNumber = 0; TileNumber < 480; ++TileNumber) {

		// Draw tile
		g_Fodder->mGraphics->Map_Tile_Draw(mTileSurface, TileNumber, X, Y, 2);

		// Next tile position
		++X;

		// 20 Tiles per row
		if (X >= 20) {
			X = 0;
			++Y;
		}
	}

	// Load the tileset palette to the surface
	g_Fodder->mGraphics->PaletteSet(mTileSurface);
	mTileSurface->surfaceSetToPaletteNew();

	// Draw the final image
	mTileSurface->draw();

	// Copy the image into a QImage
	SDL_Surface* Source = mTileSurface->GetSurface();
	mImage = QImage(static_cast<uchar*>(Source->pixels), Source->w, Source->h, QImage::Format_RGB32);

	this->repaint();
}

void cToolboxTiles::paintEvent(QPaintEvent* e) {


    mScaleWidth = (static_cast<double>(size().width()) / static_cast<double>(mImage.width()));
    mScaleHeight = (static_cast<double>(size().height() - 20) / static_cast<double>(mImage.height()));

	QPainter painter(this);
	QRectF Dest(0, 0, size().width(), size().height() - 20);
	QRectF Src(0, 0, mImage.width(), mImage.height());

	painter.drawImage(Dest, mImage, Src);
}

void cToolboxTiles::mousePressEvent(QMouseEvent *eventPress) {
	size_t MouseX = eventPress->x() / mScaleWidth;
	size_t MouseY = eventPress->y() / mScaleHeight;

	uint32 TileX = MouseX / 18;
	uint32 TileY = MouseY / 18;

	// 20 Tiles per row
	uint32 TileID = (20 * TileY) + TileX;

	if (eventPress->button() == Qt::MouseButton::LeftButton) {

		g_OFED->SetCursorTileID(TileID);
        this->repaint();
	}
}

void cToolboxTiles::mouseMoveEvent(QMouseEvent *eventMove) {

    size_t MouseX = eventMove->x() / mScaleWidth;
    size_t MouseY = eventMove->y() / mScaleHeight;

    uint32 TileX = MouseX / 18;
    uint32 TileY = MouseY / 18;

    // 20 Tiles per row
    uint32 TileID = (20 * TileY) + TileX;

    mUi->mCurrentTile->setText(QString::fromStdString(std::to_string(TileID)));
}
