#include "StdAfx.h"
#include "ColorBlockWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "BlockInfoItem.h"
#include "RLuaEx.h"

#define	RCB_OFFSET_Y	2
#define RCB_OFFSET_LEFT	50

void FreeRStockInfoMap(QMap<time_t,RStockData*>* pMap)
{
	QMap<time_t,RStockData*>::iterator iter = pMap->begin();
	QMap<time_t,RStockData*>::iterator iterEnd = pMap->end();
	while(iter!=iterEnd)
	{
		if((*iter)!=NULL)
		{
			delete (*iter);
		}
		++iter;
	}
}


CColorBlockWidget::CColorBlockWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseBlockWidget(parent,WidgetSColorBlock)
	, m_pMenuBlockList(0)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, showStockIndex(0)
	, m_pSelectedStock(0)
	, m_pBlock(0)
{
	//设置当前显示的板块
	m_pMenuBlockList = m_pMenuCustom->addMenu(tr("设置当前板块"));
}

CColorBlockWidget::~CColorBlockWidget(void)
{
	clearTmpData();
}

bool CColorBlockWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseBlockWidget::loadPanelInfo(eleWidget))
		return false;

	//当前的板块名称
	QDomElement eleBlock = eleWidget.firstChildElement("block");
	if(eleBlock.isElement())
	{
		setBlock(eleBlock.text());
	}

	return true;
}

bool CColorBlockWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseBlockWidget::savePanelInfo(doc,eleWidget))
		return false;

	//当前的板块名称
	QDomElement eleBlock = doc.createElement("block");
	eleBlock.appendChild(doc.createTextNode(m_pBlock->getBlockName()));
	eleWidget.appendChild(eleBlock);

	return true;
}

void CColorBlockWidget::updateData()
{
	setBlock(m_pBlock->getBlockName());
	return CBaseBlockWidget::updateData();
}

void CColorBlockWidget::setBlock( const QString& block )
{
	clearTmpData();
	CBlockInfoItem* pBlock = CDataEngine::getDataEngine()->getStockBlock(block);
	if(!pBlock)
		return;

	m_listStocks = pBlock->getStockList();
	showStockIndex = 0;
	for(int i=0;i<m_listStocks.size();++i)
	{
		CStockInfoItem* pItem = m_listStocks[i];

		m_mapStockIndex[pItem] = i;
		//建立更新机制
		connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateStock(const QString&)));
		connect(pItem,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateStock(const QString&)));
	}

	m_pBlock = pBlock;

	if(m_listStocks.size()>0)
	{
		clickedStock(m_listStocks.first());
	}

	updateTimesH();			//更新横坐标的时间数据
	update();

	return CBaseWidget::setBlock(block);
}

void CColorBlockWidget::updateStock( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);

	if(mapStockColorBlocks.contains(pItem))
	{
		//更新数据
		QMap<time_t,RStockData*>* pMap = mapStockColorBlocks[pItem];
		mapStockColorBlocks[pItem] = 0;
		{
			//删除之前的资源
			FreeRStockInfoMap(pMap);
			delete pMap;
		}
		mapStockColorBlocks[pItem] = getColorBlockMap(pItem);
		updateTimesH();
	}
	update(rectOfStock(pItem));
}


void CColorBlockWidget::onSetCurrentBlock()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	setBlock(pAct->data().toString());
}

void CColorBlockWidget::updateColorBlockData()
{
	QList<CStockInfoItem*> listShowItems;
	int iClientHeight = this->rect().height();

	//获取当前需要显示的股票列表
	int iIndex = showStockIndex;
	while (iIndex<m_listStocks.size())
	{
		if((iIndex-showStockIndex)*m_iCBHeight<iClientHeight)
		{
			listShowItems.push_back(m_listStocks[iIndex]);
		}
		else
		{
			break;
		}
		++iIndex;
	}

	/*
	//从map里删除不需要显示的股票
	QMap<CStockInfoItem*,QMap<time_t,stColorBlockItem>*>::iterator iter = mapStockColorBlocks.begin();
	while (iter!=mapStockColorBlocks.end())
	{
		if(!listShowItems.contains(iter.key()))
		{
			delete iter.value();
			mapStockColorBlocks.remove(iter.key());
		}

		++iter;
	}*/


	
	//将需要显示而map中没有的股票加入到map中
	foreach(CStockInfoItem* p,listShowItems)
	{
		if(!mapStockColorBlocks.contains(p))
		{
			mapStockColorBlocks[p] = getColorBlockMap(p);
		}
	}
}

void CColorBlockWidget::clearTmpData()
{
	foreach(CStockInfoItem* p,m_listStocks)
	{
		//移除所有和 updateStock关联的 信号/槽
		disconnect(p,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateStock(const QString&)));
		disconnect(p,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateStock(const QString&)));
	}
	m_pSelectedStock = 0;
	m_listStocks.clear();

	QMap<CStockInfoItem*,QMap<time_t,RStockData*>*>::iterator iter = mapStockColorBlocks.begin();
	while(iter!=mapStockColorBlocks.end())
	{
		FreeRStockInfoMap(*iter);
		delete iter.value();
		++iter;
	}

	mapStockColorBlocks.clear();
	m_mapStockIndex.clear();
}

void CColorBlockWidget::clickedStock( CStockInfoItem* pItem )
{
	if(pItem == m_pSelectedStock)
		return;
	int iShowCount = m_rtClient.height()/m_iCBHeight;
	int iRow = m_mapStockIndex[pItem];
	if((iRow<showStockIndex)||(iRow>showStockIndex+iShowCount))
	{
		showStockIndex = iRow;
		update(m_rtClient);
	}

	CStockInfoItem* pPreSelectedStock = m_pSelectedStock;
	m_pSelectedStock = pItem;
	update(rectOfStock(pPreSelectedStock));
	update(rectOfStock(m_pSelectedStock));
}

void CColorBlockWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	m_rtHeader = QRect(rtClient.left(),rtClient.top(),rtClient.width(),m_iTitleHeight);
	m_rtClient = QRect(rtClient.left(),rtClient.top()+m_iTitleHeight,rtClient.width(),rtClient.height()-m_iTitleHeight-m_iBottomHeight);
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);

	drawHeader(p,m_rtHeader);
	drawBottom(p,m_rtBottom);
	drawClient(p,m_rtClient);
}

//绘制头部信息
void CColorBlockWidget::drawHeader( QPainter& p,const QRect& rtHeader )
{
	p.fillRect(rtHeader,QColor(0,0,0));
	p.setPen(QColor(255,0,0));
	QRect rtCoord = rtHeader.adjusted(0,0,-1,-1);
	p.drawRect(rtCoord);

	p.setPen(QColor(255,255,255));
	if(!m_pBlock)
		return;
	p.drawText(rtHeader,Qt::AlignLeft|Qt::AlignVCenter,m_pBlock->getBlockName());
}

void CColorBlockWidget::drawBottom( QPainter& p,const QRect& rtBottom )
{
	p.fillRect(rtBottom,QColor(0,0,0));

	QRectF rtColors = QRectF(rtBottom.left(),rtBottom.top(),50,rtBottom.height());
	float fColorsWidth = rtColors.width()-5;
	float fColorWidth = fColorsWidth/COLOR_BLOCK_SIZE;
	for(int i=0;i<COLOR_BLOCK_SIZE;++i)
	{
		p.fillRect(QRectF(rtBottom.left()+i*fColorWidth,rtBottom.top(),fColorWidth,rtBottom.height()),
			CColorManager::getBlockColor(m_qsColorMode,i));
	}

	//从右向左绘制横坐标
	drawCoordX(p,QRect(rtBottom.left()+RCB_OFFSET_LEFT,rtBottom.top(),
		rtBottom.width()-RCB_OFFSET_Y-RCB_OFFSET_LEFT,rtBottom.height()));
}

void CColorBlockWidget::drawClient( QPainter& p,const QRect& rtClient )
{
	p.fillRect(rtClient,QColor(0,0,0));

	updateColorBlockData();
	int iCurY = rtClient.top();
	int iIndex = showStockIndex;
	for(;iCurY<rtClient.bottom();iCurY=(iCurY+m_iCBHeight))
	{
		if(iIndex<0||iIndex>=m_listStocks.size())
			break;

		drawStock(p,QRect(rtClient.left(),iCurY,rtClient.width(),m_iCBHeight),m_listStocks[iIndex]);
		++iIndex;
	}
}

void CColorBlockWidget::drawStock( QPainter& p,const QRect& rtCB,CStockInfoItem* pItem )
{
	if(pItem == m_pSelectedStock)
	{
		p.fillRect(rtCB,QColor(50,50,50));
	}

	{
		//绘制左侧的标识信息（代码或者名称）
		p.setPen(QColor(255,255,255));
		QString qsText = pItem->getName();
		if(qsText.isEmpty())
			qsText = pItem->getCode();
		p.drawText(QRect(rtCB.left(),rtCB.top(),RCB_OFFSET_LEFT,m_iCBHeight),Qt::AlignCenter,qsText);
	}

	//从右向左绘制横坐标
	float fBeginX = rtCB.right()-RCB_OFFSET_Y;
	float fEndX = rtCB.left()+RCB_OFFSET_LEFT;
	float fCBWidth = fBeginX-fEndX;
	if(fCBWidth<0)
		return;
	if(!mapStockColorBlocks.contains(pItem))
		return;

	QMap<time_t,RStockData*>* pMapCBs = mapStockColorBlocks[pItem];
	RCalcInfo calc;
	RDrawInfo draw;
	{
		calc.dwVersion = RSTOCK_VER;
		calc.emCircle = m_typeCircle;
		calc.mapData = pMapCBs;
		calc.mapDataEx = NULL;
		calc.pItem = pItem;

		draw.dwVersion = RSTOCK_VER;
		draw.pPainter = &p;
		draw.rtClient = rtCB;
	}
	{
		lua_pushlightuserdata(m_pL,&calc);
		lua_setglobal(m_pL,"_calc");

		lua_pushlightuserdata(m_pL,&draw);
		lua_setglobal(m_pL,"_draw");

		lua_getglobal(m_pL,"InitValues");
		lua_call(m_pL,0,0);
	}

	{
		luaL_dostring(m_pL,"p1 = (CLOSE-REF(CLOSE,1))/CLOSE");
//		luaL_dostring(m_pL,"p1 = CLOSE-REF(CLOSE,1)");
//		luaL_dostring(m_pL,"p2 = CLOSE");

		//QVector<float> _vvv;
		//RLuaEx::LuaPopArray(m_pL,"p1",_vvv);
		//if(pItem->getCode() == "600000")
		//{
		//	foreach(float _f,_vvv)
		//	{
		//		qDebug()<<_f;
		//	}
		//}
		
		QVector<float> _vv;
		RLuaEx::LuaPopArray(m_pL,"p1",_vv);
		drawColocBlock(p,rtCB.top(),_vv);
	}
}


void CColorBlockWidget::mouseMoveEvent( QMouseEvent* e )
{
	CStockInfoItem* pStock = hitTestStock(e->pos());
	RStockData* item = hitTestCBItem(e->pos());
	if(item == NULL || pStock==0)
	{
		QToolTip::hideText();
		return CBaseWidget::mouseMoveEvent(e);
	}

	QString qsTooltip;		//Tips
	if(m_typeCircle<Day)
	{
		qsTooltip = QString("股票代码:%1\r\n时间:%2\r\n价格:%3")
			.arg(pStock->getName()).arg(QDateTime::fromTime_t(item->tmTime).toString("hh:mm:ss"))
			.arg(item->fClose);
	}
	else
	{
		QString qsTime;
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

		qsTooltip = QString("股票代码:%1\r\n时间:%2\r\n价格:%3")
			.arg(pStock->getName()).arg(qsTime)
			.arg(item->fClose);
	}

	QToolTip::showText(e->globalPos(),qsTooltip,this);

	return CBaseWidget::mouseMoveEvent(e);
}

void CColorBlockWidget::mousePressEvent( QMouseEvent* e )
{
	e->accept();
	QPoint ptCur = e->pos();
	if(m_rtHeader.contains(ptCur))
	{

	}
	else if(m_rtClient.contains(ptCur))
	{
		int iCurIndex = showStockIndex+(ptCur.y()-m_rtClient.top())/m_iCBHeight;
		if(iCurIndex>=0&&iCurIndex<m_listStocks.size())
		{
			clickedStock(m_listStocks[iCurIndex]);
		}
	}
	else if(m_rtBottom.contains(ptCur))
	{

	}
}

void CColorBlockWidget::wheelEvent( QWheelEvent* e )
{
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;
	int iIndex = showStockIndex-numSteps*5;
	if(iIndex<0) {iIndex = 0;}
	if(iIndex>=0&&iIndex<m_listStocks.size())
	{
		e->accept();
		showStockIndex = iIndex;
		update();
	}
	return CBaseWidget::wheelEvent(e);
}

void CColorBlockWidget::keyPressEvent( QKeyEvent* e )
{
	if(Qt::Key_Left == e->key())
	{
		e->accept();
		return;
	}
	else if(Qt::Key_Right == e->key())
	{
		e->accept();
		return;
	}
	else if(Qt::Key_Down == e->key())
	{
		int iCurIndex = m_mapStockIndex[m_pSelectedStock];
		if(m_listStocks.size()>(iCurIndex+1))
		{
			CStockInfoItem* pItem = m_listStocks[iCurIndex+1];
			int iRow = m_mapStockIndex[pItem];
			int iShowCount = m_rtClient.height()/m_iCBHeight;
			if(iShowCount<1)
				return;
			if((iRow-showStockIndex)>=iShowCount)
			{
				showStockIndex = iRow-iShowCount+1;
				update(m_rtClient);
			}
			clickedStock(m_listStocks[iCurIndex+1]);
		}
		e->accept();
	}
	else if(Qt::Key_Up == e->key())
	{
		int iCurIndex = m_mapStockIndex[m_pSelectedStock];
		if(iCurIndex>0)
		{
			CStockInfoItem* pItem = m_listStocks[iCurIndex-1];
			int iRow = m_mapStockIndex[pItem];
			if(iRow<showStockIndex)
			{
				showStockIndex = iRow;
				update(m_rtClient);
			}
			clickedStock(pItem);
		}
		e->accept();
	}
	else if(Qt::Key_PageDown == e->key())
	{
		int iShowCount = m_rtClient.height()/m_iCBHeight;
		if(iShowCount<1)
			return;
		if((showStockIndex+iShowCount)<m_listStocks.size())
		{
			showStockIndex = showStockIndex+iShowCount;
			update(m_rtClient);
		}
		e->accept();
	}
	else if(Qt::Key_PageUp == e->key())
	{
		int iShowCount = m_rtClient.height()/m_iCBHeight;
		if(iShowCount<1)
			return;
		showStockIndex = (showStockIndex-iShowCount)>0 ? (showStockIndex-iShowCount) : 0;
		update(m_rtClient);

		e->accept();
	}

	return CBaseWidget::keyPressEvent(e);
}

QMenu* CColorBlockWidget::getCustomMenu()
{
	CBaseBlockWidget::getCustomMenu();
	{
		//设置所有板块的菜单
		m_pMenuBlockList->clear();
		QList<CBlockInfoItem*> list = CDataEngine::getDataEngine()->getStockBlocks();
		foreach(const CBlockInfoItem* block,list)
		{
			QAction* pAct = m_pMenuBlockList->addAction(block->getBlockName(),this,SLOT(onSetCurrentBlock()));
			pAct->setData(block->getBlockName());
			if(m_pBlock == block)
			{
				pAct->setCheckable(true);
				pAct->setChecked(true);
			}
		}
	}

	return m_pMenuCustom;
}

QRect CColorBlockWidget::rectOfStock( CStockInfoItem* pItem )
{
	if(m_mapStockIndex.contains(pItem))
	{
		int iRow = m_mapStockIndex[pItem];
		return QRect(m_rtClient.left(),(m_rtClient.top()+(iRow-showStockIndex)*m_iCBHeight),m_rtClient.width(),m_iCBHeight);
	}

	return QRect();
}

CStockInfoItem* CColorBlockWidget::hitTestStock( const QPoint& ptPoint ) const
{
	int iRow = (ptPoint.y()-m_rtClient.top())/m_iCBHeight + showStockIndex;
	if(iRow<0||iRow>=m_listStocks.size())
		return 0;

	return m_listStocks[iRow];
}

RStockData* CColorBlockWidget::hitTestCBItem( const QPoint& ptPoint ) const
{
	CStockInfoItem* pItem = hitTestStock(ptPoint);
	if(pItem && mapStockColorBlocks.contains(pItem))
	{
		QMap<time_t,RStockData*>* pMap = mapStockColorBlocks[pItem];
		QMap<time_t,int>::iterator iterLastTime = m_mapTimes.end();
		if(iterLastTime!=m_mapTimes.begin())
		{
			--iterLastTime;								//取时间轴上的最后一个时间点
			int iIndex = (m_rtClient.right() - RCB_OFFSET_Y - ptPoint.x())/m_iCBWidth + 1;
			time_t tmLast = iterLastTime.key();			//时间轴上的最后一个时间点
			time_t tmCur = tmLast;
			if (m_typeCircle<Day)
			{
				tmCur = tmCur - (m_typeCircle*iIndex);
			}
			else
			{
				time_t tmDefault = 0;
				tmCur = m_mapTimes.key(iIndex,tmDefault);
				if(tmCur==tmDefault)
					return NULL;
			}

			//获取最近的节点
			QMap<time_t,RStockData*>::iterator iterCB = pMap->lowerBound(tmCur);
			if(m_typeCircle<Day)
			{
				if(iterCB==pMap->end())
					return NULL;
				if(iterCB.key()/m_typeCircle*m_typeCircle != (tmCur))
					return NULL;
			}
			else
			{
				if(iterCB==pMap->begin())
					return NULL;
				if(m_typeCircle==Day)
				{
					return (pMap->find(tmCur)!=pMap->end()) ? pMap->find(tmCur).value() : NULL;
				}
				else if(m_typeCircle == Week)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					int iTheYear = dtKey.year();
					return dtKey.weekNumber(&iTheYear)==dtCur.weekNumber(&iTheYear) ? iterCB.value() : NULL;
				}
				else if(m_typeCircle == Month)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					return ((dtKey.year()==dtKey.year())&&(dtKey.month()==dtCur.month())) ? iterCB.value() : NULL;
				}
				else if(m_typeCircle == Month3)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					return ((dtKey.year()==dtKey.year())&&(((dtKey.month()-1)/3)==((dtCur.month()-1)/3))) ? iterCB.value() : NULL;
				}
				else if(m_typeCircle == Year)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					return (dtKey.year()==dtKey.year()) ? iterCB.value() : NULL;
				}
			}
			return iterCB.value();
		}
	}
	return NULL;
}

