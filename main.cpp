#include <QApplication>
#include <iostream>
#include <QtGui>
#include "STKDRV.h"
#include "MainWindow.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "KLineWidget.h"
#include "SplashDlg.h"
#include "RadarWatcher.h"

luaL_Reg* g_pFuncs = 0;			//所有需要注册的lua函数
QString g_native = "";			//lua源码
QVector<HINSTANCE> g_vDll;		//加载的函数扩展动态链接库

int loadAllFunc()
{
	int iDllCount = 0;
	QMap<const char*,lua_CFunction> g_func;

	QDir dirFunc(qApp->applicationDirPath() + "/plugin/");
	QFileInfoList list = dirFunc.entryInfoList(QStringList()<<"*.dll");
	foreach (const QFileInfo& v,list)
	{
		HINSTANCE hDll = LoadLibrary(v.absoluteFilePath().toStdWString().data());
		if(hDll)
		{
			g_vDll.push_back(hDll);
			qDebug()<<"Load form "<<v.absoluteFilePath();
			QMap<const char*,lua_CFunction>* (WINAPI* pfnALlFuncs)()= NULL;
			pfnALlFuncs = \
				(QMap<const char*,lua_CFunction>*(WINAPI *)()) GetProcAddress(hDll,"ExportAllFuncs");
			if(pfnALlFuncs)
			{
				QMap<const char*,lua_CFunction>* _funcs = (*pfnALlFuncs)();

				g_func.unite(*_funcs);
				qDebug()<<"Load funcs from\""<<v.baseName()<<"\":";
				delete _funcs;
			}
			else
			{
				qDebug()<<"Load Dll Error!";
			}
			//FreeLibrary(hDll);
		}
	}

	QMap<const char*,lua_CFunction>::iterator iter = g_func.begin();
	g_pFuncs= new luaL_Reg[g_func.size()+1];
	luaL_Reg* pFunc = g_pFuncs;
	qDebug()<<"---------Funcs-----------";
	while(iter!=g_func.end())
	{
		qDebug()<<iter.key();
		pFunc->name = iter.key();
		pFunc->func = iter.value();
		//			lua_register(m_pL,iter.key().toAscii(),iter.value());
		++iter;
		++pFunc;
	}
	pFunc->name = NULL;
	pFunc->func = NULL;
	return iDllCount;
}

void OnShowSplashMessage(const QString& msg,int iPro)
{
	CSplashDlg::getSplashDlg()->showMessage(msg,iPro);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	{
		//加载lua相关函数
		QFile file(qApp->applicationDirPath()+"/native.lua");
		if(file.open(QFile::ReadOnly))
		{
			g_native = file.readAll();
		}
		loadAllFunc();
	}

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));

	CSplashDlg::getSplashDlg()->show();

	CSplashDlg::getSplashDlg()->showMessage(QObject::tr("加载样式表"),10);
	QFile file(":/res/qss/FeiHu.qss");
	if(file.open(QFile::ReadOnly))
	{
		QString qsStyleSheet = file.readAll();
		app.setStyleSheet(qsStyleSheet);
	}

	//初始化颜色表
	CColorManager::initAll();

	CSplashDlg::getSplashDlg()->showMessage(QObject::tr("安装驱动"),20);
	CSTKDRV::InitStockDrv();

	CMainWindow::getMainWindow()->hide();


	//关联消息进度窗口和数据引擎
	CDataEngine::messageShowed = &OnShowSplashMessage;


	CSplashDlg::getSplashDlg()->showMessage(QObject::tr("初始化数据..."),40);
	app.processEvents();
	CDataEngine::importData();				//初始化数据


	//初始化主窗口
	CMainWindow::getMainWindow()->initTemplates();	//初始化模板
	CMainWindow::getMainWindow()->initBlockMenus();	//初始化板块菜单

	CSplashDlg::getSplashDlg()->getSplashDlg()->showMessage(QObject::tr("正在启动..."),99);
	CMainWindow::getMainWindow()->showMaximized();

	//设置银江数据接口
	CSplashDlg::getSplashDlg()->showMessage(QObject::tr("设置银江数据接口"),30);
	app.processEvents();
	if(!CMainWindow::getMainWindow()->setupStockDrv())
	{
		return app.exit();
	}

	CSplashDlg::getSplashDlg()->hide();

	app.exec();

	CSplashDlg::getSplashDlg()->show();

	CSplashDlg::getSplashDlg()->showMessage(QObject::tr("正在保存配置..."),10);
	CMainWindow::getMainWindow()->saveTemplates();	//保存所有模板

	
	CSplashDlg::getSplashDlg()->showMessage(QObject::tr("正在导出数据..."),20);

	CDataEngine::exportData();				//导出数据

	CSplashDlg::getSplashDlg()->showMessage(QObject::tr("导出数据完成..."),99);

	//收盘导出存盘
	//CDataEngine::exportCloseData();

	//释放资源
	delete CMainWindow::getMainWindow();
	CDataEngine::releaseDataEngine();

	foreach(HINSTANCE hDll,g_vDll)
	{
		FreeLibrary(hDll);
	}

	return 1;
}
