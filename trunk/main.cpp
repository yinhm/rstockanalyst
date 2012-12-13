#include <QApplication>
#include <iostream>
#include <QtGui>
#include "STKDRV.h"
#include "MainWindow.h"
#include "DataEngine.h"
#include "ColorManager.h"

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

	//初始化颜色表
	CColorManager::initAll();

	CSTKDRV::InitStockDrv();

	CMainWindow::getMainWindow()->showMaximized();

	
	//设置银江数据接口
	//if(!CMainWindow::getMainWindow()->setupStockDrv())
	//{
	//	return app.exit();
	//}
	

	CDataEngine::importData();				//初始化数据

	CMainWindow::getMainWindow()->initTemplates();	//初始化模板
	app.exec();
	CMainWindow::getMainWindow()->saveTemplates();	//保存所有模板

	CDataEngine::exportData();				//导出数据


	return 1;
}
