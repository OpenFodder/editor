class Ui_NewMapDialog;

class cNewMapDialog : public QDialog {
	Ui_NewMapDialog *mUi;

public:
	cNewMapDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

	void accept();
	void reject();
};
