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
#include "ColorManager.h"

class CColorBlockWidget : public CBaseBlockWidget
{
	Q_OBJECT
public:
	//显示类型（）
	enum RShowType
	{
		ShowIncrease = 1,	//涨幅
		ShowTurnRatio,		//换手率
		ShowVolumeRatio,	//量比
	};

	enum RAsistIndex
	{
		IndexNewPriceCount = 1,	//创新高次数
		IndexNewVolumeCount,	//量创新高次数
		IndexIncrease,			//今日涨幅
		IndexVolumeRatio,		//量比
		IndexTurnRatio,			//换手率
		IndexGuanDan,			//当前挂单情况
		Index5DayLine,			//最近5日涨幅变化
		IndexCmpPrice,			//最近5分钟内的价格变化
	};



public:
	CColorBlockWidget(CBaseWidget* parent = 0);
	~CColorBlockWidget(void);

public:
	//加载该K线图的配置信息
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//保存该K线图的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);


	//清理当前的内存
	virtual void clearTmpData();

	//更新当前的排序方式
	virtual void updateSortMode(bool bSelFirst);

protected:
	//更新当前的横坐标数据
	virtual void updateTimesH();

public:
	//通过查找keyword获取需要在按键精灵上显示的数据
	virtual void getKeyWizData(const QString& keyword,QList<KeyWizData*>& listRet);
	//键盘精灵窗口确认后触发
	virtual void keyWizEntered(KeyWizData* pData);

public slots:
	virtual void setBlock(const QString& block);

protected slots:
	void onSetCurrentBlock();								//设置当前显示的板块
	void onSetShowType();									//设置显示类型
	void onSetExpression();									//设置当前显示的表达式
	void onSetTopStock1();									//设置置顶显示股票
	void onSetTopStock2();
	void onSetTopStock3();
	void onRemoveTopStock();								//移除置顶显示股票
	void onRemoveStock();									//从当前板块中删除选中股票
	void onFocusWhenMove();									//设置鼠标移动时切换股票
	void onSetAsistIndex();									//设置当前显示的辅助指标



	virtual void updateColorBlockData();					//更新当前需要显示的数据

protected:
	void setShowType(RShowType _t);
	int getTopCount();							//获取置顶元素的数量

private:
	void clickedStock(CStockInfoItem* pItem);	//当点击股票时触发

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void mouseMoveEvent(QMouseEvent* e);			//鼠标移动事件
	virtual void mousePressEvent(QMouseEvent* e);			//鼠标点击事件
	virtual void wheelEvent(QWheelEvent* e);				//鼠标中键滚动事件
	virtual void keyPressEvent(QKeyEvent* e);				//键盘操作

	//绘制色块
	//vColor:取0-21
	//vHeight:百分比0-100%;
	//vWidth:百分比0%-100%;
	void drawColocBlock(QPainter& p,int iY,
		QVector<float>& vColor,QVector<float>& vHeight,QVector<float>& vWidth
		,CColorItem* pClrItem);

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

	virtual void onBlockClicked(CBlockInfoItem* pBlock,int iCmd);
	/*相关绘图函数*/
private:
	void drawHeader(QPainter& p,const QRect& rtHeader);			//绘制头部信息
	void drawClient(QPainter& p,const QRect& rtClient);			//绘制主区域
	void drawBottom(QPainter& p,const QRect& rtBottom);			//绘制底部区域

	void drawStock(QPainter& p,const QRect& rtCB,CStockInfoItem* pItem,CColorItem* pClrItem);	//绘制单只股票

	QRect rectOfStock(CStockInfoItem* pItem);					//获取某只股票显示的位置
	CStockInfoItem* hitTestStock(const QPoint& ptPoint);		//测试某点所指向的股票信息
	RStockData* hitTestCBItem(const QPoint& ptPoint);//测试某点所指向的色块信息

private:
	QMenu* m_pMenuShowType;					//绘制界面中的显示类型
	QAction* m_pActFocusWhenMove;			//鼠标移动时切换股票菜单
	QAction* m_pActRemoveStock;				//删除该板块中的股票

	QList<RWidgetOpData> m_listShowOp;		//显示类型列表
	QList<RWidgetOpData*> m_listAsistIndex;	//显示辅助指标

	QMap<RAsistIndex,RAsistIndex> m_mapCurAsistIndex;	//当前显示的辅助指标

	CBlockInfoItem* m_pBlock;				//当前的板块名称
	QList<CStockInfoItem*> m_listStocks;	//当前显示的所有股票列表

	QMap<CStockInfoItem*,int> m_mapStockIndex;	//用来快速查找某只股票所在的索引
	CStockInfoItem* m_pSelectedStock;			//当前选中的股票

	QMap<int ,QList<CStockInfoItem*>> m_mapTopStocks;	//置顶股票

	/*用于绘制操作的成员变量*/
private:
	int m_iTitleHeight;						//头部高度
	int m_iBottomHeight;					//底部的高度
	int m_iLeftLen;							//左侧的空闲区域
	int m_iRightLen;						//右侧的空闲区域
	int showStockIndex;						//当前显示的起始位置（列）

	QRect m_rtHeader;						//头部Header区域
	QRect m_rtBottom;						//底部区域，用于鼠标操作等信息

	QTimer m_timerUpdateUI;					//界面更新的timer

	//各个排序方式所在的矩形
	QMap<int,QRect> m_mapSorts;

	bool m_bShowIncrease;					//显示涨幅（颜色）
	bool m_bShowTurnRatio;					//显示换手率（高度）
	bool m_bShowVolumeRatio;				//显示量比（宽度）

	bool m_bFocusWhenMove;					//当鼠标移动时设置股票切换
};

#endif	//COLOR_BLOCK_WIDGET_H