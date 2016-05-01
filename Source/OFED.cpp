
#include "stdafx.hpp"

const std::string mTileType_Names[] = {
	"jun",
	"des",
	"ice",
	"mor",
	"int",
	"hid",
	"afx"		// Amiga Format Christmas Special
};

cOFED::cOFED() {

	new cResource_PC_CD();

	mMap = 0;
	mBlocksLoaded = false;

	mBlkSize = 0xFD00 * 16;

	mBlkBase = new uint8[mBlkSize];
	mBlkSub = new uint8[mBlkSize];
	mSpriteCopt = new uint8[0xD5A * 16];
	mSpriteArmy = new uint8[0xD50 * 16];

	mSurface = 0;

	SetupBlkPtrs();
	ResetCamera();
}

void cOFED::ResetCamera() {
	mMapX = 0;
	mMapY = 0;
	mMapTilePtr = 0;
	mSelectedTile = 0;

	mCameraTilesX = 0x20;
	mCameraTilesY = 0x0F * 2;

	if (mCameraTilesX > mMapWidth)
		mCameraTilesX = mMapWidth;

	if (mCameraTilesY > mMapHeight)
		mCameraTilesY = mMapHeight;

	delete mSurface;
	mSurface = new cSurface( mCameraTilesX * 16, mCameraTilesY * 16 );
	mSurface->clearBuffer();

	LoadPalette( mSurface );
}

void cOFED::SetTile( size_t pTileX, size_t pTileY, size_t pTileType ) {

	uint32 Tile = g_OFED.mMapTilePtr + (((pTileY * g_OFED.mMapWidth) + pTileX));

	uint8* CurrentMapPtr = &mMap[0x60 + (Tile*2)];

	writeLEWord( CurrentMapPtr, pTileType );
}

void cOFED::SetCursorTile( size_t pTileType ) {
	mCursorTile = pTileType;
}

void cOFED::SetSelectedTile( size_t pTile ) {
	mSelectedTile = pTile;
}

void cOFED::LoadPalette( cSurface* pSurface ) {
	
	pSurface->paletteLoad( mBlkBase + 0xFA00, 0x80, 0x00 );
	pSurface->paletteLoad( mSpriteCopt + 0xD2A0, 0x40, 0xB0 );
	pSurface->paletteLoad( mSpriteCopt + 0xD360, 0x10, 0x90 );
	pSurface->paletteLoad( mSpriteArmy + 0xD200, 0x10, 0xA0 );
	pSurface->paletteLoadNewSDL();
}

void cOFED::CreateMap( eTileTypes pTileType, size_t pWidth, size_t pHeight ) {
	
	if (pTileType < 0)
		pTileType = eTileTypes_Jungle;

	delete[] mMap;

	mMapWidth = pWidth;
	mMapHeight = pHeight;

	mMapSize = 0x60 + ((mMapWidth * mMapHeight) * 2);
	mMap = new uint8[mMapSize];
	memset( mMap, 0, mMapSize );

	mMap[0x50] = 'O';mMap[0x51] = 'F';mMap[0x52] = 'E';mMap[0x53] = 'D';

	mBaseName = mTileType_Names[pTileType];
	mSubName = mTileType_Names[pTileType];
	mBaseCoptName = mTileType_Names[pTileType];
	mBaseArmyName = mTileType_Names[pTileType];

	mBaseName.append( "base.blk" );
	mSubName.append( "sub0.blk" );
	mBaseCoptName.append( "copt.dat" );
	mBaseArmyName.append( "army.dat" );

	mBaseName.copy( (char*) mMap, 11 );
	mSubName.copy( (char*)mMap + 0x10, 0x10 + 11 );

	writeBEWord( &mMap[0x54], mMapWidth );
	writeBEWord( &mMap[0x56], mMapHeight );

	LoadBlk();
	ResetCamera();
	DrawTiles();
}

void cOFED::LoadBlk() {

	mBlkBaseSize = g_Resource.fileLoadTo( mBaseName, mBlkBase );
	mBlkSubSize = g_Resource.fileLoadTo( mSubName, mBlkSub );

	mBaseCoptSize = g_Resource.fileLoadTo( mBaseCoptName, mSpriteCopt );
	mBaseArmySize = g_Resource.fileLoadTo( mBaseArmyName, mSpriteArmy );

	mBlocksLoaded = true;
}

void cOFED::LoadMap( std::string pFilename ) {

	delete[] mMap;

	mMap = local_FileRead( pFilename, "", mMapSize, true );
	tool_EndianSwap( mMap + 0x60, mMapSize - 0x60 );

	mBaseName.clear();
	mSubName.clear();

	mBaseName.append( mMap, mMap + 11 );
	mSubName.append( mMap + 0x10, mMap + 0x10 + 11 );

	mBaseCoptName.append( mMap, mMap + 3 );
	mBaseCoptName.append( "copt.dat" );
	mBaseArmyName.append( mMap, mMap + 3 );
	mBaseArmyName.append( "army.dat" );

	mMapWidth = readBEWord( &mMap[0x54] );
	mMapHeight = readBEWord( &mMap[0x56] );

	LoadBlk();
	ResetCamera();
	DrawTiles();
}

void cOFED::SaveMap( std::string pFilename ) {
	
	std::ofstream outfile( pFilename, std::ofstream::binary );

	tool_EndianSwap( mMap + 0x60, mMapSize - 0x60 );
	outfile.write( (const char*)mMap, mMapSize );
	outfile.close();

	tool_EndianSwap( mMap + 0x60, mMapSize - 0x60 );

}

uint8* cOFED::GetSpriteData( uint16 pSegment ) {

	switch (pSegment) {

	case 0x4309:
		return mSpriteCopt;
		break;

	case 0x430B:
		return mSpriteArmy;
		break;
	}
}

void cOFED::SetupBlkPtrs() {
	uint16 bx = 0, dx = 0;

	for (uint16 cx = 0; cx < 240; ++cx) {

		mBlkPtrs[cx + 0x00] = mBlkBase + bx;
		mBlkPtrs[cx + 0xF0] = mBlkSub + bx;

		++dx;
		bx += 0x10;
		if (dx % 0x14 == 0) {
			dx = 0;
			bx += 0x12C0;
		}
	}
}

void cOFED::DrawTiles() {
	mSurface->clearBuffer();

	uint8* Target = mSurface->GetSurfaceBuffer();

	uint8* CurrentMapPtr = &mMap[0x60 + (mMapTilePtr * 2)];

	memset( mCursorRow, 60, 16 );

	// Y
	for (uint16 cx = 0; cx < mCameraTilesY; ++cx) {
		uint8* MapPtr = CurrentMapPtr;
		uint8* TargetRow = Target;
		bool isSelectedTile = false;

		// X
		for (uint16 cx2 = 0; cx2 < mCameraTilesX; ++cx2) {
			uint8* TargetTmp = TargetRow;

			//if (MapPtr == &mMap[0x60 + (mSelectedTile * 2)])
			//	isSelectedTile = true;
			//else
			//	isSelectedTile = false;

			uint16 Tile = readLEWord( MapPtr ) & 0x1FF;
			if (Tile > 0x1DF)
				Tile = 0;

			uint8* TilePtr = mBlkPtrs[Tile];
				
			//TilePtr += 0x140;

			// Each Tile Row
			for (uint16 i = 0; i < 16; ++i) {

				if ((i == 0 || i == 1 || i == 14 || i == 15) && isSelectedTile)
					memcpy( TargetTmp, mCursorRow, 16 );
				else {
					memcpy( TargetTmp, TilePtr, 16 );
					if (isSelectedTile) {
						TargetTmp[0] = 60;
						TargetTmp[1] = 60;
						TargetTmp[14] = 60;
						TargetTmp[15] = 60;
					}
				}
				TilePtr += 0x140;
				TargetTmp += mSurface->GetWidth();
			}

			MapPtr += 2;
			TargetRow += 16;
		}

		Target += mSurface->GetWidth() * 16;
		CurrentMapPtr += mMapWidth << 1;
	}


}

void cOFED::DrawSprite( cSurface* pTarget ) {

	uint8*	di = pTarget->GetSurfaceBuffer();
	uint8* 	si = mDrawSpriteFrameDataPtr;
	int16	ax, cx;

	di += pTarget->GetWidth() * mDrawSpritePositionY;

	ax = mDrawSpritePositionX;
	//ax += Fodder->word_40054;
	//ax >>= 2;

	di += ax;
	word_42066 = di;
	cx = mDrawSpritePositionX;
	//cx += Fodder->word_40054;
	cx &= 3;

	uint8 Plane = 0;

	int8 bl = byte_42070;

	mDrawSpriteColumns >>= 1;
	word_42074 = 160 - mDrawSpriteColumns;
	mDrawSpriteColumns >>= 1;

	word_42076 = (uint16)(pTarget->GetWidth() - (mDrawSpriteColumns * 4));

	di += Plane;
	for (int16 dx = mDrawSpriteRows; dx > 0; --dx) {

		for (cx = 0; cx < mDrawSpriteColumns; ++cx) {
			int8 al = (*si) >> 4;
			if (al)
				*di = al | bl;

			si += 2;
			di += 4;
		}

		si += word_42074;
		di += word_42076;
	}

	++Plane;
	if (Plane == 4) {
		Plane = 0;
		++word_42066;
	}

	si = mDrawSpriteFrameDataPtr;
	di = word_42066;
	di += Plane;
	for (int16 dx = mDrawSpriteRows; dx > 0; --dx) {

		for (cx = mDrawSpriteColumns; cx > 0; --cx) {
			int8 al = (*si) & 0x0F;
			if (al)
				*di = al | bl;

			si += 2;
			di += 4;
		}

		si += word_42074;
		di += word_42076;
	}

	++Plane;
	if (Plane == 4) {
		Plane = 0;
		++word_42066;
	}

	++mDrawSpriteFrameDataPtr;
	si = mDrawSpriteFrameDataPtr;
	di = word_42066;
	di += Plane;
	for (int16 dx = mDrawSpriteRows; dx > 0; --dx) {

		for (cx = mDrawSpriteColumns; cx > 0; --cx) {

			int8 al = (*si) >> 4;
			if (al)
				*di = al | bl;

			si += 2;
			di += 4;

		}
		si += word_42074;
		di += word_42076;
	}

	++Plane;
	if (Plane == 4) {
		Plane = 0;
		++word_42066;
	}

	si = mDrawSpriteFrameDataPtr;
	di = word_42066;
	di += Plane;
	for (int16 dx = mDrawSpriteRows; dx > 0; --dx) {

		for (cx = mDrawSpriteColumns; cx > 0; --cx) {

			int8 al = (*si) & 0x0F;
			if (al)
				*di = al | bl;

			si += 2;
			di += 4;
		}

		si += word_42074;
		di += word_42076;
	}
}

void cOFED::DrawSprite( cSurface* pTarget, uint16 pSpriteID, uint16 pDestX, uint16 pDestY ) {

	byte_42070 = off_32C0C[pSpriteID][0].field_C & 0xFF;
	mDrawSpriteFrameDataPtr = GetSpriteData( off_32C0C[pSpriteID][0].field_2 );
	mDrawSpriteFrameDataPtr += off_32C0C[pSpriteID][0].field_0;

	mDrawSpriteColumns = off_32C0C[pSpriteID][0].mColCount;
	mDrawSpriteRows = off_32C0C[pSpriteID][0].mRowCount;

	mDrawSpritePositionX = (off_32C0C[pSpriteID][0].field_E + pDestX) + 0x40;
	mDrawSpritePositionY = (off_32C0C[pSpriteID][0].field_F + pDestY) - mDrawSpriteRows;
	mDrawSpritePositionY += 0x10;

	DrawSprite( pTarget );
}

void cOFED::DrawTile( cSurface* pTarget, uint16 pTile, uint16 pDestX, uint16 pDestY, uint16 pOffset ) {
	uint8* Target = pTarget->GetSurfaceBuffer();

	if (!mBlocksLoaded)
		return;

	pDestX *= (16 + pOffset);

	Target += (pDestY * (16 + pOffset)) * pTarget->GetWidth();
	Target += pDestX;

	uint8* TilePtr = mBlkPtrs[pTile];

	for (uint16 i = 0; i < 16; ++i) {

		memcpy( Target, TilePtr, 16 );
		TilePtr += 0x140;
		Target += pTarget->GetWidth();
	}
}

void cOFED::SetMapX( int64 pMapX ) {
	if (!mMap)
		return;

	mMapX = pMapX;

	if (mMapX <= 0)
		mMapX = 0;

	if (mMapX > mMapWidth - mCameraTilesX)
		mMapX = mMapWidth - mCameraTilesX;

	mMapTilePtr = (mMapY * mMapWidth) + mMapX;

	DrawTiles();
}

void cOFED::SetMapY( int64 pMapY ) {
	if (!mMap)
		return;

	mMapY = pMapY;
	
	if (mMapY <= 0)
		mMapY = 0;

	if (mMapY > mMapHeight - mCameraTilesY)
		mMapY = mMapHeight - mCameraTilesY;

	mMapTilePtr = (mMapY * mMapWidth) + mMapX;

	DrawTiles();
}
