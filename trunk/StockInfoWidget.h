/************************************************************************/
/* 文件名称：StockInfoWidget.h
/* 创建时间：2012-12-13 17:55
/*
/* 描    述：用于显示单只股票的行情信息，继承自CBaseWidget
/************************************************************************/
#ifndef STOCK_INFO_WIDGET_H
#define STOCK_INFO_WIDGET_H
#include "BaseWidget.h"
#include "StockInfoItem.h"

class CStockInfoWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CStockInfoWidget(CBaseWidget* parent = 0);
	~CStockInfoWidget(void);

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

private:
	QMenu* m_pMenuCustom;					//自定义菜单
};


#endif	//STOCK_INFO_WIDGET_H