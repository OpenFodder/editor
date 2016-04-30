///-----------------------------------------------------------------
///
/// @file      FrameOFED.cpp
/// @author    Robbie
/// Created:   30/04/2016 17:37:38
/// @section   DESCRIPTION
///            cFrameOFED class implementation
///
///------------------------------------------------------------------
#include "PanelTileView.h"
#include "FrameOFED.h"

//Do not add custom headers between
//Header Include Start and Header Include End
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

#include "stdafx.hpp"

//----------------------------------------------------------------------------
// cFrameOFED
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(cFrameOFED,wxFrame)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(cFrameOFED::OnClose)
	EVT_SIZE(cFrameOFED::cFrameOFEDSize)
	
	EVT_SCROLLWIN_PAGEUP(cFrameOFED::cFrameOFEDScrollStepUp)
	EVT_SCROLLWIN_PAGEDOWN(cFrameOFED::cFrameOFEDScrollStepDown)
	EVT_SCROLLWIN_LINEUP(cFrameOFED::cFrameOFEDScrollStepUp)
	EVT_SCROLLWIN_LINEDOWN(cFrameOFED::cFrameOFEDScrollStepDown)
	EVT_SCROLLWIN_THUMBTRACK(cFrameOFED::cFrameOFEDScrollWin)
	//EVT_SCROLLWIN( cFrameOFED::cFrameOFEDScrollWin )
	EVT_MENU(ID_MNU_NEWMAP_1006, cFrameOFED::Mnunewmap1006Click0)
	EVT_MENU(ID_MNU_LOADMAP_1002, cFrameOFED::Mnuloadmap1002Click)
	EVT_MENU(ID_MNU_SAVEMAP_1003, cFrameOFED::Mnusavemap1003Click)
	EVT_MENU(ID_MNU_QUIT_1005, cFrameOFED::Mnuquit1005Click)
END_EVENT_TABLE()
////Event Table End

cFrameOFED::cFrameOFED(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{

	mPanelTileView = 0;

	CreateGUIControls();
}

cFrameOFED::~cFrameOFED()
{
}

void cFrameOFED::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	WxOpenFileDialog1 =  new wxFileDialog(this, _("Choose a file"), _(""), _(""), _("*.map"), wxFD_OPEN);

	WxMenuBar1 = new wxMenuBar();
	wxMenu *ID_MNU_FILE_1001_Mnu_Obj = new wxMenu();
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_NEWMAP_1006, _("&New Map"), _(""), wxITEM_NORMAL);
	ID_MNU_FILE_1001_Mnu_Obj->AppendSeparator();
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_LOADMAP_1002, _("&Load Map"), _(""), wxITEM_NORMAL);
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_SAVEMAP_1003, _("&Save Map"), _(""), wxITEM_NORMAL);
	ID_MNU_FILE_1001_Mnu_Obj->AppendSeparator();
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_QUIT_1005, _("&Quit"), _(""), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_MNU_FILE_1001_Mnu_Obj, _("&File"));
	SetMenuBar(WxMenuBar1);

	SetTitle(_("Open Fodder Editor"));
	SetIcon(wxNullIcon);
	SetSize(8,8,800,600);
	Center();
	
	////GUI Items Creation End

	mPanelTileView = new cPanelTileView( this );
	mPanelTileView->SetPosition( wxPoint( 12, 10 ) );
	mPanelTileView->SetSize( GetSize().GetWidth() - 40, GetSize().GetHeight() - 126 );

	SetMinSize( wxSize( 800, 600 ) );
	SetScrollbar( wxHORIZONTAL, 0, 2, 0 );
	SetScrollbar( wxVERTICAL, 0, 2, 0 );
}

void cFrameOFED::OnClose(wxCloseEvent& event)
{
	Destroy();
}

/*
 * Mnuloadmap1002Click
 */
void cFrameOFED::Mnuloadmap1002Click(wxCommandEvent& event) {
	WxOpenFileDialog1->SetTitle( wxT( "Load Map" ) );
	WxOpenFileDialog1->ShowModal();

	std::string filename = std::string( WxOpenFileDialog1->GetPath().mb_str() );

	if (!filename.size())
		return;

	g_OFED.LoadMap( filename );

	SetScrollbar( wxHORIZONTAL, 0, 2, (g_OFED.mMapWidth - g_OFED.mCameraTilesX) + 2);
	SetScrollbar( wxVERTICAL, 0, 2, (g_OFED.mMapHeight - g_OFED.mCameraTilesY) + 2);
	Refresh();
}

/*
 * Mnusavemap1003Click
 */
void cFrameOFED::Mnusavemap1003Click(wxCommandEvent& event) {
	// insert your code here
}

void cFrameOFED::Mnuquit1005Click( wxCommandEvent& event ) {

}

/*
 * cFrameOFEDSize
 */
void cFrameOFED::cFrameOFEDSize(wxSizeEvent& event)
{
	size_t width = event.GetSize().GetWidth();
	size_t height = event.GetSize().GetHeight();

	if(mPanelTileView)
		mPanelTileView->SetSize( width - 50, height - 90 );

	Refresh();
}

/*
 * Mnunewmap1006Click
 */
void cFrameOFED::Mnunewmap1006Click(wxCommandEvent& event)
{
	// insert your code here
}

/*
 * Mnunewmap1006Click0
 */
void cFrameOFED::Mnunewmap1006Click0(wxCommandEvent& event)
{
	// insert your code here
}

/*
 * cFrameOFEDScrollWin
 */
void cFrameOFED::cFrameOFEDScrollWin(wxScrollWinEvent& event)
{
	if (event.GetOrientation() == wxHORIZONTAL)
		g_OFED.SetMapX( event.GetPosition() );
	else
		g_OFED.SetMapY( event.GetPosition() );

	SetScrollPos( event.GetOrientation(), event.GetPosition() );
	Refresh();
}

void cFrameOFED::cFrameOFEDScrollStepUp( wxScrollWinEvent& event ) {
	if (event.GetOrientation() == wxHORIZONTAL) {
		g_OFED.SetMapX( g_OFED.mMapX - 1 );
		SetScrollPos( event.GetOrientation(), g_OFED.mMapX );
	} 
	else {
		g_OFED.SetMapY( g_OFED.mMapY - 1 );
		SetScrollPos( event.GetOrientation(), g_OFED.mMapY );
	}

	Refresh();
}

void cFrameOFED::cFrameOFEDScrollStepDown( wxScrollWinEvent& event ) {
	if (event.GetOrientation() == wxHORIZONTAL) {
		g_OFED.SetMapX( g_OFED.mMapX + 1 );
		SetScrollPos( event.GetOrientation(), g_OFED.mMapX );
	} 
	else {
		g_OFED.SetMapY( g_OFED.mMapY + 1 );
		SetScrollPos( event.GetOrientation(), g_OFED.mMapY );
	}
	
	Refresh();
}
