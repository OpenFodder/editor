///-----------------------------------------------------------------
///
/// @file      DialogToolboxSprites.cpp
/// @author    Robbie
/// Created:   1/05/2016 20:50:52
/// @section   DESCRIPTION
///            cDialogToolboxSprites class implementation
///
///------------------------------------------------------------------
#include "stdafx.hpp"
#include "DialogToolboxSprites.h"
#include "PanelToolboxSprite.h"

//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// cDialogToolboxSprites
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(cDialogToolboxSprites,wxDialog)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(cDialogToolboxSprites::OnClose)
	EVT_PAINT(cDialogToolboxSprites::OnPaint)
END_EVENT_TABLE()
////Event Table End

cDialogToolboxSprites::cDialogToolboxSprites(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxDialog(parent, id, title, position, size, style)
{
	mSpritePanel = 0;
	CreateGUIControls();
}

cDialogToolboxSprites::~cDialogToolboxSprites()
{
} 

void cDialogToolboxSprites::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End.
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	SetTitle(_("Toolbox Sprites"));
	SetIcon(wxNullIcon);
	SetSize(8,8,798,350);
	Center();
	
	////GUI Items Creation End

	mSpritePanel = new cPanelToolboxSprite( this );
	
}

void cDialogToolboxSprites::OnClose(wxCloseEvent& /*event*/) {

	Destroy();
}

void cDialogToolboxSprites::OnPaint( wxPaintEvent & event ) {

	if (mSpritePanel)
		mSpritePanel->Refresh();
}

void cDialogToolboxSprites::ReloadSprites() {

	if(mSpritePanel)
		mSpritePanel->ReloadSprites();
}

/*
 * cDialogToolboxSpritesSize
 */
void cDialogToolboxSprites::OnSize(wxSizeEvent& event)
{
	// insert your code here
}
