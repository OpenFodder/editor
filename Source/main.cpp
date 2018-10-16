
#include "ofed.hpp"
#include <QtWidgets/QApplication>

std::shared_ptr<cOFED> g_OFED;

int main(int argc, char *argv[]) {

	QApplication a(argc, argv);

	g_OFED = std::make_shared<cOFED>();
	g_OFED->show();

	return a.exec();
}