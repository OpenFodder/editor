class Ui_NewMapDialog;

class cNewMapDialog : public QDialog {
	Ui_NewMapDialog *mUi;

public:
	cNewMapDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

	void accept() override;
	void reject() override;
};
