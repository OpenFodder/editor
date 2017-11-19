#pragma once

#include "ui_ofed.h"

struct sRangeTile {
	int16 mX;
	int16 mY;
	uint16 mTileID;

	sRangeTile(int16 pMapX, int16 pMapY, uint16 pID) {
		mX = pMapX;
		mY = pMapY;
		mTileID = pID;
	}

};

struct sRangeSprite {
	int16 mX;
	int16 mY;
	uint16 mSpriteID;

	sRangeSprite(int16 pTileX, int16 pTileY, uint16 pSpriteID) {
		mX = pTileX;
		mY = pTileY;
		mSpriteID = pSpriteID;
	}
};

struct sTiles {
	std::vector<sRangeTile> mTiles;
	std::vector<sRangeSprite> mSprites;
};

extern int32 g_SpriteAnim[111];
extern std::string g_SpriteName[111];

class cOFED : public QMainWindow
{
	Q_OBJECT


protected:
	void moveEvent(QMoveEvent *event);

public:
	cOFED(QWidget *parent = Q_NULLPTR);

	void OpenFodder_Prepare();

	void ShowDialog_LoadMap();
	void ShowDialog_SaveMap();

	void ShowDialog_NewMap();

	void ShowDialog_ToolboxTiles();
	void ShowDialog_ToolboxSprites();

	void Create_NewMap(const std::string& pTileSet, const std::string& pTileSub, size_t pWidth, size_t pHeight);

	void SetCursorTileID(const size_t pCursorTile);
	void CursorUpdate();
	void CursorReset();

	int16 CursorTileGet() { return mCursorTile; }

	void SetupSprites();

private:
	Ui::mOFED ui;
	cToolboxTiles*	mToolboxTiles;
	cToolboxSprites* mToolboxSprites;

	int16	mCursorTile;
	int16	mCursorSprite;
	sTiles	mCursorRangeTiles;

	cSurface		*mCursorSurface;
	QImage			 mCursorImage;
	QImage			 mCursorImageFinal;
};
