#ifndef STOCK_DEAL_WIDGET_H
#define STOCK_DEAL_WIDGET_H
#include "CoordXBaseWidget.h"

class CStockDealWidget : public CCoordXBaseWidget
{
	Q_OBJECT
public:
	CStockDealWidget(CBaseWidget* parent = 0);
	~CStockDealWidget(void);
};


#endif	//STOCK_DEAL_WIDGET_H