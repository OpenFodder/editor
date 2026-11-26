#pragma once

#include <QWidget>

class cTilePalette;

class cToolboxTiles : public QWidget {
    cTilePalette* mPalette;

public:
    explicit cToolboxTiles(QWidget* parent = nullptr);
    void RenderTiles();
    void ClearSelection();
};
