class Ui_ToolboxSprites;

struct sRange {
	size_t mX, mY;
	size_t mWidth, mHeight;
	size_t mSpriteID;

	sRange(size_t pX, size_t pY, size_t pWidth, size_t pHeight, size_t pSpriteID) {
		mX = pX;
		mY = pY;
		mWidth = pWidth;
		mHeight = pHeight;
		mSpriteID = pSpriteID;
	}
};

class cToolboxSprites : public QDialog {
	Ui_ToolboxSprites *mUi;

	std::vector< sRange >	 mSpriteRanges;
	cSurface				*mSpriteSurface;

	QImage			 mImage;

	double			 mScaleWidth, mScaleHeight;

protected:

	void paintEvent(QPaintEvent* e) override;
	void mousePressEvent(QMouseEvent *eventPress) override;
    void mouseMoveEvent(QMouseEvent *eventMove) override;

public:
	cToolboxSprites(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~cToolboxSprites();

	void RenderSprites();

};
