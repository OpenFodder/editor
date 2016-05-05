///-----------------------------------------------------------------
///
/// @file      PanelToolboxSprite.cpp
/// @author    Robbie
/// Created:   1/05/2016 20:51:58
/// @section   DESCRIPTION
///            cPanelToolboxSprite class implementation
///
///------------------------------------------------------------------

#include "PanelToolboxSprite.h"

//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

#include "stdafx.hpp"
#include "FrameOFED.h"
#include "PanelTileView.h"

//----------------------------------------------------------------------------
// cPanelToolboxSprite
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(cPanelToolboxSprite,wxPanel)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(cPanelToolboxSprite::OnClose)
	EVT_PAINT(cPanelToolboxSprite::OnPaint)
	EVT_MOUSE_EVENTS(cPanelToolboxSprite::OnMouse)
END_EVENT_TABLE()
////Event Table End

cPanelToolboxSprite::cPanelToolboxSprite( wxWindow *parent, wxWindowID id, const wxPoint &position, const wxSize& size, long style)
: wxPanel( parent, id, position, size, wxFRAME_NO_TASKBAR | wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX) )
{
	mFinalSurface = 0;
	CreateGUIControls();
}

cPanelToolboxSprite::~cPanelToolboxSprite()
{
} 

void cPanelToolboxSprite::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End.
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	SetSize(8,8,798,350);
	Center();
	
	////GUI Items Creation End

}

void cPanelToolboxSprite::OnClose(wxCloseEvent& /*event*/)
{
	Destroy();
}

void cPanelToolboxSprite::ReloadSprites() {
	delete mFinalSurface;
	mFinalSurface = new cSurface( 20 * 17, 9 * 17 );

	size_t X = 0, Y = 0;
	size_t BigY = 0;

	for (size_t SpriteID = 0; SpriteID < 111; ++SpriteID) {
		int32 AnimID = g_SpriteAnim[SpriteID];
		if (AnimID < 0)
			continue;

		cSurface *Surface = new cSurface( off_32C0C[AnimID][0].mColCount, off_32C0C[AnimID][0].mRowCount );

		g_OFED.LoadPalette( Surface );
		g_OFED.DrawSprite( Surface, AnimID, 0, 0, false );

		if (X + off_32C0C[AnimID][0].mColCount > ( 20 * 17 )) {
			X = 0;
			Y += BigY;
			BigY = 0;
		}

		mSpriteRanges.push_back( sRange( X, Y, off_32C0C[AnimID][0].mColCount, off_32C0C[AnimID][0].mRowCount, SpriteID ) );

		SDL_To_SDL( Surface, mFinalSurface, X, Y, off_32C0C[AnimID][0].mColCount, off_32C0C[AnimID][0].mRowCount );

		if (off_32C0C[AnimID][0].mRowCount > BigY)
			BigY = off_32C0C[AnimID][0].mRowCount;

		X += off_32C0C[AnimID][0].mColCount;
		if (X >= (20 * 17)) {
			X = 0;
			Y += BigY;
			BigY = 0;
		}

		delete Surface;
	}

	Refresh();
}

void cPanelToolboxSprite::OnMouse( wxMouseEvent& event ) {

	wxCoord MouseX = event.GetX() / mScaleWidth;
	wxCoord MouseY = event.GetY() / mScaleHeight;

	if (event.LeftDown()) {

		for ( std::vector< sRange >::iterator RangeIT = mSpriteRanges.begin(); RangeIT != mSpriteRanges.end(); ++RangeIT) {

			if (MouseX >= RangeIT->mX && MouseX <= RangeIT->mX + RangeIT->mWidth) {
				if (MouseY >= RangeIT->mY && MouseY <= RangeIT->mY + RangeIT->mHeight) {

					int32 AnimID = g_SpriteAnim[RangeIT->mSpriteID];
					cSurface *Surface = new cSurface( off_32C0C[AnimID][0].mColCount, off_32C0C[AnimID][0].mRowCount );

					g_OFED.SetCursorSprite( RangeIT->mSpriteID );
					g_OFED.LoadPalette( Surface );
					g_OFED.DrawSprite( Surface, AnimID, 0, 0, false );

					Surface->draw();
					wxBitmap Cursor = SDL_To_Bitmap( Surface, off_32C0C[AnimID][0].mColCount, off_32C0C[AnimID][0].mRowCount );
					wxImage image = Cursor.ConvertToImage();
					image.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_X, 1);
					image.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 1);
					mCursor = wxCursor( image );

					this->GetParent()->GetParent()->SetCursor( mCursor );
				}
			}
		}
	}
}

void cPanelToolboxSprite::OnPaint( wxPaintEvent& event ) {
	size_t width = this->GetSize().GetWidth();
	size_t height = this->GetSize().GetHeight();


	wxBufferedPaintDC tileView( this );
	if (mFinalSurface) {

		mScaleWidth = (static_cast<double>(width) / static_cast<double>(mFinalSurface->GetWidth()));
		mScaleHeight = (static_cast<double>(height) / static_cast<double>(mFinalSurface->GetHeight()));

		tileView.DrawBitmap( SDL_To_Bitmap( mFinalSurface, width, height ), 0, 0 );
	}

}
