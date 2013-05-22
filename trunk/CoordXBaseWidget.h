#ifndef COORDX_BASE_WIDGET_H
#define	COORDX_BASE_WIDGET_H
#include "RStockFunc.h"
#include "BaseWidget.h"

class CCoordXBaseWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CCoordXBaseWidget(CBaseWidget* parent = 0, RWidgetType type = WidgetBasic);
	~CCoordXBaseWidget(void);

protected:
	//加载该K线图的配置信息
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//保存该K线图的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);
	//更新数据
	virtual void updateData();

public:
	//通过查找keyword获取需要在按键精灵上显示的数据
	virtual void getKeyWizData(const QString& keyword,QList<KeyWizData*>& listRet);
	//键盘精灵窗口确认后触发
	virtual void keyWizEntered(KeyWizData* pData);

protected:
	void updateTimesH();												//更新当前的横坐标数据
	void updateShowTimes(const QRectF& rtCoordX,float fItemWidth);		//更新需要进行绘制的时间轴

	void drawCoordX(QPainter& p,const QRectF& rtCoordX,float fItemWidth);	//绘制X坐标轴
	//绘制Y轴，主要用于K线图
	void drawCoordY(QPainter& p,const QRectF rtCoordY, float fMax, float fMin);

	//获取数据二维表，通过分析当前的周期。
	QMap<time_t,RStockData*>* getColorBlockMap(CStockInfoItem* pItem);
	QMap<time_t,RStockData*>* getColorBlockMap(CBlockInfoItem* pItem);

protected slots:
	void onSetCircle();								//设置当前的显示周期

protected:
	void setCircle(RStockCircle _c);				//设置当前的显示周期

protected:
	QMenu* m_pMenuCustom;					//自定义菜单
	QMenu* m_pMenuCircle;					//周期设置菜单
	RStockCircle m_typeCircle;				//本图的显示周期
	lua_State* m_pL;

protected:
	QMap<time_t,int> m_mapTimes;			//当前需要显示的所有时间（横向坐标）
	QMap<time_t,float> m_mapShowTimes;		//当前已经绘制的时间

	QList<RWidgetOpData> m_listCircle;	//显示周期表
};

#endif	//COORDX_BASE_WIDGET_H