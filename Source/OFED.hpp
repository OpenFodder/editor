enum eTileTypes {
	eTileTypes_Jungle = 0,
	eTileTypes_Desert = 1,
	eTileTypes_Ice = 2,
	eTileTypes_Moors = 3,
	eTileTypes_Int = 4,
	eTileTypes_Hid = 5,
	eTileTypes_AFX = 6,
};

extern int32 g_SpriteAnim[111];

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

	uint32			mMapTilePtr;

	size_t			mMapSize, mMapSptSize;
	size_t			mMapWidth, mMapHeight;

	int64			mMapX, mMapY;
	size_t			mCameraTilesX, mCameraTilesY;
	uint32			mSelectedTile;
	int32			mCursorTile;
	int32			mCursorSprite;

	uint8*			mDrawSpriteFrameDataPtr;
	uint16			mDrawSpriteColumns;
	uint16			mDrawSpriteRows;
	uint16			mDrawSpritePositionX;
	uint16			mDrawSpritePositionY;
	uint8			byte_42070;
	uint8*			word_42066;
	uint16			word_42074;
	uint16			word_42076;

	cSurface*		mSurface;

	void			SetupBlkPtrs();
	uint8*			GetSpriteData( uint16 pSegment );

	public:

					cOFED();

	void			DrawSprite( cSurface* pTarget );
	void			DrawSprite( cSurface* pTarget, uint16 pSpriteID, uint16 pDestX, uint16 pDestY, bool pAdjust );
	void			DrawTile( cSurface* pTarget, uint16 pTile, uint16 pDestX, uint16 pDestY, uint16 pOffset = 0 );
	
	void			DrawTiles();
	void			DrawSprites();

	void			SetMapX( int64 pMapX );
	void			SetMapY( int64 pMapY );

	void			LoadBlk();
	void			LoadPalette( cSurface* pSurface );

	void			CreateMap( eTileTypes pTileType, size_t pWidth, size_t pHeight );
	void			LoadMap( std::string pFilename );
	void			SaveMap( std::string pFilename );

	void			LoadSprites( std::string pFilename );
	void			SaveSprites( std::string pFilename );
	void			SetupSprites();

	void			ResetCamera();

	void			AddSprite( size_t pTileX, size_t pTileY );

	void			SetTile( size_t pTileX, size_t pTileY, size_t pTileType );
	void			SetCursorTile( size_t pTileType );
	void			SetSelectedTile( size_t pTile );
	void			SetCursorSprite( size_t pSpriteID );

	cSurface*		GetSurface() const { return mSurface; }
};
