#include "StdAfx.h"
#include "WatcherSettingDlg.h"
#include "RadarWatcher.h"
#include "BlockInfoItem.h"
#include "WatcherEditDlg.h"


CWatcherSettingDlg::CWatcherSettingDlg( QWidget* parent /*= NULL*/ )
	: QDialog(parent)
{
	setMinimumSize(500,200);
	setWindowTitle(tr("当前监视雷达"));
	m_btnAdd.setText(tr("增加"));
	m_btnDel.setText(tr("删除"));
	m_btnEdit.setText(tr("编辑"));
	m_btnRefresh.setText(tr("刷新"));

	connect(&m_btnAdd,SIGNAL(clicked()),this,SLOT(onAddWatcher()));
	connect(&m_btnDel,SIGNAL(clicked()),this,SLOT(onDelWatcher()));
	connect(&m_btnEdit,SIGNAL(clicked()),this,SLOT(onEditWatcher()));
	connect(&m_btnRefresh,SIGNAL(clicked()),this,SLOT(onRefresh()));

	m_listWatchers.setHeaderLabels(QStringList()<<tr("ID")<<tr("类型")<<tr("阈值")<<tr("间隔时间")<<tr("当前板块"));
	m_listWatchers.setSelectionMode(QTreeWidget::SingleSelection);
	connect(&m_listWatchers,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onEditWatcher()));

	QGridLayout* pLayout = new QGridLayout(this);
	pLayout->addWidget(&m_listWatchers,0,0,8,8);
	pLayout->addWidget(&m_btnAdd,9,0,2,2);
	pLayout->addWidget(&m_btnDel,9,2,2,2);
	pLayout->addWidget(&m_btnEdit,9,4,2,2);
	pLayout->addWidget(&m_btnRefresh,9,6,2,2);

	setLayout(pLayout);

	onRefresh();
}


CWatcherSettingDlg::~CWatcherSettingDlg(void)
{
}

void CWatcherSettingDlg::onRefresh()
{
	m_listWatchers.clear();
	QList<CRadarWatcher*> list = CRadarManager::getRadarManager()->getRadarWatchers();
	foreach(CRadarWatcher* pWatcher,list)
	{
		QStringList listValue;
		listValue<<QString("%1").arg(pWatcher->getId())
			<<QString("%1").arg(CRadarManager::getTypeName(pWatcher->getType()))
			<<QString("%1").arg(pWatcher->getHold())
			<<QString("%1").arg(pWatcher->getSec())
			<<pWatcher->getBlock()->getName();
		QTreeWidgetItem* pItem = new QTreeWidgetItem(listValue);
		pItem->setData(0,Qt::UserRole,reinterpret_cast<uint>(pWatcher));
		m_listWatchers.addTopLevelItem(pItem);
	}
}

void CWatcherSettingDlg::onAddWatcher()
{
	CWatcherEditDlg dlg(0,this);
	if(QDialog::Accepted == dlg.exec())
		onRefresh();
}

void CWatcherSettingDlg::onDelWatcher()
{
	QList<QTreeWidgetItem*> listSel = m_listWatchers.selectedItems();
	if(listSel.size()>0)
	{
		CRadarWatcher* pWatcher = reinterpret_cast<CRadarWatcher*>(listSel[0]->data(0,Qt::UserRole).toUInt());
		if(QMessageBox::Yes == QMessageBox::warning(this,tr("警告"),
			tr("确认要删除选择的监视雷达吗？"),QMessageBox::Yes|QMessageBox::No))
		{
			CRadarManager::getRadarManager()->removeWatcher(pWatcher->getId());
			onRefresh();
		}
	}
	else
	{
		QMessageBox::information(this,tr("信息"),tr("没有选择要删除的信息！"));
	}
}

void CWatcherSettingDlg::onEditWatcher()
{
	QList<QTreeWidgetItem*> listSel = m_listWatchers.selectedItems();
	if(listSel.size()>0)
	{
		CRadarWatcher* pWatcher = reinterpret_cast<CRadarWatcher*>(listSel[0]->data(0,Qt::UserRole).toUInt());
		CWatcherEditDlg dlg(pWatcher,this);
		if(QDialog::Accepted == dlg.exec())
			onRefresh();
	}
	else
	{
		QMessageBox::information(this,tr("信息"),tr("没有选择要编辑的信息！"));
	}
}
