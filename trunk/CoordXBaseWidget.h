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

protected:
	RStockCircle m_typeCircle;				//本图的显示周期
	lua_State* m_pL;
};

#endif	//COORDX_BASE_WIDGET_H