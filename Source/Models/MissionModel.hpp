#pragma once

class cMissionModel : public QAbstractTableModel {

public:
    cMissionModel(std::shared_ptr<cMission> pMission = 0);
    virtual ~cMissionModel();

    int rowCount(const QModelIndex&) const override {
        if(mMission)
            return mMission->mPhases.size();

        return 0;
    }

    int columnCount(const QModelIndex&) const override {
        return 1;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void DataUpdated();
    void SetMission(std::shared_ptr<cMission> pMission);
    std::shared_ptr<cMission> GetMission();

private:
    std::shared_ptr<cMission> mMission;
};
