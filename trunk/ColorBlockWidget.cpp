#include "StdAfx.h"
#include "ColorBlockWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"

/* 对某分钟的数据进行转换 (qRcvMinuteData*) -> (stColorBlockItem) */
bool getColorBlockItemByMin(stColorBlockItem& item, qRcvFenBiData* pMin)
{
	if(!pMin)
		return false;

	item.tmTime = pMin->tmTime;
	item.fAmount = pMin->fAmount;
	item.fPrice = pMin->fPrice;
	item.fVolume = pMin->fVolume;
	return true;
}

/*通过分钟数据获取指定周期内的数据*/
bool getColorBlockItemByMins(stColorBlockItem& item, const QList<qRcvFenBiData*>& list, qRcvFenBiData* pLastFenbi)
{
	if(list.size()<1)
		return false;

//	qRcvFenBiData* pBegin = list.first();
	qRcvFenBiData* pLast = list.last();
	item.tmTime = pLast->tmTime;
	item.fPrice = pLast->fPrice;

	if(pLastFenbi)
	{
		item.fAmount = pLast->fAmount-pLastFenbi->fAmount;
		item.fVolume = pLast->fVolume-pLastFenbi->fVolume;
	}
	else
	{
		item.fAmount = pLast->fAmount;
		item.fVolume = pLast->fVolume;
	}

	return true;
}

int getColorBlockMinItems(QVector<stColorBlockItem>& listItems,const QList<qRcvFenBiData*>& minutes, int iSize = 1)
{
	QList<qRcvFenBiData*> listMins;
	time_t tmT = 0;
	qRcvFenBiData* pLastFenbi = 0;
	foreach(qRcvFenBiData* p, minutes)
	{
		if((p->tmTime-tmT)>(iSize*60))
		{
			stColorBlockItem item;
			if(getColorBlockItemByMins(item,listMins,pLastFenbi))
				listItems.push_back(item);
			if(listMins.size()>0)
				pLastFenbi = listMins.last();
			listMins.clear();
			tmT = p->tmTime;
		}

		listMins.push_back(p);
	}

	return listItems.size();
}

/* 对某天的数据进行转换 (qRcvHistoryData*) -> (stColorBlockItem) */
bool getColorBlockItemByDay(stColorBlockItem& item,const qRcvHistoryData* pHistory)
{
	if(!pHistory)
		return false;
	item.tmTime = pHistory->time;
	item.fPrice = pHistory->fClose;
	item.fAmount = pHistory->fAmount;
	item.fVolume = pHistory->fVolume;
	return true;
}
/*通过多天数据获取一个周期内的数据*/
bool getColorBlockItemByDays(stColorBlockItem& item,const QList<qRcvHistoryData*>& list)
{
	if(list.size()<1)
		return false;

//	qRcvHistoryData* pBegin = list.first();
	qRcvHistoryData* pLast = list.last();
	item.tmTime = pLast->time;
	item.fPrice = pLast->fClose;

	item.fAmount = 0;
	item.fVolume = 0;
	foreach(qRcvHistoryData* p,list)
	{
		item.fAmount+=p->fAmount;
		item.fVolume+=p->fVolume;
	}
	return true;
}

int getColorBlockDayItems(QVector<stColorBlockItem>& listItems,const QList<qRcvHistoryData*>& historys)
{
	foreach(qRcvHistoryData* p,historys)
	{
		stColorBlockItem item;
		if(getColorBlockItemByDay(item,p))
			listItems.push_back(item);
	}

	return listItems.size();
}

int getColorBlockWeekItems(QVector<stColorBlockItem>& listItems,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	int iCurWeek = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
		iCurWeek = tmDate.weekNumber();
	}

	QList<qRcvHistoryData*> weekHis;		//按星期进行归类以后的日线数据
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			iCurYear = tmDate.year();
			iCurWeek = tmDate.weekNumber();
			if(tmDate.dayOfWeek()==1)
			{
				stColorBlockItem item;
				getColorBlockItemByDays(item,weekHis);
				listItems.push_back(item);
				weekHis.clear();
			}
		}
		else if(tmDate.weekNumber()!=iCurWeek)
		{
			iCurWeek = tmDate.weekNumber();

			stColorBlockItem item;
			getColorBlockItemByDays(item,weekHis);
			listItems.push_back(item);
			weekHis.clear();
		}
		weekHis.push_back(pHistory);
	}

	return listItems.size();
}

int getColorBlockMonthItems(QVector<stColorBlockItem>& listItems,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	int iCurMonth = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
		iCurMonth = tmDate.month();
	}

	QList<qRcvHistoryData*> monthHis;		//按星期进行归类以后的日线数据
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			iCurYear = tmDate.year();
			iCurMonth = tmDate.month();
			{
				stColorBlockItem item;
				getColorBlockItemByDays(item,monthHis);
				listItems.push_back(item);
				monthHis.clear();
			}
		}
		else if(tmDate.month()!=iCurMonth)
		{
			iCurMonth = tmDate.month();

			stColorBlockItem item;
			getColorBlockItemByDays(item,monthHis);
			listItems.push_back(item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	return listItems.size();
}

int getColorBlockMonth3Items(QVector<stColorBlockItem>& listItems,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	int iCurMonth = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
		iCurMonth = tmDate.month();
	}

	QList<qRcvHistoryData*> monthHis;		//按星期进行归类以后的日线数据
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			iCurYear = tmDate.year();
			iCurMonth = tmDate.month();
			{
				stColorBlockItem item;
				getColorBlockItemByDays(item,monthHis);
				listItems.push_back(item);
				monthHis.clear();
			}
		}
		else if(((tmDate.month()-1)/3)!=((iCurMonth-1)/3))
		{
			iCurMonth = tmDate.month();

			stColorBlockItem item;
			getColorBlockItemByDays(item,monthHis);
			listItems.push_back(item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	return listItems.size();
}

int getColorBlockYearItems(QVector<stColorBlockItem>& listItems,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
	}

	QList<qRcvHistoryData*> monthHis;		//按星期进行归类以后的日线数据
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			iCurYear = tmDate.year();
			{
				stColorBlockItem item;
				getColorBlockItemByDays(item,monthHis);
				listItems.push_back(item);
				monthHis.clear();
			}
		}
		monthHis.push_back(pHistory);
	}

	return listItems.size();
}




CColorBlockWidget::CColorBlockWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::ColorBlock)
	, m_pMenuCustom(0)
	, m_pMenuColorMode(0)
	, m_pMenuBlockMode(0)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, m_iCBHeight(16)
	, m_iCBWidth(16)
	, showStockIndex(0)
	, m_pSelectedStock(0)
	, m_qsColorMode("")
	, m_typeBlock(BlockCircle)
	, m_typeCircle(Day)
{
	//初始化菜单
	m_pMenuCustom = new QMenu(tr("色块图菜单"));

	{
		//设置当前K线图的显示周期
		m_pMenuCircle = m_pMenuCustom->addMenu(tr("周期设置"));
		m_pMenuCircle->addAction(tr("1分钟分时图"),this,SLOT(onSetCircle()))->setData(Min1);
		m_pMenuCircle->addAction(tr("5分钟分时图"),this,SLOT(onSetCircle()))->setData(Min5);
		m_pMenuCircle->addAction(tr("15分钟分时图"),this,SLOT(onSetCircle()))->setData(Min15);
		m_pMenuCircle->addAction(tr("30分钟分时图"),this,SLOT(onSetCircle()))->setData(Min30);
		m_pMenuCircle->addAction(tr("60分钟分时图"),this,SLOT(onSetCircle()))->setData(Min60);

		m_pMenuCircle->addAction(tr("日线图"),this,SLOT(onSetCircle()))->setData(Day);
		m_pMenuCircle->addAction(tr("周线图"),this,SLOT(onSetCircle()))->setData(Week);
		m_pMenuCircle->addAction(tr("月线图"),this,SLOT(onSetCircle()))->setData(Month);
		m_pMenuCircle->addAction(tr("季线图"),this,SLOT(onSetCircle()))->setData(Month3);
		m_pMenuCircle->addAction(tr("年线图"),this,SLOT(onSetCircle()))->setData(Year);
	}
	{
		//颜色显示模式菜单
		m_pMenuColorMode = m_pMenuCustom->addMenu("设置颜色模式");
		//设置显示方式（圆形/方块）
		m_pMenuBlockMode = m_pMenuCustom->addMenu("设置显示形状");
		m_pMenuBlockMode->addAction("圆形",this,SLOT(onSetBlockMode()))->setData(BlockCircle);
		m_pMenuBlockMode->addAction("方形",this,SLOT(onSetBlockMode()))->setData(BlockRect);
	}
}

CColorBlockWidget::~CColorBlockWidget(void)
{
	clearTmpData();
	delete m_pMenuCustom;
}

bool CColorBlockWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	//当前显示的周期
	if(eleWidget.hasAttribute("circle"))
	{
		m_typeCircle = static_cast<ColorBlockCircle>(eleWidget.attribute("circle").toInt());
	}

	//当前的板块名称
	QDomElement eleBlock = eleWidget.firstChildElement("block");
	if(eleBlock.isElement())
	{
		setBlock(eleBlock.text());
	}

	//当前的颜色模式
	QDomElement eleColorMode = eleWidget.firstChildElement("color");
	if(eleColorMode.isElement())
	{
		m_qsColorMode = eleColorMode.text();
	}


	//当前的显示形状模式
	QDomElement eleBlockMode = eleWidget.firstChildElement("mode");
	if(eleBlockMode.isElement())
	{
		m_typeBlock = static_cast<CColorBlockWidget::BlockMode>(eleBlockMode.text().toInt());
	}


	return true;
}

bool CColorBlockWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;


	//显示的周期
	eleWidget.setAttribute("circle",m_typeCircle);

	//当前的板块名称
	QDomElement eleBlock = doc.createElement("block");
	eleBlock.appendChild(doc.createTextNode(m_qsBlock));
	eleWidget.appendChild(eleBlock);

	//当前的颜色模式
	QDomElement eleColorMode = doc.createElement("color");
	eleColorMode.appendChild(doc.createTextNode(m_qsColorMode));
	eleWidget.appendChild(eleColorMode);

	//当前的显示形状模式
	QDomElement eleBlockMode = doc.createElement("mode");
	eleBlockMode.appendChild(doc.createTextNode(QString("%1").arg(m_typeBlock)));
	eleWidget.appendChild(eleBlockMode);


	return true;
}

void CColorBlockWidget::setBlock( const QString& block )
{
	clearTmpData();

	m_listStocks = CDataEngine::getDataEngine()->getStocksByBlock(block);
	showStockIndex = 0;
	for(int i=0;i<m_listStocks.size();++i)
	{
		CStockInfoItem* pItem = m_listStocks[i];

		m_mapStockIndex[pItem] = i;
		//建立更新机制
		connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateStock(const QString&)));
		connect(pItem,SIGNAL(stockItemMinuteChanged(const QString&)),this,SLOT(updateStock(const QString&)));
	}

	m_qsBlock = block;

	if(m_listStocks.size()>0)
	{
		clickedStock(m_listStocks.first());
	}

	update();
	return CBaseWidget::setBlock(block);
}

void CColorBlockWidget::updateStock( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);
	update(rectOfStock(pItem));
}

void CColorBlockWidget::setColorMode( const QString& mode )
{
	m_qsColorMode = mode;
	//QList<QAction*> listActs = m_pMenuColorMode->actions();
	//foreach(QAction* pAct,listActs)
	//{
	//	pAct->setChecked((pAct->data().toString() == mode) ? true : false);
	//}

	update();
}

void CColorBlockWidget::onSetCircle()
{
	//设置当前的显示周期
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	m_typeCircle = static_cast<ColorBlockCircle>(pAct->data().toInt());
//	resetTmpData();
}

void CColorBlockWidget::onSetColorMode()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	setColorMode(pAct->data().toString());
}

void CColorBlockWidget::onSetBlockMode()
{
	//设置当前的显示周期
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	m_typeBlock = static_cast<BlockMode>(pAct->data().toInt());
	update();
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
	}

	
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
		disconnect(p,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateStock(const QString&)));
	}
	m_pSelectedStock = 0;
	m_listStocks.clear();
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
	drawClient(p,m_rtClient);
	drawBottom(p,m_rtBottom);
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
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	{
		//设置当前选中的周期模式
		QList<QAction*> listAct = m_pMenuCircle->actions();
		foreach(QAction* pAct,listAct)
		{
			pAct->setCheckable(true);
			pAct->setChecked(pAct->data().toInt() == m_typeCircle);
		}
	}

	{
		//添加当前所有的支持的颜色模式菜单
		m_pMenuColorMode->clear();

		QStringList listColors = CColorManager::getBlockColorList();
		foreach(const QString& clr,listColors)
		{
			QAction* pAct = m_pMenuColorMode->addAction(clr,this,SLOT(onSetColorMode()));
			pAct->setData(clr);
			pAct->setCheckable(true);
			if(clr == m_qsColorMode)
				pAct->setChecked(true);
		}
	}
	{
		QList<QAction*> listAct = m_pMenuBlockMode->actions();
		foreach(QAction* pAct,listAct)
		{
			pAct->setCheckable(true);
			pAct->setChecked(pAct->data().toInt() == m_typeBlock);
		}
	}

	return m_pMenuCustom;
}

void CColorBlockWidget::drawHeader( QPainter& p,const QRect& rtHeader )
{
	p.fillRect(rtHeader,QColor(0,0,0));
	p.setPen(QColor(255,0,0));
	QRect rtCoord = rtHeader.adjusted(0,0,-1,-1);
	p.drawRect(rtCoord);

	p.setPen(QColor(255,255,255));
	p.drawText(rtHeader,Qt::AlignLeft|Qt::AlignVCenter,m_qsBlock);
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

void CColorBlockWidget::drawBottom( QPainter& p,const QRect& rtBottom )
{
	p.fillRect(rtBottom,QColor(0,0,0));

	QRectF rtColors = QRectF(rtBottom.left(),rtBottom.top(),rtBottom.width()-32,rtBottom.height());
	float fColorsWidth = rtBottom.width()-2*m_iBottomHeight;
	if(fColorsWidth<0.1)
		return;
	float fColorWidth = fColorsWidth/COLOR_BLOCK_SIZE;
	for(int i=0;i<COLOR_BLOCK_SIZE;++i)
	{
		p.fillRect(QRectF(rtBottom.left()+i*fColorWidth,rtBottom.top(),fColorWidth,rtBottom.height()),
			CColorManager::getBlockColor(m_qsColorMode,i));
	}
}

void CColorBlockWidget::drawStock( QPainter& p,const QRect& rtCB,CStockInfoItem* pItem )
{
	if(pItem == m_pSelectedStock)
	{
		p.fillRect(rtCB,QColor(50,50,50));
	}
	int iCBCount = rtCB.width()/m_iCBHeight;
	QList<qRcvHistoryData*> list = pItem->getLastHistory(iCBCount+1);

	p.setPen(QColor(255,255,255));
	p.drawText(QRect(rtCB.left(),rtCB.top(),50,m_iCBHeight),Qt::AlignCenter,pItem->getCode());

	int iCurX = rtCB.left()+50;
	for(int i=1;i<list.size();++i)
	{
		QRect rtB = QRect(iCurX,rtCB.top(),m_iCBHeight,m_iCBHeight);
		switch(m_typeBlock)
		{
		case BlockRect:
			{
				rtB.adjust(2,2,-2,-2);
				float f = (list[i]->fClose - list[i-1]->fClose)/(list[i-1]->fClose);

				p.fillRect(rtB,CColorManager::getBlockColor(m_qsColorMode,f));
			}
			break;
		case BlockCircle:
			{
			//	rtB.adjust(2,2,-2,-2);
				QPainterPath path;
				path.addEllipse(rtB);
				float f = (list[i]->fClose - list[i-1]->fClose)/(list[i-1]->fClose);

				p.fillPath(path,CColorManager::getBlockColor(m_qsColorMode,f));
			}
			break;
		}

		iCurX = iCurX+m_iCBHeight;
	}
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

QMap<time_t,stColorBlockItem>* CColorBlockWidget::getColorBlockMap(CStockInfoItem* pItem)
{
	QMap<time_t,stColorBlockItem>* pMap = new QMap<time_t,stColorBlockItem>();
	QVector<stColorBlockItem> listItems;
	if(m_typeCircle<CColorBlockWidget::Day)
	{
		//获取分钟数据，进行计算
		QList<qRcvFenBiData*> FenBis = pItem->getFenBiList();
		if(m_typeCircle == Min1)
		{
			getColorBlockMinItems(listItems,FenBis);
		}
		else if(m_typeCircle == Min5)
		{
			getColorBlockMinItems(listItems,FenBis,5);
		}
		else if(m_typeCircle == Min15)
		{
			getColorBlockMinItems(listItems,FenBis,15);
		}
		else if(m_typeCircle == Min30)
		{
			getColorBlockMinItems(listItems,FenBis,30);
		}
		else if(m_typeCircle == Min60)
		{
			getColorBlockMinItems(listItems,FenBis,60);
		}
	}
	else
	{
		//获取日线数据
		QList<qRcvHistoryData*> historys = pItem->getHistoryList();
		qRcvReportData* pLastReport = pItem->getCurrentReport();
		bool bAppendLast = true;
		if(historys.size()>0 && pLastReport)
		{
			qRcvHistoryData* pLastHistory = historys.last();
			if(QDateTime::fromTime_t(pLastHistory->time).date() == QDateTime::fromTime_t(pLastReport->tmTime).date())
				bAppendLast = false;
		}
		if(pLastReport&&bAppendLast)
		{
			qRcvHistoryData* pLastHistory = new qRcvHistoryData();
			pLastHistory->time = pLastReport->tmTime;
			pLastHistory->fAmount = pLastReport->fAmount;
			pLastHistory->fClose = pLastReport->fNewPrice;
			pLastHistory->fHigh = pLastReport->fHigh;
			pLastHistory->fLow = pLastReport->fLow;
			pLastHistory->fOpen = pLastReport->fOpen;
			pLastHistory->fVolume = pLastReport->fVolume;

			historys.push_back(pLastHistory);
		}
		if(m_typeCircle == Day)
		{
			getColorBlockDayItems(listItems,historys);
		}
		else if(m_typeCircle == DayN)
		{
			//目前未使用
			//	getLinerItem(listItems,historys,3);
		}
		else if(m_typeCircle == Week)
		{
			getColorBlockWeekItems(listItems,historys);
		}
		else if(m_typeCircle == Month)
		{
			getColorBlockMonthItems(listItems,historys);
		}
		else if(m_typeCircle == Month3)
		{
			getColorBlockMonth3Items(listItems,historys);
		}
		else if(m_typeCircle == Year)
		{
			getColorBlockYearItems(listItems,historys);
		}

		{
			//清除获取的日线数据
			foreach(qRcvHistoryData* p,historys)
				delete p;
			historys.clear();
		}
	}

	return pMap;
}