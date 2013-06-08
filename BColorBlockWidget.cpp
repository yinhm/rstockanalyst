#include "StdAfx.h"
#include "BColorBlockWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "KeyWizard.h"
#include "MainWindow.h"

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
	{
		//清理缓存表中的内存
		QMap<CBlockInfoItem*,QMap<time_t,qRcvFenBiData*>*>::iterator iter = mapBlockColorBlocks.begin();
		while(iter != mapBlockColorBlocks.end())
		{
			delete iter.value();
			++iter;
		}
	}

	m_listBlocks.clear();
	m_mapBlockIndex.clear();
	mapBlockColorBlocks.clear();
	m_pSelectedBlock = 0;
	m_pCurBlock = 0;
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
	foreach(CBlockInfoItem* pItem,m_listBlocks)
	{
		if(pItem->isMatch(keyword))
		{
			KeyWizData* pData = new KeyWizData;
			pData->cmd = CKeyWizard::CmdBlock;
			pData->arg = pItem;
			pData->desc = QString("%1 %2").arg(pItem->getCode()).arg(pItem->getName());
			listRet.push_back(pData);
			if(listRet.size()>20)
				return;
		}
	}

	return CBaseBlockWidget::getKeyWizData(keyword,listRet);
}

void CBColorBlockWidget::keyWizEntered( KeyWizData* pData )
{
	if(pData->cmd == CKeyWizard::CmdBlock)
	{
		clickedBlock(reinterpret_cast<CBlockInfoItem*>(pData->arg));
		return;
	}

	return CBaseBlockWidget::keyWizEntered(pData);
}

void CBColorBlockWidget::setBlock( const QString& block )
{
	CBlockInfoItem* pBlock = CDataEngine::getDataEngine()->getStockBlock(block);
	if(pBlock)
	{
		QList<CBlockInfoItem*> list = pBlock->getBlockList();
		if(list.size()>0)
		{
			clearTmpData();
			m_listBlocks = list;
			for (int i=0;i<m_listBlocks.size();++i)
			{
				CBlockInfoItem* pItem = m_listBlocks[i];
				m_mapBlockIndex[pItem] = i;

				QMap<time_t,qRcvFenBiData*>* pMap = new QMap<time_t,qRcvFenBiData*>;
				foreach(qRcvFenBiData* pFenBi,pItem->getFenBiList())
				{
					pMap->insert(pFenBi->tmTime,pFenBi);
				}
				mapBlockColorBlocks[pItem] = pMap;

			}
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
	if(pItem == m_pSelectedBlock)
		return;
	int iShowCount = m_rtClient.height()/m_iCBHeight;
	int iRow = m_mapBlockIndex[pItem];
	if((iRow<showStockIndex)||(iRow>showStockIndex+iShowCount))
	{
		showStockIndex = iRow;
		update(m_rtClient);
	}

	CBlockInfoItem* pPreSelectedBlock = m_pSelectedBlock;
	m_pSelectedBlock = pItem;
	update(rectOfBlock(pPreSelectedBlock));
	update(rectOfBlock(m_pSelectedBlock));
	if(m_pSelectedBlock)
	{
		CMainWindow::getMainWindow()->clickedBlock(m_pSelectedBlock->getOnly());
	}
}

void CBColorBlockWidget::paintEvent( QPaintEvent* /*e*/ )
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
	CBlockInfoItem* pBlock = hitTestBlock(e->pos());
	qRcvFenBiData* item = hitTestCBItem(e->pos());
	if(item == NULL || pBlock==0)
	{
		QToolTip::hideText();
		return CBaseWidget::mouseMoveEvent(e);
	}

	QString qsTooltip;		//Tips
	QString qsTime;
	if(m_typeCircle<Day)
	{
		qsTime = QDateTime::fromTime_t(item->tmTime).toString("hh:mm:ss");
	}
	else
	{
		QDate dtTmp = QDateTime::fromTime_t(item->tmTime).date();
		if(m_typeCircle == Week)
			qsTime = QString("%1 %2").arg(dtTmp.year()).arg(dtTmp.weekNumber());
		else if(m_typeCircle == Month)
			qsTime = dtTmp.toString("yyyy/MM");
		else if(m_typeCircle == Month3)
			qsTime = dtTmp.toString("yyyy/MM");
		else if(m_typeCircle == Year)
			qsTime = dtTmp.toString("yyyy");
		else
			qsTime = dtTmp.toString("yyyy/MM/dd");
	}

	qsTooltip = QString("板块名称:%1\r\n时间:%2\r\n当前价:%3\r\n成交量:%4\r\n成交额:%5\r\n涨跌数:")
		.arg(pBlock->getName()).arg(qsTime).arg(item->fPrice).arg(item->fVolume).arg(item->fAmount);

	float fTotal = 0.0;
	for(int i=0;i<10;++i)
	{
		fTotal+=item->fBuyPrice[i];
		qsTooltip += QString("(%1 %2)-").arg(10-i).arg((int)(item->fBuyPrice[i]+0.5));
	}

	for (int i=10;i<20;++i)
	{
		fTotal+=item->fBuyPrice[i];
		qsTooltip += QString("(%1 %2)-").arg(9-i).arg((int)(item->fBuyPrice[i]+0.5));
	}

	{
		qsTooltip += QString("(0 %1)").arg(pBlock->getStockCount()-(int)(fTotal+0.5));
	}

	QToolTip::showText(e->globalPos(),qsTooltip,this);
	return CBaseBlockWidget::mouseMoveEvent(e);
}

void CBColorBlockWidget::mousePressEvent( QMouseEvent* e )
{
	e->accept();
	QPoint ptCur = e->pos();
	if(m_rtHeader.contains(ptCur))
	{
		m_sortOrder = (m_sortOrder==Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
		updateSortMode(true);
	}
	else if(m_rtClient.contains(ptCur))
	{
		int iCurIndex = showStockIndex+(ptCur.y()-m_rtClient.top())/m_iCBHeight;
		if(iCurIndex>=0&&iCurIndex<m_listBlocks.size())
		{
			clickedBlock(m_listBlocks[iCurIndex]);
		}
	}
	else if(m_rtBottom.contains(ptCur))
	{

	}

	return CBaseBlockWidget::mousePressEvent(e);
}

void CBColorBlockWidget::wheelEvent( QWheelEvent* e )
{
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;
	int iIndex = showStockIndex-numSteps*5;
	if(iIndex<0) {iIndex = 0;}
	if(iIndex>=0&&iIndex<m_listBlocks.size())
	{
		e->accept();
		showStockIndex = iIndex;
		updateUI();
	}
	return CBaseWidget::wheelEvent(e);
}

void CBColorBlockWidget::keyPressEvent( QKeyEvent* e )
{
	int iKey = e->key();
	if(Qt::Key_Left == iKey)
	{
		e->accept();
		return;
	}
	else if(Qt::Key_Right == iKey)
	{
		e->accept();
		return;
	}
	else if(Qt::Key_Down == iKey)
	{
		int iCurIndex = m_mapBlockIndex[m_pSelectedBlock];
		if(m_listBlocks.size()>(iCurIndex+1))
		{
			CBlockInfoItem* pItem = m_listBlocks[iCurIndex+1];
			int iRow = m_mapBlockIndex[pItem];
			int iShowCount = m_rtClient.height()/m_iCBHeight;
			if(iShowCount<1)
				return;
			if((iRow-showStockIndex)>=iShowCount)
			{
				showStockIndex = iRow-iShowCount+1;
				updateUI();
			}
			clickedBlock(m_listBlocks[iCurIndex+1]);
		}
		e->accept();
		return;
	}
	else if(Qt::Key_Up == iKey)
	{
		int iCurIndex = m_mapBlockIndex[m_pSelectedBlock];
		if(iCurIndex>0)
		{
			CBlockInfoItem* pItem = m_listBlocks[iCurIndex-1];
			int iRow = m_mapBlockIndex[pItem];
			if(iRow<showStockIndex)
			{
				showStockIndex = iRow;
				updateUI();
			}
			clickedBlock(pItem);
		}
		e->accept();
		return;
	}
	else if(Qt::Key_PageDown == iKey)
	{
		int iShowCount = m_rtClient.height()/m_iCBHeight;
		if(iShowCount<1)
			return;
		if((showStockIndex+iShowCount)<m_listBlocks.size())
		{
			showStockIndex = showStockIndex+iShowCount;
			updateUI();
		}
		e->accept();
		return;
	}
	else if(Qt::Key_PageUp == iKey)
	{
		int iShowCount = m_rtClient.height()/m_iCBHeight;
		if(iShowCount<1)
			return;
		showStockIndex = (showStockIndex-iShowCount)>0 ? (showStockIndex-iShowCount) : 0;
		updateUI();
		e->accept();
		return;
	}
	else if(Qt::Key_F10 == iKey)
	{
		//F10数据
		if(m_pSelectedBlock)
		{
			if(!CDataEngine::getDataEngine()->showF10Data(m_pSelectedBlock->getCode()))
			{
				//未打开F10数据 do something
			}
		}
	}

	return CBaseBlockWidget::keyPressEvent(e);
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

	QMap<time_t,qRcvFenBiData*>* pMap = mapBlockColorBlocks[pItem];
	if(pMap)
	{
		QMap<time_t,qRcvFenBiData*>::iterator iterFenBi = pMap->end();
		do
		{
			--iterFenBi;
			qRcvFenBiData* pFenBi = iterFenBi.value();
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
		while(iterFenBi!=pMap->begin());
	}
	//绘制
	//drawColocBlock(p,rtCB.top(),_vColor,_vHeight,_vWidth);
}

QRect CBColorBlockWidget::rectOfBlock( CBlockInfoItem* pItem )
{
	if(m_mapBlockIndex.contains(pItem))
	{
		int iRow = m_mapBlockIndex[pItem];
		return QRect(m_rtClient.left(),(m_rtClient.top()+(iRow-showStockIndex)*m_iCBHeight),m_rtClient.width(),m_iCBHeight);
	}

	return QRect();
}

CBlockInfoItem* CBColorBlockWidget::hitTestBlock( const QPoint& ptPoint ) const
{
	int iRow = (ptPoint.y()-m_rtClient.top())/m_iCBHeight + showStockIndex;
	if(iRow<0||iRow>=m_listBlocks.size())
		return 0;

	return m_listBlocks[iRow];
}

qRcvFenBiData* CBColorBlockWidget::hitTestCBItem( const QPoint& ptPoint ) const
{
	CBlockInfoItem* pItem = hitTestBlock(ptPoint);

	qRcvFenBiData* pData = NULL;
	if(pItem && mapBlockColorBlocks.contains(pItem))
	{
		QMap<time_t,qRcvFenBiData*>* pMap = mapBlockColorBlocks[pItem];
		int iIndex = (m_rtClient.right() - ptPoint.x())/m_iCBWidth;
		QMap<time_t,int>::iterator iter = m_mapTimes.end();
		while(iter!=m_mapTimes.begin())
		{
			--iter;

			if(iIndex==iter.value())
			{
				QMap<time_t,qRcvFenBiData*>::iterator iterFenBi = pMap->lowerBound(iter.key());
				if(iterFenBi!=pMap->end())
				{
					pData = iterFenBi.value();
					break;
				}
			}
			else if(iIndex<iter.value())
				break;
		}
	}
	return pData;
}
