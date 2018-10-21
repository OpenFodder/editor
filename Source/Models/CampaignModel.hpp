#pragma once

class cCampaignModel : public QAbstractTableModel {

public:
    cCampaignModel(cCampaign* pCampaign = 0);
    virtual ~cCampaignModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const {
        if (!mCampaign)
            return 0;

        return mCampaign->getMissions().size();
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const {
        return 1;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    void DataUpdated();
    void SetCampaign(cCampaign* pCampaign);

private:
    cCampaign* mCampaign;
};
