class Ui_MapView;

class cMapView : public QDialog {
	Ui_MapView *mUi;

	QImage			 mImage;

	double			 mScaleWidth, mScaleHeight;

protected:

	void paintEvent(QPaintEvent* e) override;

public:
	cMapView( QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~cMapView();

	void RenderTiles();

};
