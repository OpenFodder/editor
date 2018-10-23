#pragma once

class cSpriteModel : public QAbstractTableModel {

public:
    cSpriteModel(std::shared_ptr<cMission> pMission = 0);
    virtual ~cSpriteModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const {
        int Count = 0;

        for (auto& Sprite : g_Fodder->mSprites) {
            if (Sprite.field_0 != -32768 && Sprite.field_0 != -1)
                ++Count;
        }

        return Count;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const {
        return 3;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void DataUpdated();


private:
    
};
