#include "QtDisplay.hpp"
#include <QWindow>
#include <QApplication>


int 
main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtDisplay display;
	display.Initialize();
	display.show();
	return a.exec();
}
