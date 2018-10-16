class Ui_CampaignDialog;

class cCampaignDialog : public QDialog {
    Ui_CampaignDialog *mUi;
    cCampaignModel mCampaignModel;
    cMissionModel mMissionModel;

protected:
    void LoadMissions();

public:
    cCampaignDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    void accept();
    void reject();

    void MissionClicked(QModelIndex pIndex);
    void PhaseClicked(QModelIndex pIndex);

    void LoadCampaign(cCampaign* pCampaign);

};
#pragma once
