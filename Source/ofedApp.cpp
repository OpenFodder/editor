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

	g_FrameOFED = new cFrameOFED(NULL);


    SetTopWindow( g_FrameOFED );
	g_FrameOFED->Show();

	g_OFED.CreateMap( eTileTypes::eTileTypes_Jungle, eTileSub_0, 20, 15 );
	g_FrameOFED->ReloadSprites();

    return true;
}
 
int cFrameOFEDApp::OnExit()
{
	return 0;
}

void SDL_To_SDL( cSurface *pSource, cSurface *pDest, int pDestX, int pDestY, int pDestWidth, int pDestHeight ) {
	pSource->draw();

	SDL_Rect SrcRect, DestRect;
	SrcRect.x = 0;
	SrcRect.y = 0;
	SrcRect.w = pSource->GetWidth();
	SrcRect.h = pSource->GetHeight();
	DestRect.x = pDestX;
	DestRect.y = pDestY;
	DestRect.w = pDestWidth;
	DestRect.h = pDestHeight;

	SDL_BlitScaled( pSource->GetSurface(), &SrcRect, pDest->GetSurface(), &DestRect );
}

wxBitmap SDL_To_Bitmap( cSurface* pSurface, int pDestWidth, int pDestHeight ) {

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
