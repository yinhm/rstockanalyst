#pragma once
#include <QtGui>

class CMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	CMainWindow(QWidget* parent=0, Qt::WindowFlags flags=0 );
	~CMainWindow(void);


public:
	bool setupStockDrv();

protected:
	virtual bool winEvent( MSG* message, long* result );

protected:
	virtual long OnStockDrvMsg(WPARAM wParam,LPARAM lParam);
};

