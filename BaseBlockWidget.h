/************************************************************************/
/* 文件名称：BaseBlockWidget.h
/* 创建时间：2013-01-14 13:38
/*
/* 描    述：用于色块图的抽象，继承自CCoordXBaseWidget
/*           创造性的使用不同的色块来表示股票的升降走势，从而在相同的区域中，
/*           展现更大的信息量
/************************************************************************/

#ifndef BASE_BLOCK_WIDGET_H
#define BASE_BLOCK_WIDGET_H
#include "CoordXBaseWidget.h"

class CBaseBlockWidget : public CCoordXBaseWidget
{
	Q_OBJECT
public:
	enum BlockMode
	{
		BlockCircle = 1,		//圆形
		BlockRect,				//方块
	};
public:
	CBaseBlockWidget(CBaseWidget* parent = 0, WidgetType type = CBaseWidget::Basic);
	~CBaseBlockWidget(void);

public:
	//加载该K线图的配置信息
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//保存该K线图的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

	//更新数据
	virtual void updateData();

protected:
	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

protected slots:
	void setColorMode(const QString& mode);					//设置颜色模式

	void onSetCircle();										//设置当前的显示周期
	void onSetColorMode();									//点击设置颜色模式
	void onSetBlockMode();									//设置当前的显示模式
	void onSetBlockSize();									//设置色块的大小

protected:
	QMenu* m_pMenuCustom;					//自定义菜单
	QMenu* m_pMenuCircle;					//周期设置菜单
	QMenu* m_pMenuColorMode;				//颜色模式菜单
	QMenu* m_pMenuBlockMode;				//显示模式菜单

	QString m_qsColorMode;					//当前颜色模式
protected:
	BlockMode m_typeBlock;					//block显示形状
	int m_iCBHeight;						//单个色块的高度
	int m_iCBWidth;							//单个色块的宽度
};

#endif	//BASE_BLOCK_WIDGET_H