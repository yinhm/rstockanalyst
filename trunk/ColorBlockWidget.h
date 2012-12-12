/************************************************************************/
/* 文件名称：ColorBlockWidget.h
/* 创建时间：2012-11-19 10:59
/*
/* 描    述：用于显示多只股票的走势，继承自CBaseWidget
/*           创造性的使用不同的色块来表示股票的升降走势，从而在相同的区域中，
/*           展现更大的信息量
/************************************************************************/

#ifndef COLOR_BLOCK_WIDGET_H
#define COLOR_BLOCK_WIDGET_H
#include "BaseWidget.h"
#include "StockInfoItem.h"

class CColorBlockWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CColorBlockWidget(CBaseWidget* parent = 0);
	~CColorBlockWidget(void);

public:
	//加载该K线图的配置信息
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//保存该K线图的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);


public slots:
	virtual void setBlock(const QString& block);
	virtual void updateStock(const QString& code);			//更新某只股票的显示

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

	/*相关绘图函数*/
private:
	void drawHeader(QPainter& p,const QRect& rtHeader);			//绘制头部信息
	void drawClient(QPainter& p,const QRect& rtClient);			//绘制主区域
	void drawBottom(QPainter& p,const QRect& rtBottom);			//绘制底部区域

	void drawStock(QPainter& p,const QRect& rtCB,CStockInfoItem* pItem);	//绘制单只股票

private:
	QMenu* m_pMenuCustom;					//自定义菜单
	QString m_qsBlock;						//当前的板块名称
	QList<CStockInfoItem*> m_listStocks;	//当前显示的所有股票列表

	/*用于绘制操作的成员变量*/
private:
	int m_iCBHeight;						//单个色块的高度
	int showStockIndex;
};

#endif	//COLOR_BLOCK_WIDGET_H