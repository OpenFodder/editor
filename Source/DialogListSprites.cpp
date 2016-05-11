///-----------------------------------------------------------------
///
/// @file      DialogListSprites.cpp
/// @author    Robbie
/// Created:   4/05/2016 20:08:32
/// @section   DESCRIPTION
///            cDialogListSprites class implementation
///
///------------------------------------------------------------------

#include "stdafx.hpp"
#include "DialogListSprites.h"
#include "FrameOFED.h"

//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// cDialogListSprites
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(cDialogListSprites,wxDialog)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(cDialogListSprites::OnClose)
	EVT_MENU(ID_MNU_REMOVESPRITE_1003 , cDialogListSprites::Mnuremovesprite1003Click)
	
	EVT_LIST_ITEM_ACTIVATED(ID_WXLISTCTRL1,cDialogListSprites::WxListCtrl1ItemActivated)
	EVT_LIST_ITEM_RIGHT_CLICK(ID_WXLISTCTRL1,cDialogListSprites::WxListCtrl1RightClick)
END_EVENT_TABLE()
////Event Table End

cDialogListSprites::cDialogListSprites(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxDialog(parent, id, title, position, size, style)
{
	CreateGUIControls();
}

cDialogListSprites::~cDialogListSprites()
{
} 

void cDialogListSprites::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End.
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	WxPopupMenu1 = new wxMenu(_(""));
	WxPopupMenu1->Append(ID_MNU_REMOVESPRITE_1003, _("Remove Sprite"), _(""), wxITEM_NORMAL);

	WxListCtrl1 = new wxListCtrl(this, ID_WXLISTCTRL1, wxPoint(8, 16), wxSize(361, 385), wxLC_REPORT, wxDefaultValidator, _("WxListCtrl1"));
	WxListCtrl1->InsertColumn(0, _("Number"), wxLIST_FORMAT_LEFT, 66);
	WxListCtrl1->InsertColumn(1, _("Type"), wxLIST_FORMAT_LEFT, 50);
	WxListCtrl1->InsertColumn(2, _("Name"), wxLIST_FORMAT_LEFT, 240);

	SetTitle(_("Sprite List"));
	SetIcon(wxNullIcon);
	SetSize(8,8,396,468);
	Center();
	
	////GUI Items Creation End
}

void cDialogListSprites::OnClose(wxCloseEvent& /*event*/)
{
	Destroy();
}

void cDialogListSprites::AddSprite( int pNumber, const sSpriteDef& pSpriteDef ) {
	
	int i = WxListCtrl1->InsertItem( WxListCtrl1->GetItemCount(), wxString::Format( wxT( "%i" ), pNumber ) );
	WxListCtrl1->SetItem( i, 1, wxString::Format( wxT( "%i" ), pSpriteDef.mSpriteID ) );
	WxListCtrl1->SetItem( i, 2, wxString( g_SpriteName[pSpriteDef.mSpriteID] ) );
}

void cDialogListSprites::AddSprites() {
	WxListCtrl1->DeleteAllItems();
	int Number = 0;
	for (std::vector<sSpriteDef>::iterator SpriteIT = g_OFED.mSprites.begin(); SpriteIT != g_OFED.mSprites.end(); ++SpriteIT) {

		AddSprite( Number, *SpriteIT );
		++Number;
	}
}



/*
 * WxListCtrl1RightClick
 */
void cDialogListSprites::WxListCtrl1RightClick(wxListEvent& event) {
	mSelectedListItemID = event.GetIndex();

	PopupMenu( WxPopupMenu1 );
}

/*
 * WxListCtrl1ItemActivated
 */
void cDialogListSprites::WxListCtrl1ItemActivated(wxListEvent& event) {
	int id = event.GetItem().GetId();

	wxListItem listItem;
	listItem.SetId( id );
	listItem.SetMask( wxLIST_MASK_TEXT );
	listItem.SetColumn( 0 );

	WxListCtrl1->GetItem( listItem );
	int SpriteNumber = wxAtoi( listItem.GetText() );

	sSpriteDef Def = g_OFED.mSprites[SpriteNumber];
	
	int16 TileX = Def.mX / 16;
	int16 TileY = Def.mY / 16;

	TileX -= g_OFED.mCameraTilesX / 2;
	TileY -= g_OFED.mCameraTilesY / 2;
	if (TileX < 0)
		TileX = 0;

	if (TileY < 0)
		TileY = 0;

	g_FrameOFED->SetScrollPos( wxVERTICAL, TileY );
	g_FrameOFED->SetScrollPos( wxHORIZONTAL, TileX );
	g_OFED.SetMapX( TileX );
	g_OFED.SetMapY( TileY );

	g_FrameOFED->Refresh();
}

void cDialogListSprites::Mnuremovesprite1003Click( wxCommandEvent& event ) {

	if (mSelectedListItemID < 0)
		return;

	wxListItem listItem;
	listItem.SetId( mSelectedListItemID );
	listItem.SetMask( wxLIST_MASK_TEXT );

	listItem.SetColumn( 0 );

	WxListCtrl1->GetItem( listItem );
	int SpriteNumber = wxAtoi( listItem.GetText() );

	g_OFED.mSprites.erase( g_OFED.mSprites.begin() + SpriteNumber );
	g_OFED.DrawTiles();

	g_FrameOFED->Refresh();
}
