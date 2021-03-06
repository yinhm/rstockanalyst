#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include "BaseWidget.h"
#include "BlockInfoItem.h"

class CMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	CMainWindow();
	~CMainWindow();
	static CMainWindow* getMainWindow();


public:
	bool setupStockDrv();
	void initTemplates();			//初始化所有模板
	void saveTemplates();			//保存所有模板
	void initBlockMenus();			//初始化板块的菜单（全局）

	QMenu* getBlockMenu(CBaseWidget* pWidget,const int iCmd)
	{
		m_pBlockMenuWidget = pWidget;
		m_iBlockMenuCmd = iCmd;
		return &m_menuBlocks;
	}

public:
	void clickedStock(const QString& code);
	void clickedBlock(const QString& block);

protected slots:
	void onAddTemplate();			//添加版面
	void onSaveTemplate();			//保存所有版面
	void onRemoveTemplate();		//删除版面
	void onShowFuncHelper();		//显示函数帮助窗口


	void onMarketCloseSetting();	//设置收盘后数据整理时间
	void onMarketClose();			//收盘后数据重新整理
	void onBlockActClicked();		//板块菜单项被点击后触发
	void onMarketCloseTimer();		//15点后休盘时触发此函数（需在函数中进行检查）

	void onShowAllStocks();			//显示所有的股票信息

	//
	void onWatcherSetting();		//监控雷达设置

	//重新计算板块数据的定时器响应函数
	void onCalcBlock();

protected:
	virtual bool winEvent( MSG* message, long* result );


protected:
	virtual long OnStockDrvMsg(WPARAM wParam,LPARAM lParam);

private:
	CBaseWidget* getSubWindows(const QString& title);
	void CreateSubBlockMenu(QMenu* pMenuParent,CBlockInfoItem* pBlockParent);

private:
	static CMainWindow* m_pMainWindow;
	QTabWidget* m_pTabWidget;
	QMenuBar* m_pMenuBar;
	QString m_qsTemplateDir;		//板块配置文件所在的目录

	CBaseWidget* m_pBlockMenuWidget;					//当前BlockMenu对应的窗口指针
	int m_iBlockMenuCmd;								//当前BlockMenu对应的操作
	QMenu m_menuBlocks;									//板块菜单
	QMap<CBlockInfoItem*,QAction*> mapBlockMenus;		//板块菜单map

	QTimer m_timerClose;								//是否休盘
	QTime m_tmExportClose;								//导出休盘数据的时间

	QTimer m_timerCalcBlock;							//重新计算板块数据

	bool m_bExportClose;								//是否已经导出休盘数据
};


#endif	//MAIN_WINDOW_H