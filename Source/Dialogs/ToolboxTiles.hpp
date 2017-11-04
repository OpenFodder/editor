class Ui_ToolboxTiles;

class cToolboxTiles : public QDialog {
	Ui_ToolboxTiles *mUi;
	cSurface		*mTileSurface;
	QImage			 mSurface;
protected:

	void paintEvent(QPaintEvent* e);

public:
	cToolboxTiles( QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~cToolboxTiles();

	void RenderTiles();

};
