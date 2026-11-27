#include "stdafx_ofed.hpp"

cCampaignModel::cCampaignModel(cCampaign* value) : mCampaign(value) {}
cCampaignModel::~cCampaignModel() = default;

QVariant cCampaignModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= rowCount({}) || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};
    return QString::fromStdString(mCampaign->getMission(index.row() + 1)->mName);
}

bool cCampaignModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= rowCount({}) || role != Qt::EditRole)
        return false;
    mCampaign->getMission(index.row() + 1)->mName = value.toString().toStdString();
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

QVariant cCampaignModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && section == 0 && role == Qt::DisplayRole)
        return QObject::tr("Name");
    return {};
}

void cCampaignModel::SetCampaign(cCampaign* value) {
    beginResetModel();
    mCampaign = value;
    endResetModel();
}

Qt::ItemFlags cCampaignModel::flags(const QModelIndex& index) const {
    return index.isValid() ? Qt::ItemIsEditable | QAbstractItemModel::flags(index) : Qt::NoItemFlags;
}
