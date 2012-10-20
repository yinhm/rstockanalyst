#include "StdAfx.h"
#include "BaseMarketWidget.h"

CBaseMarketWidget::CBaseMarketWidget()
	: QWidget()
{
	m_pTreemModel = new CBaseMarketTreeModel;
	m_pTreeView = new QTreeView;
	m_pTreeView->setStyleSheet("QTreeView::branch {image:none;}");
	m_pTreeView->setModel(m_pTreemModel);

	QVBoxLayout* pLayout = new QVBoxLayout;

	setLayout(pLayout);
	pLayout->addWidget(m_pTreeView);
}

CBaseMarketWidget::~CBaseMarketWidget(void)
{
}
