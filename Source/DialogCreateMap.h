///-----------------------------------------------------------------
///
/// @file      DialogCreateMap.h
/// @author    Robbie
/// Created:   1/05/2016 08:48:33
/// @section   DESCRIPTION
///            cDialogCreateMap class declaration
///
///------------------------------------------------------------------

#ifndef __CDIALOGCREATEMAP_H__
#define __CDIALOGCREATEMAP_H__

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
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/button.h>
////Header Include End

////Dialog Style Start
#undef cDialogCreateMap_STYLE
#define cDialogCreateMap_STYLE wxCAPTION | wxSYSTEM_MENU | wxDIALOG_NO_PARENT | wxCLOSE_BOX
////Dialog Style End

class cDialogCreateMap : public wxDialog
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		cDialogCreateMap(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Create Map"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = cDialogCreateMap_STYLE);
		virtual ~cDialogCreateMap();
		void ButtonCreateMap(wxCommandEvent& event);

	private:
		//Do not add custom control declarations between 
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxTextCtrl *WxEdit2;
		wxTextCtrl *WxEdit1;
		wxComboBox *WxComboBox1;
		wxButton *WxButton1;
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_WXEDIT2 = 1006,
			ID_WXEDIT1 = 1005,
			ID_WXCOMBOBOX1 = 1004,
			ID_WXBUTTON1 = 1002,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
	
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
};

#endif
