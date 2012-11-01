/*
============================================================================
文件名称	:	MainWindow.h
作者		:	李亚科
创建时间	:	2011-12-29 16:56
修改时间	:	2011-12-29 16:57
说明		:	程序的主窗口。
============================================================================
*/

#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H
#include <QtGui>
#include <QtXml>
#include "ListPanel.h"
#include "PanelViewer.h"
#include "PanelResource.h"
#include "PanelProperty.h"

namespace eGingkoPanelManager
{
	class CMainWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		CMainWindow(QWidget* parent = NULL);
		~CMainWindow(void);
		static CMainWindow* getMainWindow();

	public:
		void setPropertyWidget(QWidget* widget);
		CPanelViewer* getCentralPanel(){ return m_pMainWidget; }

	protected:
		virtual void showEvent(QShowEvent* event);

	protected slots:
		void OnLoadPanelFile();							//响应加载Panel函数
		void OnSavePanelFile();							//响应保存Panel函数
		void OnClearPanels()							//响应清空Panel函数
		{
			m_pMainWidget->clearChildren();
		}

	private:
		QWidget* m_pViewWidget;				//

		CPanelViewer* m_pMainWidget;		//中央布局窗口
		CListPanel* m_pListPanels;		//Panel管理窗口
		CPanelResource* m_pPanelResource;		//窗口资源关系图
		QMenuBar* m_pMenuBar;

		QDockWidget* m_pDockSettings;
		QDockWidget* m_pDockResource;
		QDockWidget* m_pDockListViewers;	//
		QDockWidget* m_pDockProperty;		//属性管理Dock

		QLineEdit* m_pEditName;
		QComboBox* m_pComboClinic;

		static CMainWindow* m_pMainWindow;
	};
}
#endif	//_MAIN_WINDOW_H