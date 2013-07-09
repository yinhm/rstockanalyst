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
#include "BlockInfoItem.h"

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
	//通过查找keyword获取需要在按键精灵上显示的数据
	virtual void getKeyWizData(const QString& keyword,QList<KeyWizData*>& listRet);
	//键盘精灵窗口确认后触发
	virtual void keyWizEntered(KeyWizData* pData);

public slots:
	void stockInfoChanged(const QString& code);				//某只股票的信息发生改变
	void onAddToBlock();									//添加当前选中的股票到板块
	void onAddToNewBlock();									//添加当前选中的股票到新加板块
	
	/*
		虚函数，派生类中需重载此函数以进行相应操作
	*/
	virtual void setBlock(const QString& block);

private:
	void clearTmpData();						//清理本窗口中创建的内存。
	void clickedHeader(int column);				//当点击头部时触发
	void clickedStock(CAbstractStockItem* pItem);	//当点击股票时触发
	void offsetShowHeaderIndex(int offset);		//改变当前头部显示的开始位置
	void clickedBlock(CBlockInfoItem* block);	//点击板块时触发

	void setStocks(const QList<CAbstractStockItem*>& list);		//设置要显示的股票列表

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void resizeEvent(QResizeEvent* e);				//大小改变的事件
	virtual void mousePressEvent(QMouseEvent* e);			//鼠标点击事件
	virtual void mouseDoubleClickEvent(QMouseEvent * e);	//双击事件
	virtual void wheelEvent(QWheelEvent* e);				//鼠标中键滚动事件
	virtual void keyPressEvent(QKeyEvent* e);				//键盘操作

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

	virtual void onBlockClicked(CBlockInfoItem* pBlock,int iCmd);

private:
	QMenu* m_pMenuCustom;					//自定义菜单
	QStringList m_listHeader;				//头部数据
	QList<CAbstractStockItem*> m_listStocks;	//要显示的股票列表
	QMap<CAbstractStockItem*,int> m_mapStockIndex;	//用来快速查找某只股票所在的索引
	CAbstractStockItem* m_pSelectedStock;		//当前选中的股票
	int m_iSortColumn;						//当前进行排序的列
	Qt::SortOrder m_sortOrder;				//当前排序方式

	/*用于绘制的成员函数*/
private:
	void updateDrawRect();					//更新绘制区域的大小
	void updateBlockRect();					//更新底部板块按钮的显示区域
	void drawHeaders(QPainter& p);			//绘制头部信息
	void drawStocks(QPainter& p);			//绘制股票信息
	void drawBottom(QPainter& p);			//绘制底部信息
	void drawStock(QPainter& p,const QRect& rtStock,CAbstractStockItem* pItem);	//绘制单个股票
	void drawBottomBtn(QPainter& p);		//绘制底部的两个按钮

	//判断鼠标点击的位置，做出相应的响应
	//void hitHeader(QPoint& ptCur);

	void resortStocks();					//重新对股票列表进行排序

	QString dataOfDisplay(CAbstractStockItem* itemData,int column);
	QColor dataOfColor(CAbstractStockItem* itemData,int column);
	QColor dataOfFillColor(CAbstractStockItem* itemData,int column);
	QRect rectOfStock(CAbstractStockItem* pItem);			//获取某只股票显示的位置

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
	QList<QPair<CBlockInfoItem*,QRect>> m_listBlocks;	//各个分类所在的矩形
	CBlockInfoItem* m_pSelectedBlock;					//当前选中的板块


	bool m_bLoaded;
};


#endif	//MARKET_TREND_WIDGET_H