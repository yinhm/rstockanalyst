#include <QApplication>
#include <iostream>
#include <QtGui>
#include "STKDRV.h"
#include "MainWindow.h"
#include "DataEngine.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));

	QFile file(":/res/qss/FeiHu.qss");
	if(file.open(QFile::ReadOnly))
	{
		QString qsStyleSheet = file.readAll();
		app.setStyleSheet(qsStyleSheet);
	}

	CSTKDRV::InitStockDrv();

	CMainWindow::getMainWindow()->showMaximized();

	
	if(!CMainWindow::getMainWindow()->setupStockDrv())
	{
		return app.exit();
	}
	

	CDataEngine::importData();

	CMainWindow::getMainWindow()->initTemplates();
	app.exec();

	CDataEngine::exportData();


	return 1;
}
