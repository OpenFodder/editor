class Ui_CampaignDialog;

class cCampaignDialog : public QDialog {
    Ui_CampaignDialog *mUi;
    cCampaignModel mCampaignModel;
    cMissionModel mMissionModel;
    cSpriteModel mPhaseSpriteModel;

    bool mLoadingMission;

protected:
    void LoadMissions();

public:
    cCampaignDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~cCampaignDialog();

    void accept();
    void reject();

    void closeEvent(QCloseEvent *) override;

    void AuthorsNameChange(const QString &pNewName);

    void MissionClicked(QModelIndex pIndex);
    void PhaseClicked(QModelIndex pIndex);

    void LoadPhase(const size_t pNumber);

    void AggressMinChange(int pValue);
    void AggressMaxChange(int pValue);

    void Goal_ResetCheckboxes();
    void Goal_KillAllEnemy(int pValue);
    void Goal_DestroyEnemyBuildings(int pValue);
    void Goal_RescueHostages(int pValue);
    void Goal_ProtectCivilians(int pValue);

    void Goal_Kidnap_Leader(int pValue);
    void Goal_Destroy_Factory(int pValue);
    void Goal_Destroy_Computer(int pValue);
    void Goal_Get_Civilian_Home(int pValue);
    void Goal_Activate_All_Switches(int pValue);
    void Goal_Rescue_Hostage(int pValue);

    void LoadCampaign(cCampaign* pCampaign);
    void Refresh();

};
#pragma once
