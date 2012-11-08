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
		disconnect(this,SLOT(updateKLine(const QString&)));		//移除所有和 updateKLine关联的 信号/槽

		m_pStockItem = pItem;
		//建立更新机制
		connect(pItem,SIGNAL(stockInfoItemChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
	}
}

void CKLineWidget::updateKLine( const QString& code )
{
	if(m_pStockItem&&m_pStockItem->getCode()!=code)
		return;

	//更新K线图
	update();
}

void CKLineWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	p.drawLine(0,0,100,100);
}
