#include <QApplication>
#include <iostream>
#include <QtGui>
#include "STKDRV.h"
#include "MainWindow.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "KLineWidget.h"
#include "SplashDlg.h"

lua_State* g_Lua = 0;


int loadAllFunc()
{
	int iDllCount = 0;
	QDir dirFunc(qApp->applicationDirPath() + "/plugin/");
	QFileInfoList list = dirFunc.entryInfoList(QStringList()<<"*.dll");
	foreach (const QFileInfo& v,list)
	{
		HINSTANCE hDll = LoadLibrary(v.absoluteFilePath().toStdWString().data());
		if(hDll)
		{
			qDebug()<<"Load form "<<v.absoluteFilePath();
			int (WINAPI* pfnALlFuncs)(QMap<QString,lua_CFunction>&)= NULL;
			pfnALlFuncs = \
				(int(WINAPI *)(QMap<QString,lua_CFunction>&)) GetProcAddress(hDll,"ExportAllFuncs");
			if(pfnALlFuncs)
			{
				QMap<QString,lua_CFunction> _funcs;
				(*pfnALlFuncs)(_funcs);

				qDebug()<<"Load funcs from\""<<v.baseName()<<"\":";
				QMap<QString,lua_CFunction>::iterator iter = _funcs.begin();
				while(iter!=_funcs.end())
				{
					lua_register(g_Lua,iter.key().toAscii(),iter.value());
					qDebug()<<iter.key();
					++iter;
				}
			}
			else
			{
				qDebug()<<"Load Dll Error!";
			}
		}
	}

	return iDllCount;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	g_Lua = luaL_newstate();			//实例化Lua
	loadAllFunc();


	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));

	CSplashDlg splash;
	splash.show();

	splash.showMessage(QObject::tr("加载样式表"),10);
	QFile file(":/res/qss/FeiHu.qss");
	if(file.open(QFile::ReadOnly))
	{
		QString qsStyleSheet = file.readAll();
		app.setStyleSheet(qsStyleSheet);
	}

	//初始化颜色表
	CColorManager::initAll();
	//初始化js脚本
	CKLineWidget::initJSScript();

	splash.showMessage(QObject::tr("安装驱动"),20);
	CSTKDRV::InitStockDrv();

	CMainWindow::getMainWindow()->hide();



	splash.showMessage(QObject::tr("初始化数据..."),40);
	app.processEvents();
	CDataEngine::importData();				//初始化数据


	//初始化主窗口
	CMainWindow::getMainWindow()->initTemplates();	//初始化模板

	splash.showMessage(QObject::tr("正在启动..."),99);
	CMainWindow::getMainWindow()->showMaximized();

	//设置银江数据接口
	splash.showMessage(QObject::tr("设置银江数据接口"),30);
	app.processEvents();
	if(!CMainWindow::getMainWindow()->setupStockDrv())
	{
		return app.exit();
	}

	splash.hide();

	app.exec();
	splash.show();

	splash.showMessage(QObject::tr("正在保存配置..."),10);
	CMainWindow::getMainWindow()->saveTemplates();	//保存所有模板
	splash.showMessage(QObject::tr("正在导出数据..."),20);

	CDataEngine::exportData();				//导出数据
	splash.showMessage(QObject::tr("导出数据完成..."),99);


	//释放资源
	delete CMainWindow::getMainWindow();
	CDataEngine::releaseDataEngine();

	return 1;
}
