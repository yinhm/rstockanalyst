#ifndef All_STOCK_DIALOG_H
#define All_STOCK_DIALOG_H
#include <QtGui>
#include "AllStockWidget.h"
#include "KLineWidget.h"

class CAllStockDialog : public QDialog
{
	Q_OBJECT
public:
	CAllStockDialog(QWidget* parent = 0);
	~CAllStockDialog(void);

protected slots:
	void onCircle5Min();
	void onCircleDay();
	void onCircleWeek();
	void onCircleMonth();

	void onSortCode();
	void onSortZGB();
	void onSortInc();
	void onSortTurnRatio();
	void onSortVolumeRatio();

	void onStockFocusChanged(CStockInfoItem* pStock);

private:
	CAllStockWidget* m_pWidgetAll;
	CKLineWidget* m_pWidgetKLine;
};


#endif	//All_STOCK_DIALOG_H