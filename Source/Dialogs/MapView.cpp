#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "ui_MapView.h"

#include <qpixmap.h>
#include <qpainter.h>
#include <qevent.h>

cMapView::cMapView( QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

	mUi = new Ui_MapView();
	mUi->setupUi(this);
	RenderTiles();
}

cMapView::~cMapView() {

}

void cMapView::RenderTiles() {

	g_Fodder->Map_Overview_Prepare();
	g_Fodder->mGraphics->PaletteSetOverview();
	g_Fodder->mSurfaceMapOverview->draw();
	// Copy the image into a QImage
	SDL_Surface* Source = g_Fodder->mSurfaceMapOverview->GetSurface();
	mImage = QImage(static_cast<uchar*>(Source->pixels), Source->w, Source->h, QImage::Format_RGB32);
	this->repaint();

}

void cMapView::paintEvent(QPaintEvent*) {

    mScaleWidth = (static_cast<double>(size().width()) / static_cast<double>(mImage.width()));
    mScaleHeight = (static_cast<double>(size().height()) / static_cast<double>(mImage.height()));

	QPainter painter(this);
	QRectF Dest(0, 0, size().width(), size().height());
	QRectF Src(0, 0, mImage.width(), mImage.height());

	painter.drawImage(Dest, mImage, Src);
}

