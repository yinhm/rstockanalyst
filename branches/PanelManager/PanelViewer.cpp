/*
============================================================================
文件名称	:	PanelViewer.cpp
作者		:	李亚科
创建时间	:	2011-12-29 16:56
修改时间	:	2011-12-29 16:57
说明		:	用于实现Panel的绘制，以及Panel所对应的一些操作。支持嵌套
============================================================================
*/

#include "common.h"
#include "PanelViewer.h"
#include <QPainter>
#include <QLayout>
#include <QMessageBox>
#include "ListPanel.h"
#include "PanelProperty.h"
#include "PanelResource.h"

#include "ROIViewer.h"
#include "ReconsViewer.h"
#include "ReformsViewer.h"
#include "BullsEyeViewer.h"
#include "ImageViewer.h"

using namespace eGingkoPanelManager;

//静态变量的初始化
CPanelResource* CPanelViewer::g_pPanelResource = 0;
CPanelViewer* CPanelViewer::g_pPanelFocusing = 0;
CPanelViewer* CPanelViewer::g_pPanelMain = 0;
QMap<int, QImage*> CPanelViewer::g_mapPixmapPanel;



bool CPanelViewer::g_bDraging = false;
bool CPanelViewer::g_bLoading = false;

CPanelViewer* CPanelViewer::createPanelViewer( int iType,QTreeWidgetItem* pItemParent,QWidget* parent/*=0*/ )
{
	switch (iType)
	{
	case GKG_VIEWER_ROI:
		return new CROIViewer(pItemParent,parent);
		break;
	case GKG_VIEWER_RECONS:
		return new CReconsViewer(pItemParent,parent);
		break;
	case GKG_VIEWER_REFORMS:
		return new CReformsViewer(pItemParent,parent);
		break;
	case GKG_VIEWER_BullsEye:
		return new CBullsEyeViewer(pItemParent,parent);
		break;
	case GKG_VIEWER_IMAGE:
		return new CImageViewer(pItemParent,parent);
		break;
	default:
		return new CPanelViewer(pItemParent,iType,parent);
		break;
	}

	return 0;
}

CPanelViewer::CPanelViewer( QTreeWidgetItem* pItemParent, int iType, QWidget* parent /*= 0*/ )
	: QWidget(parent),m_iTitleHeight(20),m_pParent(parent),
	m_pItemParent(pItemParent),m_bFocus(false),m_iDock(PanelFill),
	m_iViewerType(static_cast<GKG_VIEWERTYPE>(iType))
{
	static int g_iPanelCount = 0;
	m_qsName = QString(tr("Panel%1")).arg(g_iPanelCount);
	++g_iPanelCount;

	m_pImgBack = g_mapPixmapPanel[m_iViewerType];


	//初始化样式
	setAcceptDrops(true);

	//将Splitter放入到Layout中，这样使其充满整个窗口
	QVBoxLayout* pLayout = new QVBoxLayout();
	pLayout->setMargin(0);		//更改此处可设置Panel的边框大小
	m_pSplitter = new QSplitter(Qt::Vertical,this);
	m_pSplitter->setHandleWidth(1);
	m_pSplitter->setFrameShadow(QFrame::Sunken);
	pLayout->addWidget(m_pSplitter);
	setLayout(pLayout);

	if(m_pItemParent)
	{
		//设置父节点
		m_pItem = new QTreeWidgetItem(m_pItemParent);
		m_pItem->setData(0,Qt::DisplayRole,m_qsName);
		m_pItem->setData(0,Qt::UserRole,(uint)this);
	}
	else
	{
		//父节点为空，则默认该节点为主窗口节点
		m_pItem = new QTreeWidgetItem(g_pPanelResource);
		m_pItem->setData(0,Qt::DisplayRole,tr("MainWindow"));
		m_pItem->setData(0,Qt::UserRole,(uint)this);
		m_qsName = tr("MainWindow");
		g_pPanelMain = this;
	}

	//设置窗口的最小值，设置后出现问题，暂时不再设置。
//	setMinimumSize(75,20);
}

CPanelViewer::~CPanelViewer(void)
{
	//清空窗体中创建的资源
	delete m_pSplitter;
	delete m_pItem;
}

void CPanelViewer::paintEvent( QPaintEvent *event )
{
	QPainter painter;
	painter.begin(this);		//开始绘制
	painter.setRenderHint(QPainter::Antialiasing, true);

	//填充背景色
	painter.fillRect(event->rect(), Qt::gray);
	painter.fillRect(event->rect().adjusted(2,2,-2,-2),Qt::black);

	if(m_pSplitter->count()==0)
	{
		//绘制title
		QRect rtTitle = event->rect().adjusted(0,1,0,m_iTitleHeight-event->rect().height());
		if(m_bFocus)
		{
			painter.fillRect(rtTitle.adjusted(2,0,-2,0), Qt::blue);
			painter.setPen(QPen(QBrush(Qt::white),2,Qt::SolidLine));
		}
		else
			painter.fillRect(rtTitle, Qt::lightGray);

		{
			//绘制标题名称
			QFontMetrics metrics = painter.fontMetrics();
			QRect rtText = rtTitle.adjusted(5,2,-60,0);
			QString qsName = m_qsName;
			if(metrics.width(qsName)>rtText.width())
			{
				while(metrics.width(qsName+"...")>rtText.width()&&qsName.length()>1)
				{
					qsName = qsName.left(qsName.length()-1);
				}
				painter.drawText(rtText,Qt::AlignLeft,qsName+"...");
			}
			else
				painter.drawText(rtText,Qt::AlignLeft,qsName);
		}

		//最大化按钮
		QRect rtMaximum = rtTitle.adjusted(rtTitle.width()-60,2,-44,-2);
		painter.fillRect(rtMaximum,Qt::white);
		if(m_bFocus)
			painter.fillRect(rtMaximum.adjusted(2,2,-2,-2), Qt::blue);
		else
			painter.fillRect(rtMaximum.adjusted(2,2,-2,-2),Qt::lightGray);

		//最小化按钮
		QRect rtMinimum = rtTitle.adjusted(rtTitle.width()-40,8,-24,-8);
		painter.fillRect(rtMinimum,Qt::white);

		//最大化按钮
		m_rtClose = rtTitle.adjusted(rtTitle.width()-20,2,-4,-2);
		QPen pen(QBrush(Qt::white),2,Qt::SolidLine);
		painter.setPen(pen);
		painter.drawLine(m_rtClose.topLeft(),m_rtClose.bottomRight());
		painter.drawLine(m_rtClose.topRight(),m_rtClose.bottomLeft());
	}

	
	//根据设备类型绘制panel中央的显示图片
	QRect imageRect = event->rect().adjusted(2,m_iTitleHeight+2,-2,-2);

	//if(imageRect.width()>=m_pImgBack->width()&&imageRect.height()>=m_pImgBack->height())
	//{
	//	//当绘制的区域大于图片大小时，获取中心绘制区域
	//	imageRect = event->rect().adjusted(0,0,-4,-m_iTitleHeight-4);
	//	imageRect = QRect(imageRect.center()-QPoint(m_pImgBack->width()/2,m_pImgBack->height()/2),
	//		imageRect.center()+QPoint(m_pImgBack->width()/2,m_pImgBack->height()/2));
	//	imageRect = imageRect.adjusted(2,m_iTitleHeight+2,2,m_iTitleHeight+2);
	//}

	//保持比例，进行最大化绘制。
	QImage img = m_pImgBack->scaled(imageRect.size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
	if(imageRect.width()>img.width())
	{
		imageRect.setLeft(imageRect.left()+(imageRect.width()-img.width())/2);
		imageRect.setRight(imageRect.right()-(imageRect.width()-img.width())/2);
	}
	if(imageRect.height()>img.height())
	{
		imageRect.setTop(imageRect.top()+(imageRect.height()-img.height())/2);
		imageRect.setBottom(imageRect.bottom()-(imageRect.height()-img.height())/2);
	}
	painter.drawImage(imageRect,img);


	//绘制高亮
	if (m_rtHighlighted.isValid())
	{
		painter.setBrush(QColor("#ffcccc"));
		painter.setPen(Qt::NoPen);
		painter.drawRect(m_rtHighlighted.adjusted(0, 0, -1, -1));
	}


	painter.end();		//完成绘制
}

void CPanelViewer::dragEnterEvent( QDragEnterEvent *event )
{
	if (event->mimeData()->hasFormat("panel/new")||event->mimeData()->hasFormat("panel/move"))
	{
		event->accept();
		g_bDraging = true;
	}
	else
	{
		event->ignore();
		g_bDraging = false;
	}
}

void CPanelViewer::dragLeaveEvent( QDragLeaveEvent *event )
{
	m_rtHighlighted = QRect();
	update();
	event->accept();
}

void CPanelViewer::dragMoveEvent( QDragMoveEvent *event )
{
	if (event->mimeData()->hasFormat("panel/new"))
	{
		m_rtHighlighted = getHighlightedRect(event->pos());
		event->setDropAction(Qt::CopyAction);
		event->accept();
		g_bDraging = true;
	}
	else if(event->mimeData()->hasFormat("panel/move"))
	{
		//取正在移动的panel指针
		uint iPanelMoving = 0;
		QDataStream dataStream(&event->mimeData()->data("panel/move"), QIODevice::ReadOnly);
		dataStream>>iPanelMoving;

		CPanelViewer* pPanelMoving = reinterpret_cast<CPanelViewer*>(iPanelMoving);
		if(pPanelMoving&&!hasParent(pPanelMoving))
		{
			m_rtHighlighted = getHighlightedRect(event->pos());
			event->setDropAction(Qt::MoveAction);
			event->accept();
			g_bDraging = true;
		}
		else
		{
			m_rtHighlighted = QRect();
			event->ignore();
			g_bDraging = false;
		}
	}
	else
	{
		m_rtHighlighted = QRect();
		event->ignore();
		g_bDraging = false;
	}

	update();
}

void CPanelViewer::dropEvent( QDropEvent *event )
{
	m_rtHighlighted = QRect();
	update();
	g_bDraging = false;
//	g_bLoading = true;		//禁止自动设置Panel的值

	if (event->mimeData()->hasFormat("panel/new"))
	{
		int iType = 0;	//get the type
		QDataStream dataStream(&event->mimeData()->data("panel/new"), QIODevice::ReadOnly);
		dataStream>>iType;
		//添加窗口
		PanelDirect direct = getDirect(event->pos());
		if(direct == PanelFill || (!m_pParent->inherits("eGingkoPanelManager::CPanelViewer")))
		{
			//直接添加到splitter中
			CPanelViewer* pPanelChild = createPanelViewer(iType,m_pItem,this);
			pPanelChild->setPanelDock(PanelFill);

			m_pSplitter->addWidget(pPanelChild);
		}
		else if(m_pParent&&m_pParent->inherits("eGingkoPanelManager::CPanelViewer"))
		{
			//需获取父窗口，加入到父窗口中的splitter
			Qt::Orientation orien = (direct == PanelBottom||direct == PanelTop) ? Qt::Vertical : Qt::Horizontal;

			CPanelViewer* pPanelParent = static_cast<CPanelViewer*>(m_pParent);
			QSplitter* pSplitterParent = pPanelParent->getSplitter();

			if(pSplitterParent->count()>1&&pSplitterParent->orientation()!=orien)
			{
				//创建两个Panel，一个作为父Panel
				CPanelViewer* pPanelParentNew = createPanelViewer(GKG_VIEWER_NORMAL,m_pItemParent,m_pParent);
				pPanelParentNew->setPanelDock(getPanelDock());
				CPanelViewer* pPanelChild2 = createPanelViewer(iType,pPanelParentNew->getTreeItem(),pPanelParentNew);
				CPanelViewer* pPanelGrandparent = static_cast<CPanelViewer*>(m_pParent);
				QList<int> listSizesParent = pPanelGrandparent->getSplitter()->sizes();

				pPanelParentNew->getSplitter()->setOrientation(orien);
				pPanelGrandparent->getSplitter()->insertWidget(pPanelGrandparent->getSplitter()->indexOf(this),pPanelParentNew);
				resetParent(pPanelParentNew);
				pPanelGrandparent->getSplitter()->setSizes(listSizesParent);

				pPanelParentNew->getSplitter()->addWidget(this);
				if( direct == PanelTop || direct == PanelLeft )
				{
					pPanelChild2->setPanelDock(direct);
					setPanelDock(PanelFill);
					pPanelParentNew->getSplitter()->insertWidget(0,pPanelChild2);
				}
				else
				{
					pPanelChild2->setPanelDock(PanelFill);
					setPanelDock(pPanelParentNew->getSplitter()->orientation()==Qt::Horizontal ? PanelLeft : PanelTop);
					pPanelParentNew->getSplitter()->addWidget(pPanelChild2);
				}
				pPanelParentNew->resizeSplitter();
			}
			else
			{
				CPanelViewer* pPanelChild = createPanelViewer(iType,pPanelParent->getTreeItem(),pPanelParent);
				pSplitterParent->setOrientation(orien);

				int iIndex = pSplitterParent->indexOf(this);
				QList<int> listSizes = pSplitterParent->sizes();
				if(listSizes.size()>iIndex)
				{
					listSizes[iIndex] = listSizes[iIndex]/2;
					listSizes.insert(iIndex,listSizes[iIndex]);
				}

				if( direct == PanelTop || direct == PanelLeft )
				{
					pPanelChild->setPanelDock(direct);
					pSplitterParent->insertWidget(iIndex,pPanelChild);
				}
				else
				{
					if(pSplitterParent->indexOf(this)==(pSplitterParent->count()-1))
					{
						pPanelChild->setPanelDock(PanelFill);
						setPanelDock(pSplitterParent->orientation()==Qt::Horizontal ? PanelLeft : PanelTop);
					}
					else
						pPanelChild->setPanelDock(pSplitterParent->orientation()==Qt::Horizontal ? PanelLeft : PanelTop);
					pSplitterParent->insertWidget(iIndex+1,pPanelChild);
				}
				pSplitterParent->setSizes(listSizes);
			}
		}
	}
	else if(event->mimeData()->hasFormat("panel/move"))
	{
		//取正在移动的panel指针
		uint iPanelMoving = 0;
		QDataStream dataStream(&event->mimeData()->data("panel/move"), QIODevice::ReadOnly);
		dataStream>>iPanelMoving;
		CPanelViewer* pPanelMoving = reinterpret_cast<CPanelViewer*>(iPanelMoving);
		if((!pPanelMoving)||hasParent(pPanelMoving))
			return;

		//移动窗口
		PanelDirect direct = getDirect(event->pos());
		if(direct == PanelFill || (!m_pParent->inherits("eGingkoPanelManager::CPanelViewer")))
		{
			//直接添加到splitter中
			pPanelMoving->resetParent(this);
			pPanelMoving->setPanelDock(direct);
			m_pSplitter->addWidget(pPanelMoving);
		}
		else if(m_pParent&&m_pParent->inherits("eGingkoPanelManager::CPanelViewer"))
		{
			//需获取父窗口，加入到父窗口中的splitter
			Qt::Orientation orien = (direct == PanelBottom||direct == PanelTop) ? Qt::Vertical : Qt::Horizontal;

			CPanelViewer* pPanelParent = static_cast<CPanelViewer*>(m_pParent);
			QSplitter* pSplitterParent = pPanelParent->getSplitter();

			if(pSplitterParent->count()>1&&pSplitterParent->orientation()!=orien)
			{
				//创建两个Panel，一个作为父Panel
				CPanelViewer* pPanelParentNew = createPanelViewer(GKG_VIEWER_NORMAL,m_pItemParent,m_pParent);
				pPanelParentNew->setPanelDock(getPanelDock());
				CPanelViewer* pPanelGrandparent = static_cast<CPanelViewer*>(m_pParent);
				QList<int> listSizesParent = pPanelGrandparent->getSplitter()->sizes();

				pPanelParentNew->getSplitter()->setOrientation(orien);
				pPanelGrandparent->getSplitter()->insertWidget(pPanelGrandparent->getSplitter()->indexOf(this),pPanelParentNew);
				resetParent(pPanelParentNew);
				pPanelGrandparent->getSplitter()->setSizes(listSizesParent);

				pPanelParentNew->getSplitter()->addWidget(this);
				pPanelMoving->resetParent(pPanelParentNew);
				if( direct == PanelTop || direct == PanelLeft )
				{
					pPanelMoving->setPanelDock(direct);
					pPanelParentNew->getSplitter()->insertWidget(0,pPanelMoving);
				}
				else
				{
					pPanelMoving->setPanelDock(PanelFill);
					setPanelDock(pPanelParentNew->getSplitter()->orientation()==Qt::Horizontal ? PanelLeft : PanelTop);
					pPanelParentNew->getSplitter()->addWidget(pPanelMoving);
				}
			}
			else
			{
				pSplitterParent->setOrientation(orien);
				pPanelMoving->resetParent(pPanelParent);

				if( direct == PanelTop || direct == PanelLeft )
				{
					pPanelMoving->setPanelDock(direct);
					int iIndex = pSplitterParent->indexOf(this);
					QList<int> listSizes=pSplitterParent->sizes();
					if(listSizes.size()>iIndex)
					{
						listSizes[iIndex] = listSizes[iIndex]/2;
						listSizes.insert(iIndex,listSizes[iIndex]);
					}
					pSplitterParent->insertWidget(iIndex,pPanelMoving);
					pSplitterParent->setSizes(listSizes);
				}
				else
				{
					if(pSplitterParent->indexOf(this)==(pSplitterParent->count()-1))
					{
						pPanelMoving->setPanelDock(PanelFill);
						setPanelDock(pSplitterParent->orientation()==Qt::Horizontal ? PanelLeft : PanelTop);
					}
					else
						pPanelMoving->setPanelDock(pSplitterParent->orientation()==Qt::Horizontal ? PanelLeft : PanelTop);
					pSplitterParent->insertWidget(pSplitterParent->indexOf(this)+1,pPanelMoving);
				}
			}
		}

		pPanelMoving->setPanelFocus();
	}

//	g_bLoading = false;
//	g_pPanelMain->updatePanel(true);		//更新位置和大小
}

QRect CPanelViewer::getHighlightedRect( const QPoint& ptCurrent )
{
	PanelDirect direct = getDirect(ptCurrent);
	if(!m_pParent->inherits("eGingkoPanelManager::CPanelViewer"))
	{
		//如果父窗口不是从CPanelViewer继承的
		return rect().adjusted(2,m_iTitleHeight+2,-2,-2);
	}
	switch(direct)
	{
	case PanelFill:
		{
			return rect().adjusted(2,m_iTitleHeight+2,-2,-2);
		}
		break;
	case PanelLeft:
		{
			return rect().adjusted(0,m_iTitleHeight,-(rect().width()/2),0);
		}
		break;
	case PanelRight:
		{
			return rect().adjusted(rect().width()/2,m_iTitleHeight,0,0);
		}
		break;
	case PanelTop:
		{
			return rect().adjusted(0,m_iTitleHeight,0,-((rect().height()-m_iTitleHeight)/2));
		}
		break;
	case PanelBottom:
		{
			return rect().adjusted(0,(rect().height()+m_iTitleHeight)/2,0,0);
		}
		break;
	}
	return QRect();
}

CPanelViewer::PanelDirect CPanelViewer::getDirect( const QPoint& ptCurrent )
{
	QPoint ptCenter = rect().center();			//中心点
	QPoint ptSub = ptCenter - ptCurrent;		//相减后的点
	if(ptSub.y()==0||rect().height()==0)
	{
		return PanelFill;
	}

	float fDivCurrent = (float)ptSub.x()/(float)ptSub.y();			//当前角度
	float fDiv = (float)rect().width()/(float)rect().height();		//窗体的角度

	if((ptCurrent - ptCenter).manhattanLength() < ((ptCenter-rect().topLeft()).manhattanLength())/4)
	{
		return PanelFill;
	}
	else
	{
		//内侧
		if(fDivCurrent<fDiv)
		{
			if(ptCenter.y()>ptCurrent.y())
			{
				//top
				return PanelTop;
			}
			else
			{
				//bottom
				return PanelBottom;
			}
		}
		else
		{
			if(ptCenter.x()>ptCurrent.x())
			{
				//left
				return PanelLeft;
			}
			else
			{
				//right
				return PanelRight;
			}
		}
	}
}

void CPanelViewer::mousePressEvent( QMouseEvent * event )
{
	if(event->button() == Qt::LeftButton)
		m_ptMousePress = event->pos();
	else
		m_ptMousePress = QPoint(0xFFFFFF,0xFFFFFF);

	//当本身以及父Panel都未获得焦点时，设置该Panel获得焦点
	if(!(isPanelFocus()||isPanelParentFocus()))
		setPanelFocus();

	if(m_rtClose.contains(event->pos()))
	{
		if(!m_pParent->inherits("eGingkoPanelManager::CPanelViewer"))
		{
			//如果是MainWindow，直接返回。
			return;
		}
		if( g_pPanelFocusing == this )
		{
			g_pPanelFocusing = 0;
			CPanelProperty::getPanelProperty(m_iViewerType)->setPanelViewer(0);
		}


		if(m_pParent&&m_pParent->inherits("eGingkoPanelManager::CPanelViewer"))
		{
			deleteLater();
			CPanelViewer* pPanelParent = static_cast<CPanelViewer*>(m_pParent);
			if(pPanelParent->getSplitter()->count()<2&&pPanelParent->getViewerType()==GKG_VIEWER_NORMAL)
			{
				if(pPanelParent->getParent()&&pPanelParent->getParent()->inherits("eGingkoPanelManager::CPanelViewer"))
				{
					pPanelParent->deleteLater();
				}
			}
		}

		return;
	}
}

void CPanelViewer::mouseMoveEvent( QMouseEvent * event )
{
	if(g_pPanelFocusing && (m_ptMousePress-event->pos()).manhattanLength()>5)
	{
		//执行拖拽
		QDrag *drag = new QDrag(this);

		//将该Panel的指针加入到mimeData中
		QMimeData *mimeData = new QMimeData;
		QByteArray itemData;
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		dataStream << (uint)g_pPanelFocusing;
		mimeData->setData("panel/move",itemData);

		QPixmap pixmap(g_pPanelFocusing->size());
		g_pPanelFocusing->render(&pixmap);
		drag->setMimeData(mimeData);
		drag->setPixmap(pixmap);
		drag->setHotSpot(event->globalPos()-g_pPanelFocusing->mapToGlobal(QPoint(0,0)));

		Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
	}
}

void CPanelViewer::mouseReleaseEvent( QMouseEvent * event )
{
	if((!m_bFocus)&&(!g_bDraging))
		setPanelFocus();
	g_bDraging = false;
	m_ptMousePress = QPoint(0xFFFFFF,0xFFFFFF);
}

void CPanelViewer::setPanelFocus()
{
	setFocus();
	if(g_pPanelFocusing)
	{
		g_pPanelFocusing->killPanelFocus();
	}
	m_bFocus = true;
	g_pPanelFocusing = this;
	CPanelProperty::getPanelProperty(m_iViewerType)->setPanelViewer(this);
	g_pPanelResource->setCurrentItem(m_pItem);
	update();
}

bool CPanelViewer::isPanelParentFocus()
{
	CPanelViewer* pParent = static_cast<CPanelViewer*>(getParent());
	while(pParent&&pParent->inherits("eGingkoPanelManager::CPanelViewer"))
	{
		if((static_cast<CPanelViewer*>(pParent))->isPanelFocus())
		{
			return true;
		}
		else
		{
			pParent = static_cast<CPanelViewer*>(pParent->getParent());
		}
	}
	return false;
}

void CPanelViewer::resetParent( QWidget* parent )
{
	m_pParent = parent;
	QWidget::setParent(parent);
	if(m_pParent->inherits("eGingkoPanelManager::CPanelViewer"))
	{
		m_pItemParent->removeChild(m_pItem);
		static_cast<CPanelViewer*>(m_pParent)->getTreeItem()->addChild(m_pItem);
		m_pItemParent = static_cast<CPanelViewer*>(m_pParent)->getTreeItem();
	}
}

void CPanelViewer::resizeSplitter()
{
	QList<int> listSize;
	listSize.push_back(-1);
	m_pSplitter->setSizes(listSize);
}

bool CPanelViewer::hasParent( QWidget* pWidget )
{
	CPanelViewer* pParent = this;
	while(pParent&&pParent->inherits("eGingkoPanelManager::CPanelViewer"))
	{
		if(pParent == pWidget)
		{
			return true;
		}
		else
		{
			pParent = static_cast<CPanelViewer*>(pParent->getParent());
		}
	}
	return false;
}

void CPanelViewer::resizeEvent( QResizeEvent* event )
{
	//大小发生改变是更新属性面板中显示的内容。
	updatePanel(false);
	if(isPanelFocus())
		CPanelProperty::getPanelProperty(m_iViewerType)->setPanelViewer(g_pPanelFocusing);

	return QWidget::resizeEvent(event);
}

void CPanelViewer::showEvent( QShowEvent* event )
{
	setPanelFocus();
	return QWidget::showEvent(event);
}

bool CPanelViewer::loadPanelInfo( const QDomElement& elePanel )
{
	if(!elePanel.isElement())
		return false;

	//获取名称
	QDomElement eleName = elePanel.firstChildElement("NAME");
	if(eleName.isElement())
		setPanelName(eleName.text());

	//获取坐标
	QDomElement eleX = elePanel.firstChildElement("X");
	if(eleX.isElement())
		setPanelX(eleX.text().toInt());
	QDomElement eleY = elePanel.firstChildElement("Y");
	if(eleY.isElement())
		setPanelY(eleY.text().toInt());

	//获取宽高
	QDomElement eleWidth = elePanel.firstChildElement("WIDTH");
	if(eleWidth.isElement())
		setPanelWidth(eleWidth.text().toInt());
	QDomElement eleHeight = elePanel.firstChildElement("HEIGHT");
	if(eleHeight.isElement())
		setPanelHeight(eleHeight.text().toInt());

	QDomElement eleDock = elePanel.firstChildElement("DOCK");
	if(eleDock.isElement())
		setPanelDock((PanelDirect)eleDock.text().toInt());

	//显示控件类型
	QDomElement eleViewerType = elePanel.firstChildElement("VIEWTYPE");
	if(eleViewerType.isElement())
		m_iViewerType = (GKG_VIEWERTYPE)eleViewerType.text().toInt();

	//加载子Panel
	QDomElement eleChild = elePanel.lastChildElement("PANEL");
	while(eleChild.isElement())
	{
		int iType = GKG_VIEWER_NORMAL;
		QDomElement eleChildType = eleChild.firstChildElement("VIEWTYPE");
		if(eleChildType.isElement())
		{
			bool bOK;
			iType = eleChildType.text().toInt(&bOK);
			if(!bOK)
				iType = GKG_VIEWER_NORMAL;
		}
		CPanelViewer* pPanelChild = CPanelViewer::createPanelViewer(iType,m_pItem,this);
		m_pSplitter->addWidget(pPanelChild);
		pPanelChild->loadPanelInfo(eleChild);
		eleChild = eleChild.previousSiblingElement("PANEL");
	}

	return true;
}

bool CPanelViewer::savePanelInfo( QDomDocument& doc,QDomElement& elePanel )
{
	//保存Panel信息
	if(!elePanel.isElement())
		return false;

	//获取名称
	QDomElement eleName = doc.createElement("NAME");
	eleName.appendChild(doc.createTextNode(getPanelName()));
	elePanel.appendChild(eleName);

	//获取坐标
	QDomElement eleX = doc.createElement("X");
	eleX.appendChild(doc.createTextNode(QString("%1").arg(getPanelX())));
	elePanel.appendChild(eleX);
	QDomElement eleY = doc.createElement("Y");
	eleY.appendChild(doc.createTextNode(QString("%1").arg(getPanelY())));
	elePanel.appendChild(eleY);

	//获取宽高
	QDomElement eleWidth = doc.createElement("WIDTH");
	eleWidth.appendChild(doc.createTextNode(QString("%1").arg(getPanelWidth())));
	elePanel.appendChild(eleWidth);
	QDomElement eleHeight = doc.createElement("HEIGHT");
	eleHeight.appendChild(doc.createTextNode(QString("%1").arg(getPanelHeight())));
	elePanel.appendChild(eleHeight);

	QDomElement eleDock = doc.createElement("DOCK");
	eleDock.appendChild(doc.createTextNode(QString("%1").arg(getPanelDock())));
	elePanel.appendChild(eleDock);


	//加载子Panel
	QList<CPanelViewer*> listPanel = getChildren();
	if(listPanel.size()<1)
	{
		//没有子Panel
		QDomElement eleViewerType = doc.createElement("VIEWTYPE");
		eleViewerType.appendChild(doc.createTextNode(QString("%1").arg(m_iViewerType)));
		elePanel.appendChild(eleViewerType);
	}
	else
	{
		//还有子Panel
		foreach(CPanelViewer* pChild,listPanel)
		{
			static int iSplitterCount = 1;			//Splitter的个数

			QDomElement eleChild = doc.createElement("PANEL");
			elePanel.appendChild(eleChild);
			pChild->savePanelInfo(doc,eleChild);
			if(pChild!=listPanel.last())
			{
				//保存Splitter。
				QDomElement eleSplitter = doc.createElement("SPLITTER");
				elePanel.appendChild(eleSplitter);

				QDomElement eleSplitterName = doc.createElement("NAME");
				eleSplitterName.appendChild(doc.createTextNode(QString("Splitter%1").arg(iSplitterCount)));
				eleSplitter.appendChild(eleSplitterName);

				QDomElement eleSplitterDock = doc.createElement("DOCK");
				eleSplitterDock.appendChild(doc.createTextNode(m_pSplitter->orientation()==Qt::Horizontal ? QString("3") : QString("1")));			//？？？？？？？？？？
				eleSplitter.appendChild(eleSplitterDock);

				++iSplitterCount;
			}
		}
	}


	return true;
}

QList<CPanelViewer*> CPanelViewer::getChildren()
{
	QList<CPanelViewer*> listChild;		//获取所有的子panel
	for(int i = 0; i < m_pSplitter->count(); ++i)
	{
		CPanelViewer* pPanel = static_cast<CPanelViewer*>(m_pSplitter->widget(i));
		listChild.push_front(pPanel);
	}

	return listChild;
}

bool CPanelViewer::clearChildren()
{
	while(m_pSplitter->count()>0)
	{
		CPanelViewer* pPanel = static_cast<CPanelViewer*>(m_pSplitter->widget(0));
		if(pPanel)
			delete pPanel;
	}

	return true;
}

void CPanelViewer::updatePanel( bool bSave /*= false*/ )
{
	if(bSave)
	{
		//更新到界面
		///////////////////////////
		QList<CPanelViewer*> listChild = getChildren();		//获取所有的子panel
		QList<int> listSize;								//用于存放所有子Panel的大小

		QMap<int,CPanelViewer*> mapPanel;					//用于对子Panel按其位置进行排序
		foreach(CPanelViewer* pChild,listChild)
		{
			pChild->updatePanel(true);						//先更新子Panel

			if(pChild->getPanelDock() == PanelLeft)
				m_pSplitter->setOrientation(Qt::Horizontal);
			else if(pChild->getPanelDock() == PanelTop)
				m_pSplitter->setOrientation(Qt::Vertical);

			mapPanel.insert(pChild->getPositon(),pChild);
			listSize.push_back(pChild->getSize());
		}

		m_pSplitter->setSizes(listSize);										//设置比例
	}
	else if(!g_bLoading)
	{
		//如果处于加载中时，不对变量进行更新
		//从界面更新到变量
		if(m_pParent&&m_pParent->inherits("eGingkoPanelManager::CPanelViewer"))
		{
			CPanelViewer* pPanelParent = static_cast<CPanelViewer*>(m_pParent);
//			QSplitter* pSplitterParent = pPanelParent->getSplitter();
			m_iX =  m_pParent->width()>0 ? 
				(mapToParent(rect().topLeft()).x()*100)/(m_pParent->width()) : 0;

			m_iY = m_pParent->height()>0 ? 
				((mapToParent(rect().topLeft()).y()*100)/(m_pParent->height())) : 0;

			int iParentWidth = pPanelParent->width();
			int iParentHeight = pPanelParent->height();
			if(pPanelParent->getSplitter()->orientation() == Qt::Horizontal)
				iParentWidth = iParentWidth-pPanelParent->getChildrenCount()*2+2;
			else
				iParentHeight = iParentHeight-pPanelParent->getChildrenCount()*2+2;

			m_iWidth = m_pParent->width()>0 ?
				(float((rect().width()*100)/float(iParentWidth)+0.5)) : 0;

			m_iHeight = m_pParent->height()>0 ?
				(float((rect().height()*100)/float(iParentHeight)+0.5)) : 0;
		}
		else if(m_pParent)
		{
			//根Panel
			m_iX = 0;
			m_iY = 0;
			m_iHeight =100;
			m_iWidth = 100;
		}
	}
}