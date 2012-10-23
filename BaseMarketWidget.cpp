#include "StdAfx.h"
#include "BaseMarketWidget.h"
#include "DataEngine.h"

CBaseMarketWidget::CBaseMarketWidget()
	: QWidget()
{
	QTabWidget* pTabWidget = new QTabWidget(this);
	pTabWidget->setTabPosition(QTabWidget::South);
	pTabWidget->setTabShape(QTabWidget::Triangular);

	m_pModelSHA = new CBaseMarketTreeModel(SH_MARKET_EX);
	m_pViewSHA = new QTreeView;
	m_pViewSHA->setModel(m_pModelSHA);
	m_pViewSHA->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pViewSHA->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pViewSHA->setItemDelegate(new CBaseMarketItemDelegate);

	m_pModelSZ = new CBaseMarketTreeModel(SZ_MARKET_EX);
	m_pViewSZ = new QTreeView;
	m_pViewSZ->setModel(m_pModelSZ);
	m_pViewSZ->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pViewSZ->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pViewSZ->setItemDelegate(new CBaseMarketItemDelegate);

	connect(m_pViewSHA,SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(treeItemClicked(const QModelIndex&)));
	connect(CDataEngine::getDataEngine(),SIGNAL(stockInfoChanged(const QString&)),m_pModelSHA,SLOT(updateStockItem(const QString&)));
	connect(CDataEngine::getDataEngine(),SIGNAL(stockInfoChanged(const QString&)),m_pModelSZ,SLOT(updateStockItem(const QString&)));


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
		CStockInfoItem* pItem = reinterpret_cast<CStockInfoItem*>(m_pModelSHA->data(index,Qt::UserRole).toUInt());
		if(pItem)
		{
			QDialog dlg;
			QVBoxLayout layout;
			dlg.setLayout(&layout);

			QTreeWidget treeWidget(&dlg);
			layout.addWidget(&treeWidget);
			treeWidget.setHeaderLabels(QStringList()<<"Time"<<"High");

			QList<qRcvHistoryData*> lsHistory = pItem->getHistoryList();
			foreach(qRcvHistoryData* pHistory,lsHistory)
			{
				QTreeWidgetItem* pItem = new QTreeWidgetItem(&treeWidget);
				pItem->setData(0,Qt::DisplayRole,pHistory->time);
				pItem->setData(1,Qt::DisplayRole,pHistory->fHigh);
				treeWidget.addTopLevelItem(pItem);
			}

			dlg.exec();
		}
	}
}
