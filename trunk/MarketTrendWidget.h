/************************************************************************/
/* 文件名称：MarketTrendWidget.h
/* 创建时间：2012-11-08 10:59
/*
/* 描    述：用于显示市场行情图的窗口，继承自CBaseWidget
/************************************************************************/

#ifndef MARKET_TREND_WIDGET_H
#define	MARKET_TREND_WIDGET_H
#include "BaseWidget.h"

class CMarketTrendWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CMarketTrendWidget(CBaseWidget* parent = 0);
	~CMarketTrendWidget(void);

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void resizeEvent(QResizeEvent* e);				//大小改变的事件

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

private:
	QMenu* m_pMenuCustom;					//自定义菜单
	QStringList m_listHeader;				//头部数据

	/*用于绘制的成员函数*/
private:
	void updateDrawRect();					//更新绘制区域的大小
	void drawHeaders(QPainter& p);						//绘制头部信息

	/*用于绘制的成员变量*/
private:
	QRect m_rtHeader;						//头部Header区域
	QRect m_rtClient;						//实际行情区域
	QRect m_rtBottom;						//底部区域，用于绘制分类，鼠标操作等信息

	int m_iHeaderHeight;					//头部的高度
	int m_iItemHeight;						//单个行情块的高度
	int m_iBottomHeight;					//底部的高度
};


#endif	//MARKET_TREND_WIDGET_H