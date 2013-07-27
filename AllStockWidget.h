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
	enum RSortType
	{
		SortByCode = 1,		//按股票代码排序
		SortByZGB,			//按总股本排序
		SortByIncrease,		//按涨幅排序
		SortByTurnRatio,	//按换手率排序
		SortByVolumeRatio,	//按量比排序
	};
public:
	CAllStockWidget(CBaseWidget* parent = 0, RWidgetType type = WidgetAllStock);
	~CAllStockWidget(void);


public:
	void setCircle(int _cr);
	void setSortType(RSortType _st);

signals:
	void stockFocus(CStockInfoItem*);

protected:
	virtual void paintEvent(QPaintEvent*);
	virtual void mouseMoveEvent(QMouseEvent* e);

	void drawStock(CStockInfoItem* pStock,const QRect& rtStock,QPainter& p);

	void clearStockDatas();
	void updateStockDatas();


	CStockInfoItem* hitTestStock(const QPoint& ptPoint) const;

private:
	int m_iItemWidth;				//单个节点宽度
	int m_iItemHeight;				//单个节点高度
	int m_iStockWidth;				//单个股票的宽度
	int m_iStockHeight;				//单个股票的高度
	RStockCircle m_crCircle;		//当前的显示周期
	RSortType m_stSort;				//排序类型
	Qt::SortOrder m_stOrder;		//排序方式（升序、降序）

	QVector<uint> m_vColors;		//颜色表（21个）


	int m_iScaleX;					//
	int m_iScaleY;					//
	QPixmap m_pixScale;				//
	QDateTime m_tmLastUpdate;		//last update time;
private:
	QList<CStockInfoItem*> m_listStocks;		//所有的股票
	QMap<CStockInfoItem*,QList<RStockData*>*> m_mapStockDatas;
};

#endif	//All_STOCK_WIDGET_H