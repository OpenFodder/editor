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
#include "DialogCreateMap.h"
#include "DialogToolboxTiles.h"
#include "DialogToolboxSprites.h"

//----------------------------------------------------------------------------
// cFrameOFED
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(cFrameOFED,wxFrame)
	////Manual Code Start
	EVT_MOVE( cFrameOFED::OnMove )
	////Manual Code End
	
	EVT_CLOSE(cFrameOFED::OnClose)
	EVT_SIZE(cFrameOFED::cFrameOFEDSize)
	EVT_PAINT(cFrameOFED::OnPaint)
	EVT_SCROLLWIN_TOP(cFrameOFED::cFrameOFEDScrollStepUp)
	EVT_SCROLLWIN_BOTTOM(cFrameOFED::cFrameOFEDScrollStepDown)
	EVT_SCROLLWIN_LINEUP(cFrameOFED::cFrameOFEDScrollStepUp)
	EVT_SCROLLWIN_LINEDOWN(cFrameOFED::cFrameOFEDScrollStepDown)
	EVT_SCROLLWIN_THUMBTRACK(cFrameOFED::cFrameOFEDScrollWin)
	EVT_MENU(ID_MNU_NEWMAP_1006, cFrameOFED::Mnunewmap1006Click)
	EVT_MENU(ID_MNU_LOADMAP_1002, cFrameOFED::Mnuloadmap1002Click)
	EVT_MENU(ID_MNU_SAVEMAP_1003, cFrameOFED::Mnusavemap1003Click)
	EVT_MENU(ID_MNU_QUIT_1005, cFrameOFED::Mnuquit1005Click)
END_EVENT_TABLE()
////Event Table End

cFrameOFED::cFrameOFED(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	mDialogToolboxTiles = 0;
	mDialogToolboxSprites = 0;
	mDialogListSprites = 0;

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

	WxSaveFileDialog1 =  new wxFileDialog(this, _("Choose a file"), _(""), _(""), _("*.map"), wxFD_SAVE);

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

	mDialogToolboxTiles = new cDialogToolboxTiles( this );
	mDialogToolboxSprites = new cDialogToolboxSprites( this );
	mDialogListSprites = new cDialogListSprites( this );

	mPanelTileView = new cPanelTileView( this );
	mPanelTileView->SetPosition( wxPoint( 12, 10 ) );
	mPanelTileView->SetSize( GetSize().GetWidth() - 40, GetSize().GetHeight() - 126 );

	SetMinSize( wxSize( 800, 600 ) );
	SetScrollbar( wxHORIZONTAL, 0, 2, 0 );
	SetScrollbar( wxVERTICAL, 0, 2, 0 );

	mDialogToolboxSprites->Show();
	mDialogToolboxTiles->Show();
	mDialogListSprites->Show();

}

void cFrameOFED::OnClose(wxCloseEvent& event)
{
	Destroy();
}

void cFrameOFED::OnMove( wxMoveEvent& event ) {

	if (mDialogToolboxTiles) {
		wxPoint a( GetPosition().x + GetSize().GetWidth(), GetPosition().y );
		wxPoint b( GetPosition().x, GetPosition().y + GetSize().GetHeight() );
		wxPoint c( GetPosition().x - mDialogListSprites->GetSize().GetWidth(), GetPosition().y );

		mDialogToolboxTiles->SetPosition( a );
		mDialogToolboxSprites->SetPosition( b );
		mDialogListSprites->SetPosition( c );
	}
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
	mDialogToolboxSprites->ReloadSprites();

	SetScrollbar( wxHORIZONTAL, 0, 2, (g_OFED.mMapWidth - g_OFED.mCameraTilesX) + 2);
	SetScrollbar( wxVERTICAL, 0, 2, (g_OFED.mMapHeight - g_OFED.mCameraTilesY) + 2);
	Refresh();
}

void cFrameOFED::ReloadSprites() {

	mDialogToolboxSprites->ReloadSprites();
}

/*
 * Mnusavemap1003Click
 */
void cFrameOFED::Mnusavemap1003Click(wxCommandEvent& event) {
	WxSaveFileDialog1->SetTitle( wxT( "Save Map" ) );
	WxSaveFileDialog1->ShowModal();

	std::string filename = std::string( WxSaveFileDialog1->GetPath().mb_str() );

	if (!filename.size())
		return;

	g_OFED.SaveMap( filename );
	
}

void cFrameOFED::Mnuquit1005Click( wxCommandEvent& event ) {

	exit( 0 );
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

	if (mDialogToolboxTiles) {
		wxPoint a( GetPosition().x + GetSize().GetWidth(), GetPosition().y );
		wxPoint b( GetPosition().x, GetPosition().y + GetSize().GetHeight() );
		wxPoint c( GetPosition().x - mDialogListSprites->GetSize().GetWidth(), GetPosition().y );

		mDialogToolboxTiles->SetPosition( a );
		mDialogToolboxSprites->SetPosition( b );
		mDialogListSprites->SetPosition( c );
	}

	Refresh();
}

/*
 * Mnunewmap1006Click
 */
void cFrameOFED::Mnunewmap1006Click(wxCommandEvent& event)
{
	cDialogCreateMap* CreateMap = new cDialogCreateMap( this );
	CreateMap->Show();
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

/*
 * cFrameOFEDPaint
 */
void cFrameOFED::OnPaint(wxPaintEvent& event)
{
	if(mDialogToolboxTiles)
		mDialogToolboxTiles->Refresh();

	if (mDialogToolboxSprites)
		mDialogToolboxSprites->Refresh();
}
