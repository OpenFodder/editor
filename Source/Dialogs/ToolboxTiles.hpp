class Ui_ToolboxTiles;

class cToolboxTiles : public QDialog {
	Ui_ToolboxTiles *mUi;

	cSurface		*mTileSurface;

	QImage			 mImage;

	double			 mScaleWidth, mScaleHeight;

protected:

	void paintEvent(QPaintEvent* e) override;
	void mousePressEvent(QMouseEvent *eventPress) override;
    void mouseMoveEvent(QMouseEvent *eventMove) override;

public:
	cToolboxTiles( QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~cToolboxTiles();

	void RenderTiles();

};
