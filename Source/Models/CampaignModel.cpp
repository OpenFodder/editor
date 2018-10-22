
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
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole))
        return QVariant();

    int Row = index.row();
    int Column = index.column();

    if (Row >= mCampaign->getMissions().size())
        return "";

    auto Mission = mCampaign->getMissions().at(Row);

    switch (Column) {
    case 0:		// Mission Name
        return QString::fromStdString(Mission->mName);
    }

    return "";
}

bool cCampaignModel::setData(const QModelIndex &index, const QVariant &value, int role) {

    if (!index.isValid() || role != Qt::EditRole)
        return false;

    int Row = index.row();
    int Column = index.column();

    if (Row >= mCampaign->getMissions().size())
        return "";

    auto Mission = mCampaign->getMissions().at(Row);

    switch (Column) {
    case 0:		// Mission Name
        Mission->mName = value.toString().toStdString();
        return true;

    }

    return false;
}
void cCampaignModel::DataUpdated() {
    if (!mCampaign->getMissions().size())
        return;

    beginResetModel();
    endResetModel();
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

    if (!pCampaign)
        return;

    mCampaign = pCampaign;

    DataUpdated();
}

Qt::ItemFlags cCampaignModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}
