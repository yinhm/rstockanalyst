#pragma once
#include <QtGui>
#include "BaseMarketWidget.h"

class CMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	CMainWindow();
	~CMainWindow();


public:
	bool setupStockDrv();

protected:
	virtual bool winEvent( MSG* message, long* result );

protected:
	virtual long OnStockDrvMsg(WPARAM wParam,LPARAM lParam);

private:
	QMdiArea* m_pMdiArea;
	CBaseMarketWidget* m_pBaseMarketWidget;
};

