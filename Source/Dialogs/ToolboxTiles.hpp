class Ui_ToolboxTiles;

class cToolboxTiles : public QDialog {
	Ui_ToolboxTiles *mUi;

protected:

	void				paintEvent(QPaintEvent* e);

public:
	cToolboxTiles(Ui_ToolboxTiles* ui, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

};
