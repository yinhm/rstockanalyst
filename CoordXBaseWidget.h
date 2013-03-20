#ifndef COORDX_BASE_WIDGET_H
#define	COORDX_BASE_WIDGET_H
#include "RStockFunc.h"
#include "BaseWidget.h"

class CCoordXBaseWidget : public CBaseWidget
{
public:
	CCoordXBaseWidget(CBaseWidget* parent = 0, RWidgetType type = WidgetBasic);
	~CCoordXBaseWidget(void);

protected:
	void updateTimesH();								//更新当前的横坐标数据
	void drawCoordX(QPainter& p,const QRect& rtCoordX,	//绘制X坐标轴
		float fGridSize);

protected:
	RStockCircle m_typeCircle;				//本图的显示周期
	QMap<time_t,int> m_mapTimes;			//当前需要显示的所有时间（横向坐标）
	lua_State* m_pL;
};

#endif	//COORDX_BASE_WIDGET_H