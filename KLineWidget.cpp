#include "StdAfx.h"
#include "KLineWidget.h"
#include "DataEngine.h"

#define	KLINE_BORDER	2


/* 对某分钟的数据进行转换 (qRcvMinuteData*) -> (stLinerItem) */
bool getLinerItemByMin(stLinerItem& item, qRcvFenBiData* pMin)
{
	if(!pMin)
		return false;

	item.time = pMin->tmTime;
	item.fOpen = pMin->fPrice;
	item.fClose = pMin->fPrice;
	item.fLow = pMin->fPrice;
	item.fHigh = pMin->fPrice;
	item.fAmount = pMin->fAmount;
	item.fVolume = pMin->fVolume;
	return true;
}

/*通过分钟数据获取指定周期内的数据*/
RStockData* getLinerItemByMins(const QList<qRcvFenBiData*>& list, qRcvFenBiData* pLastFenbi)
{
	if(list.size()<1)
		return NULL;

	RStockData* pData = new RStockData();

	qRcvFenBiData* pBegin = list.first();
	qRcvFenBiData* pLast = list.last();
	pData->tmTime = pLast->tmTime;
	pData->fOpen = pBegin->fPrice;
	pData->fClose = pLast->fPrice;

	pData->fLow = pBegin->fPrice;
	pData->fHigh = pBegin->fPrice;

	if(pLastFenbi)
	{
		pData->fAmount = pLast->fAmount-pLastFenbi->fAmount;
		pData->fVolume = pLast->fVolume-pLastFenbi->fVolume;
	}
	else
	{
		pData->fAmount = pLast->fAmount;
		pData->fVolume = pLast->fVolume;
	}
	foreach(qRcvFenBiData* p,list)
	{
		if(pData->fLow>p->fPrice)
			pData->fLow = p->fPrice;
		if(pData->fHigh<p->fPrice)
			pData->fHigh = p->fPrice;
	}
	return pData;
}

int getLinerMinItems(QMap<time_t,RStockData*>& mapData,const QList<qRcvFenBiData*>& minutes, int iSize = 1)
{
	QList<qRcvFenBiData*> listMins;
	time_t tmT = 0;
	qRcvFenBiData* pLastFenbi = 0;
	foreach(qRcvFenBiData* p, minutes)
	{
		if((p->tmTime-tmT)>=(iSize))
		{
			RStockData* pData = getLinerItemByMins(listMins,pLastFenbi);
			if(pData)
			{
				if(!mapData.contains(pData->tmTime))
					mapData[pData->tmTime] = pData;
				else
					delete pData;
			}

			if(listMins.size()>0)
				pLastFenbi = listMins.last();
			listMins.clear();
			tmT = p->tmTime;
		}

		listMins.push_back(p);
	}

	{
		//最后一组数据
		RStockData* pData = getLinerItemByMins(listMins,pLastFenbi);
		if(pData)
		{
			if(!mapData.contains(pData->tmTime))
				mapData[pData->tmTime] = pData;
			else
				delete pData;
		}
		listMins.clear();
	}
	return mapData.size();
}

/* 对某天的数据进行转换 (qRcvHistoryData*) -> (stLinerItem) */
RStockData* getLinerItemByDay(const qRcvHistoryData* pHistory)
{
	if(!pHistory)
		return NULL;
	RStockData* pData = new RStockData();

	pData->tmTime = pHistory->time;
	pData->fOpen = pHistory->fOpen;
	pData->fClose = pHistory->fClose;
	pData->fHigh = pHistory->fHigh;
	pData->fLow = pHistory->fLow;
	pData->fAmount = pHistory->fAmount;
	pData->fVolume = pHistory->fVolume;
	pData->wAdvance = pHistory->wAdvance;
	pData->wDecline = pHistory->wDecline;
	return pData;
}
/*通过多天数据获取一个周期内的数据*/
RStockData* getLinerItemByDays(const QList<qRcvHistoryData*>& list)
{
	if(list.size()<1)
		return NULL;

	RStockData* pData = new RStockData;

	qRcvHistoryData* pBegin = list.first();
	qRcvHistoryData* pLast = list.last();
	pData->tmTime = pBegin->time;
	pData->fOpen = pBegin->fOpen;
	pData->fClose = pLast->fClose;

	pData->fLow = pBegin->fLow;
	pData->fHigh = pBegin->fHigh;
	pData->fAmount = 0;
	pData->fVolume = 0;
	foreach(qRcvHistoryData* p,list)
	{
		if(pData->fLow>p->fLow)
			pData->fLow = p->fLow;
		if(pData->fHigh<p->fHigh)
			pData->fHigh = p->fHigh;
		pData->fAmount+=p->fAmount;
		pData->fVolume+=p->fVolume;
	}
//	item.wAdvance = pHistory->wAdvance;
//	item.wDecline = pHistory->wDecline;
	return pData;
}

int getLinerDayItems(QMap<time_t,RStockData*>& mapData,const QList<qRcvHistoryData*>& historys)
{
	foreach(qRcvHistoryData* p,historys)
	{
		RStockData* pData = getLinerItemByDay(p);
		if(pData)
		{
			if(!mapData.contains(pData->tmTime))
				mapData[pData->tmTime] = pData;
			else
				delete pData;
		}
	}

	return mapData.size();
}

int getLinerWeekItems(QMap<time_t,RStockData*>& mapData,const QList<qRcvHistoryData*>& historys)
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
				RStockData* pData = getLinerItemByDays(weekHis);
				if(pData)
				{
					if(!mapData.contains(pData->tmTime))
						mapData[pData->tmTime] = pData;
					else
						delete pData;
				}
				weekHis.clear();
			}
			iCurYear = tmDate.year();
			iCurWeek = tmDate.weekNumber();
		}
		else if(tmDate.weekNumber()!=iCurWeek)
		{
			iCurWeek = tmDate.weekNumber();

			RStockData* pData = getLinerItemByDays(weekHis);
			if(pData)
			{
				if(!mapData.contains(pData->tmTime))
					mapData[pData->tmTime] = pData;
				else
					delete pData;
			}
			weekHis.clear();
		}
		weekHis.push_back(pHistory);
	}

	if(weekHis.size()>0)
	{
		//最后剩余的也补上
		RStockData* pData = getLinerItemByDays(weekHis);
		if(pData)
		{
			if(!mapData.contains(pData->tmTime))
				mapData[pData->tmTime] = pData;
			else
				delete pData;
		}
		weekHis.clear();
	}

	return mapData.size();
}

int getLinerMonthItems(QMap<time_t,RStockData*>& mapData,const QList<qRcvHistoryData*>& historys)
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
				RStockData* pData = getLinerItemByDays(monthHis);
				if(pData)
				{
					if(!mapData.contains(pData->tmTime))
						mapData[pData->tmTime] = pData;
					else
						delete pData;
				}
				monthHis.clear();
			}
		}
		else if(tmDate.month()!=iCurMonth)
		{
			iCurMonth = tmDate.month();

			RStockData* pData = getLinerItemByDays(monthHis);
			if(pData)
			{
				if(!mapData.contains(pData->tmTime))
					mapData[pData->tmTime] = pData;
				else
					delete pData;
			}
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//最后剩余的也补上
		RStockData* pData = getLinerItemByDays(monthHis);
		if(pData)
		{
			if(!mapData.contains(pData->tmTime))
				mapData[pData->tmTime] = pData;
			else
				delete pData;
		}
		monthHis.clear();
	}
	return mapData.size();
}

int getLinerMonth3Items(QMap<time_t,RStockData*>& mapData,const QList<qRcvHistoryData*>& historys)
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
				RStockData* pData = getLinerItemByDays(monthHis);
				if(pData)
				{
					if(!mapData.contains(pData->tmTime))
						mapData[pData->tmTime] = pData;
					else
						delete pData;
				}
				monthHis.clear();
			}
		}
		else if(((tmDate.month()-1)/3)!=((iCurMonth-1)/3))
		{
			iCurMonth = tmDate.month();

			RStockData* pData = getLinerItemByDays(monthHis);
			if(pData)
			{
				if(!mapData.contains(pData->tmTime))
					mapData[pData->tmTime] = pData;
				else
					delete pData;
			}
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//最后剩余的也补上
		RStockData* pData = getLinerItemByDays(monthHis);
		if(pData)
		{
			if(!mapData.contains(pData->tmTime))
				mapData[pData->tmTime] = pData;
			else
				delete pData;
		}

		monthHis.clear();
	}
	return mapData.size();
}

int getLinerYearItems(QMap<time_t,RStockData*>& mapData,const QList<qRcvHistoryData*>& historys)
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
				RStockData* pData = getLinerItemByDays(monthHis);
				if(pData)
				{
					if(!mapData.contains(pData->tmTime))
						mapData[pData->tmTime] = pData;
					else
						delete pData;
				}

				monthHis.clear();
			}
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//最后剩余的也补上
		RStockData* pData = getLinerItemByDays(monthHis);
		if(pData)
		{
			if(!mapData.contains(pData->tmTime))
				mapData[pData->tmTime] = pData;
			else
				delete pData;
		}

		monthHis.clear();
	}
	return mapData.size();
}


CKLineWidget::CKLineWidget( CBaseWidget* parent /*= 0*/ )
	: CCoordXBaseWidget(parent,WidgetKLine)
	, m_pActShowMain(0)
	, m_pStockItem(0)
	, m_iShowCount(100)
	, m_pLinerMain(0)
	, m_pCurrentLiner(0)
	, m_bShowMax(false)
	, m_iTitleHeight(16)
	, m_iCoorYWidth(50)
	, m_iCoorXHeight(16)
	, m_iMainLinerHeight(200)
	, m_mapData(NULL)
{

	m_typeCircle = Day;

	m_pLinerMain = new CMultiLiner(CMultiLiner::MainKLine,m_pL,"");
	m_vSizes.push_back(60);

	{
		m_pMenuCustom->addSeparator();
		QMenu* pMenuDeputy = m_pMenuCustom->addMenu(tr("添加副图"));
		pMenuDeputy->addAction(tr("普通副图"),this,SLOT(onAddDeputy()));
		pMenuDeputy->addAction(tr("量副图"),this,SLOT(onAddVolume()));

	}
	m_pMenuCustom->addAction(tr("删除副图"),this,SLOT(onRemoveDeputy()));
	m_pMenuCustom->addAction(tr("设置表达式"),this,SLOT(onSetExpression()));
	{
		m_pActShowMain = m_pMenuCustom->addAction(tr("显示主图"));
		m_pActShowMain->setCheckable(true);
		m_pActShowMain->setChecked(true);
		connect(m_pActShowMain,SIGNAL(toggled(bool)),this,SLOT(onShowMainChanged(bool)));
	}
	m_pMenuCustom->addAction(tr("设置所有图的显示比例"),this,SLOT(onSetSizes()));

//	setMinimumSize(200,200);
}

CKLineWidget::~CKLineWidget(void)
{
	clearTmpData();
	delete m_pLinerMain;
	foreach(CMultiLiner* p,m_listLiners)
		delete p;
	m_listLiners.clear();

	delete m_pMenuCustom;
}

bool CKLineWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CCoordXBaseWidget::loadPanelInfo(eleWidget))
		return false;

	m_vSizes.clear();

	//是否显示主图
	if(eleWidget.hasAttribute("mainliner"))
	{
		m_pActShowMain->setChecked(eleWidget.attribute("mainliner").toInt());
	}

	QDomElement eleLiners = eleWidget.firstChildElement("liners");
	if(eleLiners.isElement())
	{
		//加载所有的Liner
		{
			//加载主图的信息
			QDomElement eleMainLiner = eleLiners.firstChildElement("MainLiner");
			if(eleMainLiner.isElement())
			{
				m_vSizes.push_back(eleMainLiner.attribute("size").toInt());

				QDomElement eleExp = eleMainLiner.firstChildElement("exp");
				if(eleExp.isElement())
				{
					m_pLinerMain->setExpression(eleExp.text());
				}
			}
		}
		{
			//加载所有的副图信息
			QDomElement eleLiner = eleLiners.firstChildElement("Liner");
			while(eleLiner.isElement())
			{
				CMultiLiner::MultiLinerType t = CMultiLiner::Deputy;
				{
					//获取该副图的类型
					bool bOk = false;
					int iType = eleLiner.attribute("type").toInt(&bOk);
					if(bOk)
						t = static_cast<CMultiLiner::MultiLinerType>(iType);
				}
				CMultiLiner* pLiner = new CMultiLiner(t,m_pL,"");

				m_vSizes.push_back(eleLiner.attribute("size").toInt());
				QDomElement eleExp = eleLiner.firstChildElement("exp");
				if(eleExp.isElement())
				{
					pLiner->setExpression(eleExp.text());
				}
				m_listLiners.push_back(pLiner);

				eleLiner = eleLiner.nextSiblingElement("Liner");
			}
		}
	}

	QDomElement eleCode = eleWidget.firstChildElement("code");
	if(eleCode.isElement())
		setStockCode(eleCode.text());

	return true;
}

bool CKLineWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CCoordXBaseWidget::savePanelInfo(doc,eleWidget))
		return false;
	if(m_pStockItem)
	{
		//当前的股票值
		QDomElement eleCode = doc.createElement("code");
		eleCode.appendChild(doc.createTextNode(m_pStockItem->getCode()));
		eleWidget.appendChild(eleCode);

		//是否显示主图
		eleWidget.setAttribute("mainliner",m_pActShowMain->isChecked());

		QDomElement eleLiners = doc.createElement("liners");
		eleWidget.appendChild(eleLiners);
		{
			//创建所有的Liner
			{
				//保存主图的信息
				QDomElement eleMainLiner = doc.createElement("MainLiner");
				eleLiners.appendChild(eleMainLiner);
				int iSize = 1;
				if(m_vSizes.size()>0)
					iSize = m_vSizes[0];
				eleMainLiner.setAttribute("size",iSize);

				QDomElement eleExp = doc.createElement("exp");
				eleExp.appendChild(doc.createTextNode(m_pLinerMain->getExpression()));
				eleMainLiner.appendChild(eleExp);
			}
			{
				//保存所有的副图信息
				for(int i=0;i<m_listLiners.size();++i)
				{
					CMultiLiner* pLiner = m_listLiners[i];
					QDomElement eleLiner = doc.createElement("Liner");
					eleLiners.appendChild(eleLiner);
					int iSize = 1;
					if(m_vSizes.size()>(i+1))
						iSize = m_vSizes[i+1];
					eleLiner.setAttribute("size",iSize);
					eleLiner.setAttribute("type",pLiner->getType());

					QDomElement eleExp = doc.createElement("exp");
					eleExp.appendChild(doc.createTextNode(pLiner->getExpression()));
					eleLiner.appendChild(eleExp);
				}
			}
		}
	}

	return true;
}

void CKLineWidget::updateData()
{
	resetTmpData();
	return /*CCoordXBaseWidget::updateData()*/;
}

void CKLineWidget::setStockCode( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);
	if(pItem)
	{
		if(m_pStockItem)
		{
			//移除所有和 updateKLine关联的 信号/槽
			disconnect(m_pStockItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
			disconnect(m_pStockItem,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
//			disconnect(m_pStockItem,SIGNAL(stockItemReportChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
		}

		m_pStockItem = pItem;

		//建立更新机制
		connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
		connect(pItem,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
//		connect(pItem,SIGNAL(stockItemReportChanged(const QString&)),this,SLOT(updateKLine(const QString&)));

		//更新K线图
		updateKLine(code);
	}

	return CBaseWidget::setStockCode(code);
}

void CKLineWidget::updateKLine( const QString& code )
{
	if(m_pStockItem&&m_pStockItem->getCode()!=code)
		return;

	resetTmpData();
}

void CKLineWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	p.fillRect(rtClient,QColor(0,0,0));
	if(!m_pStockItem)
		return;

	if(m_iShowCount>m_mapData->size())
	{
		m_iShowCount = m_mapData->size();
	}

	/*画头部*/
	QRect rtTitle = QRect(rtClient.left(),rtClient.top(),rtClient.width(),m_iTitleHeight);
	drawTitle(p,rtTitle);

	/*画X坐标轴*/
	QRectF rtCoordX = QRectF(rtClient.left()-3,rtClient.bottom()-m_iCoorXHeight+1,rtClient.width()-m_iCoorYWidth-5,m_iCoorXHeight);
	m_fItemWidth = float(rtCoordX.width())/float(m_iShowCount);
	updateShowTimes(rtCoordX,m_fItemWidth);
	CCoordXBaseWidget::drawCoordX(p,rtCoordX,m_fItemWidth);

	/*画右下角的两个按钮*/
	QRect rtShowBtns = QRect(rtClient.right()-m_iCoorYWidth,rtClient.bottom()-m_iCoorXHeight,m_iCoorYWidth,m_iCoorXHeight);
	drawShowBtns(p,rtShowBtns);

	rtClient.adjust(3,m_iTitleHeight,-m_iCoorYWidth-2,-m_iCoorXHeight);			//改变为可画图区域的大小

	{
		RDrawInfo draw;
		draw.dwVersion = RSTOCK_VER;
		draw.pPainter = &p;
		draw.rtClient = rtClient;
		draw.fItemWidth = m_fItemWidth;
		draw.iEndIndex = m_mapData->size()-1;
		
		lua_pushlightuserdata(m_pL,&draw);
		lua_setglobal(m_pL,"_draw");

		luaL_dostring(m_pL,"DrawK(OPEN,CLOSE,HIGH,LOW)");
	}

	return;

	QPoint ptCurMouse = this->mapFromGlobal(QCursor::pos());
	CMultiLiner* pCurLiner = 0;		//当前鼠标所在位置的MultiLiner

	int iCurY = rtClient.top();		//当前绘制到的位置
	int iSizeIndex = 0;
	//绘制主图
	if(m_pActShowMain->isChecked())
	{
		if(m_vSizes.size()<1)
			m_vSizes.push_back(60);
		int iTotal = 0;					//比例总和
		for (int i=iSizeIndex;i<m_vSizes.size();++i)
			iTotal += m_vSizes[i];
		int iTotalHeight = rtClient.bottom()-iCurY;

		int iMainHeight = (float)((float)m_vSizes[iSizeIndex]/float(iTotal))*iTotalHeight + 0.5;
		m_pLinerMain->Draw(p,QRectF(rtClient.left(),rtClient.top(),rtClient.width(),iMainHeight),m_iShowCount);
		if(m_pLinerMain->getRect().contains(ptCurMouse))
			pCurLiner = m_pLinerMain;
		iCurY += iMainHeight;
	}
	else
	{
		m_pLinerMain->Draw(p,QRectF(),0);
	}
	++iSizeIndex;

	//绘制分割线
	if(m_listLiners.size()>0)
	{
		p.setPen(QColor(255,255,255));
		if(m_listLiners.contains(m_pCurrentLiner)&&m_bShowMax)
		{
			p.drawLine(this->rect().left(),iCurY,this->rect().right(),iCurY);
			foreach(CMultiLiner* pLiner,m_listLiners)
				pLiner->Draw(p,QRectF(),0);
			m_pCurrentLiner->Draw(p,QRectF(rtClient.left(),iCurY,rtClient.width(),rtClient.bottom()-iCurY),m_iShowCount);
			if(m_pCurrentLiner->getRect().contains(ptCurMouse))
				pCurLiner = m_pCurrentLiner;
		}
		else
		{
			for(int i=0;i<m_listLiners.size();++i)
			{
				int iTotal = 0;					//比例总和
				for (int j=iSizeIndex;j<m_vSizes.size();++j)
					iTotal += m_vSizes[j];
				int iTotalHeight = rtClient.bottom()-iCurY;
				int iHeight = (float)((float)m_vSizes[iSizeIndex]/float(iTotal))*iTotalHeight + 0.5;

				p.drawLine(this->rect().left(),iCurY,this->rect().right(),iCurY);
				m_listLiners[i]->Draw(p,QRectF(rtClient.left(),iCurY,rtClient.width(),iHeight),m_iShowCount);
				if(m_listLiners[i]->getRect().contains(ptCurMouse))
					pCurLiner = m_listLiners[i];
				iCurY += iHeight;
				++iSizeIndex;
			}
		}
	}

	if(pCurLiner)
	{
		//绘制辅助线
		QPen pen(QColor(255,0,0));
		pen.setStyle(Qt::DotLine);
		p.setPen(pen);
		p.drawLine(ptCurMouse,QPoint(rtClient.right(),ptCurMouse.y()));
		p.drawLine(ptCurMouse,QPoint(ptCurMouse.x(),rtClient.bottom()));

		//绘制y轴的值
		float fV = pCurLiner->getValueByY(ptCurMouse.y());
		{
			if(fV>10000.0)
				p.drawText(QRect(rtClient.right()+10,ptCurMouse.y()-5,50,20),Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(fV,0,'g',2));
			else
				p.drawText(QRect(rtClient.right()+10,ptCurMouse.y()-5,50,20),Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(fV,0,'f',2));				
		}

		//绘制x轴的值
		QRectF rtCurLiner = pCurLiner->getRect();
		int iAbsIndex = (rtCurLiner.right() - ptCurMouse.x())/m_fItemWidth;

		/*
		QList<time_t> times = m_mapData.keys();
		int iIndex = times.size()-iAbsIndex-1;
		if(iIndex>=0&&iIndex<m_mapData.size())
		{
			p.setPen(QColor(0,255,255));
			if(m_typeCircle<Day)
			{
				QTime tmTime = QDateTime::fromTime_t(times[iIndex]).time();
				p.drawText(QRect(ptCurMouse.x(),rtClient.bottom(),100,20),Qt::AlignCenter,tmTime.toString("HH:mm:ss"));
			}
			else
			{
				QDate tmDate = QDateTime::fromTime_t(times[iIndex]).date();
				p.drawText(QRect(ptCurMouse.x()-50,rtClient.bottom(),100,20),Qt::AlignCenter,tmDate.toString("yyyy/MM/dd"));
			}
		}*/
	}
}

void CKLineWidget::mouseMoveEvent( QMouseEvent* e )
{
	if(!((qApp->mouseButtons())&Qt::LeftButton))
	{
		update();
		QToolTip::hideText();
		return CBaseWidget::mouseMoveEvent(e);
	}
	if(!m_pStockItem)
	{
		return QToolTip::hideText();
	}
	QRectF rtClient = this->rect();
	rtClient.adjust(3,m_iTitleHeight,-m_iCoorYWidth-2,-m_iCoorXHeight);			//改变为可画图区域的大小

	float fEnd = rtClient.right();
	int iLastN = (fEnd - e->posF().x())/m_fItemWidth;
	if(iLastN<0||iLastN>=m_iShowCount)
		return QToolTip::hideText();

	return;
	/*
	int iIndex = m_mapData.size()-iLastN-1;
	if(iIndex<0)
		return QToolTip::hideText();

	QString qsTooltip;		//Tips
	stLinerItem item = m_mapData[iIndex];
	if(m_typeCircle<Day)
	{
		qsTooltip = QString("股票代码:%1\r\n时间:%2\r\n最高价:%3\r\n最低价:%4\r\n成交量:%5\r\n成交额:%6")
			.arg(m_pStockItem->getCode()).arg(QDateTime::fromTime_t(item.time).toString("HH:mm:ss"))
			.arg(item.fHigh).arg(item.fLow).arg(item.fVolume).arg(item.fAmount);
	}
	else
	{
		qsTooltip = QString("股票代码:%1\r\n时间:%2\r\n最高价:%3\r\n最低价:%4\r\n开盘价:%5\r\n收盘价:%6\r\n成交量:%7\r\n成交额:%8")
			.arg(m_pStockItem->getCode()).arg(QDateTime::fromTime_t(item.time).toString("yyyy/MM/dd"))
			.arg(item.fHigh).arg(item.fLow).arg(item.fOpen).arg(item.fClose)
			.arg(item.fVolume).arg(item.fAmount);
	}
	QToolTip::showText(e->globalPos(),qsTooltip,this);*/
	return CBaseWidget::mouseMoveEvent(e);
}

void CKLineWidget::leaveEvent( QEvent* e )
{
	update();
	return CBaseWidget::leaveEvent(e);
}

void CKLineWidget::mousePressEvent( QMouseEvent* e )
{
	QPoint ptCur = e->pos();
	if(m_pLinerMain->getRect().contains(ptCur))
	{
		m_pCurrentLiner = m_pLinerMain;
	}
	else
	{
		foreach(CMultiLiner* p, m_listLiners)
		{
			if(p->getRect().contains(ptCur))
			{
				m_pCurrentLiner = p;
				break;
			}
		}
	}
	if(e->button()==Qt::LeftButton)
	{
		if(m_rtAddShow.contains(ptCur))
		{
			onClickedAddShow();
		}
		else if(m_rtSubShow.contains(ptCur))
		{
			onClickedSubShow();
		}
	}
}

void CKLineWidget::mouseDoubleClickEvent( QMouseEvent* e )
{
	foreach(CMultiLiner* pLiner,m_listLiners)
	{
		if(pLiner->getRect().contains(e->pos()))
		{
			m_pCurrentLiner = pLiner;
			m_bShowMax = !m_bShowMax;
			update();
		}
	}
}

void CKLineWidget::keyPressEvent(QKeyEvent* e)
{

	return CBaseWidget::keyPressEvent(e);
}

QMenu* CKLineWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	{
		QList<QAction*> listAct = m_pMenuCircle->actions();
		foreach(QAction* pAct,listAct)
		{
			pAct->setCheckable(true);
			pAct->setChecked(pAct->data().toInt() == m_typeCircle);
		}
	}

	return m_pMenuCustom;
}

void CKLineWidget::onSetStockCode()
{
	QDialog dlg(this);
	QVBoxLayout layout(this);
	QLineEdit edit(this);
	QPushButton btnOk(this);
	dlg.setLayout(&layout);
	layout.addWidget(&edit);
	layout.addWidget(&btnOk);
	btnOk.setText(tr("确定"));
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	QString code = edit.text();
	setStockCode(code);
}

void CKLineWidget::onSetExpression()
{
	if(!m_pCurrentLiner)
		return;

	QDialog dlg(this);
	QVBoxLayout layout(this);
	QTextEdit edit(this);
	QPushButton btnOk(this);
	dlg.setLayout(&layout);
	layout.addWidget(&edit);
	layout.addWidget(&btnOk);
	btnOk.setText(tr("确定"));
	edit.setText(m_pCurrentLiner->getExpression());
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	m_pCurrentLiner->setExpression(edit.toPlainText());
	if(m_pCurrentLiner == m_pLinerMain)
	{
		if(m_typeCircle == FenShi)
			m_pCurrentLiner->setKLineType(CKLineLiner::FenShi);
	}
}

void CKLineWidget::onClickedAddShow()
{
	if((m_iShowCount+10)<=m_mapData->size())
	{
		m_iShowCount += 10;
		update();
	}
}

void CKLineWidget::onClickedSubShow()
{
	if((m_iShowCount-10)>0)
	{
		m_iShowCount -= 10;
		update();
	}
}

void CKLineWidget::onAddDeputy()
{
	CMultiLiner* pMultiLiner = new CMultiLiner(CMultiLiner::Deputy,m_pL,"CLOSE;");
	m_listLiners.push_back(pMultiLiner);
	m_vSizes.push_back(20);
	update();
}

void CKLineWidget::onAddVolume()
{
	CMultiLiner* pMultiLiner = new CMultiLiner(CMultiLiner::VolumeLine,m_pL,"");
	m_listLiners.push_back(pMultiLiner);
	m_vSizes.push_back(20);
	update();
}


void CKLineWidget::onRemoveDeputy()
{
	//删除当前选中的副图
	if(m_pCurrentLiner)
	{
		if(m_pCurrentLiner!=m_pLinerMain)
		{
			//如果不是副图的话再进行删除操作
			int iIndex = m_listLiners.indexOf(m_pCurrentLiner);
			if(iIndex>=0)
			{
				m_listLiners.removeOne(m_pCurrentLiner);
				delete m_pCurrentLiner;
				if(m_vSizes.size()>(iIndex+1))
					m_vSizes.remove(iIndex+1);

				update();
			}
		}
		m_pCurrentLiner = m_pLinerMain;
	}
}

void CKLineWidget::onShowMainChanged( bool bShow )
{
	if(bShow)
		m_pLinerMain->updateData();
	update();
}

void CKLineWidget::onSetSizes()
{
	//弹出让用户编辑各自窗口比例的对话框
	QDialog dlg(this);
	QGridLayout layout(&dlg);
	QPushButton btnOk(this);
	dlg.setLayout(&layout);

	int iCount = m_listLiners.size()+1;
	QVector<QLabel*> vLabels;
	QVector<QSpinBox*> vSpins;
	for (int i=0;i<iCount;++i)
	{
		//设置初始的比例
		QLabel* pLabel = new QLabel(&dlg);
		pLabel->setText(QString("%1").arg(i+1));
		vLabels.push_back(pLabel);
		QSpinBox* pSpin = new QSpinBox(&dlg);
		pSpin->setMinimum(1);
		pSpin->setMaximum(100);
		pSpin->setValue(m_vSizes[i]);
		vSpins.push_back(pSpin);

		layout.addWidget(pLabel,i,0);
		layout.addWidget(pSpin,i,1);
	}
	layout.addWidget(&btnOk,iCount,0,1,2);
	btnOk.setText(tr("确定"));
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	m_vSizes.clear();			//清空之前的比例数据

	for (int i=0;i<iCount;++i)
	{
		m_vSizes.push_back(vSpins[i]->value());
		delete vLabels[i];
		delete vSpins[i];
	}
}

void CKLineWidget::drawTitle( QPainter& p,const QRect& rtTitle )
{
	if(!rtTitle.isValid())
		return;

	QRect rtClient = rtTitle.adjusted(2,0,-2,0);
	QString qsName = m_pStockItem->getName();
	if(qsName.isEmpty())
		qsName = m_pStockItem->getCode();

	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);

	p.setPen(QColor(255,255,255));
	p.drawText(rtClient,Qt::AlignLeft|Qt::AlignVCenter,qsName);
}

void CKLineWidget::drawShowBtns( QPainter& p,const QRect& rtBtns )
{
	p.setPen(QColor(255,0,0));
	int iBtnWidth = 16;
	int iBtnHeight = rtBtns.height()-1;
	m_rtAddShow = QRect(rtBtns.right()-2*iBtnWidth,rtBtns.top()+1,iBtnWidth,iBtnHeight);
	m_rtSubShow = QRect(rtBtns.right()-iBtnWidth,rtBtns.top()+1,iBtnWidth,iBtnHeight);
	
	p.drawRect(m_rtAddShow);
	p.drawRect(m_rtSubShow);

	p.drawText(m_rtAddShow,Qt::AlignCenter,QString("+"));
	p.drawText(m_rtSubShow,Qt::AlignCenter,QString("-"));
}

void CKLineWidget::clearTmpData()
{
	//foreach(stLinerItem* p,listItems)
	//	delete p;
	if(m_mapData)
	{
		QMap<time_t,RStockData*>::iterator iter = m_mapData->begin();
		while(iter!=m_mapData->end())
		{
			delete iter.value();
			++iter;
		}
		m_mapData->clear();
		delete m_mapData;
		m_mapData = NULL;
	}
	disconnect(this,SLOT(updateKLine(const QString&)));
}

void CKLineWidget::resetTmpData()
{
	if(m_iShowCount<50)
		m_iShowCount = 100;
	clearTmpData();
	updateTimesH();			//更新最新的时间轴数据

	m_mapData = getColorBlockMap(m_pStockItem);

	//去除空值
	QMap<time_t,RStockData*>::iterator iter = m_mapData->begin();
	while(iter != m_mapData->end())
	{
		time_t tmRemove = 0;
		if(iter.value()==NULL)
		{
			tmRemove = iter.key();
		}
		++iter;

		if(tmRemove!=0)
		{
			m_mapData->remove(tmRemove);
			m_mapTimes.remove(tmRemove);
		}
	}
	if(m_mapData->size()!=m_mapTimes.size())
	{
		qDebug()<<"Something wrong in 'CKLineWidget::resetTmpData()'!";
	}
	
	QTime tmNow = QTime::currentTime();
	/*将更新后的数据设置到脚本引擎中*/
	{
		RCalcInfo calc;
		calc.dwVersion = RSTOCK_VER;
		calc.emCircle = m_typeCircle;
		calc.mapData = m_mapData;
		calc.mapDataEx = NULL;
		calc.pItem = m_pStockItem;
		
		lua_pushlightuserdata(m_pL,&calc);
		lua_setglobal(m_pL,"_calc");

		lua_getglobal(m_pL,"InitValues");
		lua_call(m_pL,0,0);
	}

	qDebug()<<"set "<<m_pStockItem->getCode()<<" data to script, use ms:"<<tmNow.msecsTo(QTime::currentTime());

	//更新绘制中的数据
	if(m_pActShowMain->isChecked())
		m_pLinerMain->updateData();
	foreach(CMultiLiner* p,m_listLiners)
		p->updateData();

	//更新界面
	update();
}
