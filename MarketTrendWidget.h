/************************************************************************/
/* 文件名称：MarketTrendWidget.h
/* 创建时间：2012-11-08 10:59
/*
/* 描    述：用于显示市场行情图的窗口，继承自CBaseWidget
/************************************************************************/

#ifndef MARKET_TREND_WIDGET_H
#define	MARKET_TREND_WIDGET_H
#include "BaseWidget.h"
#include "StockInfoItem.h"

class CMarketTrendWidget : public CBaseWidget
{
	Q_OBJECT
	//struct stHeaderInfo
	//{
	//	QString text;
	//	int column;
	//};
public:
	CMarketTrendWidget(CBaseWidget* parent = 0);
	~CMarketTrendWidget(void);

public:
	//加载该K线图的配置信息
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//保存该K线图的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

public:
	void setStocks(const QList<CStockInfoItem*>& list);		//设置要显示的股票列表

public slots:
	void stockInfoChanged(const QString& code);				//某只股票的信息发生改变

private:
	void clearTmpData();						//清理本窗口中创建的内存。
	void clickedHeader(int column);				//当点击头部时触发
	void clickedStock(CStockInfoItem* pItem);	//当点击股票时触发
	void offsetShowHeaderIndex(int offset);		//改变当前头部显示的开始位置
	void clickedBlock(const QString& block);	//点击板块时触发

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void resizeEvent(QResizeEvent* e);				//大小改变的事件
	virtual void mousePressEvent(QMouseEvent* e);			//鼠标点击事件
	virtual void wheelEvent(QWheelEvent* e);				//鼠标中键滚动事件

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

private:
	QMenu* m_pMenuCustom;					//自定义菜单
	QStringList m_listHeader;				//头部数据
	QList<CStockInfoItem*> m_listStocks;	//要显示的股票列表
	QMap<CStockInfoItem*,int> m_mapStockIndex;	//用来快速查找某只股票所在的索引
	CStockInfoItem* m_pSelectedStock;		//当前选中的股票
	int m_iSortColumn;						//当前进行排序的列
	Qt::SortOrder m_sortOrder;				//当前排序方式

	/*用于绘制的成员函数*/
private:
	void updateDrawRect();					//更新绘制区域的大小
	void updateBlockRect();					//更新底部板块按钮的显示区域
	void drawHeaders(QPainter& p);			//绘制头部信息
	void drawStocks(QPainter& p);			//绘制股票信息
	void drawBottom(QPainter& p);			//绘制底部信息
	void drawStock(QPainter& p,const QRect& rtStock,CStockInfoItem* pItem);	//绘制单个股票
	void drawBottomBtn(QPainter& p);		//绘制底部的两个按钮

	//判断鼠标点击的位置，做出相应的响应
	//void hitHeader(QPoint& ptCur);

	QString dataOfDisplay(CStockInfoItem* itemData,int column);
	QColor dataOfColor(CStockInfoItem* itemData,int column);
	QRect rectOfStock(CStockInfoItem* pItem);			//获取某只股票显示的位置

	/*用于绘制的成员变量*/
private:
	QRect m_rtHeader;						//头部Header区域
	QRect m_rtClient;						//实际行情区域
	QRect m_rtBottom;						//底部区域，用于绘制分类，鼠标操作等信息

	QRect m_rtPreIndex;						//上一个起始行
	QRect m_rtNextIndex;					//下一个起始行

	int m_iHeaderHeight;					//头部的高度
	int m_iStockHeight;						//单个行情块的高度
	int m_iBottomHeight;					//底部的高度

	int showHeaderIndex;					//开始显示的头部索引-横向（0,1,2,5即为5-3=2）
	int showStockIndex;						//开始显示的股票索引-纵向
	int showBlockIndex;						//开始显示的板块索引-底部横向
	QList<int> m_listItemWidth;				//各个item的宽度。
	QList<QPair<QString,QRect>> m_listBlocks;	//各个分类所在的矩形
	QString m_qsSelectedBlock;				//当前选中的板块
};


#endif	//MARKET_TREND_WIDGET_H