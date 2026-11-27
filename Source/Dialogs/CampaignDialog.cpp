#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "ui_CampaignDialog.h"

#include <QAction>
#include <QHeaderView>
#include <QMessageBox>
#include <QScopedValueRollback>
#include <QShortcut>
#include <QSignalBlocker>

cCampaignDialog::cCampaignDialog(QWidget* parent) : QWidget(parent), mUi(new Ui_CampaignDialog) {
    mUi->setupUi(this);
    auto titleFont = mUi->mCampaignName->font();
    titleFont.setBold(true);
    mUi->mCampaignName->setFont(titleFont);
    mUi->campaignSplitter->setStretchFactor(0, 1);
    mUi->campaignSplitter->setStretchFactor(1, 2);
    mUi->campaignSplitter->setSizes({210, 260});

    mUi->mMissionTable->setModel(&mCampaignModel);
    mUi->mPhaseTable->setModel(&mMissionModel);
    mUi->mSpritesTable->setModel(&mPhaseSpriteModel);
    connect(&mCampaignModel, &QAbstractItemModel::dataChanged, this, &cCampaignDialog::namesChanged);
    connect(&mMissionModel, &QAbstractItemModel::dataChanged, this, &cCampaignDialog::namesChanged);
    for (auto* view : {mUi->mMissionTable, mUi->mPhaseTable, mUi->mSpritesTable})
        view->verticalHeader()->setDefaultSectionSize(view->fontMetrics().height() + 8);
    for (auto* view : {mUi->mMissionTable, mUi->mPhaseTable}) {
        view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        view->setContextMenuPolicy(Qt::ActionsContextMenu);
        auto* rename = new QAction(tr("Rename"), view);
        view->addAction(rename);
        connect(rename, &QAction::triggered, view, [view]() {
            if (view->currentIndex().isValid()) view->edit(view->currentIndex());
        });
    }
    auto* spritesHeader = mUi->mSpritesTable->horizontalHeader();
    spritesHeader->setSectionResizeMode(0, QHeaderView::Stretch);
    spritesHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    spritesHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    connect(mUi->mAuthorsName, &QLineEdit::textEdited, this, [](const QString& name) {
        g_Fodder->mGame_Data.mCampaign.setAuthor(name.toStdString());
    });
    connect(mUi->mMissionTable->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &cCampaignDialog::SelectMission);
    connect(mUi->mPhaseTable->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &cCampaignDialog::UpdatePhaseDetails);
    connect(mUi->mSpritesTable->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &cCampaignDialog::UpdateSpriteActions);

    connect(mUi->mNewMission, &QPushButton::clicked, this, []() { g_OFED->Mission_AddNew(); });
    connect(mUi->mNewPhase, &QPushButton::clicked, this, []() { g_OFED->Phase_AddNew(); });
    connect(mUi->mOpenPhase, &QPushButton::clicked, this, &cCampaignDialog::OpenPhase);
    connect(mUi->mPhaseTable, &QTableView::doubleClicked, this, &cCampaignDialog::OpenPhase);
    auto* openKey = new QShortcut(QKeySequence(Qt::Key_Return), mUi->mPhaseTable);
    openKey->setContext(Qt::WidgetShortcut);
    connect(openKey, &QShortcut::activated, this, &cCampaignDialog::OpenPhase);

    const auto locate = [this]() {
        const auto* sprite = mPhaseSpriteModel.SpriteAtRow(mUi->mSpritesTable->currentIndex().row());
        if (sprite) std::dynamic_pointer_cast<cWindowQT>(g_Window)->CenterOnMap(QPointF(sprite->mPosX, sprite->mPosY));
    };
    const auto remove = [this]() {
        auto* sprite = mPhaseSpriteModel.SpriteAtRow(mUi->mSpritesTable->currentIndex().row());
        if (!sprite) return;
        sprite->Clear();
        g_OFED->Sprite_AddNew();
    };
    connect(mUi->mLocateSprite, &QPushButton::clicked, this, locate);
    connect(mUi->mSpritesTable, &QTableView::activated, this, locate);
    connect(mUi->mDeleteSprite, &QPushButton::clicked, this, remove);
    auto* deleteKey = new QShortcut(QKeySequence::Delete, mUi->mSpritesTable);
    deleteKey->setContext(Qt::WidgetShortcut);
    connect(deleteKey, &QShortcut::activated, this, remove);

    mGoals = {{
        {mUi->checkBox_KillAllEnemy, eObjective_Kill_All_Enemy},
        {mUi->checkBox_DestroyEnemyBuildings, eObjective_Destroy_Enemy_Buildings},
        {mUi->checkBox_RescueHostages, eObjective_Rescue_Hostages},
        {mUi->checkBox_ProtectCivilians, eObjective_Protect_Civilians},
        {mUi->checkBox_KidnapLeader, eObjective_Kidnap_Leader},
        {mUi->checkBox_DestroyFactory, eObjective_Destroy_Factory},
        {mUi->checkBox_DestroyComputer, eObjective_Destroy_Computer},
        {mUi->checkBox_GetCivilianHome, eObjective_Get_Civilian_Home},
        {mUi->checkBox_ActivateAllSwitches, eObjective_Activate_All_Switches},
        {mUi->checkBox_RescueHostage, eObjective_Rescue_Hostage}
    }};
    for (const auto& [checkbox, goal] : mGoals) {
        connect(checkbox, &QCheckBox::toggled, this, [this, goal](bool checked) {
            if (!mUpdatingDetails)
                if (auto phase = SelectedPhase()) phase->SetGoal(goal, checked);
        });
    }

    connect(mUi->aggression_min, &QSpinBox::valueChanged, this, [this](int value) {
        if (!mUpdatingDetails)
            if (auto phase = SelectedPhase()) phase->mAggression.mMin = value;
    });
    connect(mUi->aggression_max, &QSpinBox::valueChanged, this, [this](int value) {
        if (!mUpdatingDetails)
            if (auto phase = SelectedPhase()) phase->mAggression.mMax = value;
    });

    LoadMissions();
}

cCampaignDialog::~cCampaignDialog() {
    delete mUi;
}

void cCampaignDialog::LoadMissions() {
    auto& data = g_Fodder->mGame_Data;
    mUi->mCampaignName->setText(QString::fromStdString(data.mCampaign.getName()));
    mUi->mAuthorsName->setText(QString::fromStdString(data.mCampaign.getAuthor()));

    const QSignalBlocker block(mUi->mMissionTable->selectionModel());
    mCampaignModel.SetCampaign(&data.mCampaign);
    const auto& missions = data.mCampaign.getMissions();
    const auto active = std::find(missions.begin(), missions.end(), data.mMission_Current);
    const int row = active == missions.end() ? 0 : int(active - missions.begin());
    const auto index = mCampaignModel.index(row, 0);
    mUi->mMissionTable->setCurrentIndex(index);
    SelectMission(index);
    RefreshSprites();
}

void cCampaignDialog::SelectNewMission() {
    LoadMissions();
    const auto index = mCampaignModel.index(mCampaignModel.rowCount({}) - 1, 0);
    mUi->mMissionTable->setCurrentIndex(index);
    mUi->mMissionTable->setFocus();
    mUi->mMissionTable->edit(index);
}

int cCampaignDialog::SelectedMissionNumber() const {
    return mUi->mMissionTable->currentIndex().row() + 1;
}

void cCampaignDialog::SelectMission(const QModelIndex& index) {
    const QSignalBlocker block(mUi->mPhaseTable->selectionModel());
    auto& data = g_Fodder->mGame_Data;
    const auto mission = index.isValid() ? data.mCampaign.getMission(index.row() + 1) : nullptr;
    mMissionModel.SetMission(mission);
    mUi->mNewPhase->setEnabled(bool(mission));
    int row = 0;
    if (mission) {
        const auto active = std::find(mission->mPhases.begin(), mission->mPhases.end(), data.mPhase_Current);
        if (active != mission->mPhases.end()) row = int(active - mission->mPhases.begin());
    }
    mUi->mPhaseTable->setCurrentIndex(mMissionModel.index(row, 0));
    UpdatePhaseDetails();
}

std::shared_ptr<cPhase> cCampaignDialog::SelectedPhase() const {
    const auto index = mUi->mPhaseTable->currentIndex();
    const auto mission = mMissionModel.GetMission();
    return index.isValid() && mission ? mission->PhaseGet(index.row() + 1) : nullptr;
}

void cCampaignDialog::OpenPhase() {
    const auto phase = SelectedPhase();
    auto& data = g_Fodder->mGame_Data;
    if (!phase || phase == data.mPhase_Current) return;
    const auto map = data.mCampaign.getMap(phase);
    if (!map || map->empty()) {
        QMessageBox::warning(this, tr("Open phase"),
                             tr("Could not load %1.").arg(QString::fromStdString(phase->mMapFilename + ".map")));
        return;
    }

    data.mMission_Number = SelectedMissionNumber();
    data.mMission_Current = mMissionModel.GetMission();
    data.mMission_Phase = mUi->mPhaseTable->currentIndex().row() + 1;
    data.mMission_Phases_Remaining = data.mMission_Current->mPhases.size() - data.mMission_Phase + 1;
    data.mPhase_Current = phase;
    g_OFED->LoadMap();
    UpdatePhaseDetails();
}

void cCampaignDialog::UpdatePhaseDetails() {
    const QScopedValueRollback<bool> update(mUpdatingDetails, true);
    const auto phase = SelectedPhase();
    const bool active = phase && phase == g_Fodder->mGame_Data.mPhase_Current;
    mUi->mOpenPhase->setEnabled(phase && !active);
    mUi->mPhaseDetails->setEnabled(bool(phase));
    mUi->mPhaseDetails->setTabEnabled(mUi->mPhaseDetails->indexOf(mUi->spritesTab), active);
    for (const auto& [checkbox, goal] : mGoals)
        checkbox->setChecked(phase && std::find(phase->mGoals.begin(), phase->mGoals.end(), goal) != phase->mGoals.end());
    mUi->aggression_min->setValue(phase ? phase->mAggression.mMin : 0);
    mUi->aggression_max->setValue(phase ? phase->mAggression.mMax : 0);
}

void cCampaignDialog::RefreshSprites() {
    mPhaseSpriteModel.DataUpdated();
    UpdateSpriteActions();
}

void cCampaignDialog::UpdateSpriteActions() {
    const bool selected = mUi->mSpritesTable->currentIndex().isValid();
    mUi->mLocateSprite->setEnabled(selected);
    mUi->mDeleteSprite->setEnabled(selected);
}
