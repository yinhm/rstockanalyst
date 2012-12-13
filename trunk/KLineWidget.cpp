#include "StdAfx.h"
#include "KLineWidget.h"
#include "DataEngine.h"

#define	KLINE_BORDER	2

/*
对stLinerItem进行转换的代码，由于效率问题，取消了该转换
QScriptValue linerItem2ScriptValue(QScriptEngine *engine, const stLinerItem &s)
{
	QScriptValue obj = engine->newObject();
	obj.setProperty("time", engine->newDate(QDateTime::fromTime_t(s.time)));
	obj.setProperty("open", s.fOpen);
	obj.setProperty("high", s.fHigh);
	obj.setProperty("low", s.fLow);
	obj.setProperty("close", s.fClose);
	obj.setProperty("volume", s.fVolume);
	obj.setProperty("amount", s.fAmount);
	obj.setProperty("advance", s.wAdvance);
	obj.setProperty("decline", s.wDecline);
	return obj;
}

void scriptValue2LinerItem(const QScriptValue &obj, stLinerItem &s)
{
	s.time = obj.property("time").toDateTime().toTime_t();
	s.fOpen = obj.property("open").toNumber();
	s.fHigh = obj.property("high").toNumber();
	s.fLow = obj.property("low").toNumber();
	s.fClose = obj.property("close").toNumber();
	s.fVolume = obj.property("volume").toNumber();
	s.fAmount = obj.property("amount").toNumber();
	s.wAdvance = obj.property("advance").toUInt16();
	s.wDecline = obj.property("decline").toUInt16();
}

QScriptValue createLinerItem(QScriptContext *, QScriptEngine *engine)
{
	stLinerItem s;
	return engine->toScriptValue(s);
}
*/

/* 对某分钟的数据进行转换 (qRcvMinuteData*) -> (stLinerItem) */
bool getLinerItemByMin(stLinerItem& item, qRcvMinuteData* pMin)
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
bool getLinerItemByMins(stLinerItem& item, const QList<qRcvMinuteData*>& list)
{
	if(list.size()<1)
		return false;

	qRcvMinuteData* pBegin = list.first();
	qRcvMinuteData* pLast = list.last();
	item.time = pBegin->tmTime;
	item.fOpen = pBegin->fPrice;
	item.fClose = pLast->fPrice;

	item.fLow = pBegin->fPrice;
	item.fHigh = pBegin->fPrice;
	item.fAmount = 0;
	item.fVolume = 0;
	foreach(qRcvMinuteData* p,list)
	{
		if(item.fLow>p->fPrice)
			item.fLow = p->fPrice;
		if(item.fHigh<p->fPrice)
			item.fHigh = p->fPrice;
		item.fAmount+=p->fAmount;
		item.fVolume+=p->fVolume;
	}
	return true;
}

int getLinerMinItems(QVector<stLinerItem>& listItems,const QList<qRcvMinuteData*>& minutes, int iSize = 1)
{
	QList<qRcvMinuteData*> listMins;
	int iCount = 0;
	for(int i=minutes.size()-1;i>=0;--i)
	{
		qRcvMinuteData* p = minutes[i];
		if(iCount>=iSize)
		{
			stLinerItem item;
			if(getLinerItemByMins(item,listMins))
				listItems.push_front(item);
			listMins.clear();
			iCount = 0;
		}

		++iCount;
		listMins.push_front(p);
	}

	return listItems.size();
}

/* 对某天的数据进行转换 (qRcvHistoryData*) -> (stLinerItem) */
bool getLinerItemByDay(stLinerItem& item,const qRcvHistoryData* pHistory)
{
	if(!pHistory)
		return false;
	item.time = pHistory->time;
	item.fOpen = pHistory->fOpen;
	item.fClose = pHistory->fClose;
	item.fHigh = pHistory->fHigh;
	item.fLow = pHistory->fLow;
	item.fAmount = pHistory->fAmount;
	item.fVolume = pHistory->fVolume;
	item.wAdvance = pHistory->wAdvance;
	item.wDecline = pHistory->wDecline;
	return true;
}
/*通过多天数据获取一个周期内的数据*/
bool getLinerItemByDays(stLinerItem& item,const QList<qRcvHistoryData*>& list)
{
	if(list.size()<1)
		return false;

	qRcvHistoryData* pBegin = list.first();
	qRcvHistoryData* pLast = list.last();
	item.time = pBegin->time;
	item.fOpen = pBegin->fOpen;
	item.fClose = pLast->fClose;

	item.fLow = pBegin->fLow;
	item.fHigh = pBegin->fHigh;
	item.fAmount = 0;
	item.fVolume = 0;
	foreach(qRcvHistoryData* p,list)
	{
		if(item.fLow>p->fLow)
			item.fLow = p->fLow;
		if(item.fHigh<p->fHigh)
			item.fHigh = p->fHigh;
		item.fAmount+=p->fAmount;
		item.fVolume+=p->fVolume;
	}
//	item.wAdvance = pHistory->wAdvance;
//	item.wDecline = pHistory->wDecline;
	return true;
}

int getLinerDayItems(QVector<stLinerItem>& listItems,const QList<qRcvHistoryData*>& historys)
{
	foreach(qRcvHistoryData* p,historys)
	{
		stLinerItem item;
		if(getLinerItemByDay(item,p))
			listItems.push_back(item);
	}

	return listItems.size();
}

int getLinerWeekItems(QVector<stLinerItem>& listItems,const QList<qRcvHistoryData*>& historys)
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
				stLinerItem item;
				getLinerItemByDays(item,weekHis);
				listItems.push_back(item);
				weekHis.clear();
			}
		}
		else if(tmDate.weekNumber()!=iCurWeek)
		{
			iCurWeek = tmDate.weekNumber();

			stLinerItem item;
			getLinerItemByDays(item,weekHis);
			listItems.push_back(item);
			weekHis.clear();
		}
		weekHis.push_back(pHistory);
	}

	return listItems.size();
}

int getLinerMonthItems(QVector<stLinerItem>& listItems,const QList<qRcvHistoryData*>& historys)
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
				stLinerItem item;
				getLinerItemByDays(item,monthHis);
				listItems.push_back(item);
				monthHis.clear();
			}
		}
		else if(tmDate.month()!=iCurMonth)
		{
			iCurMonth = tmDate.month();

			stLinerItem item;
			getLinerItemByDays(item,monthHis);
			listItems.push_back(item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	return listItems.size();
}

int getLinerMonth3Items(QVector<stLinerItem>& listItems,const QList<qRcvHistoryData*>& historys)
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
				stLinerItem item;
				getLinerItemByDays(item,monthHis);
				listItems.push_back(item);
				monthHis.clear();
			}
		}
		else if(((tmDate.month()-1)/3)!=((iCurMonth-1)/3))
		{
			iCurMonth = tmDate.month();

			stLinerItem item;
			getLinerItemByDays(item,monthHis);
			listItems.push_back(item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	return listItems.size();
}

int getLinerYearItems(QVector<stLinerItem>& listItems,const QList<qRcvHistoryData*>& historys)
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
				stLinerItem item;
				getLinerItemByDays(item,monthHis);
				listItems.push_back(item);
				monthHis.clear();
			}
		}
		monthHis.push_back(pHistory);
	}

	return listItems.size();
}

CKLineWidget::CKLineWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::KLine)
	, m_pMenuCustom(0)
	, m_pActShowMain(0)
	, m_typeCircle(CKLineWidget::Day)
	, m_pStockItem(0)
	, m_iShowCount(100)
	, m_pLinerMain(0)
	, m_pCurrentLiner(0)
	, m_bShowMax(false)
	, m_iTitleHeight(16)
	, m_iCoorYWidth(50)
	, m_iCoorXHeight(16)
	, m_iMainLinerHeight(200)
	, m_pScriptEngine(0)
{
	{
		//初始化脚本解释器
		m_pScriptEngine = new QScriptEngine;
		/*
		qScriptRegisterMetaType<stLinerItem>(m_pScriptEngine, linerItem2ScriptValue, scriptValue2LinerItem);
		QScriptValue ctor = m_pScriptEngine->newFunction(createLinerItem);
		m_pScriptEngine->globalObject().setProperty("stLinerItem", ctor);
		qScriptRegisterSequenceMetaType<QVector<stLinerItem>>(m_pScriptEngine);
		*/
		qScriptRegisterSequenceMetaType<QVector<float>>(m_pScriptEngine);

		{
			//加载预置脚本
			QFile file("..\\RStockLiners.js");
			if(!file.open(QFile::ReadOnly))
				qDebug()<<"load script error.";
			QString qsScript = file.readAll();
			QScriptValue v = m_pScriptEngine->evaluate(QScriptProgram(qsScript));
			qDebug()<<"loaded script, info:"<<v.toString()<<"\r\n";
		}
	}

	m_pLinerMain = new CMultiLiner(CMultiLiner::MainKLine,m_pScriptEngine,"");
	m_vSizes.push_back(60);

	//设置菜单
	m_pMenuCustom = new QMenu("K线图操作");
	m_pMenuCustom->addAction(tr("设置股票代码"),this,SLOT(onSetStockCode()));
	{
		//设置当前K线图的显示周期
		QMenu* pMenuCircle = m_pMenuCustom->addMenu(tr("周期设置"));
		pMenuCircle->addAction(tr("1分钟分时图"),this,SLOT(onSetCircle()))->setData(Min1);
		pMenuCircle->addAction(tr("5分钟分时图"),this,SLOT(onSetCircle()))->setData(Min5);
		pMenuCircle->addAction(tr("15分钟分时图"),this,SLOT(onSetCircle()))->setData(Min15);
		pMenuCircle->addAction(tr("30分钟分时图"),this,SLOT(onSetCircle()))->setData(Min30);
		pMenuCircle->addAction(tr("60分钟分时图"),this,SLOT(onSetCircle()))->setData(Min60);

		pMenuCircle->addAction(tr("日线图"),this,SLOT(onSetCircle()))->setData(Day);
		pMenuCircle->addAction(tr("周线图"),this,SLOT(onSetCircle()))->setData(Week);
		pMenuCircle->addAction(tr("月线图"),this,SLOT(onSetCircle()))->setData(Month);
		pMenuCircle->addAction(tr("季线图"),this,SLOT(onSetCircle()))->setData(Month3);
		pMenuCircle->addAction(tr("年线图"),this,SLOT(onSetCircle()))->setData(Year);
	}
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
	delete m_pScriptEngine;

	delete m_pMenuCustom;
}

bool CKLineWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	m_vSizes.clear();

	//是否显示主图
	if(eleWidget.hasAttribute("mainliner"))
	{
		m_pActShowMain->setChecked(eleWidget.attribute("mainliner").toInt());
	}

	//当前显示的周期
	if(eleWidget.hasAttribute("circle"))
	{
		m_typeCircle = static_cast<KLineCircle>(eleWidget.attribute("circle").toInt());
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
				CMultiLiner* pLiner = new CMultiLiner(t,m_pScriptEngine,"");

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
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;
	if(m_pStockItem)
	{
		//当前的股票值
		QDomElement eleCode = doc.createElement("code");
		eleCode.appendChild(doc.createTextNode(m_pStockItem->getCode()));
		eleWidget.appendChild(eleCode);

		//是否显示主图
		eleWidget.setAttribute("mainliner",m_pActShowMain->isChecked());

		//显示的周期
		eleWidget.setAttribute("circle",m_typeCircle);

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

void CKLineWidget::setStockCode( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);
	if(pItem)
	{
		disconnect(this,SLOT(updateKLine(const QString&)));		//移除所有和 updateKLine关联的 信号/槽

		m_pStockItem = pItem;

		//建立更新机制
		connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
		connect(pItem,SIGNAL(stockItemMinuteChanged(const QString&)),this,SLOT(updateKLine(const QString&)));

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

	/*画头部*/
	QRect rtTitle = QRect(rtClient.left(),rtClient.top(),rtClient.width(),m_iTitleHeight);
	drawTitle(p,rtTitle);

	/*画X坐标轴*/
	QRect rtCoordX = QRect(rtClient.left(),rtClient.bottom()-m_iCoorXHeight+1,rtClient.width()-m_iCoorYWidth,m_iCoorXHeight);
	drawCoordX(p,rtCoordX);
	/*画右下角的两个按钮*/
	QRect rtShowBtns = QRect(rtClient.right()-m_iCoorYWidth,rtClient.bottom()-m_iCoorXHeight,m_iCoorYWidth,m_iCoorXHeight);
	drawShowBtns(p,rtShowBtns);

	rtClient.adjust(3,m_iTitleHeight,-m_iCoorYWidth-2,-m_iCoorXHeight);			//改变为可画图区域的大小


	QPoint ptCurMouse = this->mapFromGlobal(QCursor::pos());
	CMultiLiner* pCurLiner = 0;		//当前鼠标所在位置的MultiLiner

	int iCurY = rtClient.top();		//当前绘制到的位置
	int iSizeIndex = 0;
	/*绘制主图*/
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

		/*绘制y轴的值*/
		float fV = pCurLiner->getValueByY(ptCurMouse.y());
		{
			if(fV>10000.0)
				p.drawText(QRect(rtClient.right()+10,ptCurMouse.y()-5,50,20),Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(fV,0,'g',2));
			else
				p.drawText(QRect(rtClient.right()+10,ptCurMouse.y()-5,50,20),Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(fV,0,'f',2));				
		}

		/*绘制x轴的值*/
		QRectF rtCurLiner = pCurLiner->getRect();
		int iAbsIndex = (rtCurLiner.right() - ptCurMouse.x())/fItemWidth;
		int iIndex = listItems.size()-iAbsIndex-1;
		if(iIndex>=0&&iIndex<listItems.size())
		{
			p.setPen(QColor(0,255,255));
			if(m_typeCircle<Day)
			{
				QTime tmTime = QDateTime::fromTime_t(listItems[iIndex].time).time();
				p.drawText(QRect(ptCurMouse.x(),rtClient.bottom(),100,20),Qt::AlignCenter,tmTime.toString("HH:mm:ss"));
			}
			else
			{
				QDate tmDate = QDateTime::fromTime_t(listItems[iIndex].time).date();
				p.drawText(QRect(ptCurMouse.x()-50,rtClient.bottom(),100,20),Qt::AlignCenter,tmDate.toString("yyyy/MM/dd"));
			}
		}
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
	int iLastN = (fEnd - e->posF().x())/fItemWidth;
	if(iLastN<0||iLastN>=m_iShowCount)
		return QToolTip::hideText();

	int iIndex = listItems.size()-iLastN-1;
	if(iIndex<0)
		return QToolTip::hideText();

	QString qsTooltip;		//Tips
	stLinerItem item = listItems[iIndex];
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

	QToolTip::showText(e->globalPos(),qsTooltip,this);
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

QMenu* CKLineWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

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
}

void CKLineWidget::onClickedAddShow()
{
	if((m_iShowCount+10)<=listItems.size())
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

void CKLineWidget::onSetCircle()
{
	//设置当前的显示周期
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	m_typeCircle = static_cast<KLineCircle>(pAct->data().toInt());
	resetTmpData();
}

void CKLineWidget::onAddDeputy()
{
	CMultiLiner* pMultiLiner = new CMultiLiner(CMultiLiner::Deputy,m_pScriptEngine,"CLOSE;");
	m_listLiners.push_back(pMultiLiner);
	m_vSizes.push_back(20);
	update();
}

void CKLineWidget::onAddVolume()
{
	CMultiLiner* pMultiLiner = new CMultiLiner(CMultiLiner::VolumeLine,m_pScriptEngine,"");
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

void CKLineWidget::drawCoordX( QPainter& p,const QRect& rtCoordX )
{
	if(!rtCoordX.isValid())
		return;

	//绘制单线
	float fTopLine = rtCoordX.top()+1;
	p.setPen(QColor(255,0,0));
	p.drawLine(rtCoordX.right(),fTopLine,rtCoordX.left(),fTopLine);


	//左空余3Pix，右空余2Pix
	fItemWidth = float(rtCoordX.width()-2-3)/float(m_iShowCount);
	//从右向左绘制
	float fCurX = rtCoordX.right();
	float fLastX = fCurX;
	//绘制刻度
	if(m_typeCircle<Day)
	{
		//分时数据的横轴绘制（需绘制精度为小时）
		int iCurHour = 0;
		int iCurIndex = listItems.size()-1;
		int iCount = 0;
		while(iCount<m_iShowCount)
		{
			QTime tmTime = QDateTime::fromTime_t(listItems[iCurIndex].time).time();
			if(tmTime.hour()!=iCurHour)
			{
				if((fLastX-fCurX)>16)
				{
					p.drawLine(fCurX,fTopLine,fCurX,rtCoordX.bottom()-1);
					p.drawText(fCurX+2,rtCoordX.bottom()-3,QString("%1时").arg(iCurHour));
					fLastX = fCurX;
				}

				iCurHour = tmTime.hour();
			}

			fCurX -= fItemWidth;
			++iCount;
			--iCurIndex;
		}
	}
	else
	{
		//日线数据的横轴绘制（只需绘制精度为月份即可）
		int iCurMonth = 0;
		int iCurIndex = listItems.size()-1;
		int iCount = 0;
		while(iCount<m_iShowCount)
		{
			QDate tmDate = QDateTime::fromTime_t(listItems[iCurIndex].time).date();
			if(tmDate.month()!=iCurMonth)
			{				
				if((fLastX-fCurX)>16)
				{
					p.drawLine(fCurX,fTopLine,fCurX,rtCoordX.bottom()-1);
					p.drawText(fCurX+2,rtCoordX.bottom()-3,QString("%1").arg(iCurMonth));
					fLastX = fCurX;
				}

				iCurMonth = tmDate.month();
			}

			fCurX -= fItemWidth;
			++iCount;
			--iCurIndex;
		}
	}
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
	listItems.clear();
}

void CKLineWidget::resetTmpData()
{
	if(m_iShowCount<1)
		m_iShowCount = 100;
	clearTmpData();
	if(m_typeCircle<Day)
	{
		//获取分钟数据，进行计算
		QList<qRcvMinuteData*> minutes = m_pStockItem->getMinuteList();
		if(m_typeCircle == Min1)
		{
			getLinerMinItems(listItems,minutes);
		}
		else if(m_typeCircle == Min5)
		{
			getLinerMinItems(listItems,minutes,5);
		}
		else if(m_typeCircle == Min15)
		{
			getLinerMinItems(listItems,minutes,15);
		}
		else if(m_typeCircle == Min30)
		{
			getLinerMinItems(listItems,minutes,30);
		}
		else if(m_typeCircle == Min60)
		{
			getLinerMinItems(listItems,minutes,60);
		}
	}
	else
	{
		//获取日线数据
		QList<qRcvHistoryData*> historys = m_pStockItem->getHistoryList();
		if(m_typeCircle == Day)
		{
			getLinerDayItems(listItems,historys);
		}
		else if(m_typeCircle == DayN)
		{
			//目前未使用
		//	getLinerItem(listItems,historys,3);
		}
		else if(m_typeCircle == Week)
		{
			getLinerWeekItems(listItems,historys);
		}
		else if(m_typeCircle == Month)
		{
			getLinerMonthItems(listItems,historys);
		}
		else if(m_typeCircle == Month3)
		{
			getLinerMonth3Items(listItems,historys);
		}
		else if(m_typeCircle == Year)
		{
			getLinerYearItems(listItems,historys);
		}

		{
			//清除获取的日线数据
			foreach(qRcvHistoryData* p,historys)
				delete p;
			historys.clear();
		}
	}
	if(listItems.size()<m_iShowCount)
		m_iShowCount = listItems.size();


	{
		/*将更新后的数据设置到脚本引擎中*/
		QVector<float> vOpen;
		QVector<float> vHigh;
		QVector<float> vLow;
		QVector<float> vClose;
		QVector<float> vVolume;
		QVector<float> vAmount;
		QVector<float> vAdvance;
		QVector<float> vDecline;
		foreach(const stLinerItem& i,listItems)
		{
			vOpen.push_back(i.fOpen);
			vHigh.push_back(i.fHigh);
			vLow.push_back(i.fLow);
			vClose.push_back(i.fClose);
			vVolume.push_back(i.fVolume);
			vAmount.push_back(i.fAmount);
			vAdvance.push_back(i.wAdvance);
			vDecline.push_back(i.wDecline);
		}

		QTime tmNow = QTime::currentTime();
		//	m_pScriptEngine->globalObject().setProperty("ITEMS",m_pScriptEngine->toScriptValue(listItems));
		//	qDebug()<<m_pScriptEngine->evaluate(QScriptProgram("CalcBaseData();")).toString();
		m_pScriptEngine->globalObject().setProperty("OPEN",m_pScriptEngine->toScriptValue(vOpen));
		m_pScriptEngine->globalObject().setProperty("HIGH",m_pScriptEngine->toScriptValue(vHigh));
		m_pScriptEngine->globalObject().setProperty("LOW",m_pScriptEngine->toScriptValue(vLow));
		m_pScriptEngine->globalObject().setProperty("CLOSE",m_pScriptEngine->toScriptValue(vClose));
		m_pScriptEngine->globalObject().setProperty("VOLUME",m_pScriptEngine->toScriptValue(vVolume));
		m_pScriptEngine->globalObject().setProperty("AMOUNT",m_pScriptEngine->toScriptValue(vAmount));
		m_pScriptEngine->globalObject().setProperty("ADVANCE",m_pScriptEngine->toScriptValue(vAdvance));
		m_pScriptEngine->globalObject().setProperty("DECLINE",m_pScriptEngine->toScriptValue(vDecline));
		qDebug()<<"set "<<m_pStockItem->getCode()<<" data to script, use ms:"<<tmNow.msecsTo(QTime::currentTime());
	}
	//更新绘制中的数据
	if(m_pActShowMain->isChecked())
		m_pLinerMain->updateData();
	foreach(CMultiLiner* p,m_listLiners)
		p->updateData();

	//更新界面
	update();
}
