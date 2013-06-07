#include "StdAfx.h"
#include "BColorBlockWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"

#define	RCB_OFFSET_Y	2
#define RCB_OFFSET_LEFT	50

CBColorBlockWidget::CBColorBlockWidget(CBaseWidget* parent /*= 0*/ )
	: CBaseBlockWidget(parent,WidgetBColorBlock)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, showStockIndex(0)
	, m_pSelectedBlock(0)
	, m_pCurBlock(0)
{
	connect(&m_timerUpdateUI,SIGNAL(timeout()),this,SLOT(updateUI()));
	m_timerUpdateUI.start(3000);
}


CBColorBlockWidget::~CBColorBlockWidget(void)
{
	clearTmpData();
}

bool CBColorBlockWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseBlockWidget::loadPanelInfo(eleWidget))
		return false;

	//当前的板块名称
	QDomElement eleBlock = eleWidget.firstChildElement("block");
	if(eleBlock.isElement())
	{
		setBlock(eleBlock.text());
	}


	updateUI();
	return true;
}

bool CBColorBlockWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseBlockWidget::savePanelInfo(doc,eleWidget))
		return false;

	//当前的板块名称
	if(m_pCurBlock)
	{
		QDomElement eleBlock = doc.createElement("block");
		eleBlock.appendChild(doc.createTextNode(m_pCurBlock->getOnly()));
		eleWidget.appendChild(eleBlock);
	}

	return true;
}

void CBColorBlockWidget::clearTmpData()
{

}

void CBColorBlockWidget::updateSortMode( bool bSelFirst )
{
	if(bSelFirst)
	{
		m_pSelectedBlock = 0;
		showStockIndex = 0;
	}

	update();
	return;
}

void CBColorBlockWidget::getKeyWizData( const QString& keyword,QList<KeyWizData*>& listRet )
{

}

void CBColorBlockWidget::keyWizEntered( KeyWizData* pData )
{

}

void CBColorBlockWidget::setBlock( const QString& block )
{
	CBlockInfoItem* pBlock = CDataEngine::getDataEngine()->getStockBlock(block);
	if(pBlock)
	{
		QList<CBlockInfoItem*> list = pBlock->getBlockList();
		if(list.size()>0)
		{
			m_listBlocks = list;
			m_pCurBlock = pBlock;
			updateUI();
		}
	}
}

void CBColorBlockWidget::updateUI()
{
	m_typeCircle = Sec30;
	updateTimesH();
	updateSortMode(false);
}


void CBColorBlockWidget::clickedBlock( CBlockInfoItem* pItem )
{

}

void CBColorBlockWidget::paintEvent( QPaintEvent* e )
{
	m_clrTable.clear();
	for (int i=0;i<21;++i)
	{
		m_clrTable.push_back(CColorManager::getBlockColor(m_qsColorMode,i));
	}

	QPainter p(this);
	QRect rtClient = this->rect();
	m_rtHeader = QRect(rtClient.left(),rtClient.top(),rtClient.width(),m_iTitleHeight);
	m_rtClient = QRect(rtClient.left(),rtClient.top()+m_iTitleHeight,rtClient.width(),rtClient.height()-m_iTitleHeight-m_iBottomHeight);
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);

	updateShowTimes(QRect(m_rtBottom.left()+RCB_OFFSET_LEFT,m_rtBottom.top(),
		m_rtBottom.width()-RCB_OFFSET_Y-RCB_OFFSET_LEFT,m_rtBottom.height()),m_iCBWidth);

	drawHeader(p,m_rtHeader);
	drawClient(p,m_rtClient);
	drawBottom(p,m_rtBottom);
}

void CBColorBlockWidget::mouseMoveEvent( QMouseEvent* e )
{

}

void CBColorBlockWidget::mousePressEvent( QMouseEvent* e )
{

}

void CBColorBlockWidget::wheelEvent( QWheelEvent* e )
{

}

void CBColorBlockWidget::keyPressEvent( QKeyEvent* e )
{

}

void CBColorBlockWidget::drawColocBlock( QPainter& p,int iY, QVector<float>& vColor,QVector<float>& vHeight,QVector<float>& vWidth )
{

}

QMenu* CBColorBlockWidget::getCustomMenu()
{
	CBaseBlockWidget::getCustomMenu();

	return m_pMenuCustom;
}

void CBColorBlockWidget::drawHeader( QPainter& p,const QRect& rtHeader )
{
	p.fillRect(rtHeader,QColor(0,0,0));
	p.setPen(QColor(255,0,0));
	QRect rtCoord = rtHeader.adjusted(0,0,-1,-1);
	p.drawRect(rtCoord);

	p.setPen(QColor(255,255,255));
	if(!m_pCurBlock)
		return;

	QString qsText = m_pCurBlock->getBlockName();
	switch(m_sort)
	{
	case SortByCode:		//按股票代码排序
		qsText += " (代码)";
		break;
	case SortByIncrease:		//按涨幅排序
		qsText += " (涨幅)";
		break;
	case SortByTurnRatio:	//按换手率排序
		qsText += " (换手率)";
		break;
	case SortByVolumeRatio:	//按量比排序
		qsText += " (量比)";
		break;
	default:
		qsText += " (未排序)";
		break;
	}

	if(m_sortOrder == Qt::AscendingOrder)
	{
		p.drawText(rtHeader,Qt::AlignLeft|Qt::AlignVCenter,QString("↑%1").arg(qsText));
	}
	else
	{
		p.drawText(rtHeader,Qt::AlignLeft|Qt::AlignVCenter,QString("↓%1").arg(qsText));
	}
}

void CBColorBlockWidget::drawClient( QPainter& p,const QRect& rtClient )
{
	p.fillRect(rtClient,QColor(0,0,0));

	int iCurY = rtClient.top();
	int iIndex = showStockIndex;
	for(;iCurY<rtClient.bottom();iCurY=(iCurY+m_iCBHeight))
	{
		if(iIndex<0||iIndex>=m_listBlocks.size())
			break;

		drawBlock(p,QRect(rtClient.left(),iCurY,rtClient.width(),m_iCBHeight),m_listBlocks[iIndex]);
		++iIndex;
	}
}

void CBColorBlockWidget::drawBottom( QPainter& p,const QRect& rtBottom )
{
	p.fillRect(rtBottom,QColor(0,0,0));

	QRectF rtColors = QRectF(rtBottom.left(),rtBottom.top(),50,rtBottom.height());
	float fColorsWidth = rtColors.width()-5;
	float fColorWidth = fColorsWidth/COLOR_BLOCK_SIZE;
	for(int i=0;i<COLOR_BLOCK_SIZE;++i)
	{
		p.fillRect(QRectF(rtBottom.left()+i*fColorWidth,rtBottom.top(),fColorWidth,rtBottom.height()),
			QColor::fromRgb(m_clrTable[i]));
	}

	//从右向左绘制横坐标
	drawCoordX(p,QRect(rtBottom.left()+RCB_OFFSET_LEFT,rtBottom.top(),
		rtBottom.width()-RCB_OFFSET_Y-RCB_OFFSET_LEFT,rtBottom.height()),m_iCBWidth);
}

void CBColorBlockWidget::drawBlock( QPainter& p,const QRect& rtCB,CBlockInfoItem* pItem )
{
	if(pItem == m_pSelectedBlock)
	{
		p.fillRect(rtCB,QColor(50,50,50));
	}

	{
		//绘制左侧的标识信息（代码或者名称）
		p.setPen(QColor(255,255,255));
		QString qsText = pItem->getName();
		if(qsText.isEmpty())
			qsText = pItem->getCode();
		if(rtCB.height()>35)
		{
			//如果单个色块的高度足够的话，则显示两行
			if(m_sort == SortByIncrease)
				qsText += QString("\n%1%").arg(pItem->getIncrease(),0,'f',2);
			else if(m_sort == SortByTurnRatio)
				qsText += QString("\n%1%").arg(pItem->getTurnRatio(),0,'f',2);
			else if(m_sort == SortByVolumeRatio)
				qsText += QString("\n%1%").arg(pItem->getVolumeRatio(),0,'f',2);
		}
		p.drawText(QRect(rtCB.left(),rtCB.top(),RCB_OFFSET_LEFT,m_iCBHeight),Qt::AlignCenter,qsText);
	}

	//从右向左绘制横坐标
	float fBeginX = rtCB.right()-RCB_OFFSET_Y;
	float fEndX = rtCB.left()+RCB_OFFSET_LEFT;
	float fCBWidth = fBeginX-fEndX;
	if(fCBWidth<0)
		return;

	QList<qRcvFenBiData*> listFenBi = pItem->getFenBiList();
	int iSize = listFenBi.size();
	for (int i = iSize-1;i >= 0; --i)
	{
		qRcvFenBiData* pFenBi = listFenBi[i];
		QString qsTime = QDateTime::fromTime_t(pFenBi->tmTime).toString("hh:mm:ss");
		QMap<time_t,float>::iterator iter = m_mapShowTimes.lowerBound(pFenBi->tmTime);
		if(iter==m_mapShowTimes.end())
		{
			continue;
		}
		else if(iter!=m_mapShowTimes.begin())
		{
			if(iter.value()<rtCB.left())
				break;

			//色块的大小
			float fTop = rtCB.top();
			float fLeft = iter.value()-m_iCBWidth;
			float fHeight = rtCB.height()-1;

			float* fCount = &(pFenBi->fBuyPrice[0]);
			float fTotal = pItem->getStockCount();
			float fPer = fHeight/fTotal;
			float fCurY = fTop;

			float fNotEquel = 0;
			for (int i=0;i<20;++i)
			{
				fNotEquel+=fCount[i];
			}

			for (int j=0;j<10;++j)
			{
				//10-1
				float fPerH = fCount[j]*fPer;
				p.fillRect(QRectF(fLeft,fCurY,m_iCBWidth-1,fPerH),m_clrTable[20-j]);
				
				fCurY+=fPerH;
			}
			{
				//0
				float fPerH = (fTotal-fNotEquel)*fPer;
				p.fillRect(QRectF(fLeft,fCurY,m_iCBWidth-1,fPerH),m_clrTable[10]);

				fCurY+=fPerH;
			}

			for (int j=10;j<20;++j)
			{
				float fPerH = fCount[j]*fPer;
				p.fillRect(QRectF(fLeft,fCurY,m_iCBWidth-1,fPerH),m_clrTable[19-j]);

				fCurY+=fPerH;
			}

/*
			QRectF rtBlock = QRectF(iter.value()-m_iCBWidth,fTop,m_iCBWidth,rtCB.height());

			QString qsText = QString("%1 %2").arg(fTotal).arg(pItem->getStockList().size());
			p.drawText(rtBlock,QDateTime::fromTime_t(pFenBi->tmTime).toString("mm:ss"));*/

		}
		else
		{
			break;
		}
	}
	//绘制
	//drawColocBlock(p,rtCB.top(),_vColor,_vHeight,_vWidth);
}

QRect CBColorBlockWidget::rectOfBlock( CBlockInfoItem* pItem )
{
	return QRect();
}

CBlockInfoItem* CBColorBlockWidget::hitTestStock( const QPoint& ptPoint ) const
{

	return NULL;
}

RStockData* CBColorBlockWidget::hitTestCBItem( const QPoint& ptPoint ) const
{
	return NULL;
}
