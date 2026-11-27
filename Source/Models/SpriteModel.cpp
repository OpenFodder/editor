
#include "stdafx_ofed.hpp"
#include "ofed.hpp"

#include <QStandardItem>
#include <QAbstractTableModel>

cSpriteModel::cSpriteModel() : QAbstractTableModel(0) {

}

cSpriteModel::~cSpriteModel() {

}

QVariant cSpriteModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole))
        return QVariant();

    const auto* Sprite = SpriteAtRow(index.row());
    if (!Sprite)
        return "";

    switch (index.column()) {
    case 0:
        if (Sprite->mSpriteType >= 0 && Sprite->mSpriteType < 118 && !g_SpriteName[Sprite->mSpriteType].empty())
            return QString::fromStdString(g_SpriteName[Sprite->mSpriteType]);
        return QObject::tr("Sprite %1").arg(Sprite->mSpriteType);

    case 1:
        return QString::fromStdString(std::to_string(Sprite->mPosX));

    case 2:
        return QString::fromStdString(std::to_string(Sprite->mPosY));

    }

    return "";
}

sSprite* cSpriteModel::SpriteAtRow(int row) const {
    if (row < 0) return nullptr;
    for (auto& sprite : g_Fodder->mSprites) {
        if (sprite.mPosX != -32768 && sprite.mPosX != -1 && row-- == 0)
            return &sprite;
    }
    return nullptr;
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
