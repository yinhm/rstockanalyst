/************************************************************************/
/* 文件名称：FlashWidget.h
/* 创建时间：2013-07-14 11:05
/*
/* 描    述：用于显示多只股票的走势，继承自CBaseWidget
/*           创造性的使用不同的色块来表示股票的升降走势，从而在相同的区域中，
/*           展现更大的信息量
/************************************************************************/
#ifndef FLASH_WIDGET_H
#define FLASH_WIDGET_H
#include "BaseWidget.h"

class CFlashWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CFlashWidget(CBaseWidget* parent = 0);
	~CFlashWidget(void);

public:
	//加载该K线图的配置信息
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//保存该K线图的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

protected slots:
	void onAddWidget();
	void onSwitchNextWidget();

protected:
	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();
	virtual void paintEvent(QPaintEvent* e);

	void restNewWidget(QWidget* pWidget);

private:
	QMenu* m_pMenuCustom;					//自定义菜单
	QMenu* m_pMenuAddWidget;				//增加窗口

private:
	QList<CBaseWidget*> m_listWidgetsStack;	//所有显示的窗口
	int m_iSwitchSeconds;					//切换时间
	QTimer m_timerSwitch;					//切换时间器
	CBaseWidget* m_pCurWidget;				//当前显示的Widget

	int m_iTitleHeight;						//当前头部的高度
};


#endif	//FLASH_WIDGET_H