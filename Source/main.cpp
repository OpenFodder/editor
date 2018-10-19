
#include "ofed.hpp"
#include <QtWidgets/QApplication>

cOFED* g_OFED;

int main(int argc, char *argv[]) {

	QApplication a(argc, argv);

	g_OFED = new cOFED();
	g_OFED->show();

	return a.exec();
}