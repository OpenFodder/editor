
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

int32 g_SpriteAnim[111] = {};

cOFED::cOFED() {

	SetupSprites();

	new cResource_PC_CD();

	mMap = 0;
	mMapSpt = 0;
	mBlocksLoaded = false;

	mBlkSize = 0xFD00 * 16;

	mBlkBase = new uint8[mBlkSize];
	mBlkSub = new uint8[mBlkSize];
	mSpriteCopt = new uint8[0xD5A * 16];
	mSpriteArmy = new uint8[0xD50 * 16];

	mSurface = 0;

	mCursorTile = -1;
	mCursorSprite = -1;

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

void cOFED::AddSprite( size_t pTileX, size_t pTileY ) {

	sSpriteDef Sprite;

	Sprite.mIgnored = 0;
	Sprite.mX = pTileX;
	Sprite.mY = pTileY;
	Sprite.mDirection = 0x7C;
	Sprite.mSpriteID = mCursorSprite;

	mSprites.push_back( Sprite );
}

void cOFED::SetTile( size_t pTileX, size_t pTileY, size_t pTileType ) {

	uint32 Tile = g_OFED.mMapTilePtr + (((pTileY * g_OFED.mMapWidth) + pTileX));

	uint8* CurrentMapPtr = &mMap[0x60 + (Tile*2)];

	writeLEWord( CurrentMapPtr, pTileType );
}

void cOFED::SetCursorTile( size_t pTileType ) {
	mCursorSprite = -1;
	mCursorTile = pTileType;
}

void cOFED::SetCursorSprite( size_t pSpriteID ) {
	mCursorTile = -1;
	mCursorSprite = pSpriteID;
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
	delete[] mMapSpt;

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

	mSprites.clear();

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
	mBaseCoptName.clear();
	mBaseArmyName.clear();

	mBaseName.append( mMap, mMap + 11 );
	mSubName.append( mMap + 0x10, mMap + 0x10 + 11 );

	mBaseCoptName.append( mMap, mMap + 3 );
	mBaseCoptName.append( "copt.dat" );
	mBaseArmyName.append( mMap, mMap + 3 );
	mBaseArmyName.append( "army.dat" );

	mMapWidth = readBEWord( &mMap[0x54] );
	mMapHeight = readBEWord( &mMap[0x56] );

	LoadSprites( pFilename );

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
	SaveSprites( pFilename );
}

void cOFED::LoadSprites( std::string pFilename ) {
	std::string SptFilename = pFilename;

	SptFilename.replace( pFilename.length() - 3, pFilename.length(), "spt" );

	delete[] mMapSpt;

	mMapSpt = local_FileRead( SptFilename, "", mMapSptSize, true );
	if (mMapSpt == 0)
		return;

	tool_EndianSwap( mMapSpt, mMapSptSize );

	uint16* SptPtr = (uint16*) mMapSpt;
	uint16* SptPtrEnd = SptPtr + (mMapSptSize / 2);

	for (; SptPtr != SptPtrEnd; ) {
		sSpriteDef Sprite;

		++SptPtr;
		Sprite.mDirection = 0x7C;
		++SptPtr;

		uint16 ax = SptPtr[0];
		++SptPtr;
		Sprite.mX = ax;

		ax = SptPtr[0];
		++SptPtr;
		Sprite.mY = ax;

		ax = SptPtr[0];
		++SptPtr;
		Sprite.mSpriteID = ax;

		mSprites.push_back( Sprite );
	}
}

void cOFED::SaveSprites( std::string pFilename ) {
	std::string SptFilename = pFilename;

	SptFilename.replace( pFilename.length() - 3, pFilename.length(), "spt" );

	std::ofstream outfile( SptFilename, std::ofstream::binary );

	mMapSptSize = 0x0A * mSprites.size();
	mMapSpt = new uint8[mMapSptSize];

	uint8* SptPtr = mMapSpt;

	for (std::vector<sSpriteDef>::iterator SpriteIT = mSprites.begin(); SpriteIT != mSprites.end(); ++SpriteIT) {

		writeBEWord( SptPtr, SpriteIT->mDirection ); SptPtr += 2;
		writeBEWord( SptPtr, SpriteIT->mIgnored );	 SptPtr += 2;
		writeBEWord( SptPtr, SpriteIT->mX );		 SptPtr += 2;
		writeBEWord( SptPtr, SpriteIT->mY );		 SptPtr += 2;
		writeBEWord( SptPtr, SpriteIT->mSpriteID );	 SptPtr += 2;
	}

	outfile.write( (const char*)mMapSpt, mMapSptSize );
	outfile.close();

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

	DrawSprites();
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

void cOFED::DrawSprite( cSurface* pTarget, uint16 pSpriteID, uint16 pDestX, uint16 pDestY, bool pAdjust ) {

	byte_42070 = off_32C0C[pSpriteID][0].field_C & 0xFF;
	mDrawSpriteFrameDataPtr = GetSpriteData( off_32C0C[pSpriteID][0].field_2 );
	mDrawSpriteFrameDataPtr += off_32C0C[pSpriteID][0].field_0;

	mDrawSpriteColumns = off_32C0C[pSpriteID][0].mColCount;
	mDrawSpriteRows = off_32C0C[pSpriteID][0].mRowCount;

	if (pAdjust) {
		mDrawSpritePositionX = (off_32C0C[pSpriteID][0].field_E + pDestX) +0x10;
		mDrawSpritePositionY = (off_32C0C[pSpriteID][0].field_F + pDestY) - mDrawSpriteRows;
		//mDrawSpritePositionY += 0x10;
	}
	else {
		mDrawSpritePositionX = pDestX;
		mDrawSpritePositionY = pDestY;
	}
	DrawSprite( pTarget );
}

void cOFED::DrawSprites() {

	for( std::vector<sSpriteDef>::iterator SpriteIT = mSprites.begin(); SpriteIT != mSprites.end(); ++SpriteIT ) {

		DrawSprite( mSurface, g_SpriteAnim[ SpriteIT->mSpriteID ], SpriteIT->mX, SpriteIT->mY, true );
	}
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


void cOFED::SetupSprites() {
	for (int i = 0; i < 111; ++i) {
		g_SpriteAnim[i] = -1;
	}

	g_SpriteAnim[eSprite_Player] = 0x00;
	g_SpriteAnim[eSprite_Enemy] = 0x52;

	g_SpriteAnim[eSprite_Shrub] = 0x8F;
	g_SpriteAnim[eSprite_Tree] = 0x90;
	g_SpriteAnim[eSprite_BuildingRoof] = 0x91;
	g_SpriteAnim[eSprite_Shrub2] = 0x93;
	g_SpriteAnim[eSprite_Waterfall] = 0x94;
	g_SpriteAnim[eSprite_Bird2_Left] = 0x98;

	g_SpriteAnim[eSprite_BuildingDoor] = 0x99;
	g_SpriteAnim[eSprite_BuildingDoor2] = 0x9B;
	g_SpriteAnim[eSprite_Floating_Dead_Soldier] = 0x9E;
	g_SpriteAnim[eSprite_Enemy_Rocket] = 0x39;
	g_SpriteAnim[eSprite_GrenadeBox] = 0xC2;
	g_SpriteAnim[eSprite_RocketBox] = 0xC3;

	g_SpriteAnim[eSprite_Helicopter_Grenade_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Grenade2_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Missile_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Homing_Enemy] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Grenade2_Human] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Grenade_Human] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Missile_Human] = 0x8B;
	g_SpriteAnim[eSprite_Helicopter_Homing_Human] = 0x8B;

	g_SpriteAnim[eSprite_Mine] = 0xC7;
	g_SpriteAnim[eSprite_Mine2] = 0xC8;
	g_SpriteAnim[eSprite_Spike] = 0xC9;

	g_SpriteAnim[eSprite_BoilingPot] = 0xCD;
	g_SpriteAnim[eSprite_Indigenous] = 0xD0;
	g_SpriteAnim[eSprite_Indigenous2] = 0xD0;
	g_SpriteAnim[eSprite_VehicleNoGun_Human] = 0xA5;
	g_SpriteAnim[eSprite_VehicleGun_Human] = 0xA5;

	g_SpriteAnim[eSprite_Tank_Human] = 0xD1;
	g_SpriteAnim[eSprite_Bird_Left] = 0xD3;
	g_SpriteAnim[eSprite_Bird_Right] = 0xD4;
	g_SpriteAnim[eSprite_Seal] = 0xD5;
	g_SpriteAnim[eSprite_Tank_Enemy] = 0xD1;

	g_SpriteAnim[eSprite_Indigenous_Spear] = 0xD0;
	g_SpriteAnim[eSprite_Hostage] = 0xD9;
	g_SpriteAnim[eSprite_Hostage_Rescue_Tent] = 0xDD;

	g_SpriteAnim[eSprite_Door_Indigenous] = 0x9B;
	g_SpriteAnim[eSprite_Door2_Indigenous] = 0x9B;
	g_SpriteAnim[eSprite_Door_Indigenous_Spear] = 0x9B;

	g_SpriteAnim[eSprite_Turret_Missile_Human] = 0xD2;
	g_SpriteAnim[eSprite_Turret_Missile2_Human] = 0xD2;

	g_SpriteAnim[eSprite_VehicleNoGun_Enemy] = 0xA5;
	g_SpriteAnim[eSprite_VehicleGun_Enemy] = 0xA5;
	g_SpriteAnim[eSprite_Vehicle_Unk_Enemy] = 0xA5;
	g_SpriteAnim[eSprite_Indigenous_Invisible] = 0xD6;

	g_SpriteAnim[eSprite_Turret_Missile_Enemy] = 0xD2;
	g_SpriteAnim[eSprite_Turret_Missile2_Enemy] = 0xD2;
	g_SpriteAnim[eSprite_BuildingDoor3] = 0xE0;

	g_SpriteAnim[eSprite_OpenCloseDoor] = 0x9B;
	g_SpriteAnim[eSprite_Seal_Mine] = 0xD5;
	g_SpriteAnim[eSprite_Spider_Mine] = 0xE2;

	g_SpriteAnim[eSprite_Bonus_RankToGeneral] = 0x95;
	g_SpriteAnim[eSprite_Bonus_Rockets] = 0xE4;
	g_SpriteAnim[eSprite_Bonus_RocketsAndGeneral] = 0xE5;
	g_SpriteAnim[eSprite_Bonus_SquadGeneralRockets] = 0xE6;
	g_SpriteAnim[eSprite_Helicopter_CallPad] = 0xE7;

	g_SpriteAnim[eSprite_Turret_HomingMissile_Enemy] = 0xD2;
	g_SpriteAnim[eSprite_Hostage_2] = 0xD9;
	g_SpriteAnim[eSprite_Helicopter_Homing_Enemy2] = 0x8B;
	g_SpriteAnim[eSprite_Computer_1] = 0x8F;
	g_SpriteAnim[eSprite_Computer_2] = 0x8F;
	g_SpriteAnim[eSprite_Computer_3] = 0x8F;
}