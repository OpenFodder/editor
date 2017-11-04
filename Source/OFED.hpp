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

class cOFED : public QMainWindow
{
	Q_OBJECT


protected:
	void moveEvent(QMoveEvent *event);

public:
	cOFED(QWidget *parent = Q_NULLPTR);

	void OpenFodder_Prepare();

	void ShowDialog_NewMap();
	void ShowDialog_ToolboxTiles();

	void Create_NewMap(const std::string& pTileSet, const std::string& pTileSub, size_t pWidth, size_t pHeight);

	void SetCursorTileID(const size_t pCursorTile);
	void CursorUpdate();
	void CursorReset();

	int16 CursorTileGet() { return mCursorTile; }

private:
	Ui::mOFED ui;
	cToolboxTiles*	mToolboxTiles;

	int16	mCursorTile;
	int16	mCursorSprite;
	sTiles	mCursorRangeTiles;

	cSurface		*mCursorSurface;
	QImage			 mCursorImage;
	QImage			 mCursorImageFinal;
};
