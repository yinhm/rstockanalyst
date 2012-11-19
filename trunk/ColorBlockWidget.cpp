#include "StdAfx.h"
#include "ColorBlockWidget.h"

CColorBlockWidget::CColorBlockWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::ColorBlock)
	, m_pMenuCustom(0)
{
	m_pMenuCustom = new QMenu(tr("É«¿éÍ¼²Ëµ¥"));
}

CColorBlockWidget::~CColorBlockWidget(void)
{
	delete m_pMenuCustom;
}

bool CColorBlockWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	return true;
}

bool CColorBlockWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;

	return true;
}

void CColorBlockWidget::paintEvent( QPaintEvent* e )
{
	QPainter p(this);

	p.fillRect(this->rect(),QColor(127,0,0));

	p.setPen(QColor(255,255,255));
	p.drawText(this->rect(),Qt::AlignCenter,QString("É«¿éÍ¼"));
}

QMenu* CColorBlockWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	return m_pMenuCustom;
}
