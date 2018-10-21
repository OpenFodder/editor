#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include<qlineedit.h>

#include "ui_CampaignDialog.h"

cCampaignDialog::cCampaignDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

    mLoadingMission = false;
    mUi = new Ui_CampaignDialog();
    mUi->setupUi(this);

    mUi->mMissionTable->setModel(&mCampaignModel);
    mUi->mMissionTable->setColumnWidth(0, 400);

    mUi->mPhaseTable->setModel(&mMissionModel);
    mUi->mPhaseTable->setColumnWidth(0, 300);

    connect(mUi->mAuthorsName, &QLineEdit::textChanged, this, &cCampaignDialog::AuthorsNameChange);

    connect(mUi->mMissionTable, &QTableView::doubleClicked, this, &cCampaignDialog::MissionClicked);
    connect(mUi->mPhaseTable, &QTableView::doubleClicked, this, &cCampaignDialog::PhaseClicked);

    connect(mUi->aggression_min, &QSlider::valueChanged, this, &cCampaignDialog::AggressMinChange);
    connect(mUi->aggression_max, &QSlider::valueChanged, this, &cCampaignDialog::AggressMaxChange);

    connect(mUi->checkBox_KillAllEnemy, &QCheckBox::stateChanged, this, &cCampaignDialog::Goal_KillAllEnemy);
    connect(mUi->checkBox_DestroyEnemyBuildings, &QCheckBox::stateChanged, this, &cCampaignDialog::Goal_DestroyEnemyBuildings);
    connect(mUi->checkBox_RescueHostages, &QCheckBox::stateChanged, this, &cCampaignDialog::Goal_RescueHostages);
    connect(mUi->checkBox_ProtectCivilians, &QCheckBox::stateChanged, this, &cCampaignDialog::Goal_ProtectCivilians);

    connect(mUi->checkBox_KidnapLeader, &QCheckBox::stateChanged, this, &cCampaignDialog::Goal_Kidnap_Leader);
    connect(mUi->checkBox_DestroyFactory, &QCheckBox::stateChanged, this, &cCampaignDialog::Goal_Destroy_Factory);
    connect(mUi->checkBox_DestroyComputer, &QCheckBox::stateChanged, this, &cCampaignDialog::Goal_Destroy_Computer);
    connect(mUi->checkBox_GetCivilianHome, &QCheckBox::stateChanged, this, &cCampaignDialog::Goal_Get_Civilian_Home);
    connect(mUi->checkBox_ActivateAllSwitches, &QCheckBox::stateChanged, this, &cCampaignDialog::Goal_Activate_All_Switches);
    connect(mUi->checkBox_RescueHostage, &QCheckBox::stateChanged, this, &cCampaignDialog::Goal_Rescue_Hostage);

    LoadMissions();
}

void cCampaignDialog::accept() {

    QDialog::accept();
}

void cCampaignDialog::reject() {

    QDialog::reject();
}

void cCampaignDialog::LoadMissions() {
    mUi->mCampaignName->setText(QString::fromStdString(g_Fodder->mGame_Data.mCampaign.getName()));
    mUi->mAuthorsName->setText( QString::fromStdString(g_Fodder->mGame_Data.mCampaign.getAuthor()) );

    mCampaignModel.SetCampaign(&g_Fodder->mGame_Data.mCampaign);
    mMissionModel.SetMission(g_Fodder->mGame_Data.mCampaign.getMission(0));

    LoadPhase(g_Fodder->mGame_Data.mMission_Phase);
}

void cCampaignDialog::LoadCampaign(cCampaign* pCampaign) {
    
    mCampaignModel.SetCampaign(pCampaign);
    mMissionModel.SetMission(pCampaign->getMission(0));
}

void cCampaignDialog::Refresh() {
    mCampaignModel.DataUpdated();

    if (!mMissionModel.GetMission())
        mMissionModel.SetMission(g_Fodder->mGame_Data.mMission_Current);

    mMissionModel.DataUpdated();
}
void cCampaignDialog::AuthorsNameChange(const QString &pNewName) {

    g_Fodder->mGame_Data.mCampaign.setAuthor(pNewName.toStdString());
}

void cCampaignDialog::MissionClicked(QModelIndex pIndex) {

    g_Fodder->mGame_Data.mMission_Number = pIndex.row() + 1;
    g_Fodder->mGame_Data.mMission_Current = g_Fodder->mGame_Data.mCampaign.getMission(pIndex.row() + 1);
    g_Fodder->mGame_Data.mPhase_Current = mMissionModel.GetMission()->GetPhase(pIndex.row() + 1);

    mMissionModel.SetMission(g_Fodder->mGame_Data.mCampaign.getMission(pIndex.row() + 1));

    LoadPhase(1);
}

void cCampaignDialog::PhaseClicked(QModelIndex pIndex) {
    
    g_Fodder->mGame_Data.mMission_Phase = pIndex.row() + 1;

    LoadPhase(pIndex.row() + 1);
}

void cCampaignDialog::LoadPhase(const size_t pNumber) {

    if (!mMissionModel.GetMission())
        return;

    g_Fodder->mGame_Data.mPhase_Current = mMissionModel.GetMission()->GetPhase(pNumber);
    if (!g_Fodder->mGame_Data.mPhase_Current)
        return;

    mLoadingMission = true;
    g_OFED->LoadMap();

    Goal_ResetCheckboxes();

    for (auto& Goal : g_Fodder->mGame_Data.mPhase_Current->mGoals) {

        switch (Goal) {
        case eGoal_None:
            break;
        case eGoal_Kill_All_Enemy:
            mUi->checkBox_KillAllEnemy->setChecked(true);
            break;
        case eGoal_Destroy_Enemy_Buildings:
            mUi->checkBox_DestroyEnemyBuildings->setChecked(true);
            break;
        case eGoal_Rescue_Hostages:
            mUi->checkBox_RescueHostages->setChecked(true);
            break;
        case eGoal_Protect_Civilians:
            mUi->checkBox_ProtectCivilians->setChecked(true);
            break;
        case eGoal_Kidnap_Leader:
            mUi->checkBox_KidnapLeader->setChecked(true);
            break;
        case eGoal_Destroy_Factory:
            mUi->checkBox_DestroyFactory->setChecked(true);
            break;
        case eGoal_Destroy_Computer:
            mUi->checkBox_DestroyComputer->setChecked(true);
            break;
        case eGoal_Get_Civilian_Home:
            mUi->checkBox_GetCivilianHome->setChecked(true);
            break;
        case eGoal_Activate_All_Switches:
            mUi->checkBox_ActivateAllSwitches->setChecked(true);
            break;
        case eGoal_Rescue_Hostage:
            mUi->checkBox_RescueHostage->setChecked(true);
            break;
        }
    }

    mUi->aggression_min->setSliderPosition(g_Fodder->mGame_Data.mPhase_Current->mAggression.mMin);
    mUi->aggression_max->setSliderPosition(g_Fodder->mGame_Data.mPhase_Current->mAggression.mMax);
    mLoadingMission = false;
}

void cCampaignDialog::AggressMinChange(int pValue) {
    g_Fodder->mGame_Data.mPhase_Current->mAggression.mMin = pValue;
}

void cCampaignDialog::AggressMaxChange(int pValue) {
    g_Fodder->mGame_Data.mPhase_Current->mAggression.mMax = pValue;
}

void cCampaignDialog::Goal_ResetCheckboxes() {
    mUi->checkBox_KillAllEnemy->setChecked(false);
    mUi->checkBox_DestroyEnemyBuildings->setChecked(false);
    mUi->checkBox_RescueHostages->setChecked(false);
    mUi->checkBox_ProtectCivilians->setChecked(false);
    mUi->checkBox_KidnapLeader->setChecked(false);
    mUi->checkBox_DestroyFactory->setChecked(false);
    mUi->checkBox_DestroyComputer->setChecked(false);
    mUi->checkBox_GetCivilianHome->setChecked(false);
    mUi->checkBox_ActivateAllSwitches->setChecked(false);
    mUi->checkBox_RescueHostage->setChecked(false);
}

void cCampaignDialog::Goal_KillAllEnemy(int pValue) {
    if (mLoadingMission)
        return;
    g_Fodder->mGame_Data.mPhase_Current->SetGoal(eGoal_Kill_All_Enemy, pValue);
}

void cCampaignDialog::Goal_DestroyEnemyBuildings(int pValue) {
    if (mLoadingMission)
        return;
    g_Fodder->mGame_Data.mPhase_Current->SetGoal(eGoal_Destroy_Enemy_Buildings, pValue);
}

void cCampaignDialog::Goal_RescueHostages(int pValue) {
    if (mLoadingMission)
        return;
    g_Fodder->mGame_Data.mPhase_Current->SetGoal(eGoal_Rescue_Hostages, pValue);
}

void cCampaignDialog::Goal_ProtectCivilians(int pValue) {
    if (mLoadingMission)
        return;
    g_Fodder->mGame_Data.mPhase_Current->SetGoal(eGoal_Protect_Civilians, pValue);
}

void cCampaignDialog::Goal_Kidnap_Leader(int pValue) {
    if (mLoadingMission)
        return;
    g_Fodder->mGame_Data.mPhase_Current->SetGoal(eGoal_Kidnap_Leader, pValue);
}

void cCampaignDialog::Goal_Destroy_Factory(int pValue) {
    if (mLoadingMission)
        return;
    g_Fodder->mGame_Data.mPhase_Current->SetGoal(eGoal_Destroy_Factory, pValue);
}

void cCampaignDialog::Goal_Destroy_Computer(int pValue) {
    if (mLoadingMission)
        return;
    g_Fodder->mGame_Data.mPhase_Current->SetGoal(eGoal_Destroy_Computer, pValue);
}

void cCampaignDialog::Goal_Get_Civilian_Home(int pValue) {
    if (mLoadingMission)
        return;
    g_Fodder->mGame_Data.mPhase_Current->SetGoal(eGoal_Get_Civilian_Home, pValue);
}

void cCampaignDialog::Goal_Activate_All_Switches(int pValue) {
    if (mLoadingMission)
        return;
    g_Fodder->mGame_Data.mPhase_Current->SetGoal(eGoal_Activate_All_Switches, pValue);
}

void cCampaignDialog::Goal_Rescue_Hostage(int pValue) {
    if (mLoadingMission)
        return;
    g_Fodder->mGame_Data.mPhase_Current->SetGoal(eGoal_Rescue_Hostage, pValue);
}
