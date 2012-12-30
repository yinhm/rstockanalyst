#ifndef COORDX_BASE_WIDGET_H
#define	COORDX_BASE_WIDGET_H
#include "BaseWidget.h"

class CCoordXBaseWidget : public CBaseWidget
{
public:
	enum CoordXCircle		//色块图的周期
	{
		FenShi = 1,				//分时模式（直接连线）
		Sec10 = 10,						//10秒
		Sec30 = 30,						//30秒
		Min1 = 60,					//1分钟
		Min5 = 5*60,					//5分钟
		Min15 = 15*60,					//15分钟
		Min30 = 30*60,					//30分钟
		Min60 = 60*60,					//60分钟
		MinN,					//N分钟
		Day,					//日线
		DayN,					//N日线
		Week,					//周线
		Month,					//月线
		Month3,					//季线
		Year,					//年线
	};

public:
	CCoordXBaseWidget(CBaseWidget* parent = 0, WidgetType type = CBaseWidget::Basic);
	~CCoordXBaseWidget(void);

protected:
	void updateTimesH();								//更新当前的横坐标数据
	void drawCoordX(QPainter& p,const QRect& rtCoordX,	//绘制X坐标轴
		int iGridSize);

protected:
	CoordXCircle m_typeCircle;				//本图的显示周期
	QMap<time_t,int> m_mapTimes;			//当前需要显示的所有时间（横向坐标）
};

#endif	//COORDX_BASE_WIDGET_H