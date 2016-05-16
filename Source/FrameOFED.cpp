///-----------------------------------------------------------------
///
/// @file      FrameOFED.cpp
/// @author    Robbie
/// Created:   30/04/2016 17:37:38
/// @section   DESCRIPTION
///            cFrameOFED class implementation
///
///------------------------------------------------------------------
#include "stdafx.hpp"
#include "PanelTileView.h"
#include "FrameOFED.h"

//Do not add custom headers between
//Header Include Start and Header Include End
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End


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
	EVT_MENU(ID_MNU_SOLDIER_1012, cFrameOFED::MnuHutSoldier1012Click)
	EVT_MENU(ID_MNU_INDIGENOUS_1013, cFrameOFED::MnuHutIndigenous1013Click)
	EVT_MENU(ID_MNU_INDIGENOUSWITHSPEAR_1014, cFrameOFED::Mnuindigenouswithspear1014Click)
	EVT_MENU(ID_MNU_SOLDIER_1017, cFrameOFED::MnuBarracksSoldier1017Click)
	EVT_MENU(ID_MNU_SOLDIER_1015, cFrameOFED::MnuBunkerSoldier1015Click)
	EVT_MENU(ID_MNU_SOLDIERREINFORCED_1016, cFrameOFED::MnuBunkerSoldierReinforced1016Click)
	EVT_MENU(ID_MNU_CLIFF_1018, cFrameOFED::Mnucliff1018Click)
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

	WxMenuBar1 = new wxMenuBar();
	wxMenu *ID_MNU_FILE_1001_Mnu_Obj = new wxMenu();
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_NEWMAP_1006, _("&New Map"), _(""), wxITEM_NORMAL);
	ID_MNU_FILE_1001_Mnu_Obj->AppendSeparator();
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_LOADMAP_1002, _("&Load Map"), _(""), wxITEM_NORMAL);
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_SAVEMAP_1003, _("&Save Map"), _(""), wxITEM_NORMAL);
	ID_MNU_FILE_1001_Mnu_Obj->AppendSeparator();
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_QUIT_1005, _("&Quit"), _(""), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_MNU_FILE_1001_Mnu_Obj, _("&File"));
	
	wxMenu *ID_MNU_INSERT_1008_Mnu_Obj = new wxMenu();
	
	wxMenu *ID_MNU_CIVILIANHUT_1009_Mnu_Obj = new wxMenu();
	ID_MNU_CIVILIANHUT_1009_Mnu_Obj->Append(ID_MNU_SOLDIER_1012, _("Soldier"), _(""), wxITEM_NORMAL);
	ID_MNU_CIVILIANHUT_1009_Mnu_Obj->Append(ID_MNU_INDIGENOUS_1013, _("Indigenous"), _(""), wxITEM_NORMAL);
	ID_MNU_CIVILIANHUT_1009_Mnu_Obj->Append(ID_MNU_INDIGENOUSWITHSPEAR_1014, _("Indigenous with spear"), _(""), wxITEM_NORMAL);
	ID_MNU_INSERT_1008_Mnu_Obj->Append(ID_MNU_CIVILIANHUT_1009, _("Hut"), ID_MNU_CIVILIANHUT_1009_Mnu_Obj);
	
	wxMenu *ID_MNU_BUILDING_1011_Mnu_Obj = new wxMenu();
	ID_MNU_BUILDING_1011_Mnu_Obj->Append(ID_MNU_SOLDIER_1017, _("Soldier"), _(""), wxITEM_NORMAL);
	ID_MNU_INSERT_1008_Mnu_Obj->Append(ID_MNU_BUILDING_1011, _("Barracks"), ID_MNU_BUILDING_1011_Mnu_Obj);
	
	wxMenu *ID_MNU_MENUITEM10_1010_Mnu_Obj = new wxMenu();
	ID_MNU_MENUITEM10_1010_Mnu_Obj->Append(ID_MNU_SOLDIER_1015, _("Soldier"), _(""), wxITEM_NORMAL);
	ID_MNU_MENUITEM10_1010_Mnu_Obj->Append(ID_MNU_SOLDIERREINFORCED_1016, _("Soldier Reinforced"), _(""), wxITEM_NORMAL);
	ID_MNU_INSERT_1008_Mnu_Obj->Append(ID_MNU_MENUITEM10_1010, _("Bunker"), ID_MNU_MENUITEM10_1010_Mnu_Obj);
	ID_MNU_INSERT_1008_Mnu_Obj->Append(ID_MNU_CLIFF_1018, _("Cliff"), _(""), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_MNU_INSERT_1008_Mnu_Obj, _("Insert"));
	SetMenuBar(WxMenuBar1);

	WxOpenFileDialog1 =  new wxFileDialog(this, _("Choose a file"), _(""), _(""), _("*.map"), wxFD_OPEN);

	WxSaveFileDialog1 =  new wxFileDialog(this, _("Choose a file"), _(""), _(""), _("*.map"), wxFD_SAVE);

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
void cFrameOFED::Mnunewmap1006Click(wxCommandEvent& event) {

	cDialogCreateMap* CreateMap = new cDialogCreateMap( this );
	CreateMap->Show();
}

/*
 * cFrameOFEDScrollWin
 */
void cFrameOFED::cFrameOFEDScrollWin(wxScrollWinEvent& event) {

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

sTiles cFrameOFED::SetupBarracks( cSurface **pSurface ) {
	sTiles Tiles;

	if (g_OFED.mMapTileType == eTileTypes_Jungle) {
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 333 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 334 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 352 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 353 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 354 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 372 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 373 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 374 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 2, 375 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 3, 392 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 3, 393 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 3, 394 ) );

		*pSurface = new cSurface( 16 * 4, 16 * 4 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Ice) {
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 245 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 246 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 0, 247 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 264 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 265 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 266 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 1, 267 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 284 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 285 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 286 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 2, 287 ) );

		*pSurface = new cSurface( 16 * 4, 16 * 3 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Desert) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 196 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 197 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 198 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 216 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 217 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 218 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 236 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 237 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 238 ) );

		*pSurface = new cSurface( 16 * 3, 16 *3 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Moors) {
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 335 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 336 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 354 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 355 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 356 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 374 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 375 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 376 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 3, 394 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 3, 395 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 3, 396 ) );

		*pSurface = new cSurface( 16 * 3, 16 * 4 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Int) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 246 ) );
		Tiles.mTiles.push_back( sRangeTile( 0, 1, 266 ) );

		*pSurface = new cSurface( 16 * 1, 16 * 2 );
	}

	return Tiles;
}

sTiles cFrameOFED::SetupHut( cSurface **pSurface ) {
	sTiles Tiles;
	
	if (g_OFED.mMapTileType == eTileTypes_Jungle) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 255 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 256 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 257 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 275 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 276 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 277 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 295 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 296 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 297 ) );

		*pSurface = new cSurface( 16 * 3, 16 * 3 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Desert) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 12 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 15 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 16 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 0, 18 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 32 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 35 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 36 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 1, 38 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 52 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 98 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 99 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 2, 58 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 3, 72 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 3, 118 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 3, 119 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 3, 78 ) );

		*pSurface = new cSurface( 16 * 4, 16 * 4 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Ice) {
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 241 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 242 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 0, 243 ) );


		Tiles.mTiles.push_back( sRangeTile( 0, 1, 260 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 261 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 262 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 1, 263 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 280 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 281 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 282 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 2, 283 ) );

		*pSurface = new cSurface( 16 * 4, 16 * 3 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Moors) {

		Tiles.mTiles.push_back( sRangeTile( 0, 0, 240 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 241 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 242 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 0, 243 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 260 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 261 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 262 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 1, 263 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 280 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 281 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 282 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 2, 283 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 3, 300 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 3, 301 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 3, 302 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 3, 303 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 4, 320 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 4, 321 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 4, 322 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 4, 323 ) );

		*pSurface = new cSurface( 16 * 4, 16 * 5 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Int) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 246 ) );
		Tiles.mTiles.push_back( sRangeTile( 0, 1, 266 ) );


		*pSurface = new cSurface( 16 * 1, 16 * 2 );
	}

	return Tiles;
}

sTiles cFrameOFED::SetupBunker( cSurface **pSurface ) {
	sTiles Tiles;

	if (g_OFED.mMapTileType == eTileTypes_Jungle && g_OFED.mMapTileSubType == eTileSub_0) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 267 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 268 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 269 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 287 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 288 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 289 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 307 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 308 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 309 ) );

		*pSurface = new cSurface( 16 * 3, 16 * 3 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Desert) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 9 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 10 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 11 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 29 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 30 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 31 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 49 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 50 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 51 ) );

		*pSurface = new cSurface( 16 * 3, 16 * 3 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Ice ) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 307 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 308 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 309 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 327 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 328 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 329 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 347 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 348 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 349 ) );

		*pSurface = new cSurface( 16 * 3, 16 * 3 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Moors) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 160 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 161 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 162 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 180 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 181 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 182 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 200 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 201 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 202 ) );

		*pSurface = new cSurface( 16 * 3, 16 * 3 );
	}


	if (g_OFED.mMapTileType == eTileTypes_Int) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 331 ) );
		Tiles.mTiles.push_back( sRangeTile( 0, 1, 351 ) );

		*pSurface = new cSurface( 16 * 1, 16 * 2 );
	}

	return Tiles;
}

/*
 * MnuHutSoldier1012Click
 */
void cFrameOFED::MnuHutSoldier1012Click(wxCommandEvent& event) {
	cSurface* Surface = 0;

	sTiles Tiles = SetupHut( &Surface );

	if (Surface == 0)
		return;

	if (g_OFED.mMapTileType == eTileTypes_Jungle) {
		Tiles.mSprites.push_back( sRangeSprite( 20, 27, eSprite_BuildingDoor2 ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Desert) {
		Tiles.mSprites.push_back( sRangeSprite( 35, 40, eSprite_BuildingDoor2 ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Ice) {
		// In this case 'Shrub' is roof
		Tiles.mSprites.push_back( sRangeSprite( 20, 1, eSprite_Shrub ) );
		Tiles.mSprites.push_back( sRangeSprite( 12, 23, eSprite_BuildingDoor2 ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Moors) {
		Tiles.mSprites.push_back( sRangeSprite( 28, 65, eSprite_BuildingDoor2 ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Int) {
		Tiles.mSprites.push_back( sRangeSprite( 3, 5, eSprite_BuildingDoor2 ) );
	}

	SetupCursorForDraw( Surface, Tiles );
}

/*
 * MnuHutIndigenous1013Click
 */
void cFrameOFED::MnuHutIndigenous1013Click(wxCommandEvent& event) {
	cSurface* Surface = 0;

	sTiles Tiles = SetupHut( &Surface );

	if (Surface == 0)
		return;

	if (g_OFED.mMapTileType == eTileTypes_Jungle) {
		Tiles.mSprites.push_back( sRangeSprite( 20, 27, eSprite_Door_Indigenous ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Desert) {
		Tiles.mSprites.push_back( sRangeSprite( 35, 40, eSprite_Door_Indigenous ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Ice) {
		// In this case 'Shrub' is roof
		Tiles.mSprites.push_back( sRangeSprite( 20, 1, eSprite_Shrub ) );
		Tiles.mSprites.push_back( sRangeSprite( 12, 23, eSprite_Door_Indigenous ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Moors) {
		Tiles.mSprites.push_back( sRangeSprite( 28, 65, eSprite_Door_Indigenous ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Int) {
		Tiles.mSprites.push_back( sRangeSprite( 3, 5, eSprite_Door_Indigenous ) );
	}

	SetupCursorForDraw( Surface, Tiles );
}

/*
 * Mnuindigenouswithspear1014Click
 */
void cFrameOFED::Mnuindigenouswithspear1014Click(wxCommandEvent& event) {
	cSurface* Surface = 0;

	sTiles Tiles = SetupHut( &Surface );
	if (Surface == 0)
		return;

	if (g_OFED.mMapTileType == eTileTypes_Jungle) {
		Tiles.mSprites.push_back( sRangeSprite( 20, 27, eSprite_Door_Indigenous_Spear ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Desert) {
		Tiles.mSprites.push_back( sRangeSprite( 35, 40, eSprite_Door_Indigenous_Spear ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Ice) {
		// In this case 'Shrub' is roof
		Tiles.mSprites.push_back( sRangeSprite( 20, 1, eSprite_Shrub ) );
		Tiles.mSprites.push_back( sRangeSprite( 12, 23, eSprite_Door_Indigenous_Spear ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Moors) {
		Tiles.mSprites.push_back( sRangeSprite( 28, 65, eSprite_Door_Indigenous_Spear ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Int) {
		Tiles.mSprites.push_back( sRangeSprite( 3, 5, eSprite_Door_Indigenous_Spear ) );
	}

	SetupCursorForDraw( Surface, Tiles );
}

/*
 * MnuBunkerSoldier1015Click
 */
void cFrameOFED::MnuBunkerSoldier1015Click(wxCommandEvent& event) {
	cSurface* Surface = 0;

	sTiles Tiles = SetupBunker( &Surface );
	if (Surface == 0)
		return;

	if (g_OFED.mMapTileType == eTileTypes_Jungle) {
		Tiles.mSprites.push_back( sRangeSprite( 23, 32, eSprite_BuildingDoor3 ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Desert) {
		Tiles.mSprites.push_back( sRangeSprite( 23, 32, eSprite_BuildingDoor3 ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Ice) {
		Tiles.mSprites.push_back( sRangeSprite( 23, 32, eSprite_BuildingDoor3 ) );
	}
	
	if (g_OFED.mMapTileType == eTileTypes_Moors) {
		Tiles.mSprites.push_back( sRangeSprite( 23, 32, eSprite_BuildingDoor3 ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Int) {
		Tiles.mSprites.push_back( sRangeSprite( 3, 3, eSprite_BuildingDoor3 ) );
	}
	SetupCursorForDraw( Surface, Tiles );
}

/*
 * MnuBunkerSoldierReinforced1016Click
 */
void cFrameOFED::MnuBunkerSoldierReinforced1016Click(wxCommandEvent& event) {
	cSurface* Surface = 0;

	sTiles Tiles = SetupBunker( &Surface );
	if (Surface == 0)
		return;

	if (g_OFED.mMapTileType == eTileTypes_Jungle) {
		Tiles.mSprites.push_back( sRangeSprite( 23, 32, eSprite_BuildingDoor_Reinforced ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Desert) {
		Tiles.mSprites.push_back( sRangeSprite( 23, 32, eSprite_BuildingDoor_Reinforced ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Ice) {
		Tiles.mSprites.push_back( sRangeSprite( 23, 32, eSprite_BuildingDoor_Reinforced ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Moors) {
		Tiles.mSprites.push_back( sRangeSprite( 23, 32, eSprite_BuildingDoor_Reinforced ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Int) {
		Tiles.mSprites.push_back( sRangeSprite( 3, 3, eSprite_BuildingDoor_Reinforced ) );
	}
	SetupCursorForDraw( Surface, Tiles );
}

/*
 * MnuBarracksSoldier1017Click
 */
void cFrameOFED::MnuBarracksSoldier1017Click(wxCommandEvent& event) {
	sTiles Tiles;
	cSurface *Surface = 0;

	Tiles = SetupBarracks( &Surface );

	if (Surface == 0)
		return;

	if (g_OFED.mMapTileType == eTileTypes_Jungle) {
		Tiles.mSprites.push_back( sRangeSprite( 13, 2, eSprite_BuildingRoof ) );
		Tiles.mSprites.push_back( sRangeSprite( 9, 34, eSprite_BuildingDoor ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Desert) {
		Tiles.mSprites.push_back( sRangeSprite( 12, -15, eSprite_BuildingRoof ) );
		Tiles.mSprites.push_back( sRangeSprite( 7, 16, eSprite_BuildingDoor ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Ice) {
		Tiles.mSprites.push_back( sRangeSprite( 23, -5, eSprite_BuildingRoof ) );
		Tiles.mSprites.push_back( sRangeSprite( 20, 27, eSprite_BuildingDoor ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Moors) {
		Tiles.mSprites.push_back( sRangeSprite( 15, 1, eSprite_BuildingRoof ) );
		Tiles.mSprites.push_back( sRangeSprite( 7, 33, eSprite_BuildingDoor ) );
	}

	if (g_OFED.mMapTileType == eTileTypes_Int) {
		Tiles.mSprites.push_back( sRangeSprite( 3, 5, eSprite_BuildingDoor ) );
	}
	SetupCursorForDraw( Surface, Tiles );
}

void cFrameOFED::Mnucliff1018Click( wxCommandEvent& event ) {
	sTiles Tiles;
	cSurface *Surface = 0;

	if (g_OFED.mMapTileType == eTileTypes_Ice) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 120 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 121 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 122 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 0, 123 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 0, 124 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 0, 125 ) );
		Tiles.mTiles.push_back( sRangeTile( 6, 0, 126 ) );
		Tiles.mTiles.push_back( sRangeTile( 7, 0, 127 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 140 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 141 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 142 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 1, 143 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 1, 144 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 1, 145 ) );
		Tiles.mTiles.push_back( sRangeTile( 6, 1, 146 ) );
		Tiles.mTiles.push_back( sRangeTile( 7, 1, 147 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 160 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 161 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 162 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 2, 163 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 2, 164 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 2, 165 ) );
		Tiles.mTiles.push_back( sRangeTile( 6, 2, 166 ) );
		Tiles.mTiles.push_back( sRangeTile( 7, 2, 167 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 3, 180 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 3, 181 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 3, 182 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 3, 183 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 3, 184 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 3, 185 ) );
		Tiles.mTiles.push_back( sRangeTile( 6, 3, 186 ) );
		Tiles.mTiles.push_back( sRangeTile( 7, 3, 187 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 4, 200 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 4, 201 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 4, 202 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 4, 203 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 4, 204 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 4, 205 ) );
		Tiles.mTiles.push_back( sRangeTile( 6, 4, 206 ) );
		Tiles.mTiles.push_back( sRangeTile( 7, 4, 207 ) );

		Surface = new cSurface( 16 * 8, 16 * 5 );
	}

	if (g_OFED.mMapTileType == eTileTypes_Jungle) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 89 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 90 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 91 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 0, 92 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 0, 93 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 0, 94 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 109 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 110 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 111 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 1, 112 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 1, 113 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 1, 114 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 129 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 130 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 131 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 2, 132 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 2, 133 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 2, 134 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 3, 149 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 3, 150 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 3, 151 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 3, 152 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 3, 153 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 3, 154 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 4, 169 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 4, 170 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 4, 171 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 4, 172 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 4, 173 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 4, 174 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 5, 189 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 5, 190 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 5, 191 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 5, 192 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 5, 193 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 5, 194 ) );

		Tiles.mTiles.push_back( sRangeTile( 4, 6, 213 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 6, 214 ) );

		Surface = new cSurface( 16 * 6, 16 * 6 );
	}
	

	if (g_OFED.mMapTileType == eTileTypes_Desert) {
		Tiles.mTiles.push_back( sRangeTile( 0, 0, 80 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 0, 81 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 0, 82 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 0, 83 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 0, 84 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 0, 85 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 1, 100 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 1, 101 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 1, 102 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 1, 103 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 1, 104 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 1, 105 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 2, 120 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 2, 121 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 2, 122 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 2, 123 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 2, 124 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 2, 125 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 3, 140 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 3, 141 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 3, 142 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 3, 143 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 3, 144 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 3, 145 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 4, 160 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 4, 161 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 4, 162 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 4, 163 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 4, 164 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 4, 165 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 5, 180 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 5, 181 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 5, 182 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 5, 183 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 5, 184 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 5, 185 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 6, 200 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 6, 201 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 6, 202 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 6, 203 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 6, 204 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 6, 205 ) );

		Tiles.mTiles.push_back( sRangeTile( 0, 7, 220 ) );
		Tiles.mTiles.push_back( sRangeTile( 1, 7, 221 ) );
		Tiles.mTiles.push_back( sRangeTile( 2, 7, 222 ) );
		Tiles.mTiles.push_back( sRangeTile( 3, 7, 223 ) );
		Tiles.mTiles.push_back( sRangeTile( 4, 7, 224 ) );
		Tiles.mTiles.push_back( sRangeTile( 5, 7, 225 ) );

		Surface = new cSurface( 16 * 6, 16 * 8 );
	}

	if(Surface)
		SetupCursorForDraw( Surface, Tiles );
}

void cFrameOFED::SetupCursorForDraw( cSurface *pSurface, sTiles& pTiles ) {

	for (std::vector<sRangeTile>::const_iterator TileIT = pTiles.mTiles.begin(); TileIT != pTiles.mTiles.end(); ++TileIT) {
		g_OFED.DrawTile( pSurface, TileIT->mTileID, (TileIT->mX), TileIT->mY );
	}

	g_OFED.LoadPalette( pSurface );
	pSurface->draw();

	wxBitmap Cursor = SDL_To_Bitmap( pSurface, pSurface->GetWidth(), pSurface->GetHeight() );
	wxImage image = Cursor.ConvertToImage();
	image.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_X, 1 );
	image.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 1 );

	this->SetCursor( wxCursor( image ) );

	g_OFED.SetCursorTileRange( pTiles );

	delete pSurface;
}
