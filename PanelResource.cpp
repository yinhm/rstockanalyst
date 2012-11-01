/*
============================================================================
文件名称	:	PanelResource.cpp
作者		:	李亚科
创建时间	:	2011-12-29 16:56
修改时间	:	2011-12-29 16:57
说明		:	用于显示当前所有的Panel资源列表，以及Panel之间的父子关系等
============================================================================
*/

#include "common.h"
#include "PanelResource.h"
#include "PanelViewer.h"

using namespace eGingkoPanelManager;

CPanelResource::CPanelResource( QWidget* parent /*= 0*/ )
	: QTreeWidget(parent)
{
	setHeaderLabels(QStringList()<<tr("Name"));
	//连接item选中的信号
	connect(this,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		this,SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

CPanelResource::~CPanelResource(void)
{
}

void CPanelResource::onCurrentItemChanged( QTreeWidgetItem* current,QTreeWidgetItem* previous )
{
	if(!current)
		return;
	QWidget* pWidget = (QWidget*)current->data(0,Qt::UserRole).toUInt();
	if(pWidget->inherits("eGingkoPanelManager::CPanelViewer"))
	{
		//将对应的窗口设置为获得焦点的状态。
		(static_cast<CPanelViewer*>(pWidget))->setPanelFocus();
	}
}
