#pragma once

#include "ui_ofed.h"
#include <QDialog>

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

class cOFED : public QMainWindow
{
	Q_OBJECT


protected:
	void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

public:
	cOFED(QWidget *parent = nullptr);
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


	void ShowDialog_MapOverview();

    void ShowDialog_NewCampaign();
    void ShowDialog_LoadCampaign();
    void ShowDialog_SaveCampaign();

	void ShowDialog_LoadMap();
	void ShowDialog_SaveMap();

	void ShowDialog_NewMap();

	void ShowDialog_ToolboxSprites();
    void ShowDialog_ToolboxCampaigns();

    void CloseDialog_ToolboxCampaigns();

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
    void ScheduleToolWindowLayout();
    void LayoutToolWindows();
    void PrepareToolWindow(QDialog* window, const QString& title);
    bool mToolLayoutPending = false;
    bool mLayingOutTools = false;

	Ui::mOFED ui;
	cMapView* mMapView;
    cCampaignDialog* mToolboxCampaigns;
	cToolboxTiles*	mToolboxTiles;
	cToolboxSprites* mToolboxSprites;

    QLabel* mMissionLabel;
    QLabel* mPhaseLabel;
    QLabel* mZoomLabel;
    QDialog* mOverviewTool = nullptr;

	int16	mCursorSprite;
	sTiles	mCursorRangeTiles;

	std::shared_ptr<cSurface> mCursorSurface;
	QImage			 mCursorImage;
	QImage			 mCursorImageFinal;

	size_t			 mMapSub;
};
