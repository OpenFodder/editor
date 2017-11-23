#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "ui_NewMapDialog.h"

cNewMapDialog::cNewMapDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

	mUi = new Ui_NewMapDialog();
	mUi->setupUi(this);

	// Add known terrain types
	for (auto TileType : mTileTypes)
		mUi->mTerrainType->addItem(TileType.mFullName.c_str());

	// Add sub terrain types
	mUi->mTileSub->addItem("Sub0");
	mUi->mTileSub->addItem("Sub1");

	// 19 x 15 Map Default
	mUi->mWidth->setText("22");
	mUi->mHeight->setText("22");
}

void cNewMapDialog::accept() {

	size_t Width = mUi->mWidth->toPlainText().toUInt();
	size_t Height = mUi->mHeight->toPlainText().toUInt();

	bool Random = false;

	Random = mUi->mRandomise->isChecked();

	// Invalid Width/Height ?
	if (!Width || !Height)
		return;

	g_OFED->Create_NewMap( 
		mUi->mTerrainType->itemText(mUi->mTerrainType->currentIndex()).toStdString(), 
		mUi->mTileSub->itemText(mUi->mTileSub->currentIndex()).toStdString(),
		Width, Height, Random);

	QDialog::accept();
}

void cNewMapDialog::reject() {
	
	QDialog::reject();
}
