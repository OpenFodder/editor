///-----------------------------------------------------------------
///
/// @file      DialogToolboxTiles.cpp
/// @author    Robbie
/// Created:   1/05/2016 14:10:43
/// @section   DESCRIPTION
///            cDialogToolboxTiles class implementation
///
///------------------------------------------------------------------

#include "DialogToolboxTiles.h"

//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End


#include "PanelToolboxTile.h"

//----------------------------------------------------------------------------
// cDialogToolboxTiles
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(cDialogToolboxTiles,wxDialog)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(cDialogToolboxTiles::OnClose)
	EVT_PAINT(cDialogToolboxTiles::OnPaint)
END_EVENT_TABLE()
////Event Table End

cDialogToolboxTiles::cDialogToolboxTiles(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxDialog(parent, id, title, position, size, style)
{
	CreateGUIControls();
}

cDialogToolboxTiles::~cDialogToolboxTiles()
{
} 

void cDialogToolboxTiles::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End.
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	SetTitle(_("Tile Toolbox"));
	SetIcon(wxNullIcon);
	SetSize(8,8,800,640);
	Center();
	
	////GUI Items Creation End

	mToolboxTile = new cPanelToolboxTile( this );
}

void cDialogToolboxTiles::OnClose(wxCloseEvent& /*event*/)
{
	Destroy();
}

/*
 * cDialogToolboxTilesPaint
 */
void cDialogToolboxTiles::OnPaint(wxPaintEvent& event)
{
	mToolboxTile->Refresh();
}
