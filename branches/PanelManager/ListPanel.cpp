/*
============================================================================
文件名称	:	ListPanel.cpp
作者		:	李亚科
创建时间	:	2011-12-29 16:56
修改时间	:	2011-12-29 16:57
说明		:	用于存放所支持的所有窗口，目前只有Panel一种
============================================================================
*/

#include "common.h"
#include "ListPanel.h"
#include "PanelViewer.h"

using namespace eGingkoPanelManager;

CListPanel::CListPanel( QWidget* parent /*= 0*/ )
	: QListWidget(parent)
{
	//初始化样式
	setDragEnabled(true);
	setViewMode(QListView::IconMode);
	setIconSize(QSize(64, 64));
	setSpacing(10);
	setDropIndicatorShown(true);

	//初始化控件
	initListPanel();
}

CListPanel::~CListPanel(void)
{
}

void CListPanel::initListPanel()
{
	QListWidgetItem* pItemNormal = new QListWidgetItem(QIcon("./images/panel.png"),tr("Panel Viewer"),this);
	pItemNormal->setData(Qt::UserRole,static_cast<int>(GKG_VIEWER_NORMAL));
	pItemNormal->setToolTip(tr("Panel Viewer"));

	QListWidgetItem* pItemROI = new QListWidgetItem(QIcon("./images/roi.png"),tr("ROI Viewer"),this);
	pItemROI->setData(Qt::UserRole,static_cast<int>(GKG_VIEWER_ROI));
	pItemROI->setToolTip(tr("ROI Viewer"));

	QListWidgetItem* pItemRecons = new QListWidgetItem(QIcon("./images/recons.png"),tr("Recons Viewer"),this);
	pItemRecons->setData(Qt::UserRole,static_cast<int>(GKG_VIEWER_RECONS));
	pItemRecons->setToolTip(tr("Recons Viewer"));

	QListWidgetItem* pItemReforms = new QListWidgetItem(QIcon("./images/reforms.png"),tr("Reforms Viewer"),this);
	pItemReforms->setData(Qt::UserRole,static_cast<int>(GKG_VIEWER_REFORMS));
	pItemReforms->setToolTip(tr("Reforms Viewer"));

	QListWidgetItem* pItemBulls = new QListWidgetItem(QIcon("./images/bullseye.png"),tr("Bulls Eye Viewer"),this);
	pItemBulls->setData(Qt::UserRole,static_cast<int>(GKG_VIEWER_BullsEye));
	pItemBulls->setToolTip(tr("Bulls Eye Viewer"));

	QListWidgetItem* pItemImage = new QListWidgetItem(QIcon("./images/image.png"),tr("Image Viewer"),this);
	pItemImage->setData(Qt::UserRole,static_cast<int>(GKG_VIEWER_IMAGE));
	pItemImage->setToolTip(tr("Image Viewer"));

	setGridSize(QSize(76,76));
}

void CListPanel::startDrag( Qt::DropActions supportedActions )
{
	QListWidgetItem *item = currentItem();

	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);

	dataStream << item->data(Qt::UserRole).toInt();

	QPixmap pixmap = QPixmap::fromImage(*CPanelViewer::g_mapPixmapPanel[item->data(Qt::UserRole).toInt()]).scaled(128,128);

	QMimeData *mimeData = new QMimeData;
	mimeData->setData("panel/new", itemData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
	drag->setPixmap(pixmap);

	drag->exec(Qt::CopyAction);
}
