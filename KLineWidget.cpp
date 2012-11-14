#include "StdAfx.h"
#include "KLineWidget.h"
#include "DataEngine.h"

#define	KLINE_BORDER	2

CKLineWidget::CKLineWidget( QWidget* parent /*= 0*/ )
	: QWidget(parent)
	, m_pStockItem(0)
	, fKGridWidth(0)
{
	setMinimumSize(200,200);
//	setToolTip("adsffff");
	setMouseTracking(true);
}

CKLineWidget::~CKLineWidget(void)
{
	clearTmpData();
}

void CKLineWidget::setStockCode( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);
	if(pItem)
	{
		disconnect(this,SLOT(updateKLine(const QString&)));		//移除所有和 updateKLine关联的 信号/槽

		m_pStockItem = pItem;

		//建立更新机制
		connect(pItem,SIGNAL(stockInfoItemChanged(const QString&)),this,SLOT(updateKLine(const QString&)));

		//更新K线图
		updateKLine(code);
	}
}

void CKLineWidget::updateKLine( const QString& code )
{
	if(m_pStockItem&&m_pStockItem->getCode()!=code)
		return;

	resetTmpData();

	//更新K线图
	update();
}

void CKLineWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRectF rtClient = this->rect();
	p.fillRect(rtClient,QColor(0,0,0));
	if(!m_pStockItem)
		return;

	rtClient.adjust(KLINE_BORDER,KLINE_BORDER,-50,-15);

	/*画坐标轴*/
	drawCoordY(p,rtClient);
	drawKGrids(p,rtClient);
}

void CKLineWidget::mouseMoveEvent( QMouseEvent* e )
{
	if(!m_pStockItem)
	{
		QToolTip::hideText();
		return;
	}
	QRectF rtClient = this->rect();
	rtClient.adjust(KLINE_BORDER,KLINE_BORDER,-50,-15);

	float fBegin = rtClient.left();

	int iIndex = (e->posF().x() - fBegin)/fKGridWidth;
	if(iIndex>=0&&iIndex<listHistory.size())
	{
		qRcvHistoryData* pHistory = listHistory[iIndex];
		QString qsTooltip = QString("股票代码:%1\r\n时间:%2\r\n最高价:%3\r\n最低价:%4\r\n开盘价:%5\r\n收盘价:%6\r\n成交量:%7\r\n成交额:%8")
			.arg(m_pStockItem->getCode()).arg(QDateTime::fromTime_t(pHistory->time).toString("yyyy/MM/dd"))
			.arg(pHistory->fHigh).arg(pHistory->fLow).arg(pHistory->fOpen).arg(pHistory->fClose)
			.arg(pHistory->fVolume).arg(pHistory->fAmount);

		QToolTip::showText(e->globalPos(),qsTooltip,this);
	}
	else
	{
		QToolTip::hideText();
	}
}

void CKLineWidget::drawCoordY( QPainter& p,const QRectF& rtClient )
{
	//设置画笔颜色
	p.setPen(QColor(255,0,0));

	//Y坐标（价格）
	p.drawLine(rtClient.topRight(),rtClient.bottomRight());			//主线

	int iBeginPrice = fMinPrice*10;
	int iEndPrice = fMaxPrice*10;
	float fGridHeight = rtClient.height()/(iEndPrice-iBeginPrice);
	int iGridSize = 1;
	while((fGridHeight*iGridSize)<10)
		++iGridSize;
	fGridHeight = fGridHeight*iGridSize;

	float fY = rtClient.bottom();
	float fX = rtClient.right();

	int iGridCount = 0;
	for (int i=(iBeginPrice+iGridSize); i<iEndPrice; i=i+iGridSize)
	{
		fY = fY-fGridHeight;
		if(iGridCount%10 == 0)
		{
			p.drawLine(fX,fY,fX+4,fY);
			p.setPen(QColor(0,255,255));
			p.drawText(fX+7,fY+4,QString("%1").arg(float(float(i)/10),0,'f',2));
			p.setPen(QColor(255,0,0));
		}
		else if(iGridCount%5 == 0)
			p.drawLine(fX,fY,fX+4,fY);
		else
			p.drawLine(fX,fY,fX+2,fY);
		++iGridCount;
	}
}

void CKLineWidget::drawKGrids( QPainter& p,const QRectF& rtClient )
{
	p.setLayoutDirection(Qt::LeftToRight);
	//设置画笔颜色
	p.setPen(QColor(255,0,0));

	/*X坐标（时间）*/
	p.drawLine(rtClient.bottomLeft(),rtClient.bottomRight());		//线

	int iCount = listHistory.size();				//总个数
	fKGridWidth = rtClient.width()/iCount;			//单列宽度

	int iIndex = 0;
	int iMonth = 0;
	int iYear = 0;
	foreach(qRcvHistoryData* pHistory, listHistory)
	{
		p.setPen(QColor(255,0,0));
		float fBeginX=rtClient.left() + fKGridWidth*iIndex;
		QDateTime date = QDateTime::fromTime_t(pHistory->time);
		if(date.date().month()!=iMonth)
		{
			float fX = fBeginX+(fKGridWidth/2);
			p.drawLine(fX,rtClient.bottom(),fX,rtClient.bottom()+2);
			if(date.date().year()!=iYear)
			{
				iYear = date.date().year();
				iMonth = date.date().month();
				p.drawText(fBeginX,rtClient.bottom()+2,40,50,Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(iYear));
			}
			else
			{
				iMonth = date.date().month();
				p.drawText(fBeginX,rtClient.bottom()+2,40,50,Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(iMonth));
			}
		}

		drawKGrid(pHistory,p,QRectF(fBeginX,rtClient.top(),fKGridWidth,rtClient.bottom()));
		++iIndex;
	}
}

void CKLineWidget::drawKGrid( qRcvHistoryData* pHistory,QPainter& p,const QRectF& rtClient )
{
	float fHighMax = fMaxPrice-fMinPrice;
	float fHighY = ((pHistory->fHigh-fMinPrice)/fHighMax)*rtClient.height();
	float fLowY = ((pHistory->fLow-fMinPrice)/fHighMax)*rtClient.height();
	float fOpenY = ((pHistory->fOpen-fMinPrice)/fHighMax)*rtClient.height();
	float fCloseY = ((pHistory->fClose-fMinPrice)/fHighMax)*rtClient.height();

	if(pHistory->fClose>pHistory->fOpen)
	{
		//增长，绘制红色色块
		p.setPen(QColor(255,0,0));
		if(int(rtClient.width())%2==0)
		{
			QRectF rt = QRectF(rtClient.left()+0.5,rtClient.bottom()-fCloseY,rtClient.width()-1.0,fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
			p.fillRect(rt,QColor(255,0,0));
		}
		else
		{
			QRectF rt = QRectF(rtClient.left(),rtClient.bottom()-fCloseY,rtClient.width(),fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
			p.fillRect(rt,QColor(255,0,0));
		}
	}
	else
	{
		//降低，绘制蓝色色块
		p.setPen(QColor(0,255,255));
		if(int(rtClient.width())%2==0)
		{
			QRectF rt = QRectF(rtClient.left()+0.5,rtClient.bottom()-fOpenY,rtClient.width()-1.0,fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
			p.fillRect(rt,QColor(0,255,255));
		}
		else
		{
			QRectF rt = QRectF(rtClient.left(),rtClient.bottom()-fOpenY,rtClient.width(),fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
			p.fillRect(rt,QColor(0,255,255));
		}
	}

	p.drawLine(rtClient.center().x(),rtClient.bottom()-fHighY,rtClient.center().x(),rtClient.bottom()-fLowY);		//画最高价到最低价的线
}

void CKLineWidget::clearTmpData()
{
	foreach(qRcvHistoryData* p,listHistory)
		delete p;
	listHistory.clear();
}

void CKLineWidget::resetTmpData()
{
	clearTmpData();
	listHistory = m_pStockItem->getLastHistory(100);
	//初始时间和结束时间
	tmBegin = listHistory.first()->time;
	tmEnd = listHistory.last()->time;

	//计算最高价和最低价
	float fMin=FLT_MAX;
	float fMax=0;
	foreach(qRcvHistoryData* p,listHistory)
	{ 
		if(p->fLow<fMin&&p->fLow>0){fMin = p->fLow;}
		if(p->fHigh>fMax){fMax = p->fHigh;}
	}
	if(fMin<fMax)
	{
		fMaxPrice = float(int(fMax*100)/10)/float(10) + 0.1;
		fMinPrice = float(int(fMin*100)/10)/float(10);
	}
}
