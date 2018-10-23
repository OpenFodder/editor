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

	bool mOnce;

	sTiles( bool pOnce = false ) { mOnce = pOnce;  }
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
    ~cOFED();

	void OpenFodder_Prepare();

	sTiles SetupHut();
	sTiles SetupBarracks();
	sTiles SetupBunker();

    void Mission_AddNew();
    void Phase_AddNew();
    void Sprite_AddNew();

	void AddHut_With_Soldier();
	void AddHut_With_Indigenous();
	void AddHut_With_Indigenous_Spear();

	void AddBarracks_With_Soldier();

	void AddBunker_With_Soldier();
	void AddBunker_With_SoldierReinforced();

	void AddCliff();

    void ShowDialog_NewCampaign();
    void ShowDialog_LoadCampaign();
    void ShowDialog_SaveCampaign();

	void ShowDialog_LoadMap();
	void ShowDialog_SaveMap();

	void ShowDialog_NewMap();

	void ShowDialog_ToolboxTiles();
	void ShowDialog_ToolboxSprites();
    void ShowDialog_ToolboxCampaigns();

    void CloseDialog_ToolboxCampaigns();

    void LoadMap();

	void Create_NewMap(const std::string& pTileSet, const std::string& pTileSub, size_t pWidth, size_t pHeight, const bool pRandom);
    void Save_Map(std::shared_ptr<cPhase> pPhase);

	void SetCursorTileID(const size_t pCursorTile);
	void setCursorTiles( sTiles& pTiles);

    void SetCursorSprite( const size_t pSpriteID );

	void CursorUpdate();
	void CursorReset();

	void SetupSprites();

	sTiles GetCursorRangeTiles() { return mCursorRangeTiles; }
    int16 GetCursorSpriteID() { return mCursorSprite; }

private:
	Ui::mOFED ui;
    cCampaignDialog* mToolboxCampaigns;
	cToolboxTiles*	mToolboxTiles;
	cToolboxSprites* mToolboxSprites;

    QLabel* mMissionLabel;
    QLabel* mPhaseLabel;

	int16	mCursorSprite;
	sTiles	mCursorRangeTiles;

	std::shared_ptr<cSurface> mCursorSurface;
	QImage			 mCursorImage;
	QImage			 mCursorImageFinal;

	size_t			 mMapSub;
};
