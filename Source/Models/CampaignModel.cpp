
#include "stdafx_ofed.hpp"

#include <QStandardItem>
#include <QAbstractTableModel>

#include "CampaignModel.hpp"

cCampaignModel::cCampaignModel(cCampaign* pCampaign) : QAbstractTableModel(0) {

    connect(this, &cCampaignModel::dataChanged, this, &cCampaignModel::dataChanged);

    mCampaign = 0;

    SetCampaign(pCampaign);
}

cCampaignModel::~cCampaignModel() {

}

QVariant cCampaignModel::data(const QModelIndex& index, int role) const
{

    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    int Row = index.row();
    int Column = index.column();

    if (Row >= mMissions.size())
        return "";

    auto Mission = mMissions.at(Row);

    switch (Column) {
    case 0:		// Mission Name
        return QString::fromStdString(Mission->mName);
    }

    return "";
}

void cCampaignModel::DataUpdated() {
    if (!mMissions.size())
        return;

    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex topRight = createIndex(mMissions.size(), 1);

    emit dataChanged(topLeft, topRight);
}

QVariant cCampaignModel::headerData(int section, Qt::Orientation orientation, int role) const {

    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {

            switch (section) {
            case 0:
                return QString("Name");
            }
        }
    }
    return QVariant();
}

void cCampaignModel::SetCampaign(cCampaign* pCampaign) {
    beginRemoveRows(QModelIndex(), 0, rowCount());
    endRemoveRows();

    if (!pCampaign)
        return;

    mMissions = pCampaign->getMissions();
    mCampaign = pCampaign;

    if (mMissions.size()) {
        beginInsertRows(QModelIndex(), 0, mMissions.size() - 1);
        endInsertRows();
    }
}
