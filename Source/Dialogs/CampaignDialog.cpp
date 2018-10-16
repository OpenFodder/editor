#include "stdafx_ofed.hpp"
#include "ofed.hpp"

#include "ui_CampaignDialog.h"

cCampaignDialog::cCampaignDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

    mUi = new Ui_CampaignDialog();
    mUi->setupUi(this);

    mUi->mMissionTable->setModel(&mCampaignModel);
    mUi->mMissionTable->setColumnWidth(0, 400);

    mUi->mPhaseTable->setModel(&mMissionModel);
    mUi->mPhaseTable->setColumnWidth(0, 300);

    connect(mUi->mMissionTable, &QTableView::clicked, this, &cCampaignDialog::MissionClicked);
    connect(mUi->mPhaseTable, &QTableView::doubleClicked, this, &cCampaignDialog::PhaseClicked);


    LoadMissions();
}

void cCampaignDialog::accept() {

    QDialog::accept();
}

void cCampaignDialog::reject() {

    QDialog::reject();
}

void cCampaignDialog::LoadMissions() {

    mCampaignModel.SetCampaign(&g_Fodder->mGame_Data.mCampaign);
    mMissionModel.SetMission(g_Fodder->mGame_Data.mCampaign.getMission(0));
}

void cCampaignDialog::LoadCampaign(cCampaign* pCampaign) {
    
    mCampaignModel.SetCampaign(pCampaign);
    mMissionModel.SetMission(pCampaign->getMission(0));
}

void cCampaignDialog::MissionClicked(QModelIndex pIndex) {

    g_Fodder->mGame_Data.mMission_Number = pIndex.row() + 1;
    g_Fodder->mGame_Data.mMission_Current = g_Fodder->mGame_Data.mCampaign.getMission(pIndex.row() + 1);
    g_Fodder->mGame_Data.mPhase_Current = mMissionModel.GetMission()->GetPhase(pIndex.row() + 1);

    mMissionModel.SetMission(g_Fodder->mGame_Data.mCampaign.getMission(pIndex.row() + 1));
}

void cCampaignDialog::PhaseClicked(QModelIndex pIndex) {
    
    g_Fodder->mGame_Data.mMission_Phase = pIndex.row() + 1;
    g_Fodder->mGame_Data.mPhase_Current = mMissionModel.GetMission()->GetPhase(pIndex.row() + 1);


    g_OFED->LoadMap();
}
