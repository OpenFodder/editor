#include "ofed.hpp"
#include "ui_NewMapDialog.h"
#include "ui_ToolboxTiles.h"
#include <QDesktopWidget>
#include <qpainter.h>

cOFED::cOFED(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	mToolboxTiles = 0;
	mCursorSurface = new cSurface(16, 16);

	CursorReset();

	// Menu Items
	QObject::connect(ui.action_New_Map, &QAction::triggered, this, &cOFED::ShowDialog_NewMap);

	// Prepare OpenFodder
	cFodder* Fodder = new cFodder(g_Window.GetSingletonPtr());
	Fodder->Prepare();

	if (!Fodder->mVersions.size()) {

		// TODO: Show a message that no data was found
		return;
	}

	Fodder->VersionSelect_2();

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

	Fodder->mImage->surfaceSetToPaletteNew();

	//Fodder->Map_Tiles_Draw();

	// Set the top left
	Fodder->mMapTilePtr = 0x60 - Fodder->mMapWidth * 2;
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

	mToolboxTiles = new cToolboxTiles(this, 0);

	// Position to the right of map editor
	mToolboxTiles->move(x() + width(), y());

	mToolboxTiles->show();
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
