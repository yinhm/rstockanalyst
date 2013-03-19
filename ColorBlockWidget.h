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
#include "BaseBlockWidget.h"
#include "BlockInfoItem.h"

class CColorBlockWidget : public CBaseBlockWidget
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

	//更新数据
	virtual void updateData();

public slots:
	virtual void setBlock(const QString& block);
	virtual void updateStock(const QString& code);			//更新某只股票的显示

protected slots:
	void onSetCurrentBlock();								//设置当前显示的板块
	void updateColorBlockData();							//更新当前需要显示的数据

private:
	void clearTmpData();						//清理本窗口中创建的内存。
	void clickedStock(CStockInfoItem* pItem);	//当点击股票时触发

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void mouseMoveEvent(QMouseEvent* e);			//鼠标移动事件
	virtual void mousePressEvent(QMouseEvent* e);			//鼠标点击事件
	virtual void wheelEvent(QWheelEvent* e);				//鼠标中键滚动事件
	virtual void keyPressEvent(QKeyEvent* e);				//键盘操作

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

	/*相关绘图函数*/
private:
	void drawHeader(QPainter& p,const QRect& rtHeader);			//绘制头部信息
	void drawClient(QPainter& p,const QRect& rtClient);			//绘制主区域
	void drawBottom(QPainter& p,const QRect& rtBottom);			//绘制底部区域

	void drawStock(QPainter& p,const QRect& rtCB,CStockInfoItem* pItem);	//绘制单只股票

	QRect rectOfStock(CStockInfoItem* pItem);					//获取某只股票显示的位置
	CStockInfoItem* hitTestStock(const QPoint& ptPoint) const;		//测试某点所指向的股票信息
	RStockData hitTestCBItem(const QPoint& ptPoint) const;//测试某点所指向的色块信息

	//获取数据二维表，通过分析当前的周期。
	QMap<time_t,RStockData>* getColorBlockMap(CStockInfoItem* pItem);

private:
	QMenu* m_pMenuBlockList;				//所有板块信息（当前选中的板块上打勾）

	CBlockInfoItem* m_pBlock;						//当前的板块名称
	QList<CStockInfoItem*> m_listStocks;	//当前显示的所有股票列表

	QMap<CStockInfoItem*,int> m_mapStockIndex;	//用来快速查找某只股票所在的索引
	QMap<CStockInfoItem*,QMap<time_t,RStockData>*> mapStockColorBlocks;	//当前显示的ColorBlock数据
	CStockInfoItem* m_pSelectedStock;			//当前选中的股票

	/*用于绘制操作的成员变量*/
private:
	int m_iTitleHeight;						//头部高度
	int m_iBottomHeight;					//底部的高度
	int showStockIndex;						//当前显示的起始位置（列）

	QRect m_rtHeader;						//头部Header区域
	QRect m_rtClient;						//实际色块绘制区域
	QRect m_rtBottom;						//底部区域，用于鼠标操作等信息
};

#endif	//COLOR_BLOCK_WIDGET_H