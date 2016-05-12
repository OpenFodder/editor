enum eTileTypes {
	eTileTypes_Jungle = 0,
	eTileTypes_Desert = 1,
	eTileTypes_Ice = 2,
	eTileTypes_Moors = 3,
	eTileTypes_Int = 4,
	eTileTypes_Hid = 5,
	eTileTypes_AFX = 6,
};

enum eTileSub {
	eTileSub_0 = 0,
	eTileSub_1 = 1,
};

struct sRangeTile {
	int16 mX;
	int16 mY;
	uint16 mTileID;

	sRangeTile( int16 pMapX, int16 pMapY, uint16 pID ) {
		mX = pMapX;
		mY = pMapY;
		mTileID = pID;
	}

};

struct sRangeSprite {
	int16 mX;
	int16 mY;
	uint16 mSpriteID;

	sRangeSprite( int16 pTileX, int16 pTileY, uint16 pSpriteID ) {
		mX = pTileX;
		mY = pTileY;
		mSpriteID = pSpriteID;
	}
};

struct sTiles {
	std::vector<sRangeTile> mTiles;
	std::vector<sRangeSprite> mSprites;
};

class cFrameOFED;

extern int32 g_SpriteAnim[111];
extern std::string g_SpriteName[111];

extern cFrameOFED* g_FrameOFED;

class cOFED : public cSingleton < cOFED > {

	public:
	std::string		mBaseName, mSubName, mBaseCoptName, mBaseArmyName;
	size_t			mBlkSize, mBlkBaseSize, mBlkSubSize, mBaseCoptSize, mBaseArmySize;
	uint8*			mBlkBase;
	uint8*			mBlkSub;
	uint8*			mBlkPtrs[480];
	bool			mBlocksLoaded;

	uint8*			mSpriteCopt;
	uint8*			mSpriteArmy;

	std::vector<sSpriteDef> mSprites;

	uint8			mCursorRow[16];

	uint8*			mMap;
	uint8*			mMapSpt;

	eTileTypes		mMapTileType;
	eTileSub		mMapTileSubType;

	uint32			mMapTilePtr;

	size_t			mMapSize, mMapSptSize;
	size_t			mMapWidth, mMapHeight;

	int64			mMapX, mMapY;
	size_t			mCameraTilesX, mCameraTilesY;
	uint32			mSelectedTile;

	int32			mCursorTile;
	int32			mCursorSprite;
	sTiles			mCursorRangeTiles;

	uint8*			mDrawSpriteFrameDataPtr;
	uint16			mDrawSpriteColumns;
	uint16			mDrawSpriteRows;
	int16			mDrawSpritePositionX;
	int16			mDrawSpritePositionY;
	uint8			byte_42070;
	uint8*			word_42066;
	uint16			word_42074;
	uint16			word_42076;

	cSurface*		mSurface;

	void			SetupBlkPtrs();
	uint8*			GetSpriteData( uint16 pSegment );

	public:

					cOFED();
					~cOFED();

	void			DrawSprite( cSurface* pTarget );
	void			DrawSprite( cSurface* pTarget, uint16 pSpriteID, uint16 pDestX, uint16 pDestY, bool pAdjust );
	void			DrawTile( cSurface* pTarget, uint16 pTile, uint16 pDestX, uint16 pDestY, uint16 pOffset = 0 );
	
	void			DrawTiles();
	void			DrawSprites();
	bool			Sprite_OnScreen_Check();

	void			SetMapX( int64 pMapX );
	void			SetMapY( int64 pMapY );

	void			LoadBlk();
	void			LoadPalette( cSurface* pSurface );

	void			map_SetTileType();

	void			CreateMap( eTileTypes pTileType, eTileSub pTileSub, size_t pWidth, size_t pHeight );
	void			LoadMap( std::string pFilename );
	void			SaveMap( std::string pFilename );

	void			LoadSprites( std::string pFilename );
	void			SaveSprites( std::string pFilename );
	void			SetupSprites();

	void			ResetCamera();

	void			AddSprite( size_t pTileX, size_t pTileY );
	void			SetTile( size_t pTileX, size_t pTileY, size_t pTileType );

	void			SetCursorTile( size_t pTileType );
	void			SetCursorTileRange( sTiles& pRangeTiles );
	void			SetCursorSprite( size_t pSpriteID );

	void			SetSelectedTile( size_t pTile );


	cSurface*		GetSurface() const { return mSurface; }
};
