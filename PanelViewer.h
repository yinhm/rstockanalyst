/*
============================================================================
文件名称	:	PanelViewer.h
作者		:	李亚科
创建时间	:	2011-12-29 16:56
修改时间	:	2011-12-29 16:57
说明		:	用于实现Panel的绘制，以及Panel所对应的一些操作。支持嵌套
============================================================================
*/

#ifndef _PANEL_VIEWER_H
#define _PANEL_VIEWER_H
#include <QtGui>
#include <QtEvents>
#include <QDomElement>
#include <QDomDocument>


namespace eGingkoPanelManager
{
	class CPanelProperty;
	class CPanelResource;
	class CPanelViewer : public QWidget
	{
		Q_OBJECT
	public:
		enum PanelDirect
		{
			PanelTop = 1,
			PanelBottom,
			PanelLeft,
			PanelRight,
			PanelFill
		};
		static CPanelViewer* createPanelViewer(int iType,QTreeWidgetItem* pItemParent,QWidget* parent=0);
	public:
		CPanelViewer(QTreeWidgetItem* pItemParent, int iType, QWidget* parent = 0);		//构造函数
		~CPanelViewer(void);					//析构函数

	public:
		/*Panel的基本属性*/
		//名称
		void setPanelName(const QString& qsTitle){m_qsName = qsTitle;m_pItem->setData(0,Qt::DisplayRole,m_qsName);}
		QString getPanelName(){return m_qsName;}

		//X坐标
		void setPanelX(int iX){ m_iX = iX; }
		int getPanelX(){ return m_iX; }
		//Y坐标
		void setPanelY(int iY){ m_iY = iY; }
		int getPanelY(){ return m_iY; }

		//获取X和Y坐标中较大的一个（因为只有一个是可以使用的，临时做法）
		int getPositon(){ return qMax(m_iY,m_iX); }

		//宽度
		void setPanelWidth(int iWidth){ m_iWidth = iWidth; }
		int getPanelWidth(){ return m_iWidth; }
		//宽度
		void setPanelHeight(int iHeight){ m_iHeight = iHeight; }
		int getPanelHeight(){ return m_iHeight; }

		//获取Width和Height中较小的一个（因为必须有一个是100%的，而我们要取的就是较小的那个）
		int getSize(){ return qMin(m_iHeight,m_iWidth); }

		//Dock
		void setPanelDock(PanelDirect iDock){m_iDock = iDock;}
		PanelDirect getPanelDock(){return m_iDock;}

		//ViewerType		显示控件类型
		GKG_VIEWERTYPE getViewerType() const { return m_iViewerType; }

		//获取该窗口中的Splitter指针
		QSplitter* getSplitter(){return m_pSplitter;}


		//获取父窗口
		QWidget* getParent(){return m_pParent;}
		//设置父窗口
		void resetParent(QWidget* parent);


		//获取当前窗口对应的TreeWidgetItem指针
		QTreeWidgetItem* getTreeItem(){return m_pItem;}


		//设置焦点
		void setPanelFocus();
		//去除焦点
		void killPanelFocus(){m_bFocus = false;update();}
		//获取该Panel是否获得焦点
		bool isPanelFocus(){return m_bFocus;}
		//判断该Panel的父Panel是否获得焦点
		bool isPanelParentFocus();

		//重新设置布局
		void resizeSplitter();

		//判断该窗口是否为pWidget的子窗口，含自身
		bool hasParent(QWidget* pWidget);

		//加载该Panel的配置信息
		virtual bool loadPanelInfo(const QDomElement& elePanel);
		//保存该Panel的配置信息
		virtual bool savePanelInfo(QDomDocument& doc,QDomElement& elePanel);


		//获取所有的子Panel
		QList<CPanelViewer*> getChildren();
		//获取子Panel的个数
		int getChildrenCount(){ return m_pSplitter->count(); };
		//清空子Panel
		bool clearChildren();

	public slots:
		/* 更新Panel；
		/* @param1 true:从变量更新到界面; false:从界面更新到变量
		*/
		void updatePanel(bool bSave = false);

	protected:
		virtual void paintEvent(QPaintEvent *event);			//绘制

		virtual void dragEnterEvent(QDragEnterEvent *event);		//
		virtual void dragLeaveEvent(QDragLeaveEvent *event);
		virtual void dragMoveEvent(QDragMoveEvent *event);
		virtual void dropEvent(QDropEvent *event);
		virtual void mousePressEvent(QMouseEvent * event);
		virtual void mouseMoveEvent(QMouseEvent * event);
		virtual void mouseReleaseEvent(QMouseEvent * event);

		virtual void resizeEvent(QResizeEvent* event);

		virtual void showEvent(QShowEvent* event);


		//获取需高亮显示的区域
		QRect getHighlightedRect(const QPoint& ptCurrent);

		//获取某点所在的位置
		PanelDirect getDirect(const QPoint& ptCurrent);

	public:
		static CPanelResource* g_pPanelResource;		//Panel的资源窗口

		static bool g_bDraging;							//是否有窗体正在被拖拽
		static bool g_bLoading;							//正在初始化，（加载）

		static CPanelViewer* g_pPanelMain;				//主窗口
		static QMap<int, QImage*> g_mapPixmapPanel;		//panel的中央显示图片

	protected:
		/*基本属性*/
		QString m_qsName;		//名称
		int m_iX;				//left
		int m_iY;				//top
		int m_iWidth;			//width
		int m_iHeight;			//height
		PanelDirect m_iDock;	//Dock
		QImage* m_pImgBack;		//background image

		GKG_VIEWERTYPE m_iViewerType;	//显示控件类型 默认为:GKG_VIEWER_ROI

		QRect m_rtHighlighted;
		QRect m_rtClose;		//关闭按钮的绘制区域
		int m_iTitleHeight;

		QSplitter* m_pSplitter;
		QWidget* m_pParent;
		QTreeWidgetItem* m_pItemParent;
		QTreeWidgetItem* m_pItem;

		bool m_bFocus;			//该panel是否获得焦点

		QPoint m_ptMousePress;	//鼠标按下时的坐标

	private:
		static CPanelViewer* g_pPanelFocusing;			//正在获取焦点的Panel
	};
}
#endif	//_PANEL_VIEWER_H
