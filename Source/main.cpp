
#include "ofed.hpp"
#include <QtWidgets/QApplication>

cOFED* g_OFED;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	cOFED* w = new cOFED();
	w->show();

	return a.exec();
}