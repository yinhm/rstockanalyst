/************************************************************************/
/* 文件名称：AllStockWidget.h
/* 创建时间：2013-07-05 14:07
/*
/* 描    述：用于所以股票的快速预览显示
/************************************************************************/

#ifndef All_STOCK_WIDGET_H
#define All_STOCK_WIDGET_H
#include "BaseWidget.h"

class CAllStockWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CAllStockWidget(CBaseWidget* parent = 0, RWidgetType type = WidgetAllStock);
	~CAllStockWidget(void);

protected:
	virtual void paintEvent(QPaintEvent*);

	void drawStock(CStockInfoItem* pStock,const QRect& rtStock,QPainter& p);

private:
	int m_iItemWidth;				//单个节点宽度
	int m_iItemHeight;				//单个节点高度
	int m_iStockWidth;				//单个股票的宽度
	int m_iStockHeight;				//单个股票的高度

	QVector<uint> m_vColors;		//颜色表（21个）
private:
	QList<CStockInfoItem*> m_listStocks;		//所有的股票
};

#endif	//All_STOCK_WIDGET_H