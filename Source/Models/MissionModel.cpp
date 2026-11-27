#include "stdafx_ofed.hpp"

cMissionModel::cMissionModel(std::shared_ptr<cMission> value) : mMission(value) {}
cMissionModel::~cMissionModel() = default;

QVariant cMissionModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= rowCount({}) || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};
    return QString::fromStdString(mMission->mPhases[index.row()]->mName);
}

bool cMissionModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= rowCount({}) || role != Qt::EditRole)
        return false;
    mMission->mPhases[index.row()]->mName = value.toString().toStdString();
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

QVariant cMissionModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && section == 0 && role == Qt::DisplayRole)
        return QObject::tr("Name");
    return {};
}

void cMissionModel::SetMission(std::shared_ptr<cMission> value) {
    beginResetModel();
    mMission = value;
    endResetModel();
}

Qt::ItemFlags cMissionModel::flags(const QModelIndex& index) const {
    return index.isValid() ? Qt::ItemIsEditable | QAbstractItemModel::flags(index) : Qt::NoItemFlags;
}

std::shared_ptr<cMission> cMissionModel::GetMission() const {
    return mMission;
}
