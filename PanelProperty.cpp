/*
============================================================================
文件名称	:	PanelProperty.cpp
作者		:	李亚科
创建时间	:	2011-12-29 16:56
修改时间	:	2011-12-29 16:57
说明		:	用于显示Panel的属性窗口
============================================================================
*/

#include "common.h"
#include "PanelProperty.h"
#include "PanelViewer.h"
#include "MainWindow.h"
#include "ROIProperty.h"
#include "ReconsProperty.h"
#include "ReformsProperty.h"
#include "BullsEyeProperty.h"
#include "ImageProperty.h"

using namespace eGingkoPanelManager;

QMap<int, CPanelProperty*> CPanelProperty::m_mapPanelProperty;
CPanelProperty* CPanelProperty::getPanelProperty( int iType )
{
	if(!m_mapPanelProperty.contains(iType))
	{
		switch(iType)
		{
		case GKG_VIEWER_NORMAL:
			m_mapPanelProperty[iType] = new CPanelProperty();
			break;
		case GKG_VIEWER_ROI:
			m_mapPanelProperty[iType] = new CROIProperty();
			break;
		case GKG_VIEWER_RECONS:
			m_mapPanelProperty[iType] = new CReconsProperty();
			break;
		case GKG_VIEWER_REFORMS:
			m_mapPanelProperty[iType] = new CReformsProperty();
			break;
		case GKG_VIEWER_BullsEye:
			m_mapPanelProperty[iType] = new CBullsProperty();
			break;
		case GKG_VIEWER_IMAGE:
			m_mapPanelProperty[iType] = new CImageProperty();
			break;
		}
	}

	return m_mapPanelProperty[iType];
}


QMap<int, CPropertyDelegateItem*> CPropertyDelegateItem::m_mapPropertyDelegate;
CPropertyDelegateItem* CPropertyDelegateItem::getPropertyDelegateItem( int iType, CPanelProperty* parent )
{
	if(!m_mapPropertyDelegate.contains(iType))
	{
		switch(iType)
		{
		case GKG_VIEWER_NORMAL:
			m_mapPropertyDelegate[iType] = new CPropertyDelegateItem(parent);
			break;
		case GKG_VIEWER_ROI:
			m_mapPropertyDelegate[iType] = new CROIPropertyDelegate(reinterpret_cast<CROIProperty*>(parent));
			break;
		case GKG_VIEWER_RECONS:
			m_mapPropertyDelegate[iType] = new CReconsPropertyDelegate(reinterpret_cast<CReconsProperty*>(parent));
			break;
		case GKG_VIEWER_REFORMS:
			m_mapPropertyDelegate[iType] = new CReformsPropertyDelegate(reinterpret_cast<CReformsProperty*>(parent));
			break;
		case GKG_VIEWER_BullsEye:
			m_mapPropertyDelegate[iType] = new CBullsPropertyDelegate(reinterpret_cast<CBullsProperty*>(parent));
			break;
		case GKG_VIEWER_IMAGE:
			m_mapPropertyDelegate[iType] = new CImagePropertyDelegate(reinterpret_cast<CImageProperty*>(parent));
			break;
		}
	}

	return m_mapPropertyDelegate[iType];
}


CPropertyDelegateItem::CPropertyDelegateItem( CPanelProperty* parent /*= 0*/ )
	: QItemDelegate(parent),m_pPanelProperty(parent)
{

}

QWidget* CPropertyDelegateItem::createEditor( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QTreeWidgetItem* pItem = m_pPanelProperty->itemFromIndex(index);
	if((!pItem) || (index.column() != 1))
		return 0;

	if(pItem == m_pPanelProperty->m_pItemName)
		return new QLineEdit(parent);

	return 0;
}

void CPropertyDelegateItem::setEditorData( QWidget * editor, const QModelIndex & index ) const
{
	//初始化设置数据
	QTreeWidgetItem* pItem = m_pPanelProperty->itemFromIndex(index);
	if((!pItem) || (index.column() != 1))
		return;

	if(pItem == m_pPanelProperty->m_pItemName)
		return (reinterpret_cast<QLineEdit*>(editor))->setText(pItem->data(1,Qt::DisplayRole).toString());
}

void CPropertyDelegateItem::setModelData( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
	//完成编辑 设置数据
	QTreeWidgetItem* pItem = m_pPanelProperty->itemFromIndex(index);
	if((!pItem) || (index.column() != 1))
		return;

	if(pItem == m_pPanelProperty->m_pItemName)
		m_pPanelProperty->modifyPanelName((reinterpret_cast<QLineEdit*>(editor))->text());
}

CPanelProperty::CPanelProperty( QWidget* parent /*= 0*/ )
	: QTreeWidget(parent),m_pCurrentViewer(0)
{
	//初始化 控件类型 说明
	m_mapViewerType[GKG_VIEWER_ROI] = QString(tr("ROI Control"));
	m_mapViewerType[GKG_VIEWER_RECONS] = QString(tr("3D Player Control"));
	m_mapViewerType[GKG_VIEWER_REFORMS] = QString(tr("Recut Form Control"));
	m_mapViewerType[GKG_VIEWER_BullsEye] = QString(tr("Bulls Eye Control"));
	m_mapViewerType[GKG_VIEWER_IMAGE] = QString(tr("Image viewer Control"));
	m_mapViewerType[GKG_VIEWER_NORMAL] = QString(tr("Normal Control"));

	setAlternatingRowColors(true);
	setHeaderLabels(QStringList()<<tr("Property")<<tr("Value"));

	setItemDelegate(CPropertyDelegateItem::getPropertyDelegateItem(GKG_VIEWER_NORMAL,this));
	setEditTriggers(QAbstractItemView::DoubleClicked);

	//初始化
	m_pItemViewerType = new QTreeWidgetItem(this);
	m_pItemName = new QTreeWidgetItem(this);
	m_pItemX = new QTreeWidgetItem(this);
	m_pItemY = new QTreeWidgetItem(this);
	m_pItemWidth = new QTreeWidgetItem(this);
	m_pItemHeight = new QTreeWidgetItem(this);
	m_pItemDock = new QTreeWidgetItem(this);

	m_pItemName->setData(0,Qt::DisplayRole,tr("Name:"));
	m_pItemX->setData(0,Qt::DisplayRole,tr("X:"));
	m_pItemY->setData(0,Qt::DisplayRole,tr("Y:"));
	m_pItemWidth->setData(0,Qt::DisplayRole,tr("Width:"));
	m_pItemHeight->setData(0,Qt::DisplayRole,tr("Height:"));
	m_pItemDock->setData(0,Qt::DisplayRole,tr("Dock:"));
	m_pItemViewerType->setData(0,Qt::DisplayRole,tr("ViewerType:"));

	m_pItemName->setFlags(m_pItemName->flags()|Qt::ItemIsEditable);
	m_pItemViewerType->setDisabled(true);
}

CPanelProperty::~CPanelProperty( void )
{

}

void CPanelProperty::setPanelViewer( CPanelViewer* pPanel )
{
	m_pCurrentViewer = pPanel;
	if(pPanel)
	{
		//设置各个属性
		m_pItemName->setData(1,Qt::DisplayRole,pPanel->getPanelName());
		m_pItemX->setData(1,Qt::DisplayRole,pPanel->getPanelX());
		m_pItemY->setData(1,Qt::DisplayRole,pPanel->getPanelY());
		m_pItemWidth->setData(1,Qt::DisplayRole,pPanel->getPanelWidth());
		m_pItemHeight->setData(1,Qt::DisplayRole,pPanel->getPanelHeight());
		m_pItemDock->setData(1,Qt::DisplayRole,pPanel->getPanelDock());

		m_pItemViewerType->setData(1,Qt::DisplayRole,m_mapViewerType[pPanel->getViewerType()]);
		m_pItemViewerType->setData(1,Qt::UserRole,pPanel->getViewerType());
	}
	else
	{
		//清空属性
		m_pItemName->setData(1,Qt::DisplayRole,QString());
		m_pItemX->setData(1,Qt::DisplayRole,QString());
		m_pItemY->setData(1,Qt::DisplayRole,QString());
		m_pItemWidth->setData(1,Qt::DisplayRole,QString());
		m_pItemHeight->setData(1,Qt::DisplayRole,QString());
		m_pItemDock->setData(1,Qt::DisplayRole,QString());
	}
	CMainWindow::getMainWindow()->setPropertyWidget(this);
}

void CPanelProperty::modifyPanelName( const QString& name )
{
	if(name.trimmed().isEmpty())
		return;
	//更改名称
	if(m_pCurrentViewer)
		m_pCurrentViewer->setPanelName(name);
	updateProperty();
}

void CPanelProperty::updateProperty()
{
	//更新所有的属性
	setPanelViewer(m_pCurrentViewer);
	if(m_pCurrentViewer)
		m_pCurrentViewer->update();
}
