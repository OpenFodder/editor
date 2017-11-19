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
#include <qpixmap.h>
#include <qpainter.h>
#include <qevent.h>


cWindowQT::cWindowQT(QWidget* pParent) : QWidget(pParent), cWindow() {

	setAttribute(Qt::WA_Hover);
	setMouseTracking(true);

	mScaleWidth = 0;
	mScaleHeight = 0;

	// Mouse inside widget callback
	connect(&mMouseInTimer, &QTimer::timeout, this, &cWindowQT::CameraUpdate);
}

void cWindowQT::FrameEnd() {

	RenderAt(g_Fodder.mImage);
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

void cWindowQT::enterEvent(QEvent *pEvent) {

	mMouseInTimer.start(4);
}

void cWindowQT::leaveEvent(QEvent *pEvent) {

	mMouseInTimer.stop();
}

void cWindowQT::CameraSetTiles() {

	if (g_Fodder.mVersion->isAmiga()) {
		mCameraTilesX = 0x15;
		mCameraTilesY = 0x0F;
	}
	else {
		mCameraTilesX = 0x15;
		mCameraTilesY = 0x0F;
	}
}
void cWindowQT::CameraTilesUpdate() {
	mScaleWidth = (static_cast<double>(size().width()) / static_cast<double>(mScreenSize.mWidth));
	mScaleHeight = (static_cast<double>(size().height()) / static_cast<double>(mScreenSize.mHeight));
	// Number of tiles which are shown
	CameraSetTiles();

	if (mCameraTilesX > g_Fodder.mMapWidth)
		mCameraTilesX = g_Fodder.mMapWidth;

	if (mCameraTilesY > g_Fodder.mMapHeight)
		mCameraTilesY = g_Fodder.mMapHeight;
}

void cWindowQT::CameraUpdate() {
	size_t mEdgeWidth = 40;

	CameraTilesUpdate();
	g_Fodder.sub_12018();

	cFodder *Fodder = &g_Fodder;

	// Button Pressed?
	if (Fodder->mMouseButtons & 1) {
		if (Fodder->mMousePosition.mX >= 0 || Fodder->mMousePosition.mY >= 0) {

			uint32 TileX = (Fodder->mMousePosition.mX + 8) / 16;
			uint32 TileY = (Fodder->mMousePosition.mY + 8) / 16;

			for (auto Tile : g_OFED->GetCursorRangeTiles().mTiles) {

				g_Fodder.MapTile_Set(TileX + Tile.mX, TileY + Tile.mY, Tile.mTileID);
			}
		}
	}

	// Calculate the width/height of the playfield, adjusted to the window scale
	size_t Height = height() / mScaleHeight;
	size_t Width = width() / mScaleWidth;

	// Up
	if (Fodder->mMousePosition.mY < mEdgeWidth) {
		if (Fodder->mMapTile_MovedVertical > 0 || Fodder->mMapTile_RowOffset) {
			g_Fodder.MapTile_Move_Up(1);
		}
	}

	// Down
	if (Fodder->mMousePosition.mY > Height - mEdgeWidth) {
		if (Fodder->mMapTile_MovedVertical < Fodder->mMapHeight - mCameraTilesY) {
			g_Fodder.MapTile_Move_Down(1);
		}
	}

	// Left
	if (Fodder->mMousePosition.mX < mEdgeWidth) {
		if (Fodder->mMapTile_MovedHorizontal > 0 || Fodder->mMapTile_ColumnOffset) {
			g_Fodder.MapTile_Move_Left(1);
		}
	}

	// Right
	if (Fodder->mMousePosition.mX > Width - mEdgeWidth) {
		if (Fodder->mMapTile_MovedHorizontal < Fodder->mMapWidth - mCameraTilesX) {
			g_Fodder.MapTile_Move_Right(1);
		}
	}

	auto Start = 0x60 - (Fodder->mMapWidth * 2);

	if (g_Fodder.mMapTilePtr < (0x60 - (Fodder->mMapWidth * 2)))
		g_Fodder.mMapTilePtr = 0x60 - (Fodder->mMapWidth * 2);

	// 
	{
		auto CameraWidth = (mCameraTilesX * 2);
		auto CameraHeight = ((mCameraTilesY) * Fodder->mMapWidth) * 2;

		auto Right = g_Fodder.mMap->begin() + CameraWidth;
		auto MaxRight = (g_Fodder.mMap->begin() + g_Fodder.mMap->size()) - CameraHeight - CameraWidth;

		if (Right > MaxRight) {
			g_Fodder.mMapTilePtr = g_Fodder.mMap->begin() - MaxRight;
		}
	}
	g_Graphics.MapTiles_Draw();
	FrameEnd();
}

void cWindowQT::mouseMoveEvent(QMouseEvent *eventMove) {
	cEvent Event;
	Event.mType = eEvent_MouseMove;
	Event.mPosition = cPosition(eventMove->x() / mScaleWidth, eventMove->y() / mScaleHeight);

	g_Fodder.EventAdd(Event);
	g_Fodder.eventProcess();
}

void cWindowQT::mousePressEvent(QMouseEvent *eventPress) {
	cEvent Event;

	if (eventPress->button() == Qt::MouseButton::LeftButton) {
		Event.mType = eEvent_MouseLeftDown;
		Event.mButton = 1;
	}

	if (eventPress->button() == Qt::MouseButton::RightButton) {
		Event.mType = eEvent_MouseRightDown;
		Event.mButton = 3;
	}
	
	Event.mPosition = cPosition(eventPress->x() / mScaleWidth, eventPress->y() / mScaleHeight);

	g_Fodder.EventAdd(Event);
	g_Fodder.eventProcess();
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

	Event.mPosition = cPosition(releaseEvent->x() / mScaleWidth, releaseEvent->y() / mScaleHeight);

	g_Fodder.EventAdd(Event);
	g_Fodder.eventProcess();
}
