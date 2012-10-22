#include "StdAfx.h"
#include "BaseMarketWidget.h"

CBaseMarketWidget::CBaseMarketWidget()
	: QWidget()
{
	QTabWidget* pTabWidget = new QTabWidget(this);
	pTabWidget->setTabPosition(QTabWidget::South);
	pTabWidget->setTabShape(QTabWidget::Triangular);

	m_pModelSHA = new CBaseMarketTreeModel(SH_MARKET_EX);
	m_pViewSHA = new QTreeView;
	m_pViewSHA->setStyleSheet("QTreeView::branch {image:none;}");
	m_pViewSHA->setModel(m_pModelSHA);

	m_pModelSZ = new CBaseMarketTreeModel(SZ_MARKET_EX);
	m_pViewSZ = new QTreeView;
	m_pViewSZ->setStyleSheet("QTreeView::branch {image:none;}");
	m_pViewSZ->setModel(m_pModelSZ);

	connect(m_pViewSHA,SIGNAL(clicked(const QModelIndex&)),this,SLOT(treeItemClicked(const QModelIndex&)));
	connect(CDataEngine::getDataEngine(),SIGNAL(baseMarketChanged(const QString&)),m_pModelSHA,SLOT(updateBaseMarket(const QString&)));
	connect(CDataEngine::getDataEngine(),SIGNAL(baseMarketChanged(const QString&)),m_pModelSZ,SLOT(updateBaseMarket(const QString&)));


	QVBoxLayout* pLayout = new QVBoxLayout;

	setLayout(pLayout);
	pLayout->addWidget(pTabWidget);

	pTabWidget->addTab(m_pViewSHA,tr("上海A股"));
	pTabWidget->addTab(m_pViewSZ,tr("深圳指数"));
}

CBaseMarketWidget::~CBaseMarketWidget(void)
{
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
				pItem->setData(0,Qt::DisplayRole,iter->time);
				pItem->setData(1,Qt::DisplayRole,iter->fHigh);
				treeWidget.addTopLevelItem(pItem);
				++iter;
			}

			dlg.exec();
		}
	}
}
