
#include "stdafx_ofed.hpp"
#include "ofed.hpp"

#include <QStandardItem>
#include <QAbstractTableModel>

cSpriteModel::cSpriteModel(std::shared_ptr<cMission> pMission) : QAbstractTableModel(0) {

    connect(this, &cSpriteModel::dataChanged, this, &cSpriteModel::dataChanged);
}

cSpriteModel::~cSpriteModel() {

}

QVariant cSpriteModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole))
        return QVariant();

    int Row = index.row();
    int Column = index.column();

    if (Row >= sizeof(g_Fodder->mSprites))
        return "";

    auto Sprite = &g_Fodder->mSprites[Row];

    switch (Column) {
    case 0:		// Mission Name
        return QString::fromStdString(g_SpriteName[Sprite->field_18]);

    case 1:
        return QString::fromStdString(std::to_string(Sprite->field_0));

    case 2:
        return QString::fromStdString(std::to_string(Sprite->field_4));

    }

    return "";
}

void cSpriteModel::DataUpdated() {

    beginResetModel();
    endResetModel();
}

QVariant cSpriteModel::headerData(int section, Qt::Orientation orientation, int role) const {

    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {

            switch (section) {
            case 0:
                return QString("Type");

            case 1:
                return QString("X");

            case 2:
                return QString("Y");
            }
        }
    }
    return QVariant();
}
