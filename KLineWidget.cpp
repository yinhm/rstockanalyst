#include "StdAfx.h"
#include "KLineWidget.h"
#include "DataEngine.h"


CKLineWidget::CKLineWidget( QWidget* parent /*= 0*/ )
	: QWidget(parent)
{

}

CKLineWidget::~CKLineWidget(void)
{
}

void CKLineWidget::setStockCode( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);
	if(pItem)
	{
		m_pStockItem = pItem;
	}
}
