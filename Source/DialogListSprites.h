///-----------------------------------------------------------------
///
/// @file      DialogListSprites.h
/// @author    Robbie
/// Created:   4/05/2016 20:08:32
/// @section   DESCRIPTION
///            cDialogListSprites class declaration
///
///------------------------------------------------------------------

#ifndef __CDIALOGLISTSPRITES_H__
#define __CDIALOGLISTSPRITES_H__

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
#include <wx/menu.h>
#include <wx/listctrl.h>
////Header Include End

////Dialog Style Start
#undef cDialogListSprites_STYLE
#define cDialogListSprites_STYLE wxCAPTION | wxSYSTEM_MENU | wxDIALOG_NO_PARENT | wxMINIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

struct sSpriteDef;

class cDialogListSprites : public wxDialog
{
	private:
		DECLARE_EVENT_TABLE();
		
		int			mSelectedListItemID;

	public:
		cDialogListSprites(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Sprite List"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = cDialogListSprites_STYLE);
		virtual ~cDialogListSprites();
		void		AddSprite( int pNumber, const sSpriteDef& pSpriteDef );
		void		AddSprites();
		void WxListCtrl1RightClick(wxListEvent& event);
		void WxListCtrl1ItemActivated(wxListEvent& event);
		void Mnuremovesprite1003Click(wxCommandEvent& event);

	private:
		//Do not add custom control declarations between 
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxMenu *WxPopupMenu1;
		wxListCtrl *WxListCtrl1;
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_MNU_REMOVESPRITE_1003 = 1003,
			
			ID_WXLISTCTRL1 = 1002,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
	
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
};

#endif
