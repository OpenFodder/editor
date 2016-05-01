//---------------------------------------------------------------------------
//
// Name:        ofedApp.h
// Author:      Robbie
// Created:     30/04/2016 17:37:38
// Description: 
//
//---------------------------------------------------------------------------

#ifndef __CFRAMEOFEDApp_h__
#define __CFRAMEOFEDApp_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#else
	#include <wx/wxprec.h>
#endif

class cFrameOFEDApp : public wxApp
{
	public:
		bool OnInit();
		int OnExit();
};

class cSurface;
wxBitmap SDL_To_Bitmap( cSurface* pSurface, int pDestWidth, int pDestHeight );

#endif
