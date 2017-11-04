#include "ofed.hpp"
#include "ui_NewMapDialog.h"
#include "ui_ToolboxTiles.h"

cOFED::cOFED(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Menu Items
	QObject::connect(ui.action_New_Map, &QAction::triggered, this, &cOFED::ShowDialog_NewMap);

	// Prepare OpenFOdder
	cFodder* Fodder = new cFodder(g_Window.GetSingletonPtr());

	Fodder->Prepare();

	if (!Fodder->mVersions.size()) {

		// TODO: Show a message?
		return;
	}

	Fodder->VersionSelect_0();

	Fodder->Mouse_Setup();
	Fodder->Mouse_Inputs_Get();

	OpenFodder_Prepare();

	ShowDialog_ToolboxTiles();
}

void cOFED::OpenFodder_Prepare() {
	cFodder* Fodder = &g_Fodder;
	Fodder->Game_Setup(0);

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

	Fodder->Map_Tiles_Draw();
	Fodder->Camera_Reset();

	Fodder->Mouse_Inputs_Get();
	Fodder->sub_18D5E();

	Fodder->mCamera_Start_Adjust = 1;
	Fodder->mCamera_Position_X = Fodder->mSprites[0].field_0;
	Fodder->mCamera_Position_Y = Fodder->mSprites[0].field_4;
	Fodder->word_3ABE9 = 0;
	Fodder->word_3ABEB = 0;
	Fodder->word_3ABE7 = 0;
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

	while (Fodder->mImage->palette_FadeTowardNew());

	Fodder->Map_Tiles_Draw();

	// Set the top left
	Fodder->mMapTilePtr = 0x5E - (Fodder->mMapWidth << 1);
	g_Graphics.Map_Tiles_Draw();
	g_Window.FrameEnd();
}

void cOFED::moveEvent(QMoveEvent *event) {
	
	if(mToolboxTiles)
		mToolboxTiles->move(x() + width(), y());
}

/**
 * Launch the Tile Toobox dialog 
 */
void cOFED::ShowDialog_ToolboxTiles() {
	Ui_ToolboxTiles* ui = new Ui_ToolboxTiles();

	mToolboxTiles = new cToolboxTiles(ui, this, 0);

	ui->setupUi(mToolboxTiles);

	mToolboxTiles->show();
}

/**
 * Launch the New Map dialog
 */
void cOFED::ShowDialog_NewMap() {
	Ui_NewMapDialog* ui = new Ui_NewMapDialog();

	cNewMapDialog* NewMap = new cNewMapDialog(ui, this, 0);

	ui->setupUi(NewMap);

	// Add available terrain types
	for (auto TileType : mTileTypes)
		ui->mTerrainType->addItem(TileType.mFullName.c_str());
	
	// Add sub terrain types
	ui->mTileSub->addItem("Sub0");
	ui->mTileSub->addItem("Sub1");

	// 19 x 15 Map Default
	ui->mWidth->setText("19");
	ui->mHeight->setText("15");

	NewMap->show();
}

void cOFED::Create_NewMap(const std::string& pTileSet, const std::string& pTileSub, size_t pWidth, size_t pHeight) {
	
	// Loop each known tile type
	for (auto TileType : mTileTypes) {

		if (pTileSet == TileType.mFullName) {
			size_t Sub = 0;

			if (pTileSub == "Sub1")
				Sub = 1;

			g_Fodder.Map_Create(TileType, Sub, pWidth, pHeight);
			g_Window.FrameEnd();
			return;
		}
	}
}
