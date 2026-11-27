#pragma once
#include "stdafx_ofed.hpp"
#include <array>

class QCheckBox;
class Ui_CampaignDialog;

class cCampaignDialog : public QWidget {
    Q_OBJECT

    Ui_CampaignDialog* mUi;
    cCampaignModel mCampaignModel;
    cMissionModel mMissionModel;
    cSpriteModel mPhaseSpriteModel;
    std::array<std::pair<QCheckBox*, ePhaseObjective>, 10> mGoals;
    bool mUpdatingDetails = false;

    void SelectMission(const QModelIndex& index);
    void OpenPhase();
    void UpdatePhaseDetails();
    void UpdateSpriteActions();
    std::shared_ptr<cPhase> SelectedPhase() const;

public:
    explicit cCampaignDialog(QWidget* parent = nullptr);
    ~cCampaignDialog() override;

    void LoadMissions();
    void SelectNewMission();
    void RefreshSprites();
    int SelectedMissionNumber() const;

signals:
    void namesChanged();
};
