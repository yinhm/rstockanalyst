#ifndef RMDI_CTRL_H
#define RMDI_CTRL_H
#include <QMdiArea>
#include <QMdiSubWindow>

class CRMdiSubWindow : public QMdiSubWindow
{
	Q_OBJECT
public:
	CRMdiSubWindow(QWidget* parent = 0);
	~CRMdiSubWindow();

protected:
	virtual void closeEvent(QCloseEvent* e);
};

class CRMdiArea : public QMdiArea
{
	Q_OBJECT
public:
	CRMdiArea(QWidget* parent = 0);
	~CRMdiArea();


};

#endif	//RMDI_CTRL_H