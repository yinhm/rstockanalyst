/************************************************************************/
/* 文件名称：KLineWidget.h
/* 创建时间：2012-11-08 10:59
/*
/* 描    述：用于显示K线图的窗口，继承自CBaseWidget
/************************************************************************/

#ifndef K_LINE_WIDGET_H
#define K_LINE_WIDGET_H
#include <QtGui>
#include "BaseWidget.h"
#include "StockInfoItem.h"
#include "BaseLiner.h"


class CKLineWidget : public CBaseWidget
{
	Q_OBJECT
public:
	enum KLineCircle			//K线周期
	{
		Min1 = 1,				//1分钟
		Min5,					//5分钟
		Min15,					//15分钟
		Min30,					//30分钟
		Min60,					//60分钟
		MinN,					//N分钟
		Day,					//日线
		DayN,					//N日线
		Week,					//周线
		Month,					//月线
		Month3,					//季线
		Year,					//年线
	};
public:
	CKLineWidget(CBaseWidget* parent = 0);
	~CKLineWidget(void);

	static bool initJSScript();

public:
	//加载该K线图的配置信息
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//保存该K线图的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

public slots:
	virtual void setStockCode(const QString& code);
	void updateKLine(const QString& code);

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void mouseMoveEvent(QMouseEvent* e);			//鼠标移动事件
	virtual void leaveEvent(QEvent* e);						//鼠标移出事件
	virtual void mousePressEvent(QMouseEvent* e);			//鼠标点击事件
	virtual void mouseDoubleClickEvent(QMouseEvent *e);		//鼠标双击事件

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

protected slots:
	void onSetStockCode();								//弹出对话框，让用户手动输入股票代码
	void onSetExpression();								//弹出对话框，让用户手动输入表达式
	void onClickedAddShow();							//减少显示个数
	void onClickedSubShow();							//增加显示个数
	void onSetCircle();									//设置当前的显示周期
	void onAddDeputy();									//增加副图
	void onAddVolume();									//是否显示量视图
	void onRemoveDeputy();								//删除副图
	void onShowMainChanged(bool bShow);					//是否显示主图
	void onSetSizes();									//设置所有图的显示比例

private:
	void drawTitle(QPainter& p,const QRect& rtTitle);	//绘制头部
	void drawCoordX(QPainter& p,const QRect& rtCoordX);	//绘制底部的X坐标轴（时间轴）
	void drawShowBtns(QPainter& p,const QRect& rtBtns);	//绘制右下角的两个按钮
private:
	void resetTmpData();					//重新计算数据。
	void clearTmpData();					//清理本窗口中创建的内存。

private:
	QMenu* m_pMenuCustom;					//自定义菜单
	QMenu* m_pMenuCircle;					//周期设置菜单
	QAction* m_pActShowMain;				//是否显示主图

	KLineCircle m_typeCircle;				//本图的显示周期
	CStockInfoItem* m_pStockItem;			//当前K线图的股票数据指针
	QVector<stLinerItem> listItems;			//所有用于显示的数据
	int m_iShowCount;						//需要显示的数据个数（长度，理论上应小于listItems的size）
	CMultiLiner* m_pLinerMain;				//主图，K线主图
	CMultiLiner* m_pCurrentLiner;			//当前选中的图（包括主图和副图）
	QList<CMultiLiner*> m_listLiners;		//副图，包括 成交量/成交额，公式指标图等

	bool m_bShowMax;						//是否最大化显示（目前只对副图有效）


	int m_iTitleHeight;						//头部高度
	int m_iCoorYWidth;						//Y坐标轴的宽度
	int m_iCoorXHeight;						//X坐标轴的高度
	float fItemWidth;						//单个Item的宽度
	int m_iMainLinerHeight;					//主图的高度

	QVector<int> m_vSizes;					//显示比例(总和为100)

	QRect m_rtAddShow;						//增加显示个数的按钮区域
	QRect m_rtSubShow;						//减少显示个数的按钮区域

	QScriptEngine* m_pScriptEngine;			//脚本解释器

	static QString g_qsScript;				//js脚本
};


#endif	//K_LINE_WIDGET_H