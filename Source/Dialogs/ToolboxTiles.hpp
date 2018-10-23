class Ui_ToolboxTiles;

class cToolboxTiles : public QDialog {
	Ui_ToolboxTiles *mUi;

	cSurface		*mTileSurface;

	QImage			 mImage;

	double			 mScaleWidth, mScaleHeight;

protected:

	void paintEvent(QPaintEvent* e);
	void mousePressEvent(QMouseEvent *eventPress);
    void mouseMoveEvent(QMouseEvent *eventMove) override;

public:
	cToolboxTiles( QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~cToolboxTiles();

	void RenderTiles();

};
