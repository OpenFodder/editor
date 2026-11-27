#pragma once

#include "stdafx_ofed.hpp"
#include "ui_ofed.h"

class QListWidget;

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


extern int32 g_SpriteAnim[118];
extern std::string g_SpriteName[118];
extern std::shared_ptr<sFodderParameters> g_Parameters;

bool IsPlaceableSprite(size_t type);

class cOFED : public QMainWindow
{
	Q_OBJECT


public:
	cOFED(QWidget *parent = nullptr);
    ~cOFED();

	void OpenFodder_Prepare();

    void Mission_AddNew();
    void Phase_AddNew();
    void Sprite_AddNew();

    void ShowDialog_NewCampaign();
    void ShowDialog_LoadCampaign();
    void ShowDialog_SaveCampaign();

	void ShowDialog_LoadMap();
	void ShowDialog_SaveMap();

	void ShowDialog_NewMap();



    void LoadMap();

	void Create_NewMap(const std::string& pTileSet, const std::string& pTileSub, size_t pWidth, size_t pHeight, const bool pRandom);
    bool SaveMapTo(const std::string& path);
    void Save_Map(std::shared_ptr<cPhase> pPhase);

	void setCursorTiles(const sTiles& pTiles, bool fromPalette = false);

    void SetCursorSprite( const size_t pSpriteID );

	void CursorUpdate();
	void CursorReset();

	void SetupSprites();

	const sTiles& GetCursorRangeTiles() const { return mCursorRangeTiles; }
    int16 GetCursorSpriteID() { return mCursorSprite; }

private:
    sTiles SetupHut();
    sTiles SetupBarracks();
    sTiles SetupBunker();
    sTiles HutWithSoldier();
    sTiles HutWithCivilian();
    sTiles HutWithSpearCivilian();
    sTiles Barracks();
    sTiles Bunker();
    sTiles ReinforcedBunker();
    sTiles Cliff();
    void RenderStructures();
    void ClearStructureSelection();
    void UpdateMapStatus();

	Ui::mOFED ui;
    cWindowQT* mMapWindow;
	cMapView* mMapView = nullptr;
    cCampaignDialog* mToolboxCampaigns = nullptr;
	cToolboxTiles* mToolboxTiles = nullptr;
	cToolboxSprites* mToolboxSprites = nullptr;
    QListWidget* mStructurePalette = nullptr;
    std::vector<sTiles> mStructureBrushes;

    QLabel* mMissionLabel;
    QLabel* mPhaseLabel;
    QLabel* mZoomLabel;

	int16	mCursorSprite = -1;
	sTiles	mCursorRangeTiles;

};
