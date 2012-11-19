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


protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

private:
	QMenu* m_pMenuCustom;					//自定义菜单
};

#endif	//COLOR_BLOCK_WIDGET_H