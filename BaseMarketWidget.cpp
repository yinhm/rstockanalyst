#include "StdAfx.h"
#include "BaseMarketWidget.h"

CBaseMarketWidget::CBaseMarketWidget()
	: QWidget()
{
	QTabWidget* pTabWidget = new QTabWidget(this);
	pTabWidget->setTabPosition(QTabWidget::South);
	pTabWidget->setTabShape(QTabWidget::Triangular);

	m_pModelSHA = new CBaseMarketTreeModel;
	m_pViewSHA = new QTreeView;
	m_pViewSHA->setStyleSheet("QTreeView::branch {image:none;}");
	m_pViewSHA->setModel(m_pModelSHA);

	m_pModelSZ = new CBaseMarketTreeModel;
	m_pViewSZ = new QTreeView;
	m_pViewSZ->setStyleSheet("QTreeView::branch {image:none;}");
	m_pViewSZ->setModel(m_pModelSZ);

	m_pModelHK = new CBaseMarketTreeModel;
	m_pViewHK = new QTreeView;
	m_pViewHK->setStyleSheet("QTreeView::branch {image:none;}");
	m_pViewHK->setModel(m_pModelHK);

	m_pModelOther = new CBaseMarketTreeModel;
	m_pViewOther = new QTreeView;
	m_pViewOther->setStyleSheet("QTreeView::branch {image:none;}");
	m_pViewOther->setModel(m_pModelHK);

	QVBoxLayout* pLayout = new QVBoxLayout;

	setLayout(pLayout);
	pLayout->addWidget(pTabWidget);

	pTabWidget->addTab(m_pViewSHA,tr("上海A股"));
	pTabWidget->addTab(m_pViewSZ,tr("深圳指数"));
	pTabWidget->addTab(m_pViewHK,tr("香港股市"));
	pTabWidget->addTab(m_pViewOther,tr("Other"));
}

CBaseMarketWidget::~CBaseMarketWidget(void)
{
}

void CBaseMarketWidget::updateBaseMarket()
{
	QList<qRcvReportData*> listReports = CDataEngine::getDataEngine()->getBaseMarket();
	foreach(qRcvReportData* pReport,listReports)
	{
		if(pReport->wMarket==SH_MARKET_EX)
		{
			m_pModelSHA->appendReport(pReport);
		}
		else if(pReport->wMarket==SZ_MARKET_EX)
		{
			m_pModelSZ->appendReport(pReport);
		}
		else if(pReport->wMarket==HK_MARKET_EX)
		{
			m_pModelHK->appendReport(pReport);
		}
		else
		{
			m_pModelOther->appendReport(pReport);
		}
	}
}
