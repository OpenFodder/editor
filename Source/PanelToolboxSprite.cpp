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

#include "stdafx.hpp"

int32 g_SpriteAnim[111] = {};

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

cPanelToolboxSprite::cPanelToolboxSprite( wxWindow *parent, wxWindowID id, const wxPoint &position, const wxSize& size, long style)
: wxPanel( parent, id, position, size, wxFRAME_NO_TASKBAR | wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX) )
{
	SetupSprites();
	mFinalSurface = 0;
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

	SetSize(8,8,798,350);
	Center();
	
	////GUI Items Creation End

}

void cPanelToolboxSprite::OnClose(wxCloseEvent& /*event*/)
{
	Destroy();
}

void cPanelToolboxSprite::SetupSprites() {
	for (int i = 0; i < 111; ++i) {
		g_SpriteAnim[i] = -1;
	}

	g_SpriteAnim[eSprite_Player] = 0x00;
	g_SpriteAnim[eSprite_Enemy] = 0x52;

	g_SpriteAnim[eSprite_Shrub] = 0x8F;
	g_SpriteAnim[eSprite_Tree] = 0x90;
	g_SpriteAnim[eSprite_BuildingRoof] = 0x91;
	g_SpriteAnim[eSprite_Shrub2] = 0x93;
	g_SpriteAnim[eSprite_Waterfall] = 0x94;
	g_SpriteAnim[eSprite_Bird2_Left] = 0x98;

	g_SpriteAnim[eSprite_BuildingDoor] = 0x99;
	g_SpriteAnim[eSprite_BuildingDoor2] = 0x9B;
	g_SpriteAnim[eSprite_Floating_Dead_Soldier] = 0x9E;
	g_SpriteAnim[eSprite_Enemy_Rocket] = 0x3E;
	g_SpriteAnim[eSprite_GrenadeBox] = 0xC2;
	g_SpriteAnim[eSprite_RocketBox] = 0xC3;

	g_SpriteAnim[eSprite_Helicopter_Grenade_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Grenade2_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Missile_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Homing_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Grenade2_Human] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Grenade_Human] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Missile_Human] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Homing_Human] = 0x8B;

	g_SpriteAnim[eSprite_Mine] = 0xC7;
	g_SpriteAnim[eSprite_Mine2] = 0xC8;
	g_SpriteAnim[eSprite_Spike] = 0xC9;

	g_SpriteAnim[eSprite_BoilingPot] = 0xCD;
	g_SpriteAnim[eSprite_Indigenous] = 0xD0;
	g_SpriteAnim[eSprite_Indigenous2] = 0xD0;
	g_SpriteAnim[eSprite_VehicleNoGun_Human] = 0xA5;
	g_SpriteAnim[eSprite_VehicleGun_Human] = 0xA5;

	g_SpriteAnim[eSprite_Tank_Human] = 0xD1;
	g_SpriteAnim[eSprite_Bird_Left] = 0xD3;
	g_SpriteAnim[eSprite_Bird_Right] = 0xD4;
	g_SpriteAnim[eSprite_Seal] = 0xD5;
	g_SpriteAnim[eSprite_Tank_Enemy] = 0xD1;

	g_SpriteAnim[eSprite_Indigenous_Spear] = 0xD0;
	g_SpriteAnim[eSprite_Hostage] = 0xD9;
	g_SpriteAnim[eSprite_Hostage_Rescue_Tent] = 0xDD;

	g_SpriteAnim[eSprite_Door_Indigenous] = 0x9B;
	g_SpriteAnim[eSprite_Door2_Indigenous] = 0x9B;
	g_SpriteAnim[eSprite_Door_Indigenous_Spear] = 0x9B;

	g_SpriteAnim[eSprite_Turret_Missile_Human] = 0xD2;
	g_SpriteAnim[eSprite_Turret_Missile2_Human] = 0xD2;

	g_SpriteAnim[eSprite_VehicleNoGun_Enemy] = 0xA5;
	g_SpriteAnim[eSprite_VehicleGun_Enemy] = 0xA5;
	g_SpriteAnim[eSprite_Vehicle_Unk_Enemy] = 0xA5;
	g_SpriteAnim[eSprite_Indigenous_Invisible] = 0xD6;

	g_SpriteAnim[eSprite_Turret_Missile_Enemy] = 0xD2;
	g_SpriteAnim[eSprite_Turret_Missile2_Enemy] = 0xD2;
	g_SpriteAnim[eSprite_BuildingDoor3] = 0xE0;

	g_SpriteAnim[eSprite_OpenCloseDoor] = 0x9B;
	g_SpriteAnim[eSprite_Seal_Mine] = 0xD5;
	g_SpriteAnim[eSprite_Spider_Mine] = 0xE2;

	g_SpriteAnim[eSprite_Bonus_RankToGeneral] = 0x95;
	g_SpriteAnim[eSprite_Bonus_Rockets] = 0xE4;
	g_SpriteAnim[eSprite_Bonus_RocketsAndGeneral] = 0xE5;
	g_SpriteAnim[eSprite_Bonus_SquadGeneralRockets] = 0xE6;
	g_SpriteAnim[eSprite_Helicopter_CallPad] = 0xE7;

	g_SpriteAnim[eSprite_Turret_HomingMissile_Enemy] = 0xD2;
	g_SpriteAnim[eSprite_Hostage_2] = 0xD9;
	g_SpriteAnim[eSprite_Helicopter_Homing_Enemy2] = 0x8B;
	g_SpriteAnim[eSprite_Computer_1] = 0x93;
	g_SpriteAnim[eSprite_Computer_2] = 0x93;
	g_SpriteAnim[eSprite_Computer_3] = 0x93;
}

void cPanelToolboxSprite::ReloadSprites() {
	delete mFinalSurface;
	mFinalSurface = new cSurface( 20 * 17, 9 * 17 );

	size_t X = 0, Y = 0;
	size_t BigY = 0;

	for (size_t SpriteID = 0; SpriteID < 111; ++SpriteID) {
		int32 AnimID = g_SpriteAnim[SpriteID];
		if (AnimID < 0)
			continue;

		cSurface *Surface = new cSurface( off_32C0C[AnimID][0].mColCount, off_32C0C[AnimID][0].mRowCount );

		g_OFED.LoadPalette( Surface );
		g_OFED.DrawSprite( Surface, AnimID, 0, 0 );

		if (X + off_32C0C[AnimID][0].mColCount >( 20 * 17 )) {
			X = 0;
			Y += BigY;
			BigY = 0;
		}

		SDL_To_SDL( Surface, mFinalSurface, X, Y, off_32C0C[AnimID][0].mColCount, off_32C0C[AnimID][0].mRowCount );

		if (off_32C0C[AnimID][0].mRowCount > BigY)
			BigY = off_32C0C[AnimID][0].mRowCount;

		X += off_32C0C[AnimID][0].mColCount;
		if (X >= (20 * 17)) {
			X = 0;
			Y += BigY;
			BigY = 0;
		}

		delete Surface;
	}

	Refresh();
}

void cPanelToolboxSprite::OnMouse( wxMouseEvent& event ) {

}

void cPanelToolboxSprite::OnPaint( wxPaintEvent& event ) {
	size_t width = this->GetSize().GetWidth();
	size_t height = this->GetSize().GetHeight();


	wxBufferedPaintDC tileView( this );
	if (mFinalSurface) {

		mScaleWidth = (static_cast<double>(width) / static_cast<double>(mFinalSurface->GetWidth()));
		mScaleHeight = (static_cast<double>(height) / static_cast<double>(mFinalSurface->GetHeight()));

		tileView.DrawBitmap( SDL_To_Bitmap( mFinalSurface, width, height ), 0, 0 );
	}

}
