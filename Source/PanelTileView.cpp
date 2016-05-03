///-----------------------------------------------------------------
///
/// @file      TileView.cpp
/// @author    Robbie
/// Created:   30/04/2016 17:39:36
/// @section   DESCRIPTION
///            cPanelTileView class implementation
///
///------------------------------------------------------------------

#include "stdafx.hpp"
#include "PanelTileView.h"

//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// cPanelTileView
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(cPanelTileView,wxPanel)
	////Manual Code Start
	EVT_TIMER( ID_EVENT_MOUSE_TIMER, cPanelTileView::OnMouseInputTimer )
	////Manual Code End
	
	EVT_CLOSE(cPanelTileView::OnClose)
	EVT_PAINT(cPanelTileView::cTileViewPaint)
	EVT_MOUSE_EVENTS(cPanelTileView::OnMouse)
END_EVENT_TABLE()
////Event Table End

cPanelTileView::cPanelTileView(wxWindow *parent, wxWindowID id, const wxPoint &position, const wxSize& size, long style)
: wxPanel(parent, id, position, size, wxFRAME_NO_TASKBAR | wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX) )
{
	CreateGUIControls();
}

cPanelTileView::~cPanelTileView()
{
	delete mMouseTimer;
} 

void cPanelTileView::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End.
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	SetSize(8,8,320,334);
	Center();
	
	////GUI Items Creation End
	mMouseTimer = new wxTimer( this, ID_EVENT_MOUSE_TIMER );
}

void cPanelTileView::OnClose(wxCloseEvent& /*event*/)
{
	Destroy();
}

void cPanelTileView::OnMouse( wxMouseEvent& event ) {

	if (event.LeftDown()) {

		mMouseTimer->Start( 50 );

	} else if(event.LeftUp()) {
		mMouseTimer->Stop();
	}
}

void cPanelTileView::OnMouseInputTimer( wxTimerEvent& event ) {
	wxPoint Point = wxGetMousePosition();
	wxMouseState state = wxGetMouseState();

	if (state.LeftIsDown()) {

		wxCoord MouseX = (Point.x - GetScreenPosition().x) / mScaleWidth;
		wxCoord MouseY = (Point.y - GetScreenPosition().y) / mScaleHeight;

		uint32 TileX = MouseX / 16;
		uint32 TileY = MouseY / 16;

		uint32 Tile = g_OFED.mMapTilePtr + (((TileY * g_OFED.mMapWidth) + TileX));

		if (g_OFED.mCursorTile > -1) {
			g_OFED.SetTile( TileX, TileY, g_OFED.mCursorTile );
			g_OFED.SetSelectedTile( Tile );
		}

		if (g_OFED.mCursorSprite > -1) {
			g_OFED.AddSprite( MouseX - 16, MouseY + 16);
		}

		g_OFED.DrawTiles();
		Refresh();
	}
}

/*
 * cTileViewPaint
 */
void cPanelTileView::cTileViewPaint(wxPaintEvent& event) {
	cSurface* Surface = g_OFED.GetSurface();

	size_t width = this->GetSize().GetWidth();
	size_t height = this->GetSize().GetHeight();

	wxBufferedPaintDC tileView( this, wxBUFFER_VIRTUAL_AREA );
	if (Surface) {
		
		mScaleWidth = (static_cast<double>(width) / static_cast<double>(Surface->GetWidth()));
		mScaleHeight = (static_cast<double>(height) / static_cast<double>(Surface->GetHeight()));

		Surface->draw();

		tileView.DrawBitmap( SDL_To_Bitmap( Surface, width, height ), 0, 0 );
	}
}
