/************************************************************************/
/* 文件名称：BaseWidget.h
/* 创建时间：2012-11-16 10:59
/*
/* 描    述：所有绘制窗口的基类，支持拆分等操作
/************************************************************************/
#ifndef BASE_WIDGET_H
#define BASE_WIDGET_H
#include <QtGui>
#include <QtXml>

class CBaseWidget : public QWidget
{
	Q_OBJECT
public:
	enum WidgetType
	{
		Basic = 0,				//基础图
		KLine,					//K线图
		MarketTrend,			//市场行情图
	};
public:
	CBaseWidget(CBaseWidget* parent = 0, WidgetType type = CBaseWidget::Basic);
	~CBaseWidget(void);

	void initMenu();				//初始化菜单项
	void realignSplitter();			//重新对Splitter进行布局
public:
	/*Widget的基本属性*/
	//名称
	void setWidgetName(const QString& qsTitle){m_qsName = qsTitle;}
	QString getWidgetName(){return m_qsName;}


	//获取该窗口中的Splitter指针
	QSplitter* getSplitter(){return m_pSplitter;}
	//获取父窗口
	CBaseWidget* getParent(){return m_pParent;}
	//重新设置父窗口
	void resetParent(CBaseWidget* parent);

	//获取widget所在的索引
	int getWidgetIndex(CBaseWidget* widget) const;
	//用新的widget 替换之前索引index的窗口
	void replaceWidget(int index, CBaseWidget* widget);

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu(){ return 0; }
	//加载该Widget的配置信息
	virtual bool loadPanelInfo(const QDomElement& elePanel);
	//保存该Widget的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& elePanel);

protected slots:
	/*右键菜单操作*/
	void onLeftInsert();			//左插入
	void onRightInsert();			//右插入
	void onTopInsert();				//上插入
	void onBottomInsert();			//下插入

	/*右键菜单，设置不同的版面*/
	void onSetNormalWidget();		//设置为原始的窗口，无实际意义
	void onSetKLineWidget();		//设置为K线图
	void onSetMarketTrendWidget();	//设置为市场行情图

protected:
	virtual void paintEvent(QPaintEvent* e);
	virtual void contextMenuEvent(QContextMenuEvent* e);

protected:
	CBaseWidget* m_pParent;			//父窗口指针
	QMenu* m_pMenu;					//右键菜单
	QSplitter* m_pSplitter;			//分割器
	QString m_qsName;
	WidgetType m_type;
};


#endif	//BASE_WIDGET_H