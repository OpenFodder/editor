#pragma once

class cCampaignModel : public QAbstractTableModel {

public:
    cCampaignModel(cCampaign* pCampaign = 0);
    virtual ~cCampaignModel();

    int rowCount(const QModelIndex&) const override {
        if (!mCampaign)
            return 0;

        return mCampaign->getMissions().size();
    }

    int columnCount(const QModelIndex&) const override {
        return 1;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void DataUpdated();
    void SetCampaign(cCampaign* pCampaign);

private:
    cCampaign* mCampaign;
};
