#include "ofed.hpp"
#include "ui_NewMapDialog.h"

#include <QPainter>
#include <QDockWidget>
#include <QDir>
#include <QMessageBox>
#include <QListWidget>
#include <QPixmap>
#include <QSignalBlocker>
#include <QShortcut>
#include <QScreen>
#include <QTabWidget>
#include <QFileDialog>
#include <QToolBar>
#include <QSplitter>
#include <chrono>
#include <iterator>

int32 g_SpriteAnim[118] = {};
std::string g_SpriteName[118] = {};

bool IsPlaceableSprite(size_t type) {
    // The engine uses a shared map format. The loaded game version determines
    // which sprite set can be used, including when editing a standalone map.
    return type < std::size(g_SpriteAnim) && g_SpriteAnim[type] >= 0 &&
           (type < eSprite_UFO_Callpad ||
            (g_Fodder && g_Fodder->mVersionCurrent && g_Fodder->mVersionCurrent->isCannonFodder2()));
}


cOFED::~cOFED() {
    mMapWindow->setParent(nullptr);
}

cOFED::cOFED(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);

    auto* editorPanes = new QSplitter(Qt::Horizontal, ui.centralWidget);
    editorPanes->setChildrenCollapsible(false);
    ui.verticalLayout->addWidget(editorPanes);

    auto* mapPane = new QWidget(editorPanes);
    auto* mapLayout = new QVBoxLayout(mapPane);
    mapLayout->setContentsMargins(0, 0, 0, 0);
    mapLayout->setSpacing(4);
    auto* navigation = new QToolBar(tr("Map navigation"), mapPane);
    navigation->setObjectName("mapNavigation");
    navigation->setMovable(false);
    mapLayout->addWidget(navigation);
    auto window = std::make_shared<cWindowQT>(mapPane);
    g_Window = window;
    mMapWindow = window.get();
    mapLayout->addWidget(mMapWindow, 1);
    editorPanes->addWidget(mapPane);

	SetupSprites();

    mMissionLabel = new QLabel();
    mPhaseLabel = new QLabel();

    statusBar()->addWidget(mMissionLabel);
    statusBar()->addWidget(mPhaseLabel);
    mZoomLabel = new QLabel();
    statusBar()->addPermanentWidget(mZoomLabel);
    connect(mMapWindow, &cWindowQT::viewChanged, this, [this, window]() {
        mZoomLabel->setText(tr("%1%").arg(qRound(window->Zoom() * 100)));
    });
    auto zoomOut = navigation->addAction(tr("Zoom out"));
    zoomOut->setShortcut(QKeySequence(Qt::Key_Minus));
    connect(zoomOut, &QAction::triggered, this, [window]() { window->SetViewZoom(window->Zoom() / 1.25, window->rect().center()); });
    auto zoomIn = navigation->addAction(tr("Zoom in"));
    zoomIn->setShortcut(QKeySequence(Qt::Key_Plus));
    connect(zoomIn, &QAction::triggered, this, [window]() { window->SetViewZoom(window->Zoom() * 1.25, window->rect().center()); });
    auto fit = navigation->addAction(tr("Fit map"));
    fit->setCheckable(true);
    fit->setChecked(mMapWindow->IsFittingMap());
    fit->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(fit, &QAction::triggered, window.get(), &cWindowQT::FitMap);
    connect(mMapWindow, &cWindowQT::viewChanged, fit, [fit, map = mMapWindow]() {
        fit->setChecked(map->IsFittingMap());
    });
    auto actual = navigation->addAction(tr("100%"));
    connect(actual, &QAction::triggered, this, [window]() { window->SetViewZoom(1.0, window->rect().center()); });

	// Menu Items
    QObject::connect(ui.actionCampaign_New, &QAction::triggered, this, &cOFED::ShowDialog_NewCampaign);
    QObject::connect(ui.actionCampaign_Load, &QAction::triggered, this, &cOFED::ShowDialog_LoadCampaign);
    QObject::connect(ui.actionCampaign_Save, &QAction::triggered, this, &cOFED::ShowDialog_SaveCampaign);

	QObject::connect(ui.action_New_Map, &QAction::triggered, this, &cOFED::ShowDialog_NewMap);
	QObject::connect(ui.action_Load_Map, &QAction::triggered, this, &cOFED::ShowDialog_LoadMap);
	QObject::connect(ui.action_Save_Map, &QAction::triggered, this, &cOFED::ShowDialog_SaveMap);
	QObject::connect(ui.actionExit, &QAction::triggered, this, &QWidget::close);

    QObject::connect(ui.actionNew_Mission, &QAction::triggered, this, &cOFED::Mission_AddNew);
    QObject::connect(ui.actionNew_Phase, &QAction::triggered, this, &cOFED::Phase_AddNew);

	g_Debugger = std::make_shared<cDebugger>();
    g_Fodder = std::make_shared<cFodder>(g_Window);

	// Prepare OpenFodder
    g_Fodder->Prepare(g_Parameters);

	if (!g_ResourceMan->isDataAvailable()) {

		// TODO: Show a message that no data was found
		return;
	}

    g_Fodder->VersionSwitch(g_Fodder->mVersions->GetRetail(ePlatform::Any, eGame::CF1));
    g_Fodder->mVersionDefault = g_Fodder->mVersionCurrent;

    g_Fodder->Mouse_Setup();
    g_Fodder->Mouse_ReadInputs();

    auto* assets = new QTabWidget(editorPanes);
    assets->setObjectName("assetTabs");
    mToolboxTiles = new cToolboxTiles(assets);
    mToolboxSprites = new cToolboxSprites(assets);
    assets->addTab(mToolboxTiles, tr("Tiles"));
    assets->addTab(mToolboxSprites, tr("Sprites"));
    mStructurePalette = new QListWidget(assets);
    mStructurePalette->setObjectName("structurePalette");
    mStructurePalette->setAccessibleName(tr("Structures"));
    mStructurePalette->setIconSize(QSize(64, 64));
    mStructurePalette->setSpacing(2);
    mStructurePalette->setUniformItemSizes(true);
    mStructurePalette->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    assets->addTab(mStructurePalette, tr("Structures"));
    connect(mStructurePalette, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row >= 0 && row < int(mStructureBrushes.size()) && !mStructureBrushes[row].mTiles.empty()) {
            setCursorTiles(mStructureBrushes[row]);
            statusBar()->showMessage(tr("Click the map to place %1.").arg(mStructurePalette->item(row)->text()));
        }
    });

    auto* clearBrush = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(clearBrush, &QShortcut::activated, this, &cOFED::CursorReset);

    assets->setMinimumWidth(210);
    editorPanes->addWidget(assets);
    editorPanes->setSizes({700, 420});
    editorPanes->setStretchFactor(0, 1);
    editorPanes->setStretchFactor(1, 1);

    OpenFodder_Prepare();

    auto* overviewDock = new QDockWidget(tr("Map overview"), this);
    overviewDock->setObjectName("mapOverviewDock");
    overviewDock->setFeatures(QDockWidget::DockWidgetClosable);
    mMapView = new cMapView(overviewDock);
    overviewDock->setWidget(mMapView);
    addDockWidget(Qt::LeftDockWidgetArea, overviewDock);

    auto* campaignDock = new QDockWidget(tr("Campaign"), this);
    campaignDock->setObjectName("campaignDock");
    campaignDock->setFeatures(QDockWidget::DockWidgetClosable);
    mToolboxCampaigns = new cCampaignDialog(campaignDock);
    connect(mToolboxCampaigns, &cCampaignDialog::namesChanged, this, &cOFED::UpdateMapStatus);
    campaignDock->setWidget(mToolboxCampaigns);
    splitDockWidget(overviewDock, campaignDock, Qt::Vertical);
    const QSize available = screen()->availableGeometry().size();
    resize(QSize(1640, 820).boundedTo(available));
    resizeDocks({overviewDock}, {300}, Qt::Horizontal);
    resizeDocks({overviewDock, campaignDock}, {175, 550}, Qt::Vertical);
    ui.menuView->addAction(campaignDock->toggleViewAction());

    ui.actionMap_Overview->setCheckable(true);
    ui.actionMap_Overview->setChecked(true);
    connect(ui.actionMap_Overview, &QAction::triggered, overviewDock, &QDockWidget::setVisible);
    connect(overviewDock, &QDockWidget::visibilityChanged, ui.actionMap_Overview, &QAction::setChecked);

    CursorReset();
    mMapWindow->CameraUpdate();

    // Use the tile viewport's actual height once the docks and tabs are laid out.
    QTimer::singleShot(0, this, [this, editorPanes, mapPane, assets]() {
        const auto sizes = editorPanes->sizes();
        const int space = sizes[0] + sizes[1];
        const int preferred = mToolboxTiles->WidthForFittedTiles() + assets->width() - mToolboxTiles->width();
        const int assetWidth = std::min(preferred, space - mapPane->minimumSizeHint().width());
        if (assetWidth > 0) editorPanes->setSizes({space - assetWidth, assetWidth});
    });
}

void cOFED::OpenFodder_Prepare() {

    if (!g_Fodder->Campaign_Load("Cannon Fodder")) {
        // TODO: Fail

        exit(1);
    }
    g_Fodder->Game_Setup();

	g_Fodder->Phase_EngineReset();
	g_Fodder->Phase_SquadPrepare();

    LoadMap();

	g_Fodder->mPhase_In_Progress = true;

    g_Fodder->Mission_Sprites_Handle(); 
	g_Fodder->mWindow->FrameEnd();


}

/**
 * Launch the New Map dialog
 */
void cOFED::ShowDialog_NewMap() {


    cNewMapDialog* NewMap = new cNewMapDialog(this);

    if (NewMap->exec()) {
        g_Fodder->mGame_Data.mCampaign.SetSingleMapCampaign();
        g_Fodder->mGame_Data.mCampaign.setCustomMap();
        g_Fodder->mGame_Data.mMission_Number = 1;
        g_Fodder->mGame_Data.mMission_Phase = 1;
        g_Fodder->mGame_Data.Phase_Start();

        g_Fodder->mGame_Data.mMission_Current->mName = "Single Mission";

        auto NewPhase = g_Fodder->mGame_Data.mMission_Current->mPhases.back();

        NewPhase->mName = "Single Phase";

        auto path = g_ResourceMan->getCustomMapPath() + g_Fodder->mGame_Data.mPhase_Current->mMapFilename;
		
        QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Map"), tr(path.c_str()),
            tr("Open Fodder (*.map);;All Files (*)"));

        if (!fileName.size())
            return;

        NewPhase->mMapFilename = fileName.toStdString().substr(0, fileName.size() - 4);

        SaveMapTo(fileName.toStdString());
        mToolboxCampaigns->LoadMissions();
        UpdateMapStatus();
    }
    else {

    }
    mToolboxCampaigns->RefreshSprites();
}

sTiles cOFED::SetupHut() {
	sTiles Tiles(true);

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Jungle) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 255));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 256));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 257));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 275));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 276));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 277));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 295));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 296));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 297));

	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Desert) {
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

	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Ice) {
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

	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Moors) {

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

	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Int) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 246));
		Tiles.mTiles.push_back(sRangeTile(0, 1, 266));


	}

	return Tiles;
}

sTiles cOFED::SetupBarracks() {
	sTiles Tiles(true);

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Jungle) {
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

	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Ice) {
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

	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Desert) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 196));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 197));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 198));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 216));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 217));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 218));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 236));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 237));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 238));

	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Moors) {
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

	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Int) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 246));
		Tiles.mTiles.push_back(sRangeTile(0, 1, 266));

	}

	return Tiles;
}


sTiles cOFED::SetupBunker() {
	sTiles Tiles(true);
	
	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Jungle && g_Fodder->mMapLoaded->getTileSub() == eTileSub_0) {
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

	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Desert) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 9));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 10));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 11));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 29));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 30));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 31));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 49));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 50));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 51));

	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Ice) {
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

	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Moors) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 160));
		Tiles.mTiles.push_back(sRangeTile(1, 0, 161));
		Tiles.mTiles.push_back(sRangeTile(2, 0, 162));

		Tiles.mTiles.push_back(sRangeTile(0, 1, 180));
		Tiles.mTiles.push_back(sRangeTile(1, 1, 181));
		Tiles.mTiles.push_back(sRangeTile(2, 1, 182));

		Tiles.mTiles.push_back(sRangeTile(0, 2, 200));
		Tiles.mTiles.push_back(sRangeTile(1, 2, 201));
		Tiles.mTiles.push_back(sRangeTile(2, 2, 202));

	}


	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Int) {
		Tiles.mTiles.push_back(sRangeTile(0, 0, 331));
		Tiles.mTiles.push_back(sRangeTile(0, 1, 351));

	}

	return Tiles;
}

void cOFED::Mission_AddNew() {
    if (!mToolboxCampaigns) return;
    auto mission = std::make_shared<cMission>();
    mission->mName = "New Mission";
    g_Fodder->mGame_Data.mCampaign.missionAdd(mission);
    mToolboxCampaigns->SelectNewMission();
}

void cOFED::Phase_AddNew() {
    if (!mToolboxCampaigns) return;
    const int missionNumber = mToolboxCampaigns->SelectedMissionNumber();
    if (!missionNumber) return;
    auto& data = g_Fodder->mGame_Data;
    const auto mission = data.mCampaign.getMission(missionNumber);

    cNewMapDialog newMap(this);
    if (!mission || !newMap.exec()) return;

    auto phase = std::make_shared<cPhase>();
    phase->mName = "New Phase";
    const auto timestamp = std::chrono::system_clock::now().time_since_epoch();
    phase->mMapFilename = "m" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timestamp).count());
    mission->mPhases.push_back(phase);

    data.mMission_Current = mission;
    data.mMission_Number = missionNumber;
    data.mPhase_Current = phase;
    data.mMission_Phase = mission->mPhases.size();
    data.mMission_Phases_Remaining = 1;
    Save_Map(phase);
    UpdateMapStatus();
    mToolboxCampaigns->LoadMissions();
}

void cOFED::Sprite_AddNew() {
    if (mToolboxCampaigns) mToolboxCampaigns->RefreshSprites();
    auto window = std::dynamic_pointer_cast<cWindowQT>(g_Window);
    window->mapEdited();
    window->update();
}

// The runtime owns edited sprites separately from the map's original sprite list.
// Serialize a snapshot so placements and deletions are included in the saved map.
bool cOFED::SaveMapTo(const std::string& path) {
    class cEditorMapSnapshot : public cOriginalMap {
    public:
        cEditorMapSnapshot(const std::shared_ptr<cMapData>& map, const std::vector<sSprite>& sprites) {
            mParams = *map->getMapParams();
            mData = map->getData();
            mSprites = sprites;
        }
    };
    cEditorMapSnapshot snapshot(g_Fodder->mMapLoaded, g_Fodder->mSprites);
    return snapshot.save(path, true);
}

void cOFED::Save_Map(std::shared_ptr<cPhase> phase) {
    if (!phase || !g_Fodder->mMapLoaded) return;

    const auto& campaign = g_Fodder->mGame_Data.mCampaign;
    const auto directory = QString::fromStdString(campaign.GetPath());
    const auto path = campaign.GetPathToFile(phase->mMapFilename + ".map");
    if ((!directory.isEmpty() && !QDir().mkpath(directory)) || !SaveMapTo(path))
        QMessageBox::warning(this, tr("Save map"), tr("Could not save %1.").arg(QString::fromStdString(path)));
}

sTiles cOFED::HutWithSoldier() {

	sTiles Tiles = SetupHut();

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(20, 27, eSprite_BuildingDoor_Hut));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(35, 40, eSprite_BuildingDoor_Hut));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Ice) {
		// In this case 'Shrub' is roof
		Tiles.mSprites.push_back(sRangeSprite(20, 1, eSprite_Shrub));
		Tiles.mSprites.push_back(sRangeSprite(12, 23, eSprite_BuildingDoor_Hut));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(28, 65, eSprite_BuildingDoor_Hut));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 5, eSprite_BuildingDoor_Hut));
	}

	return Tiles;
}

sTiles cOFED::HutWithCivilian() {
	sTiles Tiles = SetupHut();

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(20, 27, eSprite_Door_Civilian));
	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(35, 40, eSprite_Door_Civilian));
	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Ice) {
		// In this case 'Shrub' is roof
		Tiles.mSprites.push_back(sRangeSprite(20, 1, eSprite_Shrub));
		Tiles.mSprites.push_back(sRangeSprite(12, 23, eSprite_Door_Civilian));
	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(28, 65, eSprite_Door_Civilian));
	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 5, eSprite_Door_Civilian));
	}

	return Tiles;
}

sTiles cOFED::HutWithSpearCivilian() {
	sTiles Tiles = SetupHut();

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(20, 27, eSprite_Door_Civilian_Spear));
	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(35, 40, eSprite_Door_Civilian_Spear));
	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Ice) {
		// In this case 'Shrub' is roof
		Tiles.mSprites.push_back(sRangeSprite(20, 1, eSprite_Shrub));
		Tiles.mSprites.push_back(sRangeSprite(12, 23, eSprite_Door_Civilian_Spear));
	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(28, 65, eSprite_Door_Civilian_Spear));
	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 5, eSprite_Door_Civilian_Spear));
	}
	return Tiles;
}

sTiles cOFED::Barracks() {
	sTiles Tiles = SetupBarracks();

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(13, 2, eSprite_BuildingRoof));
		Tiles.mSprites.push_back(sRangeSprite(9, 34, eSprite_BuildingDoor));
	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(12, -15, eSprite_BuildingRoof));
		Tiles.mSprites.push_back(sRangeSprite(7, 16, eSprite_BuildingDoor));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Ice) {
		Tiles.mSprites.push_back(sRangeSprite(23, -5, eSprite_BuildingRoof));
		Tiles.mSprites.push_back(sRangeSprite(20, 27, eSprite_BuildingDoor));
	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(15, 1, eSprite_BuildingRoof));
		Tiles.mSprites.push_back(sRangeSprite(7, 33, eSprite_BuildingDoor));
	}

	if (g_Fodder->mMapLoaded->getTileType()   == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 5, eSprite_BuildingDoor));
	}

	return Tiles;
}

sTiles cOFED::Bunker() {
	sTiles Tiles = SetupBunker();

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BunkerDoor_HeavyExplosionOnly));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BunkerDoor_HeavyExplosionOnly));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Ice) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BunkerDoor_HeavyExplosionOnly));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BunkerDoor_HeavyExplosionOnly));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 3, eSprite_BunkerDoor_HeavyExplosionOnly));
	}

	return Tiles;
}

sTiles cOFED::ReinforcedBunker() {
	sTiles Tiles = SetupBunker();

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Jungle) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BunkerDoor_ReinforcedHeavyExplosionOnly));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Desert) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BunkerDoor_ReinforcedHeavyExplosionOnly));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Ice) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BunkerDoor_ReinforcedHeavyExplosionOnly));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Moors) {
		Tiles.mSprites.push_back(sRangeSprite(23, 32, eSprite_BunkerDoor_ReinforcedHeavyExplosionOnly));
	}

	if (g_Fodder->mMapLoaded->getTileType()  == eTileTypes_Int) {
		Tiles.mSprites.push_back(sRangeSprite(3, 3, eSprite_BunkerDoor_ReinforcedHeavyExplosionOnly));
	}

	return Tiles;
}

sTiles cOFED::Cliff() {
	sTiles Tiles(true);

	if (g_Fodder->mMapLoaded->getTileType() == eTileTypes_Ice) {
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

	}

	if (g_Fodder->mMapLoaded->getTileType() == eTileTypes_Jungle) {
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

	}


	if (g_Fodder->mMapLoaded->getTileType() == eTileTypes_Desert) {
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

	}

	return Tiles;
}

void cOFED::ShowDialog_NewCampaign() {

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Create Campaign"), tr(g_ResourceMan->GetCampaignData("Cannon Fodder").c_str()),
        tr("Open Fodder Campaign (*.ofc);"));

    std::string filename = fileName.toStdString();
    if (!filename.size())
        return;

    auto a = filename.find_last_of(".ofc");
    filename.erase(filename.begin() + a - 3, filename.end());

    auto& data = g_Fodder->mGame_Data;
    data.mCampaign.Clear(filename, true);
    data.mMission_Current.reset();
    data.mPhase_Current.reset();
    data.mMission_Number = 0;
    data.mMission_Phase = 0;
    UpdateMapStatus();
    mToolboxCampaigns->LoadMissions();

    ShowDialog_SaveCampaign();
}

void cOFED::ShowDialog_LoadCampaign() {

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Load Campaign"), tr(g_ResourceMan->GetCampaignData("Cannon Fodder").c_str()),
        tr("Open Fodder Campaign (*.ofc);"));

    std::string filename = fileName.toStdString();
    if (!filename.size())
        return;

    auto a = filename.find_last_of(".ofc");
    filename.erase(filename.begin() + a - 3, filename.end());

    g_Fodder->mGame_Data.mCampaign.LoadCampaign(filename, true, true);
    g_Fodder->mGame_Data.mMission_Current = 0;
    g_Fodder->mGame_Data.mPhase_Current = 0;
    g_Fodder->mGame_Data.mMission_Phases_Remaining = 1;
    g_Fodder->mGame_Data.mMission_Number = 0;
    g_Fodder->mGame_Data.mMission_Phase = 0;
    g_Fodder->mGame_Data.Phase_Start();

    // Disable this, otherwise it reloads the current version because of the current map
    g_Fodder->mPhase_In_Progress = false;
    g_Fodder->VersionSwitch(g_Fodder->mVersions->GetForCampaign(g_Fodder->mGame_Data.mCampaign.getName()));
    g_Fodder->mPhase_In_Progress = true;

    LoadMap();

    mToolboxCampaigns->LoadMissions();
}

void cOFED::ShowDialog_SaveCampaign() {

    g_Fodder->mGame_Data.mCampaign.SaveCampaign();
}

/**
* Show the Load Map Dialog
*/
void cOFED::ShowDialog_LoadMap() {

	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Load Map"), tr(g_ResourceMan->getCustomMapPath().c_str()),
		tr("Open Fodder (*.map);;All Files (*)"));

	CursorReset();

	if (!fileName.size())
		return;

	g_Fodder->mGame_Data.mCampaign.LoadCustomMapFromPath(fileName.toStdString());
    g_Fodder->mGame_Data.mMission_Current = 0;
    g_Fodder->mGame_Data.mPhase_Current = 0;

    g_Fodder->mGame_Data.mMission_Phases_Remaining = 1;
    g_Fodder->mGame_Data.mMission_Number = 0;
    g_Fodder->mGame_Data.mMission_Phase = 0;
    g_Fodder->mGame_Data.Phase_Next();

    LoadMap();

    mToolboxCampaigns->LoadMissions();
}


/**
 * Show the Save Map Dialog
 */
void cOFED::ShowDialog_SaveMap() {
    if (!g_Fodder->mGame_Data.mPhase_Current || !g_Fodder->mMapLoaded) return;

    // Custom maps can be saved as anything
    if (g_Fodder->mGame_Data.mCampaign.isCustomMap()) {

        QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Map"), tr(g_Fodder->mGame_Data.mPhase_Current->mMapFilename.c_str()),
            tr("Open Fodder (*.map);;All Files (*)"));

        if (!fileName.size())
            return;

        SaveMapTo(fileName.toStdString());
    }
    else {
        // Campaign maps have to keep the filename
        Save_Map(g_Fodder->mGame_Data.mPhase_Current);
    }
}

/**
 * Create a new map
 */
void cOFED::Create_NewMap(const std::string& pTileSet, const std::string& pTileSub, size_t pWidth, size_t pHeight, bool pRandom) {
	
	// Loop each known tile type
	for (auto TileType : mTileTypes) {

		if (pTileSet == TileType.mFullName) {
            const auto sub = pTileSub == "Sub1" ? eTileSub_1 : eTileSub_0;
            CursorReset();

            sMapParams Params(pWidth, pHeight, TileType.mType, sub);
			Params.mRandom.setSeed(g_Fodder->mRandom.get());

			if (pRandom) {
				g_Fodder->mParams->mScriptRun = "mapeditor.js";
				g_Fodder->CreateRandom(Params);

			} else
				g_Fodder->Map_Create(Params);

			// Editor needs to render the surface now
			// Draw the tiles
			g_Fodder->MapTiles_ResetScrollState();

			g_Fodder->Mission_Sprites_Handle();

			// Refresh the palette
			g_Fodder->mGraphics->PaletteSet(g_Fodder->mSurface);
			g_Fodder->mSurface->surfaceSetToPaletteNew();

			g_Fodder->mWindow->FrameEnd();

			std::dynamic_pointer_cast<cWindowQT>(g_Window)->RefreshMap();

			// Update the Toolboxes
			mToolboxSprites->RenderSprites();
			mToolboxTiles->RenderTiles();
            RenderStructures();

			if (mMapView)
				mMapView->RenderTiles();
			if (mToolboxCampaigns)
				mToolboxCampaigns->RefreshSprites();
			return;
		}
	}
}

void cOFED::RenderStructures() {
    const QSignalBlocker block(mStructurePalette);
    if (mCursorRangeTiles.mOnce) CursorReset();
    mStructurePalette->clear();
    const std::pair<QString, sTiles (cOFED::*)()> structures[] = {
        {tr("Hut with soldier"), &cOFED::HutWithSoldier},
        {tr("Hut with civilian"), &cOFED::HutWithCivilian},
        {tr("Hut with spear-armed civilian"), &cOFED::HutWithSpearCivilian},
        {tr("Barracks"), &cOFED::Barracks},
        {tr("Bunker"), &cOFED::Bunker},
        {tr("Reinforced bunker"), &cOFED::ReinforcedBunker},
        {tr("Cliff"), &cOFED::Cliff}
    };
    mStructureBrushes.clear();
    for (const auto& [name, makeBrush] : structures) {
        auto* item = new QListWidgetItem(name, mStructurePalette);
        const auto& brush = mStructureBrushes.emplace_back((this->*makeBrush)());
        if (brush.mTiles.empty()) {
            item->setFlags(Qt::NoItemFlags);
            item->setToolTip(tr("Unavailable for this terrain"));
        } else {
            item->setIcon(QPixmap::fromImage(mMapWindow->StructureThumbnail(brush)));
            item->setToolTip(tr("Select %1, then click the map to place it").arg(name));
        }
    }
}

void cOFED::ClearStructureSelection() {
    if (!mStructurePalette) return;
    mStructurePalette->clearSelection();
    mStructurePalette->setCurrentItem(nullptr);
}

void cOFED::UpdateMapStatus() {
    const auto& data = g_Fodder->mGame_Data;
    mMissionLabel->setText(data.mMission_Current ? tr("Mission: %1").arg(QString::fromStdString(data.mMission_Current->mName)) : QString());
    mPhaseLabel->setText(data.mPhase_Current ? tr("Phase: %1").arg(QString::fromStdString(data.mPhase_Current->mName)) : QString());
}

void cOFED::LoadMap() {
    sSprite* Sprite = nullptr;
    UpdateMapStatus();

	// Phase_Prepare()
    g_Fodder->Map_Load();
    g_Fodder->Map_Load_Sprites();

    g_Fodder->Phase_Soldiers_Count();
    g_Fodder->mGame_Data.Soldier_Sort();
    g_Fodder->Phase_Soldiers_Prepare(false);
    g_Fodder->Phase_Soldiers_AttachToSprites();

    g_Fodder->Squad_Rebuild();

    g_Fodder->mSquad_Selected = 0;
    g_Fodder->Squad_UpdateLeader(Sprite);
    g_Fodder->Sprite_Handle_Loop();

    g_Fodder->MapTiles_ResetScrollState();

    if (mToolboxTiles)
        mToolboxTiles->RenderTiles();

    g_Fodder->Camera_Reset();

    g_Fodder->mCamera_SquadLeaderX = 0;
    g_Fodder->mCamera_SquadLeaderY = 0;

    g_Fodder->mCamera_Panning_ToTarget = true;
    g_Fodder->Camera_PanTarget_AdjustToward_SquadLeader();
    g_Fodder->mCamera_Start_Adjust = 1;

    for (;;) {
        g_Fodder->Camera_Pan_To_Target();
        g_Fodder->Camera_Pan_To_Target();

        if (!g_Fodder->mCamera_Reached_Target)
            break;
    }

    g_Fodder->mGraphics->PaletteSet();
    g_Fodder->mSurface->surfaceSetToPaletteNew();

    g_Fodder->mGraphics->MapTiles_Draw();
    g_Fodder->Mission_Sprites_Handle();
    g_Fodder->mWindow->FrameEnd(); 

	mMapWindow->RefreshMap();
    if (mToolboxSprites)
        mToolboxSprites->RenderSprites();
    if (mStructurePalette) RenderStructures();
	if(mMapView)
		mMapView->RenderTiles();

    // Loading replaces the runtime sprites; notify the sidebar even when the
    // map was opened directly rather than selected through the phase table.
    if (mToolboxCampaigns)
        mToolboxCampaigns->RefreshSprites();
}

/**
 * Clear the cursor status
 */
void cOFED::CursorReset() {
    ClearStructureSelection();
    statusBar()->clearMessage();

	mCursorSprite = -1;
	mCursorRangeTiles = sTiles();
    if (mToolboxTiles) mToolboxTiles->ClearSelection();

	CursorUpdate();
}

/**
 * Refresh the drawn cursor
 */
void cOFED::CursorUpdate() {
    if (mCursorSprite < 0 && mToolboxSprites) mToolboxSprites->ClearSelection();
    auto window = std::dynamic_pointer_cast<cWindowQT>(g_Window);
    window->setCursor((!mCursorRangeTiles.mTiles.empty() || mCursorSprite >= 0) ? Qt::CrossCursor : Qt::ArrowCursor);
    window->update();
}

/**
 * Set the cursor to a tile brush
 */
void cOFED::setCursorTiles(const sTiles& pTiles, bool fromPalette) {
    if (fromPalette) ClearStructureSelection();
    else if (mToolboxTiles) mToolboxTiles->ClearSelection();
	mCursorRangeTiles = pTiles;
    mCursorSprite = -1;

	CursorUpdate();
}

void cOFED::SetCursorSprite(const size_t pSpriteID) {
    if (!IsPlaceableSprite(pSpriteID)) return;
    if (mToolboxTiles) mToolboxTiles->ClearSelection();
    ClearStructureSelection();
    mCursorRangeTiles = sTiles();
    mCursorSprite = pSpriteID;
    CursorUpdate();
}

void cOFED::SetupSprites() {
	for (int i = 0; i < 118; ++i) {
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
	g_SpriteAnim[eSprite_Bird_Left_Wraparound] = 0x98;

	g_SpriteAnim[eSprite_BuildingDoor] = 0x99;
	//g_SpriteAnim[eSprite_GroundHole] = ;
	g_SpriteAnim[eSprite_BuildingDoor_Hut] = 0x9B;
	g_SpriteAnim[eSprite_Floating_Dead_Soldier] = 0x9E;
	g_SpriteAnim[eSprite_Enemy_Rocket] = 0x39;
	g_SpriteAnim[eSprite_GrenadeBox] = 0xC2;
	g_SpriteAnim[eSprite_RocketBox] = 0xC3;

	g_SpriteAnim[eSprite_Helicopter_Grenade_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Flashing_Light] = 0xC4;
	g_SpriteAnim[eSprite_Helicopter_Unarmed_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Missile_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Homing_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Grenade_Human] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Unarmed_Human] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Missile_Human] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Homing_Human] = 0x8B;

	g_SpriteAnim[eSprite_Mine] = 0xC7;
	g_SpriteAnim[eSprite_Mine_SlideTrigger] = 0xC8;
	g_SpriteAnim[eSprite_Spike] = 0xC9;

	g_SpriteAnim[eSprite_BoilingPot] = 0xCD;
	g_SpriteAnim[eSprite_Civilian] = 0xD0;
	g_SpriteAnim[eSprite_Civilian_Fast] = 0xD0;
	g_SpriteAnim[eSprite_VehicleNoGun_Human] = 0xA5;
	g_SpriteAnim[eSprite_VehicleGun_Human] = 0xA5;

	g_SpriteAnim[eSprite_Tank_Human] = 0xD1;
	g_SpriteAnim[eSprite_Bird_Left_CameraRespawn] = 0xD3;
	g_SpriteAnim[eSprite_Bird_Right_CameraRespawn] = 0xD4;
	g_SpriteAnim[eSprite_Seal] = 0xD5;
	g_SpriteAnim[eSprite_Tank_Enemy] = 0xD1;

	g_SpriteAnim[eSprite_Civilian_Spear] = 0xD0;
	g_SpriteAnim[eSprite_Hostage] = 0xD9;
	g_SpriteAnim[eSprite_Hostage_Rescue_Tent] = 0xDD;

	g_SpriteAnim[eSprite_Door_Civilian] = 0x9B;
	g_SpriteAnim[eSprite_Door_Civilian_Fast] = 0x9B;
	g_SpriteAnim[eSprite_Door_Civilian_Spear] = 0x9B;
	g_SpriteAnim[eSprite_Cannon] = 0x96;

	g_SpriteAnim[eSprite_Turret_Cannon_Human] = 0xD2;
	g_SpriteAnim[eSprite_Turret_Missile_Human] = 0xD2;

	g_SpriteAnim[eSprite_VehicleNoGun_Enemy] = 0xA5;
	g_SpriteAnim[eSprite_VehicleGun_Enemy] = 0xA5;
	g_SpriteAnim[eSprite_Vehicle_Unk_Enemy] = 0xA5;
	g_SpriteAnim[eSprite_Civilian_Invisible] = 0xD6;

	g_SpriteAnim[eSprite_Turret_Cannon_Enemy] = 0xD2;
	g_SpriteAnim[eSprite_Turret_Missile_Enemy] = 0xD2;
	g_SpriteAnim[eSprite_BunkerDoor_HeavyExplosionOnly] = 0xE0;

	g_SpriteAnim[eSprite_Door_Civilian_Rescue] = 0x9B;
	g_SpriteAnim[eSprite_Seal_Mine] = 0xD5;
	g_SpriteAnim[eSprite_Spider_Mine] = 0xE2;

	g_SpriteAnim[eSprite_Bonus_RankToGeneral] = 0x95;
	g_SpriteAnim[eSprite_Bonus_Rockets] = 0xE4;
	g_SpriteAnim[eSprite_Bonus_Armour] = 0xE3;
	g_SpriteAnim[eSprite_Bonus_RankHomingInvin_SquadLeader] = 0xE5;

	g_SpriteAnim[eSprite_Bonus_RankHomingInvin_Squad] = 0xE6;
	g_SpriteAnim[eSprite_Helicopter_CallPad] = 0xE7;

	g_SpriteAnim[eSprite_BunkerDoor_ReinforcedHeavyExplosionOnly] = 0xE0;
	g_SpriteAnim[eSprite_Helicopter_Grenade_Human_Called] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Unarmed_Human_Called] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Missile_Human_Called] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Homing_Human_Called] = 0x8B;
	g_SpriteAnim[eSprite_Turret_HomingMissile_Enemy] = 0xD2;

	g_SpriteAnim[eSprite_Enemy_Leader] = 0xD9;
	g_SpriteAnim[eSprite_Helicopter_Homing_Enemy_Called] = 0x8B;
	g_SpriteAnim[eSprite_Computer_1] = 0x8F;
	g_SpriteAnim[eSprite_Computer_2] = 0x8F;
	g_SpriteAnim[eSprite_Computer_3] = 0x8F;


    g_SpriteAnim[eSprite_UFO_Callpad] = 0x7C;
    g_SpriteAnim[eSprite_Turret_Cannon_Invulnerable] = 0xD2;
    g_SpriteAnim[eSprite_Turret_Missile_Invulnerable] = 0xD2;
    g_SpriteAnim[eSprite_Looping_Vehicle_Left] = 0xA5;
    g_SpriteAnim[eSprite_Looping_Vehicle_Right] = 0xA5;
    g_SpriteAnim[eSprite_Looping_Vehicle_Up] = 0xA5;
    g_SpriteAnim[eSprite_Looping_Vehicle_Down] = 0xA5;

    g_SpriteName[eSprite_Player] = "Human Soldier";
	g_SpriteName[eSprite_Enemy] = "Enemy Soldier";
	g_SpriteName[eSprite_Null] = "Null";

	g_SpriteName[eSprite_Snowman] = "Snowman";
	g_SpriteName[eSprite_Shrub] = "Shrub";
	g_SpriteName[eSprite_Tree] = "Tree";
	g_SpriteName[eSprite_BuildingRoof] = "Building Roof";
	g_SpriteName[eSprite_Shrub2] = "Shrub 2";
	g_SpriteName[eSprite_Waterfall] = "Waterfall";
	g_SpriteName[eSprite_Bird_Left_Wraparound] = "Bird Left (Wraparound)";

	g_SpriteName[eSprite_BuildingDoor] = "Building Door";
	g_SpriteName[eSprite_GroundHole] = "Ground Hole";
	g_SpriteName[eSprite_BuildingDoor_Hut] = "Hut Door";
	g_SpriteName[eSprite_Floating_Dead_Soldier] = "Floating Dead Soldier";
	g_SpriteName[eSprite_Enemy_Rocket] = "Enemy Rocket Soldier";
	g_SpriteName[eSprite_GrenadeBox] = "Box of Grenades";
	g_SpriteName[eSprite_RocketBox] = "Box of Rockets";

	g_SpriteName[eSprite_Helicopter_Grenade_Enemy] = "Enemy Helicopter: Grenade";
	g_SpriteName[eSprite_Flashing_Light] = "Flashing Light";
	g_SpriteName[eSprite_Helicopter_Unarmed_Enemy] = "Enemy Helicopter: Unarmed";
	g_SpriteName[eSprite_Helicopter_Missile_Enemy] = "Enemy Helicopter: Missile";
	g_SpriteName[eSprite_Helicopter_Homing_Enemy] = "Enemy Helicopter: Homing Missile";
	g_SpriteName[eSprite_Helicopter_Grenade_Human] = "Human Helicopter: Grenade";
	g_SpriteName[eSprite_Helicopter_Unarmed_Human] = "Human Helicopter: Unarmed";
	g_SpriteName[eSprite_Helicopter_Missile_Human] = "Human Helicopter: Missile";
	g_SpriteName[eSprite_Helicopter_Homing_Human] = "Human Helicopter: Homing Missile";

	g_SpriteName[eSprite_Mine] = "Mine";
	g_SpriteName[eSprite_Mine_SlideTrigger] = "Mine (Slide Trigger)";
	g_SpriteName[eSprite_Spike] = "Spike";

	g_SpriteName[eSprite_BoilingPot] = "Boiling Pot";
	g_SpriteName[eSprite_Civilian] = "Civilian";
	g_SpriteName[eSprite_Civilian_Fast] = "Fast Civilian";
	g_SpriteName[eSprite_VehicleNoGun_Human] = "Human Vehicle: No Weapon";
	g_SpriteName[eSprite_VehicleGun_Human] = "Human Vehicle: Weapon";

	g_SpriteName[eSprite_Tank_Human] = "Human Tank";
	g_SpriteName[eSprite_Bird_Left_CameraRespawn] = "Bird Left (Camera Respawn)";
	g_SpriteName[eSprite_Bird_Right_CameraRespawn] = "Bird Right (Camera Respawn)";
	g_SpriteName[eSprite_Seal] = "Seal";
	g_SpriteName[eSprite_Tank_Enemy] = "Enemy Tank";

	g_SpriteName[eSprite_Civilian_Spear] = "Civilian with Spear";
	g_SpriteName[eSprite_Civilian_Spear_Projectile] = "Civilian Spear Projectile";
	g_SpriteName[eSprite_Hostage] = "Hostage";
	g_SpriteName[eSprite_Hostage_Rescue_Tent] = "Hostage Rescue Tent";

	g_SpriteName[eSprite_Door_Civilian] = "Building Door: Civilian";
	g_SpriteName[eSprite_Door_Civilian_Fast] = "Building Door: Fast Civilian";
	g_SpriteName[eSprite_Door_Civilian_Spear] = "Building Door: Civilian with Spear";
	g_SpriteName[eSprite_Cannon] = "Cannon";

	g_SpriteName[eSprite_Turret_Cannon_Human] = "Human Turret: Cannon";
	g_SpriteName[eSprite_Turret_Missile_Human] = "Human Turret: Missile";

	g_SpriteName[eSprite_VehicleNoGun_Enemy] = "Enemy Vehicle: No Weapon";
	g_SpriteName[eSprite_VehicleGun_Enemy] = "Enemy Vehicle: Weapon";
	g_SpriteName[eSprite_Vehicle_Unk_Enemy] = "Enemy Vehicle: Unknown";
	g_SpriteName[eSprite_Civilian_Invisible] = "Invisible Civilian";

	g_SpriteName[eSprite_Turret_Cannon_Enemy] = "Enemy Turret: Cannon";
	g_SpriteName[eSprite_Turret_Missile_Enemy] = "Enemy Turret: Missile";
	g_SpriteName[eSprite_BunkerDoor_HeavyExplosionOnly] = "Bunker Door: Heavy Explosion Only";

	g_SpriteName[eSprite_Door_Civilian_Rescue] = "Building Door: Civilian Rescue";
	g_SpriteName[eSprite_Seal_Mine] = "Seal Mine";
	g_SpriteName[eSprite_Spider_Mine] = "Spider Mine";

	g_SpriteName[eSprite_Bonus_RankToGeneral] = "Bonus: Rank to General";
	g_SpriteName[eSprite_Bonus_Rockets] = "Bonus: Rockets";
	g_SpriteName[eSprite_Bonus_Armour] = "Bonus: Armour";
	g_SpriteName[eSprite_Bonus_RankHomingInvin_SquadLeader] = "Squad Leader Bonus: General, Homing Missiles, Invincibility";
	g_SpriteName[eSprite_MissileHoming_Human] = "Human Homing Missile";
	g_SpriteName[eSprite_Bonus_RankHomingInvin_Squad] = "Squad Bonus: General, Homing Missiles, Invincibility";
	g_SpriteName[eSprite_Helicopter_CallPad] = "Helicopter Call Pad";
	g_SpriteName[eSprite_BunkerDoor_ReinforcedHeavyExplosionOnly] = "Bunker Door: Reinforced, Heavy Explosion Only";
	g_SpriteName[eSprite_Helicopter_Grenade_Human_Called] = "Human Helicopter: Grenade (Call Pad)";
	g_SpriteName[eSprite_Helicopter_Unarmed_Human_Called] = "Human Helicopter: Unarmed (Call Pad)";
	g_SpriteName[eSprite_Helicopter_Missile_Human_Called] = "Human Helicopter: Missile (Call Pad)";
	g_SpriteName[eSprite_Helicopter_Homing_Human_Called] = "Human Helicopter: Homing Missile (Call Pad)";

	g_SpriteName[eSprite_Turret_HomingMissile_Enemy] = "Enemy Turret: Homing Missile";
	g_SpriteName[eSprite_Enemy_Leader] = "Enemy Leader";
	g_SpriteName[eSprite_Helicopter_Homing_Enemy_Called] = "Enemy Helicopter: Homing Missile (Call Pad)";
	g_SpriteName[eSprite_Computer_1] = "Computer 1";
	g_SpriteName[eSprite_Computer_2] = "Computer 2";
	g_SpriteName[eSprite_Computer_3] = "Computer 3";

    g_SpriteName[eSprite_UFO_Callpad] = "UFO Call Pad";
    g_SpriteName[eSprite_Turret_Cannon_Invulnerable] = "Invulnerable Turret: Cannon";
    g_SpriteName[eSprite_Turret_Missile_Invulnerable] = "Invulnerable Turret: Missile";
    g_SpriteName[eSprite_Looping_Vehicle_Left] = "Looping Vehicle: Left";
    g_SpriteName[eSprite_Looping_Vehicle_Right] = "Looping Vehicle: Right";
    g_SpriteName[eSprite_Looping_Vehicle_Up] = "Looping Vehicle: Up";
    g_SpriteName[eSprite_Looping_Vehicle_Down] = "Looping Vehicle: Down";

}
