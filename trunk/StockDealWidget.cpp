#include "StdAfx.h"
#include "StockDealWidget.h"
#include "DataEngine.h"

bool getDealKind(QList<qRcvFenBiData*> listFenBi, qRcvFenBiData* pLastFenBi,float* fKind)
{
	foreach(qRcvFenBiData* pFenBi,listFenBi)
	{
		float fVOL = 0.0;
		if(pLastFenBi)
		{
			fVOL = pFenBi->fVolume - pLastFenBi->fVolume;
		}
		else
		{
			fVOL = pFenBi->fVolume;
		}

		//判断买入还是卖出
		if(pFenBi->fPrice>pFenBi->fBuyPrice[0])
		{
			fKind[0] += fVOL;
		}
		else
		{
			fKind[1] += fVOL;
		}


	}

	return true;
}


CStockDealWidget::CStockDealWidget( CBaseWidget* parent /*= 0*/ )
	: CCoordXBaseWidget(parent,WidgetStockDeal)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, m_pCurStock(0)
	, m_iItemWidth(50)
	, m_iItemHeight(50)
	, m_typeWidget(DealKind)
{

}


CStockDealWidget::~CStockDealWidget(void)
{
}

void CStockDealWidget::setStockCode( const QString& code )
{
	m_pCurStock = CDataEngine::getDataEngine()->getStockInfoItem(code);
	updateData();
}

void CStockDealWidget::updateData()
{
	//更新当前的横坐标数据，从后向前计算时间
	m_mapTimes = CDataEngine::getTodayTimeMap(Min5);
	if(m_pCurStock)
	{
		m_listFenbi = m_pCurStock->getFenBiList();
	}
	update();
}

void CStockDealWidget::paintEvent( QPaintEvent* /*e*/ )
{
	QPainter p(this);
	QRect rtClient = this->rect();

//	p.setPen(QColor(127,0,0));
//	p.drawRect(rtClient);


	m_rtTitle = QRect(rtClient.left(),rtClient.top(),rtClient.width(),m_iTitleHeight);
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);
	m_rtClient = QRect(rtClient.left(),rtClient.top()+m_iTitleHeight,rtClient.width(),rtClient.height()-m_iBottomHeight-m_iTitleHeight);
	drawTitle(p);
	drawBottom(p);
	drawClient(p);
}

void CStockDealWidget::drawTitle( QPainter& p )
{
	QRect rtTitle = m_rtTitle.adjusted(1,1,-1,-1);
	p.setPen(QColor(255,0,0));
	p.drawRect(rtTitle);
	if(m_pCurStock)
	{
		QString qsText = m_pCurStock->getName();
		if(qsText.isEmpty())
			qsText = m_pCurStock->getCode();
		p.setPen(QColor(255,255,255));
		p.drawText(rtTitle,qsText);
	}
}

void CStockDealWidget::drawClient( QPainter& p )
{
	if(!m_pCurStock)
		return;

	int iBeginX = m_rtClient.right();
	int iEndX = m_rtClient.left();
	int iBeginY = m_rtClient.top();

	qRcvFenBiData* pLastFenBi = 0;
	int iLastIndex = -1;
	QList<qRcvFenBiData*> listFenBi;
	for (int i=m_listFenbi.size()-1;i>=0;--i)
	{
		qRcvFenBiData* pFenBi = m_listFenbi[i];
		QMap<time_t,int>::const_iterator iter = m_mapTimes.lowerBound(pFenBi->tmTime);
		if(iter==m_mapTimes.end())
			continue;

		if(iLastIndex>=0)
		{
			if(iLastIndex == iter.value())
			{
				listFenBi.push_front(pFenBi);
			}
			else
			{
				QRect rtBlock = QRect(iBeginX-(iLastIndex+2)*m_iItemWidth,iBeginY,m_iItemWidth,m_iItemHeight);
				//do something;
				switch(m_typeWidget)
				{
				case DealKind:
					{
						float fDealKind[7];
						memset(&fDealKind[0],0,sizeof(float)*6);
						getDealKind(listFenBi,pFenBi,&fDealKind[0]);

					}
					break;
				}


				listFenBi.clear();
				pLastFenBi = pFenBi;
				iLastIndex = iter.value();
				listFenBi.push_front(pFenBi);
			}
		}
		else
		{
			iLastIndex = iter.value();
			listFenBi.push_front(pFenBi);
		}
	}
}

void CStockDealWidget::drawBottom( QPainter& p )
{
	//从右向左绘制横坐标
	int iBeginX = m_rtBottom.right();
	int iEndX = m_rtBottom.left();
	if((iBeginX-iEndX)<0)
		return;

	QList<time_t> listTimes = m_mapTimes.keys();
	int iCurX = iBeginX-m_iItemWidth;
	int iLastX = iBeginX;
	int iCount = listTimes.size()-1;
	if(iCount<0)
		return;

	int iTimeCount = 0;				//只是用来区分时间的颜色（隔开颜色，便于查看）

	while(iCurX>iEndX && iCount>=0)
	{
		time_t tmTime = listTimes[iCount];
		if((iLastX-iCurX)>30)
		{
			p.setPen( iTimeCount%2 ? QColor(255,0,0) : QColor(0,255,255));
			p.drawLine(iCurX,m_rtBottom.top(),iCurX,m_rtBottom.top()+2);
			p.drawText(iCurX-14,m_rtBottom.top()+2,30,m_rtBottom.height()-2,
				Qt::AlignCenter,QDateTime::fromTime_t(tmTime).toString("hh:mm"));

			iLastX = iCurX;
			++iTimeCount;
		}

		--iCount;
		iCurX = iCurX- m_iItemWidth;
	}
	return;
}
