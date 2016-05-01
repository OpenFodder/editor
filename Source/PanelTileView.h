///-----------------------------------------------------------------
///
/// @file      TileView.h
/// @author    Robbie
/// Created:   30/04/2016 17:39:36
/// @section   DESCRIPTION
///            cPanelTileView class declaration
///
///------------------------------------------------------------------

#ifndef __cPanelTileView_H__
#define __cPanelTileView_H__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/dialog.h>
#else
	#include <wx/wxprec.h>
#endif

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
////Header Include End

#include <wx/dcbuffer.h>

////Dialog Style Start
#undef cPanelTileView_STYLE
#define cPanelTileView_STYLE wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

class cPanelTileView : public wxPanel
{
	private:
		DECLARE_EVENT_TABLE();
		
		double				mScaleWidth, mScaleHeight;
		wxTimer				*mMouseTimer;

	public:
		cPanelTileView(wxWindow *parent, wxWindowID id = 1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = cPanelTileView_STYLE);
		virtual ~cPanelTileView();
		void cTileViewPaint(wxPaintEvent& event);
		void OnMouse( wxMouseEvent& event );
		void OnMouseInputTimer( wxTimerEvent& event );

	private:
		//Do not add custom control declarations between 
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			////GUI Enum Control ID End
			ID_EVENT_MOUSE_TIMER,
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
	
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
};

#endif
