
#include "stdafx_ofed.hpp"

#include <QStandardItem>
#include <QAbstractTableModel>

#include "CampaignModel.hpp"

cMissionModel::cMissionModel(std::shared_ptr<cMission> pMission) : QAbstractTableModel(0) {

    connect(this, &cMissionModel::dataChanged, this, &cMissionModel::dataChanged);

    mMission = 0;

    SetMission(pMission);
}

cMissionModel::~cMissionModel() {

}

QVariant cMissionModel::data(const QModelIndex& index, int role) const
{

    if (!index.isValid() || role != Qt::DisplayRole || !mMission)
        return QVariant();

    int Row = index.row();
    int Column = index.column();

    if (Row >= mMission->mPhases.size())
        return "";

    auto Mission = mMission->mPhases.at(Row);

    switch (Column) {
    case 0:		// Mission Name
        return QString::fromStdString(Mission->mName);

    case 1:
        return QString::fromStdString(Mission->mMapFilename);

    }

    return "";
}

void cMissionModel::DataUpdated() {
    if (!mMission)
        return;
    
    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex topRight = createIndex(mMission->mPhases.size(), 1);

    emit dataChanged(topLeft, topRight);
}

QVariant cMissionModel::headerData(int section, Qt::Orientation orientation, int role) const {

    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {

            switch (section) {
            case 0:
                return QString("Name");

            case 1:
                return QString("Filename");
            }
        }
    }
    return QVariant();
}

void cMissionModel::SetMission(std::shared_ptr<cMission> pMission) {

    beginRemoveRows(QModelIndex(), 0, rowCount());
    endRemoveRows();

    if (pMission && pMission->mPhases.size()) {
        beginInsertRows(QModelIndex(), 0, pMission->mPhases.size() - 1);
        mMission = pMission;
        endInsertRows();
    }
}

std::shared_ptr<cMission> cMissionModel::GetMission() {
    return mMission; 
}
