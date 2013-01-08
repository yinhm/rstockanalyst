#include <QApplication>
#include <iostream>
#include <QtGui>
#include "STKDRV.h"
#include "MainWindow.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "KLineWidget.h"
#include "SplashDlg.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

static int pppp(lua_State* _L)
{
	int n = lua_gettop(_L);
	const char* aaa = lua_tostring(_L,0);
	lua_getglobal(_L,"QString");
	QString* p = (QString*)(lua_topointer(_L,-1));
	qDebug()<<*p;
	return 1;
	for (int i=1;i<=n;++i)
	{
		int _type = lua_type(_L,i);
		printf(lua_tostring(_L,i));
	}
	return 1;
}

int main(int argc, char *argv[])
{
	lua_CFunction pFun = &pppp;
	lua_State* L;
	L = luaL_newstate();
	QString qsTitle = "test for pointer to lua";

	lua_register(L,"print",pFun);
	lua_pushlightuserdata(L,&qsTitle);
	lua_setglobal(L,"QString");
	luaL_dostring(L,"print(\"Lua worked!!!\\r\\n\")");
//	luaL_dostring(L,"print(math.abs(-33))");
	//luaL_error(L,aaa);
	//qDebug()<<QString::fromLocal8Bit(aaa);
	lua_close(L);
	return 0;


    QApplication app(argc, argv);

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
