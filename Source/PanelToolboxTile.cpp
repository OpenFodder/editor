///-----------------------------------------------------------------
///
/// @file      PanelToolboxTile.cpp
/// @author    Robbie
/// Created:   1/05/2016 14:13:14
/// @section   DESCRIPTION
///            cPanelToolboxTile class implementation
///
///------------------------------------------------------------------

#include "PanelToolboxTile.h"

//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

#include "stdafx.hpp"

//----------------------------------------------------------------------------
// cPanelToolboxTile
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(cPanelToolboxTile,wxPanel)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(cPanelToolboxTile::OnClose)
	EVT_PAINT(cPanelToolboxTile::cPanelToolboxTilePaint)
	EVT_MOUSE_EVENTS(cPanelToolboxTile::OnMouse)
END_EVENT_TABLE()
////Event Table End

cPanelToolboxTile::cPanelToolboxTile( wxWindow *parent, wxWindowID id, const wxPoint &position, const wxSize& size, long style )
: wxPanel( parent, id, position, size, wxFRAME_NO_TASKBAR | wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX) )
{
	mCursorSurface = new cSurface( 16, 16 );

	CreateGUIControls();
}

cPanelToolboxTile::~cPanelToolboxTile()
{
} 

void cPanelToolboxTile::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End.
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	SetSize(8,8,800,640);
	Center();
	
	////GUI Items Creation End
}

void cPanelToolboxTile::OnClose(wxCloseEvent& /*event*/)
{
	Destroy();
}

void cPanelToolboxTile::OnMouse( wxMouseEvent& event ) {

	wxCoord MouseX = event.GetX() / mScaleWidth;
	wxCoord MouseY = event.GetY() / mScaleHeight;

	uint32 TileX = MouseX / 16;
	uint32 TileY = MouseY / 16;

	uint32 Tile = (25 * TileY) + TileX;

	if (event.LeftDown()) {

		g_OFED.SetCursorTile( Tile );

		g_OFED.DrawTile( mCursorSurface, Tile, 0, 0 );
		g_OFED.LoadPalette( mCursorSurface );
		mCursorSurface->draw();

		wxBitmap Cursor = SDL_To_Bitmap( mCursorSurface->GetSurface() );
		mCursor = wxCursor( Cursor.ConvertToImage() );
		this->GetParent()->SetCursor( mCursor );
		this->GetParent()->GetParent()->SetCursor( mCursor );
	}
}

/*
 * cPanelToolboxTilePaint
 */
void cPanelToolboxTile::cPanelToolboxTilePaint(wxPaintEvent& event)
{
	cSurface* Surface = new cSurface( 400, 320 );
	size_t width = this->GetSize().GetWidth();
	size_t height = this->GetSize().GetHeight();

	int32 X = 0, Y = 0;

	for (int16 TileNumber = 0; TileNumber < 480; ++TileNumber) {

		g_OFED.DrawTile( Surface, TileNumber, X, Y );

		++X;
		if (X >= 25) {
			X = 0;
			++Y;
		}
	}

	g_OFED.LoadPalette( Surface );

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
		mScaleWidth = (static_cast<double>(width) / static_cast<double>(Surface->GetWidth()));
		mScaleHeight = (static_cast<double>(height) / static_cast<double>(Surface->GetHeight()));

		tileView.DrawBitmap( SDL_To_Bitmap( Dest ), 0, 0 );

		SDL_FreeSurface( Dest );
	}

	delete Surface;
}
