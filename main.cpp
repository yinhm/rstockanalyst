#include <QApplication>
#include <QtGui>
#include "MyWindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	QMainWindow mainWindow;
	CMainWindow widget;
	mainWindow.setCentralWidget(&widget);

	mainWindow.showMaximized();
	app.exec();
	return 1;
}
