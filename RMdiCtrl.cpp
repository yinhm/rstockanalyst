#include "StdAfx.h"
#include "RMdiCtrl.h"

CRMdiSubWindow::CRMdiSubWindow( QWidget* parent /*= 0*/ )
	: QMdiSubWindow(parent)
{
}

CRMdiSubWindow::~CRMdiSubWindow()
{
}

void CRMdiSubWindow::closeEvent( QCloseEvent* )
{

	return hide();
}





CRMdiArea::CRMdiArea( QWidget* parent /*= 0*/ )
	: QMdiArea(parent)
{

}

CRMdiArea::~CRMdiArea()
{

}
