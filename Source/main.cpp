
#include "ofed.hpp"
#include <QtWidgets/QApplication>

cOFED* g_OFED;
std::shared_ptr<sFodderParameters> g_Parameters;

int	start(int argc, char *argv[]) {

	g_Parameters = std::make_shared<sFodderParameters>();
	g_Parameters->Process(argc, argv);

	QApplication a(argc, argv);

	g_OFED = new cOFED();
	g_OFED->show();

	return a.exec();
}