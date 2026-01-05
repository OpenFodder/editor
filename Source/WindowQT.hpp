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

class QEvent;
class QEnterEvent;

class cWindowQT : public QWidget, public cWindow {

	QImage				mSurface;
	QTimer				mMouseInTimer;
	QTimer				mRedrawTimer;

	int16				mCameraTilesX;
	int16				mCameraTilesY;

	int16				mMouseX;
	int16				mMouseY;

public:
	double				mScaleWidth, mScaleHeight;

protected:
	void				paintEvent(QPaintEvent* e);

	void				mouseMoveEvent(QMouseEvent *eventMove);
	void				mousePressEvent(QMouseEvent *eventPress);
	void				mouseReleaseEvent(QMouseEvent *releaseEvent);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	void				enterEvent(QEnterEvent* pEvent) override;
	void				leaveEvent(QEvent* pEvent) override;
#else
	void				enterEvent(QEvent* pEvent) override;
	void				leaveEvent(QEvent* pEvent) override;
#endif

public:
						cWindowQT(QWidget* pParent);

	virtual void		EventCheck();
	virtual void		FrameEnd();

	virtual bool		InitWindow(const std::string& pWindowTitle);

	virtual void		PositionWindow();

	virtual void		RenderAt(cSurface* pImage);
	virtual void		RenderShrunk(cSurface* pImage);

	void				CameraSetTiles();
	void				CameraTilesUpdate();
	void				CameraUpdate();
};
