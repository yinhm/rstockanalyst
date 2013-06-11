#ifndef STOCK_DEAL_WIDGET_H
#define STOCK_DEAL_WIDGET_H
#include "CoordXBaseWidget.h"

enum RDealWidgetType
{
	DealKind = 1,
};

class CStockDealWidget : public CCoordXBaseWidget
{
	Q_OBJECT
public:
	CStockDealWidget(CBaseWidget* parent = 0);
	~CStockDealWidget(void);


protected:
	//设置当前显示的股票
	virtual void setStockCode(const QString& code);

	//更新数据，临时使用
	virtual void updateData();

	//绘图
	virtual void paintEvent(QPaintEvent* e);

protected:
	void drawTitle(QPainter& p);
	void drawClient(QPainter& p);
	void drawBottom(QPainter& p);

private:
	int m_iTitleHeight;		//标题高度
	int m_iBottomHeight;	//底部高度

	int m_iItemWidth;		//单个色块的宽度
	int m_iItemHeight;	//单个色块的高度

	QRect m_rtTitle;		//标题区域
	QRect m_rtClient;		//绘图区域
	QRect m_rtBottom;		//底部区域

	RDealWidgetType m_typeWidget; //当前窗口的显示类型

private:
	CStockInfoItem* m_pCurStock;			//当前的股票
	QList<qRcvFenBiData*> m_listFenbi;		//当前股票的分笔数据
	QMap<time_t,int> m_mapTimes;			//当前需要显示的所有时间（横向坐标）
};


#endif	//STOCK_DEAL_WIDGET_H