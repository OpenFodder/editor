#pragma once

class cMissionModel : public QAbstractTableModel {

public:
    cMissionModel(std::shared_ptr<cMission> pMission = 0);
    virtual ~cMissionModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const {
        if(mMission)
            return mMission->mPhases.size();

        return 0;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const {
        return 1;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    void DataUpdated();
    void SetMission(std::shared_ptr<cMission> pMission);
    std::shared_ptr<cMission> GetMission();

private:
    std::shared_ptr<cMission> mMission;
};
