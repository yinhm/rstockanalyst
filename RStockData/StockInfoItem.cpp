/************************************************************************/
/* 文件名称：StockInfoItem.cpp
/* 创建时间：2013-11-08 09:28
/*
/* 描    述：用于存储单只股票数据
/************************************************************************/

#include "StdAfx.h"
#include "StockInfoItem.h"
#include "DataEngine.h"
#include "Hz2Py.h"

CStockInfoItem::CStockInfoItem( const QString& code, WORD market )
	: fNowVolume(FLOAT_NAN)
	, pLastReport(NULL)
	, fIncreaseSpeed(FLOAT_NAN)
	, fBuyVolume(FLOAT_NAN)
	, fSellVolume(FLOAT_NAN)
	, fBuyVolume3(FLOAT_NAN)
	, fSellVolume3(FLOAT_NAN)
	, fIncrease(FLOAT_NAN)
	, fVolumeRatio(FLOAT_NAN)
	, fTurnRatio(FLOAT_NAN)
	, fPriceFluctuate(FLOAT_NAN)
	, fAmplitude(FLOAT_NAN)
	, fAvePrice(FLOAT_NAN)
	, fCommRatio(FLOAT_NAN)
	, fCommSent(FLOAT_NAN)
	, fPERatio(FLOAT_NAN)
	, fLTSZ(FLOAT_NAN)
	, fSellVOL(0.0)
	, fBuyVOL(0.0)
	, fLastMinVolume(0)
	, fLastMinAmount(0)
	, fLast5MinVolume(0)
	, fLast5MinAmount(0)
	, fLastCmpPrice(0)
	, m_bIsIndex(false)
{
	pCurrentReport = new qRcvReportData;
	pLastReport = new qRcvReportData;
	pCurrent5Min = new RStockData;
	qsCode = code;
	wMarket = market;
	qsMarket = CDataEngine::getMarketStr(wMarket);
	qsOnly = qsCode+qsMarket;
	m_bIsIndex = CDataEngine::isIndexStock(qsOnly);

	initStockItem();
}

CStockInfoItem::CStockInfoItem( const qRcvBaseInfoData& info )
	: fNowVolume(FLOAT_NAN)
	, pLastReport(NULL)
	, fIncreaseSpeed(FLOAT_NAN)
	, fBuyVolume(FLOAT_NAN)
	, fSellVolume(FLOAT_NAN)
	, fBuyVolume3(FLOAT_NAN)
	, fSellVolume3(FLOAT_NAN)
	, fIncrease(FLOAT_NAN)
	, fVolumeRatio(FLOAT_NAN)
	, fTurnRatio(FLOAT_NAN)
	, fPriceFluctuate(FLOAT_NAN)
	, fAmplitude(FLOAT_NAN)
	, fAvePrice(FLOAT_NAN)
	, fCommRatio(FLOAT_NAN)
	, fCommSent(FLOAT_NAN)
	, fPERatio(FLOAT_NAN)
	, fLTSZ(FLOAT_NAN)
	, fSellVOL(0.0)
	, fBuyVOL(0.0)
	, fLastMinVolume(0)
	, fLastMinAmount(0)
	, fLast5MinVolume(0)
	, fLast5MinAmount(0)
	, fLastCmpPrice(0)
	, m_bIsIndex(false)
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));
	pCurrentReport = new qRcvReportData;
	pLastReport = new qRcvReportData;
	pCurrent5Min = new RStockData;

	qsCode = QString::fromLocal8Bit(info.code);
	wMarket = info.wMarket;
	qsMarket = CDataEngine::getMarketStr(wMarket);
	qsOnly = qsCode+qsMarket;

	m_bIsIndex = CDataEngine::isIndexStock(qsOnly);

	initStockItem();
}

CStockInfoItem::~CStockInfoItem(void)
{
	delete pLastReport;
	delete pCurrentReport;

	{
		QMap<time_t,qRcvPowerData*>::iterator iter = mapPowers.begin();			//除权数据
		while(iter!=mapPowers.end())
		{
			delete iter.value();
			++iter;
		}
		mapPowers.clear();
	}
}

void CStockInfoItem::setReport( qRcvReportData* p )
{
	if(p->tmTime<=pCurrentReport->tmTime||p->fNewPrice<0.01)
	{
		return;
	}

	pLastReport->resetItem(pCurrentReport);
	pCurrentReport->resetItem(p);
	CDataEngine::setCurrentTime(pCurrentReport->tmTime);

	//设置对比价格
	fLastCmpPrice = p->fNewPrice;

	updateItemInfo();
}

void CStockInfoItem::setReport( RCV_REPORT_STRUCTExV3* p )
{
	if(p->m_time<=pCurrentReport->tmTime)
		return;

	if( ((p->m_time+8*3600)%(3600*24))<(9*3600+26*60) )
	{
		//竞价时段
		pLastReport->resetItem(pCurrentReport);
		pCurrentReport->resetItem(p);
		CDataEngine::setCurrentTime(pCurrentReport->tmTime);

		//将新的Report数据添加到竞价数据中
		qRcvFenBiData* pFenBi = new qRcvFenBiData(pCurrentReport);
		appendJingJias(pFenBi);
		updateItemInfo();
		return;
	}
	else if(p->m_fNewPrice<0.01)
	{
		return;
	}

	if((p->m_time - pCurrentReport->tmTime) > 60)
	{
		pLastReport->resetItem(p);
		pCurrentReport->resetItem(p);
		fLastMinVolume = pLastReport->fVolume;
		fLastMinAmount = pLastReport->fAmount;
	}
	else
	{
		pLastReport->resetItem(pCurrentReport);
		pCurrentReport->resetItem(p);
	}
	CDataEngine::setCurrentTime(pCurrentReport->tmTime);

	//将新的Report数据添加到分笔数据中
	qRcvFenBiData* pFenBi = new qRcvFenBiData(pCurrentReport);

	updateItemInfo();
	appendFenBis(QList<qRcvFenBiData*>()<<pFenBi);

	{
		//计算1分钟数据
		if(pCurrentMin->tmTime>0)
		{
			if((p->m_time/60) > (pCurrentMin->tmTime/60))
			{
				//追加到1分钟数据中，并重新对当前1分钟数据分配内存
				appendMinData(pCurrentMin);
				pCurrentMin = new RStockData;
				//将最后的5分钟数据进行保存
				fLastMinVolume = pLastReport->fVolume;
				fLastMinAmount = pLastReport->fAmount;
			}
		}

		//将新数据跟当前的1分钟数据进行整合
		if(pCurrentMin->tmTime>0)
		{
//			pCurrentMin->tmTime = p->m_time/60*60+59;			//设置为最后一个周期
			pCurrentMin->fClose = p->m_fNewPrice;
			if(pCurrentMin->fHigh<p->m_fNewPrice)
				pCurrentMin->fHigh = p->m_fNewPrice;
			if(pCurrentMin->fLow>p->m_fNewPrice)
				pCurrentMin->fLow = p->m_fNewPrice;
			pCurrentMin->fAmount = p->m_fAmount - fLastMinAmount;
			pCurrentMin->fVolume = p->m_fVolume - fLastMinVolume;
		}
		else
		{
			pCurrentMin->tmTime = p->m_time/60*60+59;			//设置为最后一个周期
			pCurrentMin->fClose = p->m_fNewPrice;
			pCurrentMin->fHigh = p->m_fNewPrice;
			pCurrentMin->fLow = p->m_fNewPrice;
			pCurrentMin->fOpen = p->m_fNewPrice;
			pCurrentMin->fAmount = p->m_fAmount - fLastMinAmount;
			pCurrentMin->fVolume = p->m_fVolume - fLastMinVolume;
		}
	}
	{
		//计算5分钟数据
		if(pCurrent5Min->tmTime>0)
		{
			if((p->m_time/300) > (pCurrent5Min->tmTime/300))
			{
				//追加到5分钟数据中，并重新对当前5分钟数据分配内存
				vCmpPrices.clear();
				appendToday5MinData(pCurrent5Min);
				pCurrent5Min = new RStockData;
				//将最后的5分钟数据进行保存
				fLast5MinVolume = pLastReport->fVolume;
				fLast5MinAmount = pLastReport->fAmount;
			}
		}

		{
			//计算最近5分钟内的涨跌比
			if(fLastCmpPrice<0.001)
				fLastCmpPrice = p->m_fNewPrice;
			if(fLastCmpPrice>0.001)
			{
				float fInc = (p->m_fNewPrice-fLastCmpPrice)/fLastCmpPrice*100;
				if(fInc>0.9)
				{
					vCmpPrices.append(fInc+0.5);
					fLastCmpPrice = p->m_fNewPrice;
				}
				else if(fInc<-0.9)
				{
					vCmpPrices.append(fInc-0.5);
					fLastCmpPrice = p->m_fNewPrice;
				}
			}
		}
		//将新数据跟当前的5分钟数据进行整合
		if(pCurrent5Min->tmTime>0)
		{
			//			pCurrent5Min->tmTime = p->m_time/300*300+299;			//设置为最后一个周期
			pCurrent5Min->fClose = p->m_fNewPrice;
			if(pCurrent5Min->fHigh<p->m_fNewPrice)
				pCurrent5Min->fHigh = p->m_fNewPrice;
			if(pCurrent5Min->fLow>p->m_fNewPrice)
				pCurrent5Min->fLow = p->m_fNewPrice;
			pCurrent5Min->fAmount = p->m_fAmount - fLast5MinAmount;
			pCurrent5Min->fVolume = p->m_fVolume - fLast5MinVolume;
		}
		else
		{
			pCurrent5Min->tmTime = p->m_time/300*300+299;			//设置为最后一个周期
			pCurrent5Min->fClose = p->m_fNewPrice;
			pCurrent5Min->fHigh = p->m_fNewPrice;
			pCurrent5Min->fLow = p->m_fNewPrice;
			pCurrent5Min->fOpen = p->m_fNewPrice;
			pCurrent5Min->fAmount = p->m_fAmount - fLast5MinAmount;
			pCurrent5Min->fVolume = p->m_fVolume - fLast5MinVolume;
		}
	}
}

QList<qRcvPowerData*> CStockInfoItem::getPowerList()
{
	return mapPowers.values();
}

void CStockInfoItem::appendPowers( const QList<qRcvPowerData*>& list )
{
	foreach(qRcvPowerData* p,list)
	{
		if(mapPowers.contains(p->tmTime))
		{
			qRcvPowerData* pBefore = mapPowers[p->tmTime];
			if(pBefore!=p)
				delete pBefore;
		}
		mapPowers[p->tmTime] = p;
	}
}


void CStockInfoItem::appendFenBis( const QList<qRcvFenBiData*>& list )
{
	CAbstractStockItem::appendFenBis(list);
	resetBuySellVOL();
	return;
//	CDataEngine::getDataEngine()->exportFenBiData(qsCode,mapFenBis.values());
}

void CStockInfoItem::appendJingJias( qRcvFenBiData* pJingJia )
{
	if(mapJingJias.contains(pJingJia->tmTime))
	{
		delete pJingJia;
	}
	else
	{
		mapJingJias.insert(pJingJia->tmTime,pJingJia);
	}
	return;
}

QList<tagRStockData*> CStockInfoItem::getToday5MinList()
{
	QList<tagRStockData*> list = mapToday5MinDatas.values();
	if(pCurrent5Min->tmTime>0 && (!mapToday5MinDatas.contains(pCurrent5Min->tmTime)))
	{
		list.push_back(pCurrent5Min);
	}
	return list;
}

RStockData* CStockInfoItem::getTodayMinData( const time_t& tmTime )
{
	if(mapMinDatas.contains(tmTime))
		return mapMinDatas[tmTime];

	return 0;
}

void CStockInfoItem::recalcMinData()
{
	//重新计算5分钟数据
	//不计算集合竞价时间
	time_t tmBegin = (CDataEngine::getCurrentTime()/(3600*24)*(3600*24))+1800*3-1;
	QMap<time_t,qRcvFenBiData*>::iterator iter = mapFenBis.upperBound(tmBegin);
	RStockData* pMin = new RStockData();
	qRcvFenBiData* pLastFenBi = 0;
	float fLastVolume = 0;
	float fLastAmount = 0;
	while (iter!=mapFenBis.end())
	{
		qRcvFenBiData* pFenBi = iter.value();

		//计算5分钟数据
		if(pMin->tmTime>0)
		{
			if((pFenBi->tmTime/60) > (pMin->tmTime/60))
			{
				//追加到5分钟数据中，并重新对当前5分钟数据分配内存
				appendMinData(pMin);
				pMin = new RStockData;
				//将最后的5分钟数据进行保存
				if(pLastFenBi)
				{
					fLastVolume = pLastFenBi->fVolume;
					fLastAmount = pLastFenBi->fAmount;
				}
			}
		}
		//将新数据跟当前的5分钟数据进行整合
		if(pMin->tmTime>0)
		{
			pMin->fClose = pFenBi->fPrice;
			if(pMin->fHigh<pFenBi->fPrice)
				pMin->fHigh = pFenBi->fPrice;
			if(pMin->fLow>pFenBi->fPrice)
				pMin->fLow = pFenBi->fPrice;
			pMin->fAmount = pFenBi->fAmount - fLastAmount;
			pMin->fVolume = pFenBi->fVolume - fLastVolume;
		}
		else
		{
			pMin->tmTime = pFenBi->tmTime/60*60+59;
			pMin->fClose = pFenBi->fPrice;
			pMin->fHigh = pFenBi->fPrice;
			pMin->fLow = pFenBi->fPrice;
			pMin->fOpen = pFenBi->fPrice;
			pMin->fAmount = pFenBi->fAmount - fLastAmount;
			pMin->fVolume = pFenBi->fVolume - fLastVolume;
		}

		pLastFenBi = pFenBi;
		++iter;
	}

	//将最后一笔分笔数据加入到队列中
	if(pMin->tmTime>0)
	{
		appendMinData(pMin);
	}
	emit stockItemFenBiChanged(qsCode);
}

void CStockInfoItem::recalc5MinData()
{
	//重新计算5分钟数据
	QMap<time_t,qRcvFenBiData*>::iterator iter = mapFenBis.begin();
	RStockData* p5Min = new RStockData();
	qRcvFenBiData* pLastFenBi = 0;
	float fLastVolume = 0;
	float fLastAmount = 0;
	while (iter!=mapFenBis.end())
	{
		qRcvFenBiData* pFenBi = iter.value();

		//计算5分钟数据
		if(p5Min->tmTime>0)
		{
			if((pFenBi->tmTime/300) > (p5Min->tmTime/300))
			{
				//追加到5分钟数据中，并重新对当前5分钟数据分配内存
				appendToday5MinData(p5Min);
				p5Min = new RStockData;
				//将最后的5分钟数据进行保存
				if(pLastFenBi)
				{
					fLastVolume = pLastFenBi->fVolume;
					fLastAmount = pLastFenBi->fAmount;
				}
			}
		}
		//将新数据跟当前的5分钟数据进行整合
		if(p5Min->tmTime>0)
		{
			p5Min->fClose = pFenBi->fPrice;
			if(p5Min->fHigh<pFenBi->fPrice)
				p5Min->fHigh = pFenBi->fPrice;
			if(p5Min->fLow>pFenBi->fPrice)
				p5Min->fLow = pFenBi->fPrice;
			p5Min->fAmount = pFenBi->fAmount - fLastAmount;
			p5Min->fVolume = pFenBi->fVolume - fLastVolume;
		}
		else
		{
			p5Min->tmTime = pFenBi->tmTime/300*300+299;
			p5Min->fClose = pFenBi->fPrice;
			p5Min->fHigh = pFenBi->fPrice;
			p5Min->fLow = pFenBi->fPrice;
			p5Min->fOpen = pFenBi->fPrice;
			p5Min->fAmount = pFenBi->fAmount - fLastAmount;
			p5Min->fVolume = pFenBi->fVolume - fLastVolume;
		}

		pLastFenBi = pFenBi;
		++iter;
	}

	//将最后一笔分笔数据加入到队列中
	if(p5Min->tmTime>0)
	{
		appendToday5MinData(p5Min);
	}
	emit stockItemFenBiChanged(qsCode);
}


void CStockInfoItem::setBaseInfo( const qRcvBaseInfoData& info )
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));


	//市盈率
	if(pCurrentReport&&baseInfo.fMgsy>0)
		fPERatio = pCurrentReport->fNewPrice/baseInfo.fMgsy/2;	//不除以2则跟飞狐的数据不匹配

	//流通市值
	if(pCurrentReport&&baseInfo.fLtAg>0)
		fLTSZ = baseInfo.fLtAg*pCurrentReport->fNewPrice;

	emit stockItemReportChanged(qsOnly);
}


QString CStockInfoItem::getName() const
{

	return qsName;
}

float CStockInfoItem::getIncrease() const
{
	return fIncrease;
}

float CStockInfoItem::getVolumeRatio() const
{
	return fVolumeRatio;
}

float CStockInfoItem::getTurnRatio() const
{
	return fTurnRatio;
}

float CStockInfoItem::getLastClose() const
{
	if(pCurrentReport)
		return pCurrentReport->fLastClose;
	return FLOAT_NAN;
}

float CStockInfoItem::getOpenPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fOpen;

	return FLOAT_NAN;
}

float CStockInfoItem::getHighPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fHigh;

	return FLOAT_NAN;
}

float CStockInfoItem::getLowPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fLow;

	return FLOAT_NAN;
}

float CStockInfoItem::getNewPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fNewPrice;

	return FLOAT_NAN;
}

float CStockInfoItem::getTotalVolume() const
{
	if(pCurrentReport)
		return pCurrentReport->fVolume;

	return FLOAT_NAN;
}

float CStockInfoItem::getTotalAmount() const
{
	if(pCurrentReport)
		return pCurrentReport->fAmount;

	return FLOAT_NAN;
}

float CStockInfoItem::getNowVolume() const
{
	return fNowVolume;
}

float CStockInfoItem::getIncSpeed() const
{
	//例如，某个股票5分钟之前的股价是10元，而现在的价格是10.1元，则这个股票的5分钟涨速为：
	return fIncreaseSpeed;
}

float CStockInfoItem::getPriceFluctuate() const
{
	//今日涨跌幅（%）=（当前价-前收盘价）/ 前收盘价 * 100
	return fPriceFluctuate;
}

float CStockInfoItem::getAmplitude() const
{
	/*
		一、股票振幅=当期最高价/当期最低价×100%-100%
		二、股票振幅=（当期最高价－当期最低价)/上期收盘价×100%
		依据飞狐，采用第二种计算方法。
	*/
	return fAmplitude;
}

float CStockInfoItem::getAvePrice() const
{
	return fAvePrice;
}

float CStockInfoItem::getPERatio() const
{
	//市盈率（静态市盈率）=最新价／每股收益
	return fPERatio;
}

float CStockInfoItem::getLTSZ() const
{
	//流通市值 总市值就是以目前股票市价乘以流通股，流通值是以目前股票市价乘以流[1]通股本。
	return fLTSZ;
}

float CStockInfoItem::getZGB() const
{
	//总股本 总市值就是以目前股票市价乘以总股本，流通值是以目前股票市价乘以流[1]通股本。
	return fZGB;
}

float CStockInfoItem::getSellVOL() const
{
	//外盘量
	return fSellVOL;
}

float CStockInfoItem::getBuyVOL() const
{
	//内盘量
	return fBuyVOL;
}

float CStockInfoItem::getBIDVOL() const
{
	//委买量
	return fBuyVolume;
}

float CStockInfoItem::getASKVOL() const
{
	//委卖量
	return fSellVolume;
}

float CStockInfoItem::getBIDVOL3() const
{
	return fBuyVolume3;
}

float CStockInfoItem::getASKVOL3() const
{
	return fSellVolume3;
}

float CStockInfoItem::getCommRatio() const
{
	//委比
	//(委买手数－委卖手数)/(委买手数+委卖手数)*100
	return fCommRatio;
}

float CStockInfoItem::getCommSent() const
{
	//委差
	return fCommSent;
}


float CStockInfoItem::getLast5Volume()
{
	//过去5日的成交总量
	return fLast5Volume;
}

float CStockInfoItem::getMgjzc()
{
	//获取每股净资产
	return baseInfo.fMgjz;
}

float CStockInfoItem::getMgsy()
{
	//获取每股收益
	return baseInfo.fMgsy;
}


void CStockInfoItem::updateItemInfo()
{
	//设置股票名称
	if(qsName.isEmpty())
	{
		qsName = pCurrentReport->qsName;
		//更新词库表中的简拼
		shortName = CHz2Py::getHzFirstLetter(qsName);
	}
	if(baseInfo.code[0]==0)
	{
		strcpy_s(baseInfo.code,qsCode.toLocal8Bit().data());
		baseInfo.wMarket = wMarket;
	}

	//涨幅
	if(pCurrentReport->fNewPrice>0.0 && pCurrentReport->fLastClose>0.0)
		fIncrease = (pCurrentReport->fNewPrice-pCurrentReport->fLastClose)*100.0/pCurrentReport->fLastClose;
	/*换手率（仓差）
		换手率=某一段时期内的成交量/发行总股数*100%
		（在中国：成交量/流通总股数*100%）
	*/
	if(baseInfo.fLtAg>0)
		fTurnRatio = ((pCurrentReport->fVolume/100)/baseInfo.fLtAg)*100;

	//市盈率
	if(baseInfo.fMgsy>0)
		fPERatio = pCurrentReport->fNewPrice/baseInfo.fMgsy/2;	//不除以2则跟飞狐的数据不匹配
	//流通市值
	if(baseInfo.fLtAg>0)
		fLTSZ = baseInfo.fLtAg*pCurrentReport->fNewPrice;
	if(baseInfo.fZgb>0)
		fZGB = baseInfo.fZgb*pCurrentReport->fNewPrice;

	//涨跌，价格波动
	fPriceFluctuate = (pCurrentReport->fNewPrice-pCurrentReport->fLastClose);

	//振幅
	fAmplitude = (pCurrentReport->fHigh-pCurrentReport->fLow)/pCurrentReport->fLastClose;

	//均价
	fAvePrice = (pCurrentReport->fAmount/pCurrentReport->fVolume)/100;

	{
		//委买量计算
		fBuyVolume = 0.0;
		fBuyVolume3 = 0.0;
		fBuyVolume += pCurrentReport->fBuyVolume[0];
		fBuyVolume += pCurrentReport->fBuyVolume[1];
		fBuyVolume += pCurrentReport->fBuyVolume[2];
		fBuyVolume += pCurrentReport->fBuyVolume4;
		fBuyVolume += pCurrentReport->fBuyVolume5;


		fBuyVolume3 += pCurrentReport->fBuyVolume[0];
		fBuyVolume3 += pCurrentReport->fBuyVolume[1];
		fBuyVolume3 += pCurrentReport->fBuyVolume[2];

		//委卖量计算
		fSellVolume = 0.0;
		fSellVolume3 = 0.0;
		fSellVolume += pCurrentReport->fSellVolume[0];
		fSellVolume += pCurrentReport->fSellVolume[1];
		fSellVolume += pCurrentReport->fSellVolume[2];
		fSellVolume += pCurrentReport->fSellVolume4;
		fSellVolume += pCurrentReport->fSellVolume5;

		fSellVolume3 += pCurrentReport->fSellVolume[0];
		fSellVolume3 += pCurrentReport->fSellVolume[1];
		fSellVolume3 += pCurrentReport->fSellVolume[2];

		//委比
		if(pCurrentReport&&(fBuyVolume>0||fSellVolume>0))
		{
			fCommRatio = ((fBuyVolume-fSellVolume)/(fBuyVolume+fSellVolume))*100;
			fCommSent = fBuyVolume-fSellVolume;
		}
		else
		{
			fCommRatio = FLOAT_NAN;
			fCommSent = FLOAT_NAN;
		}
	}

	{
		//内外盘计算
		//if(pCurrentReport->fNewPrice>0&&pLastReport->fNewPrice>0)
		//{
		//	if(QDateTime::fromTime_t(pCurrentReport->tmTime).date()>QDateTime::fromTime_t(pLastReport->tmTime).date())
		//	{
		//		fSellVOL = fBuyVOL = 0;
		//	}
		//}
		fNowVolume = (pCurrentReport->fVolume)-(fBuyVOL+fSellVOL);
		//if(fNowVolume>0)
		//{
		//	if(pCurrentReport->fNewPrice>pCurrentReport->fBuyPrice[0])
		//		fSellVOL += fNowVolume;
		//	else
		//		fBuyVOL += fNowVolume;
		//}
	}

	if(pLastReport->fNewPrice > 0.0)
	{
		//两个Report的对比计算
		fIncreaseSpeed = (pCurrentReport->fNewPrice-pLastReport->fNewPrice)/pLastReport->fNewPrice;
	}

	{
		/*量比计算：
			量比＝现成交总手/（过去5日平均每分钟成交量*当日累计开市时间（分）） 
			当量比大于1时，说明当日每分钟的平均成交量要大于过去5日的平均数值，交易比过去5日火爆；
			而当量比小于1时，说明现在的成交比不上过去5日的平均水平。
		*/
		if(fLast5Volume>0.0)
		{
			time_t tmSeconds = CDataEngine::getOpenSeconds(pCurrentReport->tmTime);
			fVolumeRatio = (pCurrentReport->fVolume)/((fLast5Volume/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60));
		}
	}

	emit stockItemReportChanged(qsOnly);
	emit stockItemReportComing(this);
}

void CStockInfoItem::resetBuySellVOL()
{
	//重新计算内外盘
	if(mapFenBis.size()>0)
	{
		//以分笔数据来计算内外盘
		fBuyVOL = 0;		//内外盘清零
		fSellVOL = 0;

		bool bSell = true;			//假设刚开始是外盘

		qRcvFenBiData* pLastFenBi = 0;
		QList<qRcvFenBiData*> list = mapFenBis.values();
		foreach(qRcvFenBiData* p,list)
		{
			float fVOL = p->fVolume;
			if(pLastFenBi)
				fVOL = (p->fVolume)-(pLastFenBi->fVolume);

			if(p->fBuyPrice[0]>0.0)
			{
				if(p->fPrice>p->fBuyPrice[0])
				{
					fSellVOL += fVOL;
					bSell = true;
				}
				else
				{
					fBuyVOL += fVOL;
					bSell = false;
				}
			}
			else
			{
				if(pLastFenBi)
				{
					if(p->fPrice<pLastFenBi->fPrice)
					{
						fSellVOL += fVOL;
						bSell = true;
					}
					else if(p->fPrice>pLastFenBi->fPrice)
					{
						fBuyVOL += fVOL;
						bSell = false;
					}
					else
					{
						if(bSell)
							fSellVOL+=fVOL;
						else
							fBuyVOL +=fVOL;
					}
				}
				else
				{
					if(bSell)
						fSellVOL+=fVOL;
					else
						fBuyVOL +=fVOL;
				}

			}

			pLastFenBi = p;
		}
	}
}

bool CStockInfoItem::isMatch( const QString& _key )
{
	//判断代码是否匹配
	if(qsCode.indexOf(_key,0,Qt::CaseInsensitive)>-1)
		return true;

	//判断名称简拼是否匹配
	for (int i = 0; i < _key.size(); ++i)
	{
		if(i>=shortName.size())
			return false;
		QList<QChar> _l = shortName[i];
		bool bMatch = false;
		foreach(const QChar& _c,_l)
		{
			if(_c == _key[i])
			{
				bMatch = true;
				break;
			}
		}
		if(!bMatch)
			return false;
	}

	return true;
}

float CStockInfoItem::getLtag()
{
	if(baseInfo.fLtAg>0.0)
		return baseInfo.fLtAg;
	return -1.0;
}

QVector<int> CStockInfoItem::getLast5CmpPrices()
{
	//QVector<int> vvv;
	//for (int i=0;i<3;++i)
	//{
	//	int v = qrand()%2 ? (qrand()%3) : (-qrand()%3);
	//	if(v!=0)
	//		vvv.append(v);
	//}
	//return vvv;
	return vCmpPrices;
}
