#pragma once

#include "ui_ofed.h"

class cOFED : public QMainWindow
{
	Q_OBJECT

public:
	cOFED(QWidget *parent = Q_NULLPTR);

	void OpenFodder_Prepare();
	void ShowDialog_NewMap();

	void Create_NewMap(const std::string& pTileSet, const std::string& pTileSub, size_t pWidth, size_t pHeight);

private:
	Ui::mOFED ui;
};
