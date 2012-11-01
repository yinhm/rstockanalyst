/*
============================================================================
文件名称	:	PanelProperty.h
作者		:	李亚科
创建时间	:	2011-12-29 16:56
修改时间	:	2011-12-29 16:57
说明		:	用于显示Panel的属性窗口
============================================================================
*/

#ifndef _PANEL_PROPERTY_H
#define _PANEL_PROPERTY_H
#include <QtGui>

namespace eGingkoPanelManager
{
	class CPanelViewer;
	class CPanelProperty;
	class CROIProperty;

	class CPropertyDelegateItem : public QItemDelegate
	{
		Q_OBJECT
	public:
		CPropertyDelegateItem(CPanelProperty* parent);
		~CPropertyDelegateItem(){}

		static CPropertyDelegateItem* getPropertyDelegateItem(int iType,CPanelProperty* parent);

	protected:
		virtual QWidget* createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
		virtual void setEditorData ( QWidget * editor, const QModelIndex & index ) const;
		virtual void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;

	private:
		CPanelProperty* m_pPanelProperty;
		static QMap<int, CPropertyDelegateItem*> m_mapPropertyDelegate;
	};

	class CPanelProperty : public QTreeWidget
	{
		Q_OBJECT
		friend class CPropertyDelegateItem;
	public:
		CPanelProperty(QWidget* parent = 0);
		~CPanelProperty(void);

		static CPanelProperty* getPanelProperty(int iType);

	public:
		void modifyPanelName(const QString& name);	//修改名称

	public slots:
		virtual void setPanelViewer(CPanelViewer* pPanel);		//设置当前的Panel
		void updateProperty();							//更新当前Panel的显示内容


	private:
		QTreeWidgetItem* m_pItemName;	//Panel的名称
		QTreeWidgetItem* m_pItemX;		//Panel的X坐标
		QTreeWidgetItem* m_pItemY;		//Panel的Y坐标
		QTreeWidgetItem* m_pItemWidth;	//Panel的宽度
		QTreeWidgetItem* m_pItemHeight;	//Panel的高度
		QTreeWidgetItem* m_pItemDock;	//Panel的Dock属性
		QTreeWidgetItem* m_pItemViewerType;	//显示控件类别 属性

		CPanelViewer* m_pCurrentViewer;

		QMap<int,QString> m_mapViewerType;

	private:
		static QMap<int, CPanelProperty*> m_mapPanelProperty;
	};
}
#endif	//_PANEL_PROPERTY_H