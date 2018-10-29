#pragma once

class cSpriteModel : public QAbstractTableModel {

public:
    cSpriteModel();
    virtual ~cSpriteModel();

    int rowCount(const QModelIndex&) const override {
        int Count = 0;

        for (auto& Sprite : g_Fodder->mSprites) {
            if (Sprite.field_0 != -32768 && Sprite.field_0 != -1)
                ++Count;
        }

        return Count;
    }

    int columnCount(const QModelIndex&) const override {
        return 3;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void DataUpdated();


private:
    
};
