#ifndef STOCK_DEAL_WIDGET_H
#define STOCK_DEAL_WIDGET_H
#include "CoordXBaseWidget.h"

enum RDealWidgetType
{
	DealKind = 1,		//类型
	DealIncrese,		//幅度
	DealOrder,			//挂单
	DealOrder2,			//挂单第2种显示方式
};

class CStockDealWidget : public CCoordXBaseWidget
{
	Q_OBJECT
public:
	CStockDealWidget(CBaseWidget* parent = 0);
	~CStockDealWidget(void);


protected:
	//加载该K线图的配置信息
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//保存该K线图的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);
	//通过查找keyword获取需要在按键精灵上显示的数据
	virtual void getKeyWizData(const QString& keyword,QList<KeyWizData*>& listRet);
	//键盘精灵窗口确认后触发
	virtual void keyWizEntered(KeyWizData* pData);


protected slots:
	void onSetDealType();
	void onSetColorMode();									//点击设置颜色模式

protected:
	//设置当前显示的股票
	virtual void setStockCode(const QString& code);

	//设置显示类型
	void setDealType(RDealWidgetType _t);

	//设置颜色模式
	void setColorMode(const QString& mode);

	//更新数据，临时使用
	virtual void updateData();

	//
	virtual void mousePressEvent(QMouseEvent* e);

	//绘图
	virtual void paintEvent(QPaintEvent* e);


	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

protected:
	void drawTitle(QPainter& p);
	void drawClient(QPainter& p);
	void drawBottom(QPainter& p);

private:
	int m_iTitleHeight;		//标题高度
	int m_iBottomHeight;	//底部高度

	int m_iItemWidth;		//单个色块的宽度
	int m_iItemHeight;	//单个色块的高度

	QRect m_rtTitle;		//标题区域
	QRect m_rtClient;		//绘图区域
	QRect m_rtBottom;		//底部区域

	RDealWidgetType m_typeWidget; //当前窗口的显示类型

	//各个周期所在的矩形
	QMap<int,QRect> m_mapDealTypes;

	QString m_qsColorMode;					//当前颜色模式
	QVector<QColor> m_vColor;				//当前的颜色
private:
	QMenu* m_pMenuDealType;					//成交种类图
	QMenu* m_pMenuColorMode;				//颜色模式选择菜单

private:
	CStockInfoItem* m_pCurStock;			//当前的股票
	QMap<time_t,int> m_mapTimes;			//当前需要显示的所有时间（横向坐标）


	QList<RWidgetOpData> m_listDealType;	//显示类型表
};


#endif	//STOCK_DEAL_WIDGET_H