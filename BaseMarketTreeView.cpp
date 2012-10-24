#include "StdAfx.h"
#include <QHeaderView>
#include "BaseMarketTreeView.h"
#include "DataEngine.h"


CBaseMarketTreeView::CBaseMarketTreeView(WORD wMarket, QWidget* parent /*= 0*/ )
	: QTreeView(parent)
	, m_pModel(NULL)
	, iHBarValue(0)
{
	setUniformRowHeights(true);
	setSelectionMode(QTreeView::SingleSelection);
	setSelectionBehavior(QTreeView::SelectRows);
	setItemDelegate(new CBaseMarketItemDelegate);

	setHorizontalScrollMode(QTreeView::ScrollPerItem);
	setVerticalScrollMode(QTreeView::ScrollPerItem);

	//ÉèÖÃÃ»ÓÐbranch
	setRootIsDecorated(false);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	m_pModel = new CBaseMarketTreeModel(wMarket);
	setModel(m_pModel);

	connect(CDataEngine::getDataEngine(),SIGNAL(stockInfoChanged(const QString&)),m_pModel,SLOT(updateStockItem(const QString&)));
//	setHorizontalScrollBar(new CBaseMarketScrollBar(this));
}

CBaseMarketTreeView::~CBaseMarketTreeView(void)
{
}

void CBaseMarketTreeView::keyPressEvent( QKeyEvent* event )
{
	if(event->key() == Qt::Key_Right)
	{

		return;
	}
	else if(event->key() == Qt::Key_Left)
	{

		return;
	}
	
	return QTreeView::keyPressEvent(event);
}

void CBaseMarketTreeView::horizonScolled( int v )
{
	if(v>iHBarValue)
	{
		for(int i=iHBarValue;i<v;++i)
		{
			int iIndex = header()->logicalIndex(i+3);
			this->hideColumn(iIndex);
			stackHiddenColumn.push(iIndex);
		}
	}
	else if(iHBarValue>v)
	{
		for(int i=iHBarValue;i>v;--i)
		{
			if(stackHiddenColumn.size()>0)
			{
				this->showColumn(stackHiddenColumn.pop());
			}
		}
	}

	iHBarValue = v;
}
