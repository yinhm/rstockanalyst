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

class CKLineWidget : QWidget
{
	Q_OBJECT
public:
	CKLineWidget(QWidget* parent = 0);
	~CKLineWidget(void);

public slots:
	void setStockCode(const QString& code);
	void updateKLine(const QString& code);

protected:
	void paintEvent(QPaintEvent* e);

private:
	CStockInfoItem* m_pStockItem;	//当前K线图的股票数据指针
};


#endif	//K_LINE_WIDGET_H