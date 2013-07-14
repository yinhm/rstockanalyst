#include "StdAfx.h"
#include "AllStockWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"


CAllStockWidget::CAllStockWidget( CBaseWidget* parent /*= 0*/, RWidgetType type /*= WidgetAllStock*/ )
	: CBaseWidget(parent,type)
	, m_iItemWidth(1)
	, m_iItemHeight(1)
{
	QList<CStockInfoItem*> list = CDataEngine::getDataEngine()->getStockInfoList();

	QMultiMap<float,CStockInfoItem*> mapSort;
	foreach(CStockInfoItem* pItem,list)
	{
		if(pItem->getCurrentReport()->tmTime>0)
		{
//			mapSort.insert(pItem->getIncrease(),pItem);
			mapSort.insert(pItem->getZGB(),pItem);
		}
	}
	m_listStocks = mapSort.values();

	CColorManager::getBlockColor("红绿",m_vColors);
}

CAllStockWidget::~CAllStockWidget(void)
{
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
}

void CAllStockWidget::drawStock( CStockInfoItem* pStock,const QRect& rtStock,QPainter& p )
{
//	p.drawRect(rtStock);

	float fLastClose = pStock->getLastClose();
	if(fLastClose<0.1)
		return;
	time_t tmToday = (pStock->getCurrentReport()->tmTime/(3600*24))*3600*24;

	int iX = rtStock.right() - m_iItemWidth - 1;
	int iY = rtStock.top() + 1;

	QList<RStockData*> listDatas = pStock->get5MinList();
	int iIndex = listDatas.size()-1;
	RStockData* pLastData = NULL;
	RStockData* pCurData = NULL;
	while(iIndex>0)
	{
		pLastData = listDatas[iIndex-1];
		pCurData = listDatas[iIndex];
		if(pLastData->tmTime<tmToday)
			break;

		int iColor = ((pCurData->fClose-fLastClose)/fLastClose)*100 + 10.5;
		if(iColor>20)
			iColor = 20;
		if(iColor<0)
			iColor = 0;

		p.fillRect(iX,iY,m_iItemWidth,m_iItemHeight,m_vColors[iColor]);
		iX -= m_iItemWidth;
		--iIndex;
	}
	if(pCurData)
	{
		int iIndex = ((pCurData->fClose-fLastClose)/fLastClose)*100 + 10.5;
		if(iIndex>20)
			iIndex = 20;
		if(iIndex<0)
			iIndex = 0;

		p.fillRect(iX,iY,m_iItemWidth,m_iItemHeight,m_vColors[iIndex]);
	}
}
