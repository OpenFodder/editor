#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "ui_ToolboxTiles.h"

#include <qpixmap.h>
#include <qpainter.h>

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

	for (int16 TileNumber = 0; TileNumber < 480; ++TileNumber) {

		g_Graphics.Map_Tile_Draw(mTileSurface, TileNumber, X, Y, 2);

		++X;
		if (X >= 20) {
			X = 0;
			++Y;
		}
	}

	g_Graphics.PaletteSet(mTileSurface);

	mTileSurface->surfaceSetToPaletteNew();
	mTileSurface->draw();

	SDL_Surface* Source = mTileSurface->GetSurface();
	mSurface = QImage(static_cast<uchar*>(Source->pixels), Source->w, Source->h, QImage::Format_RGB32);
}

void cToolboxTiles::paintEvent(QPaintEvent* e) {

	QPainter painter(this);
	QRectF Dest(0, 0, size().width(), size().height());
	QRectF Src(0, 0, mSurface.width(), mSurface.height());

	painter.drawImage(Dest, mSurface, Src);
}

