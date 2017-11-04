#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "ui_ToolboxTiles.h"

cToolboxTiles::cToolboxTiles(Ui_ToolboxTiles* pUi, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

	mUi = pUi;

	this->move(parent->x() + parent->width(), parent->y());
}

void cToolboxTiles::paintEvent(QPaintEvent* e) {
	
}

