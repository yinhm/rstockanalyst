/*
============================================================================
文件名称	:	ListPanel.h
作者		:	李亚科
创建时间	:	2011-12-29 16:56
修改时间	:	2011-12-29 16:57
说明		:	用于存放所支持的所有窗口，目前只有Panel一种
============================================================================
*/

#ifndef _LIST_PANEL_H
#define _LIST_PANEL_H
#include <QListWidget>
#include <QtEvents>

namespace eGingkoPanelManager
{
	class CListPanel : public QListWidget
	{
		Q_OBJECT
	public:
		CListPanel(QWidget* parent = 0);
		~CListPanel(void);

	public:
		void initListPanel();

	protected:
		void startDrag(Qt::DropActions supportedActions);
	};
}

#endif	//_LIST_PANEL_H