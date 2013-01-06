#include "StdAfx.h"
#include "ColorBlockWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"
#define	RCB_OFFSET_Y	2
#define RCB_OFFSET_LEFT	50

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

int getColorBlockMinItems(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvFenBiData*>& minutes, int iSize = 1)
{
	QList<qRcvFenBiData*> listMins;
	time_t tmT = 0;
	qRcvFenBiData* pLastFenbi = 0;
	foreach(qRcvFenBiData* p, minutes)
	{
		if((p->tmTime-tmT)>(iSize))
		{
			stColorBlockItem item;
			if(getColorBlockItemByMins(item,listMins,pLastFenbi))
			{
//				QString qsTime = QDateTime::fromTime_t(item.tmTime).toString("hh:mm:ss");
				pMap->insert(item.tmTime,item);
			}
			if(listMins.size()>0)
				pLastFenbi = listMins.last();
			listMins.clear();
			tmT = p->tmTime;
		}

		listMins.push_back(p);
	}

	
	{
		//最后的数据
		stColorBlockItem item;
		if(getColorBlockItemByMins(item,listMins,pLastFenbi))
			pMap->insert(item.tmTime,item);
		listMins.clear();
	}


	return pMap->size();
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

	qRcvHistoryData* pBegin = list.first();
	qRcvHistoryData* pLast = list.last();
	item.tmTime = pBegin->time;
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

int getColorBlockDayItems(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvHistoryData*>& historys)
{
	foreach(qRcvHistoryData* p,historys)
	{
		stColorBlockItem item;
		if(getColorBlockItemByDay(item,p))
			pMap->insert(item.tmTime,item);
	}

	return pMap->size();
}

int getColorBlockWeekItems(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvHistoryData*>& historys)
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
			if(tmDate.weekNumber(&iCurYear)!=iCurWeek)
			{
				stColorBlockItem item;
				getColorBlockItemByDays(item,weekHis);
				pMap->insert(item.tmTime,item);
				weekHis.clear();
			}
			iCurYear = tmDate.year();
			iCurWeek = tmDate.weekNumber();
		}
		else if(tmDate.weekNumber()!=iCurWeek)
		{
			iCurWeek = tmDate.weekNumber();

			stColorBlockItem item;
			getColorBlockItemByDays(item,weekHis);
			pMap->insert(item.tmTime,item);
			weekHis.clear();
		}
		weekHis.push_back(pHistory);
	}

	if(weekHis.size()>0)
	{
		//最后剩余的也补上
		stColorBlockItem item;
		getColorBlockItemByDays(item,weekHis);
		pMap->insert(item.tmTime,item);
		weekHis.clear();
	}
	return pMap->size();
}

int getColorBlockMonthItems(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvHistoryData*>& historys)
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
				pMap->insert(item.tmTime,item);
				monthHis.clear();
			}
		}
		else if(tmDate.month()!=iCurMonth)
		{
			iCurMonth = tmDate.month();

			stColorBlockItem item;
			getColorBlockItemByDays(item,monthHis);
			pMap->insert(item.tmTime,item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//最后剩余的也补上
		stColorBlockItem item;
		getColorBlockItemByDays(item,monthHis);
		pMap->insert(item.tmTime,item);
		monthHis.clear();
	}
	return pMap->size();
}

int getColorBlockMonth3Items(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvHistoryData*>& historys)
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
				pMap->insert(item.tmTime,item);
				monthHis.clear();
			}
		}
		else if(((tmDate.month()-1)/3)!=((iCurMonth-1)/3))
		{
			iCurMonth = tmDate.month();

			stColorBlockItem item;
			getColorBlockItemByDays(item,monthHis);
			pMap->insert(item.tmTime,item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//最后剩余的也补上
		stColorBlockItem item;
		getColorBlockItemByDays(item,monthHis);
		pMap->insert(item.tmTime,item);
		monthHis.clear();
	}
	return pMap->size();
}

int getColorBlockYearItems(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvHistoryData*>& historys)
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
				pMap->insert(item.tmTime,item);
				monthHis.clear();
			}
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//最后剩余的也补上
		stColorBlockItem item;
		getColorBlockItemByDays(item,monthHis);
		pMap->insert(item.tmTime,item);
		monthHis.clear();
	}
	return pMap->size();
}


CColorBlockWidget::CColorBlockWidget( CBaseWidget* parent /*= 0*/ )
	: CCoordXBaseWidget(parent,CBaseWidget::ColorBlock)
	, m_pMenuCustom(0)
	, m_pMenuColorMode(0)
	, m_pMenuBlockMode(0)
	, m_pMenuBlockList(0)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, m_iCBHeight(16)
	, m_iCBWidth(16)
	, showStockIndex(0)
	, m_pSelectedStock(0)
	, m_qsColorMode("")
	, m_typeBlock(BlockCircle)
{
	m_typeCircle = Min1;				//设置初始显示周期为1分钟
	//初始化菜单
	m_pMenuCustom = new QMenu(tr("色块图菜单"));

	{
		//设置当前K线图的显示周期
		m_pMenuCircle = m_pMenuCustom->addMenu(tr("周期设置"));
		m_pMenuCircle->addAction(tr("10秒分时图"),this,SLOT(onSetCircle()))->setData(Sec10);
		m_pMenuCircle->addAction(tr("30秒分时图"),this,SLOT(onSetCircle()))->setData(Sec30);
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
		//设置色块的大小
		m_pMenuCustom->addAction(tr("设置色块大小"),this,SLOT(onSetBlockSize()));

		//设置当前显示的板块
		m_pMenuBlockList = m_pMenuCustom->addMenu(tr("设置当前板块"));
	}

	m_pMenuCustom->addSeparator();
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
		m_typeCircle = static_cast<CoordXCircle>(eleWidget.attribute("circle").toInt());
	}
	//色块的宽度
	if(eleWidget.hasAttribute("CBWidth"))
	{
		m_iCBWidth = eleWidget.attribute("CBWidth").toInt();
	}
	//色块的高度
	if(eleWidget.hasAttribute("CBHeight"))
	{
		m_iCBHeight = eleWidget.attribute("CBHeight").toInt();
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
	eleWidget.setAttribute("CBWidth",m_iCBWidth);
	eleWidget.setAttribute("CBHeight",m_iCBHeight);

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
		connect(pItem,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateStock(const QString&)));
	}

	m_qsBlock = block;

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
		QMap<time_t,stColorBlockItem>* pMap = mapStockColorBlocks[pItem];
		mapStockColorBlocks[pItem] = 0;
		delete pMap;
		mapStockColorBlocks[pItem] = getColorBlockMap(pItem);
		updateTimesH();
	}
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
	m_typeCircle = static_cast<CoordXCircle>(pAct->data().toInt());
	setBlock(m_qsBlock);
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

void CColorBlockWidget::onSetBlockSize()
{
	//弹出设置色块大小的对话框，用来设置色块的大小
	QDialog dlg(this);
	QGridLayout layout(&dlg);
	QLabel label1(tr("宽度"),&dlg);
	QLabel label2(tr("高度"),&dlg);
	QLineEdit editW(&dlg);
	QLineEdit editH(&dlg);
	QPushButton btnOk(&dlg);
	dlg.setLayout(&layout);
	layout.addWidget(&label1,0,0,1,1);
	layout.addWidget(&label2,1,0,1,1);
	layout.addWidget(&editW,0,1,1,1);
	layout.addWidget(&editH,1,1,1,1);
	layout.addWidget(&btnOk,2,0,1,2);
	btnOk.setText(tr("确定"));
	QIntValidator intValidator(1,100);
	editW.setValidator(&intValidator);
	editH.setValidator(&intValidator);
	editW.setText(QString("%1").arg(m_iCBWidth));
	editH.setText(QString("%1").arg(m_iCBHeight));
	dlg.setWindowTitle(tr("色块大小设置"));
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	int iW = editW.text().toInt();
	int iH = editH.text().toInt();
	if((iW>0&&iH>0))
	{
		if(iW!=m_iCBWidth||iH!=m_iCBHeight)
		{
			m_iCBHeight = iH;
			m_iCBWidth = iW;
			update();
		}
	}
	else
	{
		QMessageBox::information(this,tr("提示"),tr("设置色块大小失败，请检查你的输入！"));
	}
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

	QMap<CStockInfoItem*,QMap<time_t,stColorBlockItem>*>::iterator iter = mapStockColorBlocks.begin();
	while(iter!=mapStockColorBlocks.end())
	{
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
	drawClient(p,m_rtClient);
	drawBottom(p,m_rtBottom);
}

void CColorBlockWidget::mouseMoveEvent( QMouseEvent* e )
{
	CStockInfoItem* pStock = hitTestStock(e->pos());
	stColorBlockItem item = hitTestCBItem(e->pos());
	if(item.fPrice<0.1 || pStock==0)
	{
		QToolTip::hideText();
		return CBaseWidget::mouseMoveEvent(e);
	}

	QString qsTooltip;		//Tips
	if(m_typeCircle<Day)
	{
		qsTooltip = QString("股票代码:%1\r\n时间:%2\r\n价格:%3")
			.arg(pStock->getName()).arg(QDateTime::fromTime_t(item.tmTime).toString("hh:mm:ss"))
			.arg(item.fPrice);
	}
	else
	{
		QString qsTime;
		QDate dtTmp = QDateTime::fromTime_t(item.tmTime).date();
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
			.arg(item.fPrice);
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

	{
		//设置所有板块的菜单
		m_pMenuBlockList->clear();
		QList<QString> list = CDataEngine::getDataEngine()->getStockBlocks();
		foreach(const QString& block,list)
		{
			QAction* pAct = m_pMenuBlockList->addAction(block,this,SLOT(onSetCurrentBlock()));
			pAct->setData(block);
			if(m_qsBlock == block)
			{
				pAct->setCheckable(true);
				pAct->setChecked(true);
			}
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
		rtBottom.width()-RCB_OFFSET_Y-RCB_OFFSET_LEFT,rtBottom.height()),
		m_iCBWidth);
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

	QMap<time_t,stColorBlockItem>* pMapCBs = mapStockColorBlocks[pItem];
	QMap<time_t,stColorBlockItem>::iterator iter = pMapCBs->begin();
	float fLastPrice = pItem->getCurrentReport()->fLastClose;
	while(iter!=pMapCBs->end())
	{
		float f = FLOAT_NAN;
		QRect rtB;
		if(m_typeCircle<Day)
		{
			time_t tmCur = iter.key()/(m_typeCircle)*(m_typeCircle);		//向下取整
			if(m_mapTimes.contains(tmCur))
			{
				float fCurX = fBeginX - ((m_mapTimes[tmCur])*m_iCBWidth);
				if(fCurX>=fEndX)
				{
					//计算增长百分比
					f = (iter->fPrice - fLastPrice)/fLastPrice*10.0;
					rtB = QRect(fCurX,rtCB.top(),m_iCBWidth,m_iCBHeight);
				}
			}
		}
		else
		{
			time_t tmCur = iter.key();
			QMap<time_t,int>::iterator iterTime = m_mapTimes.upperBound(tmCur);
			if(iterTime!=m_mapTimes.begin())
			{
				--iterTime;
				float fCurX = fBeginX - ((iterTime.value())*m_iCBWidth);
				if(fCurX>=fEndX)
				{
					//计算增长百分比
					f = (iter->fPrice - fLastPrice)/fLastPrice;
					if(m_typeCircle>DayN)
						f = f/10.0;							//大于周线的，则对比例进行缩小
					rtB = QRect(fCurX,rtCB.top(),m_iCBWidth,m_iCBHeight);
				}
			}
		}
		if(f!=FLOAT_NAN)
		{
			switch(m_typeBlock)
			{
			case BlockRect:
				{
					rtB.adjust(1,1,-1,-1);
					p.fillRect(rtB,CColorManager::getBlockColor(m_qsColorMode,f));
				}
				break;
			case BlockCircle:
				{
					QPainterPath path;
					path.addEllipse(rtB);
					p.fillPath(path,CColorManager::getBlockColor(m_qsColorMode,f));
				}
				break;
			}
		}
		fLastPrice = iter->fPrice;
		++iter;
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

CStockInfoItem* CColorBlockWidget::hitTestStock( const QPoint& ptPoint ) const
{
	int iRow = (ptPoint.y()-m_rtClient.top())/m_iCBHeight + showStockIndex;
	if(iRow<0||iRow>=m_listStocks.size())
		return 0;

	return m_listStocks[iRow];
}

stColorBlockItem CColorBlockWidget::hitTestCBItem( const QPoint& ptPoint ) const
{
	CStockInfoItem* pItem = hitTestStock(ptPoint);
	if(pItem && mapStockColorBlocks.contains(pItem))
	{
		QMap<time_t,stColorBlockItem>* pMap = mapStockColorBlocks[pItem];
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
					return stColorBlockItem();
			}

			//获取最近的节点
			QMap<time_t,stColorBlockItem>::iterator iterCB = pMap->lowerBound(tmCur);
			if(m_typeCircle<Day)
			{
				if(iterCB==pMap->end())
					return stColorBlockItem();
				if(iterCB.key()/m_typeCircle*m_typeCircle != (tmCur))
					return stColorBlockItem();
			}
			else
			{
				if(iterCB==pMap->begin())
					return stColorBlockItem();
				if(m_typeCircle==Day)
				{
					return (pMap->find(tmCur)!=pMap->end()) ? pMap->find(tmCur).value() : stColorBlockItem();
				}
				else if(m_typeCircle == Week)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					int iTheYear = dtKey.year();
					return dtKey.weekNumber(&iTheYear)==dtCur.weekNumber(&iTheYear) ? iterCB.value() : stColorBlockItem();
				}
				else if(m_typeCircle == Month)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					return ((dtKey.year()==dtKey.year())&&(dtKey.month()==dtCur.month())) ? iterCB.value() : stColorBlockItem();
				}
				else if(m_typeCircle == Month3)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					return ((dtKey.year()==dtKey.year())&&(((dtKey.month()-1)/3)==((dtCur.month()-1)/3))) ? iterCB.value() : stColorBlockItem();
				}
				else if(m_typeCircle == Year)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					return (dtKey.year()==dtKey.year()) ? iterCB.value() : stColorBlockItem();
				}
			}
			return iterCB.value();
		}
	}
	return stColorBlockItem();
}

QMap<time_t,stColorBlockItem>* CColorBlockWidget::getColorBlockMap(CStockInfoItem* pItem)
{
	QMap<time_t,stColorBlockItem>* pMap = new QMap<time_t,stColorBlockItem>();
	if(m_typeCircle<CColorBlockWidget::Day)
	{
		//获取分钟数据，进行计算
		QList<qRcvFenBiData*> FenBis = pItem->getFenBiList();
		getColorBlockMinItems(pMap,FenBis,m_typeCircle);
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
			historys.push_back(new qRcvHistoryData(pLastReport));
		}
		if(m_typeCircle == Day)
		{
			getColorBlockDayItems(pMap,historys);
		}
		else if(m_typeCircle == DayN)
		{
			//目前未使用
			//	getLinerItem(listItems,historys,3);
		}
		else if(m_typeCircle == Week)
		{
			getColorBlockWeekItems(pMap,historys);
		}
		else if(m_typeCircle == Month)
		{
			getColorBlockMonthItems(pMap,historys);
		}
		else if(m_typeCircle == Month3)
		{
			getColorBlockMonth3Items(pMap,historys);
		}
		else if(m_typeCircle == Year)
		{
			getColorBlockYearItems(pMap,historys);
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