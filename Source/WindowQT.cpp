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
#include <qpixmap.h>
#include <qpainter.h>
#include <qevent.h>

cWindowQT::cWindowQT(QWidget* pParent) : QWidget(pParent), cWindow() {

	setAttribute(Qt::WA_Hover);
	setMouseTracking(true);
	mCameraTilesX = 0x15;
	mCameraTilesY = 0x0F;

	connect(&mMouseInTimer, &QTimer::timeout, this, &cWindowQT::CameraUpdate);
}

void cWindowQT::FrameEnd() {

	g_Window.RenderAt(g_Fodder.mImage);
	this->repaint();
}

bool cWindowQT::InitWindow(const std::string& pWindowTitle) {

	return true;
}

void cWindowQT::PositionWindow() {

}

void cWindowQT::RenderAt(cSurface* pImage, cPosition pSource ) {

	pImage->draw();

	SDL_Surface* Surface = pImage->GetSurface();

	// Copy to surface
	mSurface = QImage(static_cast<uchar*>(Surface->pixels), Surface->w, Surface->h, QImage::Format_RGB32);
}

void cWindowQT::RenderShrunk(cSurface* pImage) {
	//pImage->draw();
	// Copy to surface
}

void cWindowQT::EventCheck() {
	
}

void cWindowQT::paintEvent(QPaintEvent* e) {
	QPainter painter(this);
	QRectF Dest(0,0, size().width(), size().height());
	QRectF Src(16, 16, mScreenSize.mWidth - 16, mScreenSize.mHeight - 16);
	
	painter.drawImage( Dest, mSurface, Src);
}

void cWindowQT::resizeEvent(QResizeEvent *event) {
	
}

void cWindowQT::CameraTilesUpdate() {
	if (mCameraTilesX > g_Fodder.mMapWidth)
		mCameraTilesX = g_Fodder.mMapWidth;

	if (mCameraTilesY > g_Fodder.mMapHeight)
		mCameraTilesY = g_Fodder.mMapHeight;
}

void cWindowQT::enterEvent(QEvent *pEvent) {

	mMouseInTimer.start(2);
}

void cWindowQT::leaveEvent(QEvent *pEvent) {

	mMouseInTimer.stop();
}

void cWindowQT::CameraUpdate() {

	CameraTilesUpdate();
	g_Fodder.sub_12018();

	cFodder *Fodder = &g_Fodder;

	size_t mEdgeWidth = 40;

	// Up
	if (Fodder->mMousePosition.mY < mEdgeWidth) {
		if (Fodder->mMapTile_MovedVertical >= 0) {
			g_Fodder.MapTile_Move_Up(1);
		}
	}

	// Down
	if (Fodder->mMousePosition.mY > height() - mEdgeWidth) {
		if (Fodder->mMapTile_MovedVertical < Fodder->mMapHeight - mCameraTilesY) {
			g_Fodder.MapTile_Move_Down(1);
		}
	}

	// Left
	if (Fodder->mMousePosition.mX < mEdgeWidth) {
		if (Fodder->mMapTile_MovedHorizontal >= 0) {
			g_Fodder.MapTile_Move_Left(1);
		}
	}

	// Right
	if (Fodder->mMousePosition.mX > width() - mEdgeWidth) {
		if (Fodder->mMapTile_MovedHorizontal < Fodder->mMapWidth - mCameraTilesX) {
			g_Fodder.MapTile_Move_Right(1);
		}
	}

	g_Graphics.Map_Tiles_Draw();
	FrameEnd();
}

void cWindowQT::mouseMoveEvent(QMouseEvent *eventMove) {

	cEvent Event;
	Event.mType = eEvent_MouseMove;
	Event.mPosition = cPosition(eventMove->x(), eventMove->y());

	g_Fodder.EventAdd(Event);
	g_Fodder.eventProcess();
}

void cWindowQT::mousePressEvent(QMouseEvent *eventPress) {
	cEvent Event;
	/*g_Fodder.VersionSelect_2();

	g_Fodder.Map_Load_Resources();
	g_Fodder.Map_Load_Sprites();

	g_Fodder.mImage->surfaceSetToPaletteNew();
	*/
	if (eventPress->button() == Qt::MouseButton::LeftButton) {
		Event.mType = eEvent_MouseLeftDown;
		Event.mButton = 1;
	}

	if (eventPress->button() == Qt::MouseButton::RightButton) {
		Event.mType = eEvent_MouseRightDown;
		Event.mButton = 3;
	}
	
	Event.mPosition = cPosition(eventPress->x(), eventPress->y());
	g_Fodder.EventAdd(Event);

	g_Graphics.Map_Tiles_Draw();
	FrameEnd();
	
}

void cWindowQT::mouseReleaseEvent(QMouseEvent *releaseEvent) {
	cEvent Event;

	if (releaseEvent->button() == Qt::MouseButton::LeftButton) {
		Event.mType = eEvent_MouseLeftUp;
		Event.mButton = 1;
	}

	if (releaseEvent->button() == Qt::MouseButton::RightButton) {
		Event.mType = eEvent_MouseRightUp;
		Event.mButton = 3;
	}

	Event.mPosition = cPosition(releaseEvent->x(), releaseEvent->y());
	
	g_Fodder.EventAdd(Event);
}
