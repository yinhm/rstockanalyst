/************************************************************************/
/* 文件名称：KLineWidget.h
/* 创建时间：2012-11-08 10:59
/*
/* 描    述：用于显示K线图的窗口
/************************************************************************/

#ifndef K_LINE_WIDGET_H
#define K_LINE_WIDGET_H
#include <QtGui>
#include "StockInfoItem.h"

class CKLineWidget : public QWidget
{
	Q_OBJECT
public:
	CKLineWidget(QWidget* parent = 0);
	~CKLineWidget(void);

public slots:
	void setStockCode(const QString& code);
	void updateKLine(const QString& code);

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void mouseMoveEvent(QMouseEvent* e);			//鼠标移动事件

private:
	void drawCoordY(QPainter& p,const QRectF& rtClient);	//绘制Y坐标轴
	void drawKGrids(QPainter& p,const QRectF& rtClient);	//绘制X坐标轴和数据
	void drawKGrid(qRcvHistoryData* pHistory,QPainter& p,const QRectF& rtClient);		//单个日线数据的绘制
private:
	void resetTmpData();					//重新计算数据。
	void clearTmpData();					//清理本窗口中创建的内存。

private:
	CStockInfoItem* m_pStockItem;			//当前K线图的股票数据指针
	QList<qRcvHistoryData*> listHistory;	//K线图所用到的历史数据。
	/*纵坐标*/
	float fMaxPrice;						//K线图中的最低价
	float fMinPrice;						//K线图中的最高价
	/*横坐标*/
	time_t tmBegin;
	time_t tmEnd;

	float fKGridWidth;						//单个日线数据的宽度
};


#endif	//K_LINE_WIDGET_H