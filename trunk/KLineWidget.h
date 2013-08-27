/************************************************************************/
/* 文件名称：KLineWidget.h
/* 创建时间：2012-11-08 10:59
/*
/* 描    述：用于显示K线图的窗口，继承自CBaseWidget
/************************************************************************/

#ifndef K_LINE_WIDGET_H
#define K_LINE_WIDGET_H
#include <QtGui>
#include "CoordXBaseWidget.h"
#include "StockInfoItem.h"


class CKLineWidget : public CCoordXBaseWidget
{
	Q_OBJECT
public:
	enum FenShiCoordY			//分时图Y轴的绘制方式
	{
		Percent10 = 0,				//10个百分比
		PercentAuto = 1,			//自适应百分比
	};
	enum FenShiAsis				//分时图上的辅助图
	{
		FenShiVol = 0,			//量（无其它指标）
		FenShiVolRatio,			//量比
	};
public:
	CKLineWidget(CBaseWidget* parent = 0);
	~CKLineWidget(void);

public:
	//加载该K线图的配置信息
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//保存该K线图的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

	//更新数据
	virtual void updateData();
	//清理当前的内存
	virtual void clearTmpData();

public:
	//通过查找keyword获取需要在按键精灵上显示的数据
	virtual void getKeyWizData(const QString& keyword,QList<KeyWizData*>& listRet);
	//键盘精灵窗口确认后触发
	virtual void keyWizEntered(KeyWizData* pData);

public slots:
	virtual void setStockCode(const QString& only);
	virtual void setBlock(const QString& block);
	void updateMinLine(const QString& only);
	void updateDayLine(const QString& only);

protected:
	void setStockItem(CAbstractStockItem* pItem);

	//更新当前的横坐标数据
	virtual void updateTimesH();

	//获取数据二维表，通过分析当前的周期。
	QMap<time_t,RStockData*>* getColorBlockMap(CAbstractStockItem* pItem);

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void mouseMoveEvent(QMouseEvent* e);			//鼠标移动事件
	virtual void leaveEvent(QEvent* e);						//鼠标移出事件
	virtual void mousePressEvent(QMouseEvent* e);			//鼠标点击事件
	virtual void mouseDoubleClickEvent(QMouseEvent *e);		//鼠标双击事件
	virtual void keyPressEvent(QKeyEvent* e);				//键盘按下事件，用于处理快捷键设置

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

protected slots:
	void onSetStockCode();								//弹出对话框，让用户手动输入股票代码
	void onSetExpression();								//弹出对话框，让用户手动输入表达式
	void onClickedAddShow();							//减少显示个数
	void onClickedSubShow();							//增加显示个数
	void onAddDeputy();									//增加副图
	void onAddVolume();									//是否显示量视图
	void onRemoveDeputy();								//删除副图
	void onSetSizes();									//设置所有图的显示比例

	void onAdd2Block();									//添加到板块
	void onAdd2NewBlock();								//添加到新建板块

	void onSetYCoordAuto();								//设置自动显示Y轴的范围
	void onSetYCoordPercent();							//设置Y轴以百分比方式显示

protected:
	void setShowCount(int _iShow);						//设置当前的显示个数

private:
	void drawTitle(QPainter& p,const QRect& rtTitle);	//绘制头部
	void drawShowBtns(QPainter& p,const QRect& rtBtns);	//绘制右下角的两个按钮
	void drawExpArgs(QPainter& p,const QRect& rtClient,
		const QString& e,QList<uint>& lsColor);

	virtual void drawCoordX(QPainter& p,const QRectF& rtCoordX,float fItemWidth);	//绘制X坐标轴
	//绘制Y轴，主要用于K线图
	virtual void drawCoordY(QPainter& p,const QRectF rtCoordY, float fMax, float fMin, bool bPercent = false);

	//绘制分时图
	void drawFenShi(QPainter& p, QRect rtClient);
private:
	void resetTmpData();					//重新计算数据。

private:
	QMenu* m_pMenuAdd2Block;				//自选板块菜单
	QMenu* m_pMenuYCoordMode;				//Y轴显示样式

private:
	CAbstractStockItem* m_pStockItem;		//当前K线图的股票数据指针
	QMap<time_t,RStockData*>* m_mapData;	//所有用于显示的数据
	int m_iShowCount;						//需要显示的数据个数（长度，理论上应小于listItems的size）

	int m_iCurExp;							//当前选中的表达式
	bool m_bShowMax;						//是否最大化显示（目前只对副图有效）

	int m_iTitleHeight;						//头部高度
	int m_iCoorYWidth;						//Y坐标轴的宽度
	int m_iCoorXHeight;						//X坐标轴的高度
	float m_fItemWidth;						//单个Item的宽度
	int m_iMainLinerHeight;					//主图的高度

	QVector<int> m_vSizes;					//显示比例(总和为100)
	QVector<QString> m_vExps;				//显示的表达式

	QRect m_rtAddShow;						//增加显示个数的按钮区域
	QRect m_rtSubShow;						//减少显示个数的按钮区域

	QRect m_rtTitle;						//绘制Title的区域
	QRect m_rtClient;						//绘制K线图的区域

	//各个周期所在的矩形
	QMap<int,QRect> m_mapCircles;

	bool m_bLock;							//用于切换时的加锁
	QDateTime m_tmLastUpdate;				//最后一次更新的时间

	bool m_bYCoordPercent;					//Y轴坐标以相对于昨收价百分比显示
	bool m_bYCoordAuto;						//是否为自动显示Y轴范围

	//分时图设置项
	int m_iFenShiCount;						//绘制分时图的天数
	FenShiCoordY m_iFenShiCoordY;			//分时图Y轴的绘制方式（）
	FenShiAsis m_iFenShiAsis;				//分时图的辅助项
};


#endif	//K_LINE_WIDGET_H