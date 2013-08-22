#include "StdAfx.h"
#include "AllStockWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"


CAllStockWidget::CAllStockWidget( CBaseWidget* parent /*= 0*/, RWidgetType type /*= WidgetAllStock*/ )
	: CBaseWidget(parent,type)
	, m_iItemWidth(1)
	, m_iItemHeight(2)
	, m_crCircle(Min5)
	, m_stOrder(Qt::DescendingOrder)
{
	setMouseTracking(true);

	setSortType(SortByIncrease);
	updateStockDatas();
}

CAllStockWidget::~CAllStockWidget(void)
{
	clearStockDatas();
}

void CAllStockWidget::setCircle( int _cr )
{
	if(m_crCircle == _cr)
		return;

	clearStockDatas();
	m_crCircle = static_cast<RStockCircle>(_cr);
	updateStockDatas();
}

void CAllStockWidget::setSortType( RSortType _st )
{
	QList<CStockInfoItem*> list = CDataEngine::getDataEngine()->getStockInfoList();

	switch(_st)
	{
	case SortByZGB:
		{
			QMultiMap<float,CStockInfoItem*> mapSort;
			foreach(CStockInfoItem* pItem,list)
			{
				mapSort.insert(pItem->getZGB(),pItem);
			}
			list = mapSort.values();
		}
		break;
	case SortByCode:
		{
			QMultiMap<QString,CStockInfoItem*> mapSort;
			foreach(CStockInfoItem* pItem,list)
			{
				mapSort.insert(pItem->getCode(),pItem);
			}
			list = mapSort.values();
		}
		break;

	case SortByIncrease:		//按涨幅排序
		{
			QMultiMap<float,CStockInfoItem*> mapSort;
			foreach(CStockInfoItem* pItem,list)
			{
				mapSort.insert(pItem->getIncrease(),pItem);
			}
			list = mapSort.values();
		}
		break;
	case SortByTurnRatio:	//按换手率排序
		{
			QMultiMap<float,CStockInfoItem*> mapSort;
			foreach(CStockInfoItem* pItem,list)
			{
				mapSort.insert(pItem->getTurnRatio(),pItem);
			}
			list = mapSort.values();
		}
		break;
	case SortByVolumeRatio:	//按量比排序
		{
			QMultiMap<float,CStockInfoItem*> mapSort;
			foreach(CStockInfoItem* pItem,list)
			{
				mapSort.insert(pItem->getVolumeRatio(),pItem);
			}
			list = mapSort.values();
		}
		break;
	}

	if(m_stSort==_st)
	{
		m_stSort = _st;
		m_stOrder = (m_stOrder==Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
	}
	else
	{
		m_stSort = _st;
		m_stOrder = Qt::AscendingOrder;
	}

	if(m_stOrder == Qt::AscendingOrder)
	{
		m_listStocks = list;
	}
	else
	{
		m_listStocks.clear();
		for(int i=list.size()-1;i>=0;--i)
		{
			m_listStocks.append(list[i]);
		}
	}

	update();
}


void CAllStockWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	int iX = rtClient.left();
	int iY = rtClient.top();
	int m_iStockWidth = m_iItemWidth*48 + 4;
	int m_iStockHeight= m_iItemHeight + 1;

	p.fillRect(rtClient,QColor(0,0,0));
	p.setPen(QColor(255,0,0));
	for (int iXL = iX;iXL<rtClient.right();iXL=iXL+m_iStockWidth)
	{
		p.drawLine(iXL-2,iY,iXL-2,rtClient.bottom());
	}

	foreach(CStockInfoItem* pItem,m_listStocks)
	{
		if(iX+m_iStockWidth > rtClient.right())
		{
			//宽度越界则进行下一行绘制
			iX = rtClient.left();
			iY += m_iStockHeight;
		}

		//高度越界则停止绘制
		if(iY+m_iStockHeight > rtClient.bottom())
			break;

		drawStock(pItem,QRect(iX,iY,m_iStockWidth,m_iStockHeight),p);
		iX += m_iStockWidth;
	}

//	p.drawPixmap(m_iScaleX-30,m_iScaleY+20,m_pixScale);
}

void CAllStockWidget::drawStock( CStockInfoItem* pStock,const QRect& rtStock,QPainter& p )
{
//	p.drawRect(rtStock);
	if(!m_mapStockDatas.contains(pStock))
		return;

	float fLastClose = pStock->getLastClose();
	if(fLastClose<0.1)
		return;

	int iX = rtStock.right() - m_iItemWidth - 1;
	int iY = rtStock.top() + 1;

	QList<RStockData*>* pListDatas = m_mapStockDatas[pStock];

	int iIndex = pListDatas->size()-1;
	RStockData* pCurData = NULL;
	while(iIndex>=0)
	{
		if(iX>rtStock.left())
		{
			pCurData = (*pListDatas)[iIndex];
			if(pCurData)
			{
				int iColor = ((pCurData->fClose-fLastClose)/fLastClose)*100 + 10.5;
				if(iColor>20)
					iColor = 20;
				if(iColor<0)
					iColor = 0;

				p.fillRect(iX,iY,m_iItemWidth,m_iItemHeight,m_vColors[iColor]);
			}
			iX -= m_iItemWidth;
			--iIndex;
		}
		else
		{
			break;
		}
	}
}

void CAllStockWidget::mouseMoveEvent( QMouseEvent* e )
{
//	if(abs(QDateTime::currentDateTime().secsTo(m_tmLastUpdate))<3)
//		return;

//	m_iScaleX = e->x();
//	m_iScaleY = e->y();
	CStockInfoItem* pStock = hitTestStock(e->pos());
	if(pStock)
		emit stockFocus(pStock);

//	m_pixScale = QPixmap::grabWidget(this,m_iScaleX-30,m_iScaleY-5,60,10);
//	m_pixScale = m_pixScale.scaled(240,40,Qt::KeepAspectRatio);
//	update(m_iScaleX-150,m_iScaleY-10,300,40);
//	update();
//	m_tmLastUpdate = QDateTime::currentDateTime();
}


void CAllStockWidget::clearStockDatas()
{
	if(m_crCircle!=Min5)
	{
		QMap<CStockInfoItem*,QList<RStockData*>*>::iterator iter = m_mapStockDatas.begin();
		while(iter!=m_mapStockDatas.end())
		{
			foreach(RStockData* pData,(*iter.value()))
			{
				delete pData;
			}
			delete iter.value();
			++iter;
		}
		m_mapStockDatas.clear();
	}
	else
	{
		QMap<CStockInfoItem*,QList<RStockData*>*>::iterator iter = m_mapStockDatas.begin();
		while(iter!=m_mapStockDatas.end())
		{
			delete iter.value();
			++iter;
		}
		m_mapStockDatas.clear();
	}
}

void CAllStockWidget::updateStockDatas()
{
	if(m_crCircle == Min5)
	{
		foreach(CStockInfoItem* pItem,m_listStocks)
		{
			m_mapStockDatas.insert(pItem,new QList<RStockData*>(pItem->getToday5MinList()));
		}
	}
	else if(m_crCircle>=Day)
	{
		int iCount = 50;
		if(m_crCircle == Week)
			iCount = 50*7;
		else if(m_crCircle == Month)
			iCount = 50*31;

		QMap<time_t,int> mapTimes = CDataEngine::getHistoryTimeMap(m_crCircle,iCount);
		foreach(CStockInfoItem* pItem,m_listStocks)
		{
			QMap<time_t,RStockData*>* pMapDatas = CDataEngine::getColorBlockItems(mapTimes,pItem->getHistoryList());
			m_mapStockDatas.insert(pItem,new QList<RStockData*>(pMapDatas->values()));
			delete pMapDatas;
		}
	}
}

CStockInfoItem* CAllStockWidget::hitTestStock( const QPoint& ptPoint ) const
{
	int m_iStockWidth = m_iItemWidth*48 + 4;
	int m_iStockHeight= m_iItemHeight + 1;
	int iRowCount = this->rect().width()/m_iStockWidth;

	int iRow = ptPoint.y()/m_iStockHeight;
	int iCol = ptPoint.x()/m_iStockWidth;

	int iIndex = iRow*iRowCount+iCol;
	if(iIndex>=m_listStocks.size()||iIndex<0)
		return 0;

	return m_listStocks[iIndex];
}
