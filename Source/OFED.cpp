#include "ofed.hpp"
#include "ui_NewMapDialog.h"
#include "ui_ToolboxTiles.h"
#include <QDesktopWidget>
#include <qpainter.h>

#include <QFileDialog>

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
	QObject::connect(ui.action_Load_Map, &QAction::triggered, this, &cOFED::ShowDialog_LoadMap);
	QObject::connect(ui.action_Save_Map, &QAction::triggered, this, &cOFED::ShowDialog_SaveMap);

	QObject::connect(ui.action_Hut_Soldier, &QAction::triggered, this, &cOFED::AddHut_With_Soldier);
	QObject::connect(ui.action_Hut_Indeigenous, &QAction::triggered, this, &cOFED::AddHut_With_Indigenous);
	QObject::connect(ui.action_Hut_Indigenous_With_Spear, &QAction::triggered, this, &cOFED::AddHut_With_Indigenous_Spear);

	QObject::connect(ui.action_Barracks_Soldier, &QAction::triggered, this, &cOFED::AddBarracks_With_Soldier);
	
	QObject::connect(ui.action_Bunker_Soldier, &QAction::triggered, this, &cOFED::AddBunker_With_Soldier);
	QObject::connect(ui.action_Bunker_Soldier_Reinforced, &QAction::triggered, this, &cOFED::AddBunker_With_SoldierReinforced);
	
	QObject::connect(ui.action_Cliff, &QAction::triggered, this, &cOFED::AddCliff);

	
	// Prepare OpenFodder
	cFodder* Fodder = new cFodder(g_Window.GetSingletonPtr());
	Fodder->Prepare();

	if (!g_AvailableDataVersions.size()) {

		// TODO: Show a message that no data was found
		return;
	}

	Fodder->VersionSelect_0();
	Fodder->mVersionDefault = Fodder->mVersion;

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

sTiles cOFED::SetupHut() {
	sTiles Tiles(true);

	if (g_Fodder.mMap_TileSet  == eTileTypes_Jungle) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 255));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 256));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 257));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 275));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 276));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 277));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 295));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 296));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 297));

		mCursorSurface= new cSurface(16 * 3, 16 * 3);
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Desert) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 12));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 15));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 16));
		Tiles.mTiles.push_back(sRangeTile(3, 0, 18));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 32));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 35));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 36));
		Tiles.mTiles.push_back(sRangeTile(3, 1, 38));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 52));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 98));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 99));
		Tiles.mTiles.push_back(sRangeTile(3, 2, 58));

		Tiles.mTiles.push_back(sRangeTile(0, 3, 72));
		Tiles.mTiles.push_back(sRangeTile(1, 3, 118));
		Tiles.mTiles.push_back(sRangeTile(2, 3, 119));
		Tiles.mTiles.push_back(sRangeTile(3, 3, 78));

		mCursorSurface= new cSurface(16 * 4, 16 * 4);
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Ice) {
		Tiles.mTiles.push_back(sRangeTile(1, 0, 241));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 242));
		Tiles.mTiles.push_back(sRangeTile(3, 0, 243));


		Tiles.mTiles.push_back(sRangeTile(0, 1, 260));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 261));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 262));
		Tiles.mTiles.push_back(sRangeTile(3, 1, 263));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 280));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 281));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 282));
		Tiles.mTiles.push_back(sRangeTile(3, 2, 283));

		mCursorSurface= new cSurface(16 * 4, 16 * 3);
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Moors) {

		Tiles.mTiles.push_back(sRangeTile(0, 0, 240));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 241));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 242));
		Tiles.mTiles.push_back(sRangeTile(3, 0, 243));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 260));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 261));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 262));
		Tiles.mTiles.push_back(sRangeTile(3, 1, 263));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 280));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 281));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 282));
		Tiles.mTiles.push_back(sRangeTile(3, 2, 283));

		Tiles.mTiles.push_back(sRangeTile(0, 3, 300));
		Tiles.mTiles.push_back(sRangeTile(1, 3, 301));
		Tiles.mTiles.push_back(sRangeTile(2, 3, 302));
		Tiles.mTiles.push_back(sRangeTile(3, 3, 303));

		Tiles.mTiles.push_back(sRangeTile(0, 4, 320));
		Tiles.mTiles.push_back(sRangeTile(1, 4, 321));
		Tiles.mTiles.push_back(sRangeTile(2, 4, 322));
		Tiles.mTiles.push_back(sRangeTile(3, 4, 323));

		mCursorSurface= new cSurface(16 * 4, 16 * 5);
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Int) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 246));
		Tiles.mTiles.push_back(sRangeTile(0, 1, 266));


		mCursorSurface= new cSurface(16 * 1, 16 * 2);
	}

	return Tiles;
}

sTiles cOFED::SetupBarracks() {
	sTiles Tiles(true);

	if (g_Fodder.mMap_TileSet  == eTileTypes_Jungle) {
		Tiles.mTiles.push_back(sRangeTile(1, 0, 333));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 334));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 352));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 353));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 354));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 372));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 373));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 374));
		Tiles.mTiles.push_back(sRangeTile(3, 2, 375));

		Tiles.mTiles.push_back(sRangeTile(0, 3, 392));
		Tiles.mTiles.push_back(sRangeTile(1, 3, 393));
		Tiles.mTiles.push_back(sRangeTile(2, 3, 394));

		mCursorSurface= new cSurface(16 * 4, 16 * 4);
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Ice) {
		Tiles.mTiles.push_back(sRangeTile(1, 0, 245));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 246));
		Tiles.mTiles.push_back(sRangeTile(3, 0, 247));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 264));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 265));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 266));
		Tiles.mTiles.push_back(sRangeTile(3, 1, 267));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 284));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 285));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 286));
		Tiles.mTiles.push_back(sRangeTile(3, 2, 287));

		mCursorSurface= new cSurface(16 * 4, 16 * 3);
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Desert) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 196));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 197));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 198));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 216));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 217));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 218));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 236));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 237));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 238));

		mCursorSurface= new cSurface(16 * 3, 16 * 3);
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Moors) {
		Tiles.mTiles.push_back(sRangeTile(1, 0, 335));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 336));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 354));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 355));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 356));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 374));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 375));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 376));

		Tiles.mTiles.push_back(sRangeTile(0, 3, 394));
		Tiles.mTiles.push_back(sRangeTile(1, 3, 395));
		Tiles.mTiles.push_back(sRangeTile(2, 3, 396));

		mCursorSurface= new cSurface(16 * 3, 16 * 4);
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Int) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 246));
		Tiles.mTiles.push_back(sRangeTile(0, 1, 266));

		mCursorSurface= new cSurface(16 * 1, 16 * 2);
	}

	return Tiles;
}


sTiles cOFED::SetupBunker() {
	sTiles Tiles(true);
	
	if (g_Fodder.mMap_TileSet  == eTileTypes_Jungle && mMapSub == 0) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 267));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 268));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 269));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 287));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 288));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 289));
		Tiles.mTiles.push_back(sRangeTile(3, 1, 330));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 307));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 308));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 309));
		Tiles.mTiles.push_back(sRangeTile(3, 2, 350));

		Tiles.mTiles.push_back(sRangeTile(0, 3, 315));
		Tiles.mTiles.push_back(sRangeTile(1, 3, 316));
		Tiles.mTiles.push_back(sRangeTile(2, 3, 317));
		Tiles.mTiles.push_back(sRangeTile(3, 3, 371));

		mCursorSurface = new cSurface(16 * 4, 16 * 4);
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Desert) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 9));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 10));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 11));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 29));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 30));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 31));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 49));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 50));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 51));

		mCursorSurface= new cSurface(16 * 3, 16 * 3);
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Ice) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 307));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 308));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 309));
		Tiles.mTiles.push_back(sRangeTile(3, 0, 310));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 327));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 328));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 329));
		Tiles.mTiles.push_back(sRangeTile(3, 1, 330));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 347));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 348));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 349));
		Tiles.mTiles.push_back(sRangeTile(3, 2, 350));

		Tiles.mTiles.push_back(sRangeTile(0, 3, 367));
		Tiles.mTiles.push_back(sRangeTile(1, 3, 368));
		Tiles.mTiles.push_back(sRangeTile(2, 3, 369));
		Tiles.mTiles.push_back(sRangeTile(3, 3, 370));

		mCursorSurface= new cSurface(16 * 4, 16 * 4);
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Moors) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 160));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 161));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 162));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 180));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 181));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 182));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 200));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 201));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 202));

		mCursorSurface= new cSurface(16 * 3, 16 * 3);
	}


	if (g_Fodder.mMap_TileSet  == eTileTypes_Int) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 331));
		Tiles.mTiles.push_back(sRangeTile(0, 1, 351));

		mCursorSurface= new cSurface(16 * 1, 16 * 2);
	}

	return Tiles;
}

void cOFED::AddHut_With_Soldier() {

	sTiles Tiles = SetupHut();

	if (g_Fodder.mMap_TileSet == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(20, 27, eSprite_BuildingDoor2));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(35, 40, eSprite_BuildingDoor2));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Ice) {
		// In this case 'Shrub' is roof
		Tiles.mSprites.push_back(sRangeSprite(20, 1, eSprite_Shrub));
		Tiles.mSprites.push_back(sRangeSprite(12, 23, eSprite_BuildingDoor2));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(28, 65, eSprite_BuildingDoor2));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 5, eSprite_BuildingDoor2));
	}

	setCursorTiles(Tiles);
	CursorUpdate();
}

void cOFED::AddHut_With_Indigenous() {
	sTiles Tiles = SetupHut();

	if (g_Fodder.mMap_TileSet  == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(20, 27, eSprite_Door_Indigenous));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(35, 40, eSprite_Door_Indigenous));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Ice) {
		// In this case 'Shrub' is roof
		Tiles.mSprites.push_back(sRangeSprite(20, 1, eSprite_Shrub));
		Tiles.mSprites.push_back(sRangeSprite(12, 23, eSprite_Door_Indigenous));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(28, 65, eSprite_Door_Indigenous));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 5, eSprite_Door_Indigenous));
	}

	setCursorTiles(Tiles);
	CursorUpdate();
}

void cOFED::AddHut_With_Indigenous_Spear() {
	sTiles Tiles = SetupHut();

	if (g_Fodder.mMap_TileSet  == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(20, 27, eSprite_Door_Indigenous_Spear));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(35, 40, eSprite_Door_Indigenous_Spear));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Ice) {
		// In this case 'Shrub' is roof
		Tiles.mSprites.push_back(sRangeSprite(20, 1, eSprite_Shrub));
		Tiles.mSprites.push_back(sRangeSprite(12, 23, eSprite_Door_Indigenous_Spear));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(28, 65, eSprite_Door_Indigenous_Spear));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 5, eSprite_Door_Indigenous_Spear));
	}
	setCursorTiles(Tiles);
	CursorUpdate();
}

void cOFED::AddBarracks_With_Soldier() {
	sTiles Tiles = SetupBarracks();

	if (g_Fodder.mMap_TileSet  == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(13, 2, eSprite_BuildingRoof));
		Tiles.mSprites.push_back(sRangeSprite(9, 34, eSprite_BuildingDoor));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(12, -15, eSprite_BuildingRoof));
		Tiles.mSprites.push_back(sRangeSprite(7, 16, eSprite_BuildingDoor));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Ice) {
		Tiles.mSprites.push_back(sRangeSprite(23, -5, eSprite_BuildingRoof));
		Tiles.mSprites.push_back(sRangeSprite(20, 27, eSprite_BuildingDoor));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(15, 1, eSprite_BuildingRoof));
		Tiles.mSprites.push_back(sRangeSprite(7, 33, eSprite_BuildingDoor));
	}

	if (g_Fodder.mMap_TileSet  == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 5, eSprite_BuildingDoor));
	}

	setCursorTiles(Tiles);
	CursorUpdate();
}

void cOFED::AddBunker_With_Soldier() {
	sTiles Tiles = SetupBunker();

	if (g_Fodder.mMap_TileSet == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BuildingDoor3));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BuildingDoor3));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Ice) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BuildingDoor3));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BuildingDoor3));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 3, eSprite_BuildingDoor3));
	}

	setCursorTiles(Tiles);
	CursorUpdate();
}

void cOFED::AddBunker_With_SoldierReinforced() {
	sTiles Tiles = SetupBunker();

	if (g_Fodder.mMap_TileSet == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BuildingDoor_Reinforced));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BuildingDoor_Reinforced));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Ice) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BuildingDoor_Reinforced));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BuildingDoor_Reinforced));
	}

	if (g_Fodder.mMap_TileSet == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 3, eSprite_BuildingDoor_Reinforced));
	}

	setCursorTiles(Tiles);
	CursorUpdate();
}

void cOFED::AddCliff() {
	sTiles Tiles(true);
	CursorReset();

	if (g_Fodder.mMap_TileSet== eTileTypes_Ice) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 120));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 121));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 122));
		Tiles.mTiles.push_back(sRangeTile(3, 0, 123));
		Tiles.mTiles.push_back(sRangeTile(4, 0, 124));
		Tiles.mTiles.push_back(sRangeTile(5, 0, 125));
		Tiles.mTiles.push_back(sRangeTile(6, 0, 126));
		Tiles.mTiles.push_back(sRangeTile(7, 0, 127));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 140));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 141));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 142));
		Tiles.mTiles.push_back(sRangeTile(3, 1, 143));
		Tiles.mTiles.push_back(sRangeTile(4, 1, 144));
		Tiles.mTiles.push_back(sRangeTile(5, 1, 145));
		Tiles.mTiles.push_back(sRangeTile(6, 1, 146));
		Tiles.mTiles.push_back(sRangeTile(7, 1, 147));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 160));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 161));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 162));
		Tiles.mTiles.push_back(sRangeTile(3, 2, 163));
		Tiles.mTiles.push_back(sRangeTile(4, 2, 164));
		Tiles.mTiles.push_back(sRangeTile(5, 2, 165));
		Tiles.mTiles.push_back(sRangeTile(6, 2, 166));
		Tiles.mTiles.push_back(sRangeTile(7, 2, 167));

		Tiles.mTiles.push_back(sRangeTile(0, 3, 180));
		Tiles.mTiles.push_back(sRangeTile(1, 3, 181));
		Tiles.mTiles.push_back(sRangeTile(2, 3, 182));
		Tiles.mTiles.push_back(sRangeTile(3, 3, 183));
		Tiles.mTiles.push_back(sRangeTile(4, 3, 184));
		Tiles.mTiles.push_back(sRangeTile(5, 3, 185));
		Tiles.mTiles.push_back(sRangeTile(6, 3, 186));
		Tiles.mTiles.push_back(sRangeTile(7, 3, 187));

		Tiles.mTiles.push_back(sRangeTile(0, 4, 200));
		Tiles.mTiles.push_back(sRangeTile(1, 4, 201));
		Tiles.mTiles.push_back(sRangeTile(2, 4, 202));
		Tiles.mTiles.push_back(sRangeTile(3, 4, 203));
		Tiles.mTiles.push_back(sRangeTile(4, 4, 204));
		Tiles.mTiles.push_back(sRangeTile(5, 4, 205));
		Tiles.mTiles.push_back(sRangeTile(6, 4, 206));
		Tiles.mTiles.push_back(sRangeTile(7, 4, 207));

		mCursorSurface = new cSurface(16 * 8, 16 * 5);
	}

	if (g_Fodder.mMap_TileSet== eTileTypes_Jungle) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 89));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 90));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 91));
		Tiles.mTiles.push_back(sRangeTile(3, 0, 92));
		Tiles.mTiles.push_back(sRangeTile(4, 0, 93));
		Tiles.mTiles.push_back(sRangeTile(5, 0, 94));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 109));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 110));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 111));
		Tiles.mTiles.push_back(sRangeTile(3, 1, 112));
		Tiles.mTiles.push_back(sRangeTile(4, 1, 113));
		Tiles.mTiles.push_back(sRangeTile(5, 1, 114));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 129));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 130));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 131));
		Tiles.mTiles.push_back(sRangeTile(3, 2, 132));
		Tiles.mTiles.push_back(sRangeTile(4, 2, 133));
		Tiles.mTiles.push_back(sRangeTile(5, 2, 134));

		Tiles.mTiles.push_back(sRangeTile(0, 3, 149));
		Tiles.mTiles.push_back(sRangeTile(1, 3, 150));
		Tiles.mTiles.push_back(sRangeTile(2, 3, 151));
		Tiles.mTiles.push_back(sRangeTile(3, 3, 152));
		Tiles.mTiles.push_back(sRangeTile(4, 3, 153));
		Tiles.mTiles.push_back(sRangeTile(5, 3, 154));

		Tiles.mTiles.push_back(sRangeTile(0, 4, 169));
		Tiles.mTiles.push_back(sRangeTile(1, 4, 170));
		Tiles.mTiles.push_back(sRangeTile(2, 4, 171));
		Tiles.mTiles.push_back(sRangeTile(3, 4, 172));
		Tiles.mTiles.push_back(sRangeTile(4, 4, 173));
		Tiles.mTiles.push_back(sRangeTile(5, 4, 174));

		Tiles.mTiles.push_back(sRangeTile(0, 5, 189));
		Tiles.mTiles.push_back(sRangeTile(1, 5, 190));
		Tiles.mTiles.push_back(sRangeTile(2, 5, 191));
		Tiles.mTiles.push_back(sRangeTile(3, 5, 192));
		Tiles.mTiles.push_back(sRangeTile(4, 5, 193));
		Tiles.mTiles.push_back(sRangeTile(5, 5, 194));

		Tiles.mTiles.push_back(sRangeTile(4, 6, 213));
		Tiles.mTiles.push_back(sRangeTile(5, 6, 214));

		mCursorSurface = new cSurface(16 * 6, 16 * 6);
	}


	if (g_Fodder.mMap_TileSet== eTileTypes_Desert) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 80));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 81));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 82));
		Tiles.mTiles.push_back(sRangeTile(3, 0, 83));
		Tiles.mTiles.push_back(sRangeTile(4, 0, 84));
		Tiles.mTiles.push_back(sRangeTile(5, 0, 85));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 100));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 101));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 102));
		Tiles.mTiles.push_back(sRangeTile(3, 1, 103));
		Tiles.mTiles.push_back(sRangeTile(4, 1, 104));
		Tiles.mTiles.push_back(sRangeTile(5, 1, 105));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 120));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 121));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 122));
		Tiles.mTiles.push_back(sRangeTile(3, 2, 123));
		Tiles.mTiles.push_back(sRangeTile(4, 2, 124));
		Tiles.mTiles.push_back(sRangeTile(5, 2, 125));

		Tiles.mTiles.push_back(sRangeTile(0, 3, 140));
		Tiles.mTiles.push_back(sRangeTile(1, 3, 141));
		Tiles.mTiles.push_back(sRangeTile(2, 3, 142));
		Tiles.mTiles.push_back(sRangeTile(3, 3, 143));
		Tiles.mTiles.push_back(sRangeTile(4, 3, 144));
		Tiles.mTiles.push_back(sRangeTile(5, 3, 145));

		Tiles.mTiles.push_back(sRangeTile(0, 4, 160));
		Tiles.mTiles.push_back(sRangeTile(1, 4, 161));
		Tiles.mTiles.push_back(sRangeTile(2, 4, 162));
		Tiles.mTiles.push_back(sRangeTile(3, 4, 163));
		Tiles.mTiles.push_back(sRangeTile(4, 4, 164));
		Tiles.mTiles.push_back(sRangeTile(5, 4, 165));

		Tiles.mTiles.push_back(sRangeTile(0, 5, 180));
		Tiles.mTiles.push_back(sRangeTile(1, 5, 181));
		Tiles.mTiles.push_back(sRangeTile(2, 5, 182));
		Tiles.mTiles.push_back(sRangeTile(3, 5, 183));
		Tiles.mTiles.push_back(sRangeTile(4, 5, 184));
		Tiles.mTiles.push_back(sRangeTile(5, 5, 185));

		Tiles.mTiles.push_back(sRangeTile(0, 6, 200));
		Tiles.mTiles.push_back(sRangeTile(1, 6, 201));
		Tiles.mTiles.push_back(sRangeTile(2, 6, 202));
		Tiles.mTiles.push_back(sRangeTile(3, 6, 203));
		Tiles.mTiles.push_back(sRangeTile(4, 6, 204));
		Tiles.mTiles.push_back(sRangeTile(5, 6, 205));

		Tiles.mTiles.push_back(sRangeTile(0, 7, 220));
		Tiles.mTiles.push_back(sRangeTile(1, 7, 221));
		Tiles.mTiles.push_back(sRangeTile(2, 7, 222));
		Tiles.mTiles.push_back(sRangeTile(3, 7, 223));
		Tiles.mTiles.push_back(sRangeTile(4, 7, 224));
		Tiles.mTiles.push_back(sRangeTile(5, 7, 225));

		mCursorSurface = new cSurface(16 * 6, 16 * 8);
	}

	if(Tiles.mTiles.size())
		setCursorTiles(Tiles);

	CursorUpdate();
}

/**
* Show the Load Map Dialog
*/
void cOFED::ShowDialog_LoadMap() {

	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Load Map"), "",
		tr("Open Fodder (*.map);;All Files (*)"));

	CursorReset();

	if (!fileName.size())
		return;

	g_Fodder.mCampaign.LoadCustomFromPath(fileName.toStdString());

	g_Fodder.Map_Load();
	g_Fodder.Map_Load_Sprites();
	g_Fodder.MapTiles_Draw();

	g_Graphics.PaletteSet();
	g_Fodder.mImage->surfaceSetToPaletteNew();

	g_Window.FrameEnd();

	mToolboxSprites->RenderSprites();
	mToolboxTiles->RenderTiles();
}


/**
 * Show the Save Map Dialog
 */
void cOFED::ShowDialog_SaveMap() {

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save Map"), "",
		tr("Open Fodder (*.map);;All Files (*)"));

	if (!fileName.size())
		return;

	g_Fodder.Map_Save(fileName.toStdString());
}

/**
 * Create a new map
 */
void cOFED::Create_NewMap(const std::string& pTileSet, const std::string& pTileSub, size_t pWidth, size_t pHeight) {
	
	// Loop each known tile type
	for (auto TileType : mTileTypes) {

		if (pTileSet == TileType.mFullName) {
			mMapSub = 0;

			if (pTileSub == "Sub1")
				mMapSub = 1;

			CursorReset();

			g_Fodder.Map_Create(TileType, mMapSub, pWidth, pHeight);
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
	if (mCursorRangeTiles.mTiles.size()) {
		mCursorImageFinal = QImage(mCursorImage.width() * ScaleWidth, mCursorImage.height() * ScaleHeight, QImage::Format_RGB32);
		
		QPainter painter(&mCursorImageFinal);
		QRectF Dest(0, 0, mCursorImageFinal.width(), mCursorImageFinal.height());
		QRectF Src(0, 0, mCursorImage.width(), mCursorImage.height());
		painter.drawImage(Dest, mCursorImage, Src);
	}
	else {
		this->setCursor(Qt::ArrowCursor);
		return;
	}

	// Set the actual cursor
	if(!mCursorImageFinal.isNull())
		setCursor(QCursor(QPixmap::fromImage(mCursorImageFinal)));
}

/**
 * Set the cursor to a tile
 */
void cOFED::SetCursorTileID(const size_t pCursorTileID) {
	sTiles Tiles;
	Tiles.mTiles.push_back(sRangeTile(0,0, pCursorTileID));

	setCursorTiles(Tiles);
}

void cOFED::setCursorTiles( sTiles& pTiles) {

	CursorReset();

	for ( const auto& Tile : pTiles.mTiles) {
		g_Graphics.Map_Tile_Draw(mCursorSurface, Tile.mTileID, Tile.mX, Tile.mY, 0);
	}
	g_Graphics.PaletteSet(mCursorSurface);

	mCursorSurface->surfaceSetToPaletteNew();
	mCursorSurface->draw();

	SDL_Surface* Source = mCursorSurface->GetSurface();
	mCursorImage = QImage(static_cast<uchar*>(Source->pixels), Source->w, Source->h, QImage::Format_RGB32);

	mCursorRangeTiles = pTiles;
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
