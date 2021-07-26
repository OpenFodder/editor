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

    mCameraTilesX = mCameraTilesY = 0;
    mMouseX = mMouseY = 0;

	// Mouse inside widget callback
	connect(&mMouseInTimer, &QTimer::timeout, this, &cWindowQT::CameraUpdate);

    
}

void cWindowQT::FrameEnd() {

	RenderAt(g_Fodder->mSurface);
	this->repaint();
}

bool cWindowQT::InitWindow(const std::string&) {

	return true;
}

void cWindowQT::PositionWindow() {

}

void cWindowQT::RenderAt(cSurface* pImage ) {

	pImage->draw();

	SDL_Surface* Surface = pImage->GetSurface();

	// Copy to surface
	mSurface = QImage(static_cast<uchar*>(Surface->pixels), Surface->w, Surface->h, QImage::Format_RGB32);
}

void cWindowQT::RenderShrunk(cSurface*) {
	//pImage->draw();
	// Copy to surface
}

void cWindowQT::EventCheck() {
	
}

void cWindowQT::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	QRectF Dest(0,0, size().width(), size().height());
	QRectF Src(16, 16, mScreenSize.mWidth - 16, mScreenSize.mHeight - 16);
	
	painter.drawImage( Dest, mSurface, Src);
}

void cWindowQT::enterEvent(QEvent *) {

	mMouseInTimer.start(4);
}

void cWindowQT::leaveEvent(QEvent *) {

	mMouseInTimer.stop();
}

void cWindowQT::CameraSetTiles() {

		mCameraTilesX = 0x15;
		mCameraTilesY = PLATFORM_BASED(0x0C, 0x0D);
}
void cWindowQT::CameraTilesUpdate() {
	mScaleWidth = (static_cast<double>(size().width()) / static_cast<double>(mScreenSize.mWidth));
	mScaleHeight = (static_cast<double>(size().height()) / static_cast<double>(mScreenSize.mHeight));
	// Number of tiles which are shown
	CameraSetTiles();

	if (mCameraTilesX > g_Fodder->mMapLoaded->getWidth())
		mCameraTilesX = g_Fodder->mMapLoaded->getWidth();

	if (mCameraTilesY > g_Fodder->mMapLoaded->getHeight())
		mCameraTilesY = g_Fodder->mMapLoaded->getHeight();
}

void cWindowQT::CameraUpdate() {
	int32 mEdgeWidth = 40;

	CameraTilesUpdate();
	g_Fodder->MapTile_UpdateFromCamera();

	// Button Pressed?
	if (g_Fodder->mMouse_EventLastButtonsPressed & 1) {
		if (g_Fodder->mMouse_EventLastPosition.mX >= 0 || g_Fodder->mMouse_EventLastPosition.mY >= 0) {
            
            uint32 TileX = (double)((g_Fodder->mMouse_EventLastPosition.mX) / 17) + 1;
            uint32 TileY = (double)((g_Fodder->mMouse_EventLastPosition.mY) / 17);


            // Adding tiles
            if(g_OFED->GetCursorRangeTiles().mTiles.size()) {
            
                for (auto Tile : g_OFED->GetCursorRangeTiles().mTiles) {

                    g_Fodder->MapTile_Set(TileX + Tile.mX, TileY + Tile.mY, Tile.mTileID);
                }

                for (auto Sprite : g_OFED->GetCursorRangeTiles().mSprites) {

                    auto Sheet = g_Fodder->Sprite_Get_Sheet(Sprite.mSpriteID, 0);

                    g_Fodder->Sprite_Add(Sprite.mSpriteID,
                        ((TileX * 16) + Sprite.mX) - Sheet->mModX + (g_Fodder->mMapTile_MovedHorizontal * 16),
                        (((TileY + 1) * 16)+ Sprite.mY) + Sheet->mModY + (g_Fodder->mMapTile_MovedVertical * 16));

                }

            } else {
                // Adding a sprite?
                if (g_OFED->GetCursorSpriteID() != -1) {

                    auto Sheet = g_Fodder->Sprite_Get_Sheet(g_OFED->GetCursorSpriteID(), 0);

                    g_Fodder->Sprite_Add(g_OFED->GetCursorSpriteID(),
                        (g_Fodder->mMouse_EventLastPosition.mX) - Sheet->mModX + (g_Fodder->mMapTile_MovedHorizontal * 16),
                        (g_Fodder->mMouse_EventLastPosition.mY) + Sheet->mModY + (g_Fodder->mMapTile_MovedVertical * 16));

                    // Prevent adding a sprite more than once
                    g_Fodder->mMouse_EventLastButtonsPressed &= ~1;
                }
            } 

            g_OFED->Sprite_AddNew();

			if (g_OFED->GetCursorRangeTiles().mOnce) {
				g_OFED->CursorReset();
			}
		}
	}

    // Tick the engine on right click
    if (g_Fodder->mMouse_EventLastButtonsPressed & 2) {
        g_Fodder->Sprite_Handle_Loop();
    }

	// Calculate the width/height of the playfield, adjusted to the window scale
	int32 Height = height() / mScaleHeight;
	int32 Width = width() / mScaleWidth;

	// Up
	if (g_Fodder->mMouse_EventLastPosition.mY < mEdgeWidth) {
		if (g_Fodder->mMapTile_MovedVertical > 0 || g_Fodder->mMapTile_RowOffset) {
			g_Fodder->MapTile_Move_Up(1);
		}
	}

	// Down
	if (g_Fodder->mMouse_EventLastPosition.mY > Height - mEdgeWidth) {
		if (g_Fodder->mMapTile_MovedVertical < g_Fodder->mMapLoaded->getHeight() - mCameraTilesY) {
			g_Fodder->MapTile_Move_Down(1);
		}
	}

	// Left
	if (g_Fodder->mMouse_EventLastPosition.mX < mEdgeWidth) {
		if (g_Fodder->mMapTile_MovedHorizontal > 0 || g_Fodder->mMapTile_ColumnOffset) {
			g_Fodder->MapTile_Move_Left(1);
		}
	}

	// Right
	if (g_Fodder->mMouse_EventLastPosition.mX > Width - mEdgeWidth) {
		if (g_Fodder->mMapTile_MovedHorizontal < (g_Fodder->mMapLoaded->getWidth()) - mCameraTilesX) {
			g_Fodder->MapTile_Move_Right(1);
		}
	}

    g_Fodder->MapTile_Update_Position();
	g_Fodder->mGraphics->MapTiles_Draw();
    g_Fodder->Mission_Sprites_Handle();

	FrameEnd();
}

void cWindowQT::mouseMoveEvent(QMouseEvent *eventMove) {
	cEvent Event;
	Event.mType = eEvent_MouseMove;
	Event.mPosition = cPosition(eventMove->x() / mScaleWidth, eventMove->y() / mScaleHeight);

    g_Fodder->mWindow->EventGet()->push_back(Event);
	g_Fodder->Cycle_End();
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

	g_Fodder->mWindow->EventGet()->push_back(Event);
	g_Fodder->Cycle_End();
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

	g_Fodder->mWindow->EventGet()->push_back(Event);
	g_Fodder->Cycle_End();
}
