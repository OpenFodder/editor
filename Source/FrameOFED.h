///-----------------------------------------------------------------
///
/// @file      FrameOFED.h
/// @author    Robbie
/// Created:   30/04/2016 17:37:38
/// @section   DESCRIPTION
///            cFrameOFED class declaration
///
///------------------------------------------------------------------

#ifndef __CFRAMEOFED_H__
#define __CFRAMEOFED_H__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/frame.h>
#else
	#include <wx/wxprec.h>
#endif

class cPanelTileView;
class cSurface;

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
#include <wx/menu.h>
#include <wx/filedlg.h>
////Header Include End

#include "DialogToolboxTiles.h"
#include "DialogToolboxSprites.h"
#include "DialogListSprites.h"

////Dialog Style Start
#undef cFrameOFED_STYLE
#define cFrameOFED_STYLE wxVSCROLL | wxHSCROLL | wxALWAYS_SHOW_SB | wxCAPTION | wxRESIZE_BORDER | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

class cFrameOFED : public wxFrame
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		cFrameOFED(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Open Fodder Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = cFrameOFED_STYLE);
		virtual ~cFrameOFED();
		void Mnuloadmap1002Click(wxCommandEvent& event);
		void Mnusavemap1003Click(wxCommandEvent& event);
		void Mnuquit1005Click( wxCommandEvent& event );
		void cFrameOFEDSize(wxSizeEvent& event);
		void Mnunewmap1006Click(wxCommandEvent& event);
		void cFrameOFEDScrollWin( wxScrollWinEvent& event);
		void cFrameOFEDScrollStepUp( wxScrollWinEvent& event );
		void cFrameOFEDScrollStepDown( wxScrollWinEvent& event );
		void OnMove( wxMoveEvent& event );
		void OnPaint(wxPaintEvent& event);

		void ReloadSprites();

		cDialogListSprites*		GetDialogListSprites() const {	return mDialogListSprites; }
		cPanelTileView*			GetPanelTileView() const { return mPanelTileView; }
		void MnuHutSoldier1012Click(wxCommandEvent& event);
		void MnuHutIndigenous1013Click(wxCommandEvent& event);
		void Mnuindigenouswithspear1014Click(wxCommandEvent& event);
		void MnuBunkerSoldier1015Click(wxCommandEvent& event);
		void MnuBunkerSoldierReinforced1016Click(wxCommandEvent& event);
		void MnuBarracksSoldier1017Click(wxCommandEvent& event);

		sTiles SetupBarracks( cSurface **pSurface );
		sTiles SetupHut( cSurface **pSurface );
		sTiles SetupBunker( cSurface **pSurface );

		void SetupCursorForDraw( cSurface *pSurface, sTiles& pTiles );

	private:
		//Do not add custom control declarations between
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxMenuBar *WxMenuBar1;
		wxFileDialog *WxSaveFileDialog1;
		wxFileDialog *WxOpenFileDialog1;
		////GUI Control Declaration End
		
		cPanelTileView*			mPanelTileView;
		cDialogToolboxTiles*	mDialogToolboxTiles;
		cDialogToolboxSprites*	mDialogToolboxSprites;
		cDialogListSprites*		mDialogListSprites;

	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_MNU_FILE_1001 = 1001,
			ID_MNU_NEWMAP_1006 = 1006,
			ID_MNU_LOADMAP_1002 = 1002,
			ID_MNU_SAVEMAP_1003 = 1003,
			ID_MNU_QUIT_1005 = 1005,
			ID_MNU_INSERT_1008 = 1008,
			ID_MNU_CIVILIANHUT_1009 = 1009,
			ID_MNU_SOLDIER_1012 = 1012,
			ID_MNU_INDIGENOUS_1013 = 1013,
			ID_MNU_INDIGENOUSWITHSPEAR_1014 = 1014,
			ID_MNU_BUILDING_1011 = 1011,
			ID_MNU_SOLDIER_1017 = 1017,
			ID_MNU_MENUITEM10_1010 = 1010,
			ID_MNU_SOLDIER_1015 = 1015,
			ID_MNU_SOLDIERREINFORCED_1016 = 1016,
			
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
		
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
};

#endif
