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

protected:
	void setStockItem(CStockInfoItem* pItem);

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void keyPressEvent(QKeyEvent* e);				//键盘操作

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

private:
	QMenu* m_pMenuCustom;					//自定义菜单

private:

};

#endif	//RADAR_WIDGET_H