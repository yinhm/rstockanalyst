/*
============================================================================
文件名称	:	PanelResource.h
作者		:	李亚科
创建时间	:	2011-12-29 16:56
修改时间	:	2011-12-29 16:57
说明		:	用于显示当前所有的Panel资源列表，以及Panel之间的父子关系等
============================================================================
*/

#ifndef _PANEL_RESOURCE_H
#define _PANEL_RESOURCE_H

#include <QtGui>

namespace eGingkoPanelManager
{
	class CPanelResource : public QTreeWidget
	{
		Q_OBJECT
	public:
		CPanelResource(QWidget* parent = 0);
		~CPanelResource(void);

	protected slots:
		void onCurrentItemChanged(QTreeWidgetItem* current,QTreeWidgetItem* previous);
	};
}

#endif	//_PANEL_RESOURCE_H