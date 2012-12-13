/************************************************************************/
/* 文件名称：StockInfoWidget.cpp
/* 创建时间：2012-12-13 17:55
/*
/* 描    述：用于显示单只股票的行情信息，继承自CBaseWidget
/************************************************************************/

#include "StdAfx.h"
#include "StockInfoWidget.h"

CStockInfoWidget::CStockInfoWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent, CBaseWidget::StockInfo)
{
	m_pMenuCustom = new QMenu(tr("行情信息菜单"));
}

CStockInfoWidget::~CStockInfoWidget(void)
{
	delete m_pMenuCustom;
}

void CStockInfoWidget::paintEvent( QPaintEvent* e )
{
	QPainter p(this);
	p.fillRect(this->rect(),QColor(255,0,0));
}

QMenu* CStockInfoWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	return m_pMenuCustom;
}
