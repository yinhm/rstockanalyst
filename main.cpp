#include <QApplication>
#include <iostream>
#include <QtGui>
#include "STKDRV.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	CSTKDRV::InitStockDrv();

	CMainWindow mainWindow;
	mainWindow.show();

	if(!mainWindow.setupStockDrv())
		return app.exit();

	return app.exec();
}
