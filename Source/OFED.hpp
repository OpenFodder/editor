enum eTileTypes {
	eTileTypes_Jungle = 0,
	eTileTypes_Desert = 1,
	eTileTypes_Ice = 2,
	eTileTypes_Moors = 3,
	eTileTypes_Int = 4,
	eTileTypes_Hid = 5,
	eTileTypes_AFX = 6,
};

class cOFED : public cSingleton < cOFED > {

	public:
	std::string		mBaseName, mSubName;
	size_t			mBlkSize, mBlkBaseSize, mBlkSubSize;
	uint8*			mBlkBase;
	uint8*			mBlkSub;
	uint8*			mBlkPtrs[480];
	bool			mBlocksLoaded;

	uint8			mCursorRow[16];

	uint8*			mMap;
	uint32			mMapTilePtr;

	size_t			mMapSize;
	size_t			mMapWidth, mMapHeight;

	int64			mMapX, mMapY;
	size_t			mCameraTilesX, mCameraTilesY;
	uint32			mSelectedTile;
	uint32			mCursorTile;

	cSurface*		mSurface;

	void			SetupBlkPtrs();

	public:

					cOFED();

	void			DrawTile( cSurface* pTarget, uint16 pTile, uint16 pDestX, uint16 pDestY );
	void			DrawTiles();

	void			SetMapX( int64 pMapX );
	void			SetMapY( int64 pMapY );

	void			LoadBlk();
	void			LoadPalette( cSurface* pSurface );

	void			CreateMap( eTileTypes pTileType, size_t pWidth, size_t pHeight );
	void			LoadMap( std::string pFilename );
	void			SaveMap( std::string pFilename );

	void			ResetCamera();

	void			SetTile( size_t pTileX, size_t pTileY, size_t pTileType );
	void			SetCursorTile( size_t pTileType );
	void			SetSelectedTile( size_t pTile );

	cSurface*		GetSurface() const { return mSurface; }
};
