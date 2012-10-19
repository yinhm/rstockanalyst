#include <QApplication>
#include <iostream>
#include <QtGui>
#include "STKDRV.h"

static CSTKDRV g_drv;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


	return app.exec();
}
