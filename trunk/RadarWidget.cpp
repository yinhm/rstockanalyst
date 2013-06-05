#include "StdAfx.h"
#include "RadarWidget.h"

CRadarWidget::CRadarWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent, WidgetRadar)
{
	m_pMenuCustom = new QMenu(tr("行情信息菜单"));
}


CRadarWidget::~CRadarWidget(void)
{
}

bool CRadarWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	return true;
}

bool CRadarWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;

	return true;
}

void CRadarWidget::getKeyWizData( const QString& keyword,QList<KeyWizData*>& listRet )
{

}

void CRadarWidget::keyWizEntered( KeyWizData* pData )
{

}

void CRadarWidget::setStockCode( const QString& only )
{

}

void CRadarWidget::setStockItem( CStockInfoItem* pItem )
{

}

void CRadarWidget::paintEvent( QPaintEvent* e )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	p.fillRect(rtClient,QColor(0,0,0));
}

void CRadarWidget::keyPressEvent( QKeyEvent* e )
{

}

QMenu* CRadarWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	return m_pMenuCustom;
}
