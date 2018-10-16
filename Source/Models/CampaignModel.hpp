#pragma once

class cCampaignModel : public QAbstractTableModel {

public:
    cCampaignModel(cCampaign* pCampaign = 0);
    virtual ~cCampaignModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const {
        return mMissions.size();
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const {
        return 1;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void DataUpdated();
    void SetCampaign(cCampaign* pCampaign);

private:
    cCampaign* mCampaign;
    std::vector<std::shared_ptr<cMission>> mMissions;
};
