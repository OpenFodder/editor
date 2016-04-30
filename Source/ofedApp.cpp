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

wxBitmap SDL_To_Bitmap( SDL_Surface* pSurface ) {

#ifdef WIN32
	return wxBitmap( (const char*)pSurface->pixels, pSurface->w, pSurface->h, pSurface->format->BitsPerPixel );
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
