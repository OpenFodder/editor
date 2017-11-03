#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "ui_NewMapDialog.h"

cNewMapDialog::cNewMapDialog(Ui_NewMapDialog* pUi, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

	mUi = pUi;
}

void cNewMapDialog::accept() {

	size_t Width = mUi->mWidth->toPlainText().toUInt();
	size_t Height = mUi->mHeight->toPlainText().toUInt();

	g_OFED->Create_NewMap( 
		mUi->mTerrainType->itemText(mUi->mTerrainType->currentIndex()).toStdString(), 
		mUi->mTileSub->itemText(mUi->mTileSub->currentIndex()).toStdString(),
		Width, Height);

	QDialog::accept();
}

void cNewMapDialog::reject() {
	
	QDialog::reject();
}
