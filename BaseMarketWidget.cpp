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

	connect(m_pViewSHA,SIGNAL(clicked(const QModelIndex&)),this,SLOT(treeItemClicked(const QModelIndex&)));
	connect(CDataEngine::getDataEngine(),SIGNAL(historyChanged(const QString&)),m_pModelSHA,SLOT(historyChanged(const QString&)));
	connect(CDataEngine::getDataEngine(),SIGNAL(historyChanged(const QString&)),m_pModelSZ,SLOT(historyChanged(const QString&)));


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

void CBaseMarketWidget::treeItemClicked( const QModelIndex& index )
{
	QTreeView* pTreeView = reinterpret_cast<QTreeView*>(sender());
	if(pTreeView == m_pViewSHA)
	{
		qRcvReportData* pReport = m_pModelSHA->data(index,Qt::UserRole).value<qRcvReportData*>();
		if(pReport)
		{
			QDialog dlg;
			QVBoxLayout layout;
			dlg.setLayout(&layout);

			QTreeWidget treeWidget(&dlg);
			layout.addWidget(&treeWidget);
			treeWidget.setHeaderLabels(QStringList()<<"Time"<<"High");

			QMap<time_t,qRcvHistoryData>::iterator iter = pReport->mapHistorys.begin();
			while(iter!=pReport->mapHistorys.end())
			{
				QTreeWidgetItem* pItem = new QTreeWidgetItem(&treeWidget);
				pItem->setData(0,Qt::DisplayRole,QDateTime::fromTime_t(iter->time).toString());
				pItem->setData(1,Qt::DisplayRole,iter->fHigh);
				treeWidget.addTopLevelItem(pItem);
				++iter;
			}

			dlg.exec();
		}
	}
}
