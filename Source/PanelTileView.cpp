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
	////Manual Code End
	
	EVT_CLOSE(cPanelTileView::OnClose)
	EVT_PAINT(cPanelTileView::cTileViewPaint)
END_EVENT_TABLE()
////Event Table End

cPanelTileView::cPanelTileView(wxWindow *parent, wxWindowID id, const wxPoint &position, const wxSize& size, long style)
: wxPanel(parent, id, position, size, wxFRAME_NO_TASKBAR | wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX) )
{
	CreateGUIControls();
}

cPanelTileView::~cPanelTileView()
{
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
}

void cPanelTileView::OnClose(wxCloseEvent& /*event*/)
{
	Destroy();
}

/*
 * cTileViewPaint
 */
void cPanelTileView::cTileViewPaint(wxPaintEvent& event)
{
	cSurface* Surface = g_OFED.GetSurface();

	size_t width = this->GetSize().GetWidth();
	size_t height = this->GetSize().GetHeight();


	wxBufferedPaintDC tileView( this );
	if (Surface) {
		SDL_Rect SrcRect, DestRect;
		SrcRect.x = 0;
		SrcRect.y = 0;
		SrcRect.w = Surface->GetWidth();
		SrcRect.h = Surface->GetHeight();
		DestRect.x = 0;
		DestRect.y = 0;
		DestRect.w = width;
		DestRect.h = height;

		SDL_Surface *Dest = SDL_CreateRGBSurface( 0, (int)width, (int)height, 32, 0xFF << 16, 0xFF << 8, 0xFF, 0 );

		Surface->draw();
		SDL_BlitScaled( Surface->GetSurface(), &SrcRect, Dest, &DestRect );
		tileView.DrawBitmap( SDL_To_Bitmap( Dest ), 0, 0 );

		SDL_FreeSurface( Dest );
	}
}
