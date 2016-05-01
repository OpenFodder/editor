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

cPanelToolboxSprite::cPanelToolboxSprite(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxDialog(parent, id, title, position, size, style)
{
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

	SetTitle(_("Untitled7"));
	SetIcon(wxNullIcon);
	SetSize(8,8,798,200);
	Center();
	
	////GUI Items Creation End
}

void cPanelToolboxSprite::OnClose(wxCloseEvent& /*event*/)
{
	Destroy();
}

void cPanelToolboxSprite::OnPaint( wxPaintEvent& event ) {

}

void cPanelToolboxSprite::OnMouse( wxMouseEvent& event ) {

}
