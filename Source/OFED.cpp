
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
	
	mBlkSize = 0xFD00 * 16;

	mBlkBase = new uint8[mBlkSize];
	mBlkSub = new uint8[mBlkSize];

	mSurface = 0;

	SetupBlkPtrs();
	ResetCamera();
}

void cOFED::ResetCamera() {
	mMapX = 0;
	mMapY = 0;
	mMapTilePtr = 0;

	mCameraTilesX = 0x20;
	mCameraTilesY = 0x0F * 2;

	if (mCameraTilesX > mMapWidth)
		mCameraTilesX = mMapWidth;

	if (mCameraTilesY > mMapHeight)
		mCameraTilesY = mMapHeight;

	delete mSurface;
	mSurface = new cSurface( mCameraTilesX * 16, mCameraTilesY * 16 );
	mSurface->clearBuffer();
	mSurface->paletteLoad( mBlkBase + 0xFA00, 0x80, 0x00 );
	mSurface->paletteLoadNewSDL();
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
	mBaseName.append( "base.blk" );
	mSubName.append( "sub0.blk" );

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
}

void cOFED::LoadMap( std::string pFilename ) {

	delete[] mMap;

	mMap = local_FileRead( pFilename, "", mMapSize, true );
	tool_EndianSwap( mMap + 0x60, mMapSize - 0x60 );

	mBaseName.clear();
	mSubName.clear();

	mBaseName.append( mMap, mMap + 11 );
	mSubName.append( mMap + 0x10, mMap + 0x10 + 11 );

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

	// Y
	for (uint16 cx = 0; cx < mCameraTilesY; ++cx) {
		uint8* MapPtr = CurrentMapPtr;
		uint8* TargetRow = Target;

		uint16 StartY = 0;

			StartY = 0;

		// X
		for (uint16 cx2 = 0; cx2 < mCameraTilesX; ++cx2) {
			uint8* TargetTmp = TargetRow;

			uint16 Tile = readLEWord( MapPtr ) & 0x1FF;
			if (Tile > 0x1DF)
				Tile = 0;

			uint8* TilePtr = mBlkPtrs[Tile];
			uint16 StartX = 0;

			TilePtr += StartY * 0x140;

				StartX = 0;

			// Each Tile Row
			for (uint16 i = StartX; i < 16; ++i) {

				memcpy( TargetTmp, TilePtr + StartX, 16 - StartX );
				TilePtr += 0x140;
				TargetTmp += mSurface->GetWidth();
			}

			MapPtr += 2;
			TargetRow += (16 - StartX);
		}

		Target += mSurface->GetWidth() * (16 - StartY);
		CurrentMapPtr += mMapWidth << 1;
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
