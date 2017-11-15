#include "ofed.hpp"
#include "ui_NewMapDialog.h"
#include "ui_ToolboxTiles.h"
#include <QDesktopWidget>
#include <qpainter.h>

int32 g_SpriteAnim[111] = {};
std::string g_SpriteName[111] = {};


cOFED::cOFED(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	mToolboxTiles = 0;
	mToolboxSprites = 0;

	mCursorSurface = new cSurface(16, 16);

	CursorReset();
	SetupSprites();

	// Menu Items
	QObject::connect(ui.action_New_Map, &QAction::triggered, this, &cOFED::ShowDialog_NewMap);

	// Prepare OpenFodder
	cFodder* Fodder = new cFodder(g_Window.GetSingletonPtr());
	Fodder->Prepare();

	if (!g_AvailableDataVersions.size()) {

		// TODO: Show a message that no data was found
		return;
	}

	Fodder->VersionSelect_0();

	Fodder->Mouse_Setup();
	Fodder->Mouse_Inputs_Get();

	OpenFodder_Prepare();

	setGeometry(
		QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			size(),
			qApp->desktop()->availableGeometry()
		)
	);

	ShowDialog_ToolboxTiles();
	ShowDialog_ToolboxSprites();
}

void cOFED::OpenFodder_Prepare() {
	cFodder* Fodder = &g_Fodder;

	Fodder->Game_Setup(0);
	Fodder->Campaign_Load("Cannon Fodder");

	Fodder->Mission_Memory_Clear();
	Fodder->Mission_Prepare_Squads();
	Fodder->sub_10DEC();

	Fodder->Squad_Set_Squad_Leader();
	Fodder->Sprite_Clear_All();


	Fodder->Mission_Phase_Goals_Set();

	Fodder->Map_Load();
	Fodder->Map_Load_Sprites();

	Fodder->Mission_Troop_Count();
	Fodder->Mission_Troop_Sort();
	Fodder->Mission_Troop_Prepare(false);
	Fodder->Mission_Troop_Attach_Sprites();

	Fodder->mMission_Aborted = 0;

	Fodder->MapTiles_Draw();
	Fodder->Camera_Reset();

	Fodder->Mouse_Inputs_Get();
	Fodder->Sprite_Frame_Modifier_Update();

	Fodder->mCamera_Start_Adjust = 1;
	Fodder->mCamera_Position_X = Fodder->mSprites[0].field_0;
	Fodder->mCamera_Position_Y = Fodder->mSprites[0].field_4;

	Fodder->mInput_Enabled = -1;
	Fodder->sub_11CAD();

	Fodder->mGUI_Mouse_Modifier_X = 0;
	Fodder->mGUI_Mouse_Modifier_Y = 4;
	Fodder->mCamera_Start_Adjust = 1;

	g_Graphics.PaletteSet();

	Fodder->GUI_Sidebar_Prepare_Squads();
	Fodder->Squad_Select_Grenades();
	Fodder->Map_Clear_Destroy_Tiles();
	Fodder->Sprite_Count_HelicopterCallPads();

	Fodder->mMouseSpriteNew = eSprite_pStuff_Mouse_Cursor;
	Fodder->mMission_Aborted = 0;
	Fodder->mMission_Paused = 0;
	Fodder->mMission_In_Progress = -1;
	Fodder->mMission_Finished = 0;
	Fodder->mMission_ShowMapOverview = 0;

	Fodder->mImage->surfaceSetToPaletteNew();

	//Fodder->Map_Tiles_Draw();

	// Set the top left
	Fodder->mMapTilePtr = 0x60 - Fodder->mMapWidth * 2;
	g_Graphics.MapTiles_Draw();
	g_Window.FrameEnd();
}

void cOFED::moveEvent(QMoveEvent *event) {
	
	if(mToolboxTiles)
		mToolboxTiles->move(x() + width(), y());

	if(mToolboxSprites)
		mToolboxSprites->move(x(), y() + height());
}

/**
 * Launch the Tile Toobox dialog 
 */
void cOFED::ShowDialog_ToolboxTiles() {

	mToolboxTiles = new cToolboxTiles(this, 0);

	// Position to the right of map editor
	mToolboxTiles->move(x() + width(), y());

	mToolboxTiles->show();
}

void cOFED::ShowDialog_ToolboxSprites() {
	
	mToolboxSprites = new cToolboxSprites(this, 0);

	// Position to the bottom of map editor
	mToolboxSprites->move(x(), y() + height());

	mToolboxSprites->show();
}

/**
 * Launch the New Map dialog
 */
void cOFED::ShowDialog_NewMap() {

	cNewMapDialog* NewMap = new cNewMapDialog( this, 0);

	NewMap->show();
}

/**
 * Create a new map
 */
void cOFED::Create_NewMap(const std::string& pTileSet, const std::string& pTileSub, size_t pWidth, size_t pHeight) {
	
	// Loop each known tile type
	for (auto TileType : mTileTypes) {

		if (pTileSet == TileType.mFullName) {
			size_t Sub = 0;

			if (pTileSub == "Sub1")
				Sub = 1;

			CursorReset();

			g_Fodder.Map_Create(TileType, Sub, pWidth, pHeight);
			g_Window.FrameEnd();

			// Update the Toolboxes
			mToolboxSprites->RenderSprites();
			mToolboxTiles->RenderTiles();

			return;
		}
	}
}

/**
 * Clear the cursor status
 */
void cOFED::CursorReset() {
	mCursorTile = -1;
	mCursorSprite = -1;
	mCursorRangeTiles = sTiles();

	CursorUpdate();
}

/**
 * Refresh the drawn cursor
 */
void cOFED::CursorUpdate() {

	double ScaleWidth = ((cWindowQT*)&g_Window)->mScaleWidth;
	double ScaleHeight = ((cWindowQT*)&g_Window)->mScaleHeight;

	// Tile Mode?
	if (mCursorTile != -1) {

		mCursorImageFinal = QImage(mCursorImage.width() * ScaleWidth, mCursorImage.height() * ScaleHeight, QImage::Format_RGB32);

		// Scale the cursor into mCursorImageFinal
		QPainter painter(&mCursorImageFinal);
		QRectF Dest(0, 0, mCursorImageFinal.width(), mCursorImageFinal.height());
		QRectF Src(0, 0, mCursorImage.width(), mCursorImage.height());
		painter.drawImage(Dest, mCursorImage, Src);
	}
	
	// Set the actual cursor
	if(!mCursorImageFinal.isNull())
		setCursor(QCursor(QPixmap::fromImage(mCursorImageFinal)));
}

/**
 * Set the cursor to a tile
 */
void cOFED::SetCursorTileID(const size_t pCursorTileID) {

	CursorReset();
	mCursorTile = pCursorTileID;

	mCursorSurface->clearBuffer();

	g_Graphics.Map_Tile_Draw(mCursorSurface, mCursorTile, 0, 0, 0);
	g_Graphics.PaletteSet(mCursorSurface);

	mCursorSurface->surfaceSetToPaletteNew();
	mCursorSurface->draw();

	SDL_Surface* Source = mCursorSurface->GetSurface();
	mCursorImage = QImage(static_cast<uchar*>(Source->pixels), Source->w, Source->h, QImage::Format_RGB32);

	CursorUpdate();
}

void cOFED::SetupSprites() {
	for (int i = 0; i < 111; ++i) {
		g_SpriteAnim[i] = -1;
		g_SpriteName[i] = "";
	}

	g_SpriteAnim[eSprite_Player] = 0x00;
	g_SpriteAnim[eSprite_Enemy] = 0x52;

	g_SpriteAnim[eSprite_Shrub] = 0x8F;
	g_SpriteAnim[eSprite_Tree] = 0x90;
	g_SpriteAnim[eSprite_BuildingRoof] = 0x91;
	g_SpriteAnim[eSprite_Snowman] = 0x92;
	g_SpriteAnim[eSprite_Shrub2] = 0x93;
	g_SpriteAnim[eSprite_Waterfall] = 0x94;
	g_SpriteAnim[eSprite_Bird2_Left] = 0x98;

	g_SpriteAnim[eSprite_BuildingDoor] = 0x99;
	//g_SpriteAnim[eSprite_GroundHole] = ;
	g_SpriteAnim[eSprite_BuildingDoor2] = 0x9B;
	g_SpriteAnim[eSprite_Floating_Dead_Soldier] = 0x9E;
	g_SpriteAnim[eSprite_Enemy_Rocket] = 0x39;
	g_SpriteAnim[eSprite_GrenadeBox] = 0xC2;
	g_SpriteAnim[eSprite_RocketBox] = 0xC3;

	g_SpriteAnim[eSprite_Helicopter_Grenade_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Flashing_Light] = 0xC4;
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
	g_SpriteAnim[eSprite_Cannon] = 0x96;

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
	g_SpriteAnim[eSprite_Bonus_Armour] = 0xE3;
	g_SpriteAnim[eSprite_Bonus_RankHomingInvin_SquadLeader] = 0xE5;

	g_SpriteAnim[eSprite_Bonus_RankHomingInvin_Squad] = 0xE6;
	g_SpriteAnim[eSprite_Helicopter_CallPad] = 0xE7;

	g_SpriteAnim[eSprite_BuildingDoor_Reinforced] = 0xE0;
	g_SpriteAnim[eSprite_Helicopter_Grenade2_Human_Called] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Grenade_Human_Called] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Missile_Human_Called] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Homing_Human_Called] = 0x8B;
	g_SpriteAnim[eSprite_Turret_HomingMissile_Enemy] = 0xD2;

	g_SpriteAnim[eSprite_Hostage_2] = 0xD9;
	g_SpriteAnim[eSprite_Helicopter_Homing_Enemy2] = 0x8B;
	g_SpriteAnim[eSprite_Computer_1] = 0x8F;
	g_SpriteAnim[eSprite_Computer_2] = 0x8F;
	g_SpriteAnim[eSprite_Computer_3] = 0x8F;


	g_SpriteName[eSprite_Player] = "Human Soldier";
	g_SpriteName[eSprite_Enemy] = "Enemy Soldier";
	g_SpriteName[eSprite_Null] = "Null";

	g_SpriteName[eSprite_Snowman] = "Snowman";
	g_SpriteName[eSprite_Shrub] = "Shrub";
	g_SpriteName[eSprite_Tree] = "Tree";
	g_SpriteName[eSprite_BuildingRoof] = "Building Roof";
	g_SpriteName[eSprite_Shrub2] = "Shrub2";
	g_SpriteName[eSprite_Waterfall] = "Waterfall";
	g_SpriteName[eSprite_Bird2_Left] = "Bird2 Left";

	g_SpriteName[eSprite_BuildingDoor] = "Building Door";
	g_SpriteName[eSprite_GroundHole] = "Ground Hole";
	g_SpriteName[eSprite_BuildingDoor2] = "Building Door 2";
	g_SpriteName[eSprite_Floating_Dead_Soldier] = "Floating Dead Soldier";
	g_SpriteName[eSprite_Enemy_Rocket] = "Enemy Rocket Soldier";
	g_SpriteName[eSprite_GrenadeBox] = "Box of Grenades";
	g_SpriteName[eSprite_RocketBox] = "Box of Rockets";

	g_SpriteName[eSprite_Helicopter_Grenade_Enemy] = "Enemy Helicopter: Grenade";
	g_SpriteName[eSprite_Flashing_Light] = "Flashing Light";
	g_SpriteName[eSprite_Helicopter_Grenade2_Enemy] = "Enemy Helicopter: Grenade2";
	g_SpriteName[eSprite_Helicopter_Missile_Enemy] = "Enemy Helicopter: Missile";
	g_SpriteName[eSprite_Helicopter_Homing_Enemy] = "Enemy Helicopter: Homing Missile";
	g_SpriteName[eSprite_Helicopter_Grenade2_Human] = "Human Helicopter: Grenade2";
	g_SpriteName[eSprite_Helicopter_Grenade_Human] = "Human Helicopter: Grenade";
	g_SpriteName[eSprite_Helicopter_Missile_Human] = "Human Helicopter: Missile";
	g_SpriteName[eSprite_Helicopter_Homing_Human] = "Human Helicopter: Homing Missile";

	g_SpriteName[eSprite_Mine] = "Mine";
	g_SpriteName[eSprite_Mine2] = "Mine2";
	g_SpriteName[eSprite_Spike] = "Spike";

	g_SpriteName[eSprite_BoilingPot] = "Boiling Pot";
	g_SpriteName[eSprite_Indigenous] = "Indigenous";
	g_SpriteName[eSprite_Indigenous2] = "Indigenous 2";
	g_SpriteName[eSprite_VehicleNoGun_Human] = "Human Vehicle: No Weapon";
	g_SpriteName[eSprite_VehicleGun_Human] = "Human Vehicle: Weapon";

	g_SpriteName[eSprite_Tank_Human] = "Human Tank";
	g_SpriteName[eSprite_Bird_Left] = "Bird Left";
	g_SpriteName[eSprite_Bird_Right] = "Bird Right";
	g_SpriteName[eSprite_Seal] = "Seal";
	g_SpriteName[eSprite_Tank_Enemy] = "Enemy Tank";

	g_SpriteName[eSprite_Indigenous_Spear] = "Indigenous: Spear";
	g_SpriteName[eSprite_Hostage] = "Hostage";
	g_SpriteName[eSprite_Hostage_Rescue_Tent] = "Hostage Rescue Tent";

	g_SpriteName[eSprite_Door_Indigenous] = "Building Door: Indigenous";
	g_SpriteName[eSprite_Door2_Indigenous] = "Building Door: Indigenous 2";
	g_SpriteName[eSprite_Door_Indigenous_Spear] = "Building Door: Indigenous Spear";

	g_SpriteName[eSprite_Turret_Missile_Human] = "Human Turret: Missile";
	g_SpriteName[eSprite_Turret_Missile2_Human] = "Human Turret: Missile 2";

	g_SpriteName[eSprite_VehicleNoGun_Enemy] = "Enemy Vehicle: No Weapon";
	g_SpriteName[eSprite_VehicleGun_Enemy] = "Enemy Vehicle: Weapon";
	g_SpriteName[eSprite_Vehicle_Unk_Enemy] = "Enemy Vehicle: Unknown";
	g_SpriteName[eSprite_Indigenous_Invisible] = "Indigenous: Invisible?";

	g_SpriteName[eSprite_Turret_Missile_Enemy] = "Enemy Turret: Missile";
	g_SpriteName[eSprite_Turret_Missile2_Enemy] = "Enemy Turret: Missile 2";
	g_SpriteName[eSprite_BuildingDoor3] = "Building Door3";

	g_SpriteName[eSprite_OpenCloseDoor] = "Building Door: In";
	g_SpriteName[eSprite_Seal_Mine] = "Seal Mine";
	g_SpriteName[eSprite_Spider_Mine] = "Spider Mine";

	g_SpriteName[eSprite_Bonus_RankToGeneral] = "Bonus: Rank to General";
	g_SpriteName[eSprite_Bonus_Rockets] = "Bonus: Rockets";
	g_SpriteName[eSprite_Bonus_Armour] = "Bonus Armour";
	g_SpriteName[eSprite_Bonus_RankHomingInvin_SquadLeader] = "Bonus: Rockets and General";
	g_SpriteName[eSprite_Bonus_RankHomingInvin_Squad] = "Bonus: Squad General and Rockets";
	g_SpriteName[eSprite_Helicopter_CallPad] = "Helicopter Callpad";
	g_SpriteName[eSprite_BuildingDoor_Reinforced] = "Building Door: Reinforced";
	g_SpriteName[eSprite_Helicopter_Grenade2_Human_Called] = "Human Helicopter: Grenade2 Callable";
	g_SpriteName[eSprite_Helicopter_Grenade_Human_Called] = "Human Helicopter: Grenade Callable";
	g_SpriteName[eSprite_Helicopter_Missile_Human_Called] = "Human Helicopter: Missile Callable";
	g_SpriteName[eSprite_Helicopter_Homing_Human_Called] = "Human Helicopter: Homing Missile Callable";

	g_SpriteName[eSprite_Turret_HomingMissile_Enemy] = "Enemey Turret: Homing Missile";
	g_SpriteName[eSprite_Hostage_2] = "Hostage 2";
	g_SpriteName[eSprite_Helicopter_Homing_Enemy2] = "Enemy Helicopter: Homing Missile2";
	g_SpriteName[eSprite_Computer_1] = "Computer 1";
	g_SpriteName[eSprite_Computer_2] = "Computer 2";
	g_SpriteName[eSprite_Computer_3] = "Computer 3";
}
