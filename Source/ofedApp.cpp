//---------------------------------------------------------------------------
//
// Name:        ofedApp.cpp
// Author:      Robbie
// Created:     30/04/2016 17:37:38
// Description: 
//
//---------------------------------------------------------------------------

#include "stdafx.hpp"
#include "ofedApp.h"
#include "FrameOFED.h"

IMPLEMENT_APP(cFrameOFEDApp)

bool cFrameOFEDApp::OnInit()
{
	new cOFED();

    cFrameOFED* frame = new cFrameOFED(NULL);
    SetTopWindow(frame);
    frame->Show();
    return true;
}
 
int cFrameOFEDApp::OnExit()
{
	return 0;
}

wxBitmap SDL_To_Bitmap( cSurface* pSurface, int pDestWidth, int pDestHeight ) {
	pSurface->draw(); 
	
	SDL_Rect SrcRect, DestRect;
	SrcRect.x = 0;
	SrcRect.y = 0;
	SrcRect.w = pSurface->GetWidth();
	SrcRect.h = pSurface->GetHeight();
	DestRect.x = 0;
	DestRect.y = 0;
	DestRect.w = pDestWidth;
	DestRect.h = pDestHeight;

	SDL_Surface *Dest = SDL_CreateRGBSurface( 0, pDestWidth, pDestHeight, 32, 0xFF << 16, 0xFF << 8, 0xFF, 0 );
	SDL_BlitScaled( pSurface->GetSurface(), &SrcRect, Dest, &DestRect );

#ifdef WIN32
	wxBitmap Bitmap( (const char*)Dest->pixels, Dest->w, Dest->h, Dest->format->BitsPerPixel );
	SDL_FreeSurface( Dest );

	return Bitmap;
#else
	wxImage image( surface->w, surface->h, true );

	for (int y = 0; y < pSurface->h; y++) {
		for (int x = 0; x < pSurface->w; x++) {
			Uint32 color = *((Uint32*)((unsigned char*)pSurface->pixels + y * pSurface->pitch + x * pSurface->format->BytesPerPixel));
			Uint8 r, g, b;
			SDL_GetRGB( color, pSurface->format, &r, &g, &b );
			image.SetRGB( x, y, r, g, b );
		}
	}

	return wxBitmap( image );
#endif
}
