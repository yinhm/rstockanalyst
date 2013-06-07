/************************************************************************/
/* 文件名称：RadarWidget.h
/* 创建时间：2013-06-05 10:30
/*
/* 描    述：用于显示市场雷达检测，继承自CBaseWidget
/************************************************************************/
#ifndef RADAR_WIDGET_H
#define RADAR_WIDGET_H
#include "BaseWidget.h"
#include "StockInfoItem.h"
#include "RadarWatcher.h"

class CRadarWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CRadarWidget(CBaseWidget* parent = 0);
	~CRadarWidget(void);

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
	virtual void setStockCode(const QString& only);

protected slots:
	//新的数据到来
	void onRadarAlert(RRadarData* pRadar);
	void testRandomRadar();			//临时调试使用函数，自动生成雷达数据

protected:
	void setStockItem(CStockInfoItem* pItem);

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void keyPressEvent(QKeyEvent* e);				//键盘操作
	virtual void mouseMoveEvent(QMouseEvent* e);			//鼠标移动事件
	virtual void mousePressEvent(QMouseEvent* e);			//鼠标点击事件
	virtual void wheelEvent(QWheelEvent* e);				//滚轮事件

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

private:
	void drawTitle(QPainter& p);
	void drawClient(QPainter& p);
	void clickedRadar(RRadarData* pData);

	//测试某点所对应的数据
	RRadarData* testRadarData(const QPoint& ptClicked);
private:
	//绘图相关变量
	int m_iItemHeight;						//单个雷达数据的显示高度
	int m_iTitleHeight;						//标题的高度
	QRect m_rtClient;						//主显示区的矩形区域
	QRect m_rtTitle;						//标题显示区域

	int m_iShowIndex;						//当前显示的起始位置

private:
	QMenu* m_pMenuCustom;					//自定义菜单
	QList<RRadarData*> m_listRadars;		//显示的数据
	QMap<RRadarData*,int> m_mapRadarsIndex;	//用来快速查找某只雷达数据所在的索引
	RRadarData* m_pSelRadar;				//当前选中的数据
};

#endif	//RADAR_WIDGET_H