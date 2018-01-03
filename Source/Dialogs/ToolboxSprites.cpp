#include "stdafx_ofed.hpp"
#include "ofed.hpp"
#include "ui_ToolboxSprites.h"

#include <qpixmap.h>
#include <qpainter.h>
#include <qevent.h>

cToolboxSprites::cToolboxSprites(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

	mSpriteSurface = new cSurface(20 * 17, 9 * 17);

	mUi = new Ui_ToolboxSprites();
	mUi->setupUi(this);

	RenderSprites();
}

cToolboxSprites::~cToolboxSprites() {

	delete mSpriteSurface;
}

void cToolboxSprites::RenderSprites() {
	size_t width = this->width();
	size_t height = this->height();

	size_t X = 0, Y = 0;
	size_t BigY = 0;
	size_t MultiplierX = g_Fodder.mVersionCurrent->mPlatform == ePlatform::Amiga ? 8 : 1;
	size_t MultiplierY = g_Fodder.mVersionCurrent->mPlatform == ePlatform::Amiga ? 1 : 1;

	mSpriteRanges.clear();

	g_Graphics.SetActiveSpriteSheet(eGFX_IN_GAME);

	for (size_t SpriteID = 0; SpriteID < 111; ++SpriteID) {
		sSprite Sprite;

		int32 AnimID = g_SpriteAnim[SpriteID];
		if (AnimID < 0)
			continue;

		size_t SpriteWidth = g_Fodder.mSprite_SheetPtr[AnimID][0].mColCount  ;
		size_t SpriteHeight = g_Fodder.mSprite_SheetPtr[AnimID][0].mRowCount ;

		if (g_Fodder.mVersionCurrent->mPlatform == ePlatform::Amiga) {
			SpriteWidth -= 1;
			SpriteWidth <<= 1;
			SpriteWidth *= MultiplierX;
		}
		Sprite.field_0 = -0x40;
		Sprite.field_4 = SpriteHeight - 0x10;
		Sprite.field_52 = 0;
		Sprite.field_20 = 0;

		cSurface *Surface = new cSurface(SpriteWidth , SpriteHeight );

		g_Graphics.PaletteSet(Surface);
		g_Fodder.Sprite_Draw_Frame(&Sprite, AnimID, 0, Surface);

		if (X + SpriteWidth > (20 * 17)) {
			X = 0;
			Y += BigY;
			BigY = 0;
		}

		mSpriteRanges.push_back(sRange(X, Y, SpriteWidth, SpriteHeight, SpriteID));

		SDL_Rect SrcRect;
		SrcRect.x = 0;
		SrcRect.y = 0;
		SrcRect.w = SpriteWidth ;
		SrcRect.h = SpriteHeight ;

		SDL_Rect DstRect;
		DstRect.x = X;
		DstRect.y = Y;
		DstRect.w = SpriteWidth ;
		DstRect.h = SpriteHeight ;

		g_Graphics.PaletteSet(Surface);
		Surface->surfaceSetToPaletteNew();
		Surface->draw();

		SDL_BlitSurface(Surface->GetSurface(), &SrcRect, mSpriteSurface->GetSurface(), &DstRect);

		if (SpriteHeight > BigY)
			BigY = SpriteHeight;

		X += SpriteWidth;
		if (X >= (20 * 17)) {
			X = 0;
			Y += BigY;
			BigY = 0;
		}

		delete Surface;
	}

	// Load the tileset palette to the surface

	// Copy the image into a QImage
	SDL_Surface* Source = mSpriteSurface->GetSurface();
	mImage = QImage(static_cast<uchar*>(Source->pixels), Source->w, Source->h, QImage::Format_RGB32);

	mScaleWidth = (static_cast<double>(size().width()) / static_cast<double>(mImage.width()));
	mScaleHeight = (static_cast<double>(size().height()) / static_cast<double>(mImage.height()));

	this->repaint();
}

void cToolboxSprites::paintEvent(QPaintEvent* e) {

	QPainter painter(this);
	QRectF Dest(0, 0, size().width(), size().height());
	QRectF Src(0, 0, mImage.width(), mImage.height());

	painter.drawImage(Dest, mImage, Src);
}

void cToolboxSprites::mousePressEvent(QMouseEvent *eventPress) {
	size_t MouseX = eventPress->x() / mScaleWidth;
	size_t MouseY = eventPress->y() / mScaleHeight;

	uint32 TileX = MouseX / 18;
	uint32 TileY = MouseY / 18;

	// 20 Tiles per row
	uint32 TileID = (20 * TileY) + TileX;

	if (eventPress->button() == Qt::MouseButton::LeftButton) {

		g_OFED->SetCursorTileID(TileID);
	}
}
