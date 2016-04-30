
class cOFED : public cSingleton < cOFED > {

	public:

	size_t			mBlkSize, mBlkBaseSize, mBlkSubSize;
	uint8*			mBlkBase;
	uint8*			mBlkSub;
	uint8*			mBlkPtrs[480];

	uint8*			mMap;
	uint32			mMapTilePtr;

	size_t			mMapSize;

	size_t			mMapWidth, mMapHeight;

	int64			mMapX, mMapY;
	size_t			mCameraTilesX, mCameraTilesY;

	cSurface*		mSurface;

	void			SetupBlkPtrs();

	public:

					cOFED();

	void			DrawTiles();

	void			SetMapX( int64 pMapX );
	void			SetMapY( int64 pMapY );

	void			LoadMap( std::string pFilename );
	void			ResetCamera();

	cSurface*		GetSurface() const { return mSurface; }
};
