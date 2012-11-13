#include "StdAfx.h"
#include "StockInfoItem.h"
#include "DataEngine.h"
#define	FLOAT_NAN	(std::numeric_limits<float>::quiet_NaN())

CStockInfoItem::CStockInfoItem( const QString& code, WORD market )
	: qsCode(code)
	, wMarket(market)
	, fNowVolume(FLOAT_NAN)
	, pCurrentReport(NULL)
	, pLastReport(NULL)
	, fIncreaseSpeed(FLOAT_NAN)
	, fBuyVolume(FLOAT_NAN)
	, fSellVolume(FLOAT_NAN)
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
{
	pCurrentReport = new qRcvReportData;
	pLastReport = new qRcvReportData;
}

CStockInfoItem::CStockInfoItem( const qRcvBaseInfoData& info )
	: qsCode(QString::fromLocal8Bit(info.code))
	, wMarket(info.wMarket)
	, fNowVolume(FLOAT_NAN)
	, pCurrentReport(NULL)
	, pLastReport(NULL)
	, fIncreaseSpeed(FLOAT_NAN)
	, fBuyVolume(FLOAT_NAN)
	, fSellVolume(FLOAT_NAN)
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
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));
	pCurrentReport = new qRcvReportData;
	pLastReport = new qRcvReportData;
}

CStockInfoItem::~CStockInfoItem(void)
{
	delete pLastReport;
	delete pCurrentReport;
}

qRcvReportData* CStockInfoItem::getCurrentReport() const
{
	return pCurrentReport;
}

void CStockInfoItem::setReport( qRcvReportData* p )
{
	if(p->tmTime<=pCurrentReport->tmTime)
		return;
	pLastReport->resetItem(pCurrentReport);
	pCurrentReport->resetItem(p);
	
	updateItemInfo();
}

void CStockInfoItem::setReport( RCV_REPORT_STRUCTExV3* p )
{
	if(p->m_time<=pCurrentReport->tmTime)
		return;
	pLastReport->resetItem(pCurrentReport);
	pCurrentReport->resetItem(p);

	updateItemInfo();
}

QList<qRcvHistoryData*> CStockInfoItem::getHistoryList()
{
	return CDataEngine::getDataEngine()->getHistoryList(qsCode);
}

void CStockInfoItem::appendHistorys( const QList<qRcvHistoryData*>& list )
{
	/*计算量比*/
	QList<qRcvHistoryData*> listHistory = CDataEngine::getDataEngine()->getHistoryList(qsCode);

	QMap<time_t,qRcvHistoryData*> mapHistorys;		//日线数据
	foreach(qRcvHistoryData* p,listHistory)
	{
		if(mapHistorys.contains(p->time))
		{
			qRcvHistoryData* pBefore = mapHistorys[p->time];
			if(pBefore!=p)
				delete pBefore;
		}
		mapHistorys[p->time] = p;
	}
	foreach(qRcvHistoryData* p,list)
	{
		if(mapHistorys.contains(p->time))
		{
			qRcvHistoryData* pBefore = mapHistorys[p->time];
			if(pBefore!=p)
				delete pBefore;
		}
		mapHistorys[p->time] = p;
	}
	{
		/*量比计算：
			量比＝现成交总手/（过去5日平均每分钟成交量*当日累计开市时间（分）） 
			当量比大于1时，说明当日每分钟的平均成交量要大于过去5日的平均数值，交易比过去5日火爆；
			而当量比小于1时，说明现在的成交比不上过去5日的平均水平。
		*/
		if(mapHistorys.size()>5)
		{
			//判断最新的数据是否是今天开市后的数据
			if(pCurrentReport)
			{
				time_t tmSeconds = CDataEngine::getOpenSeconds(pCurrentReport->tmTime);
				if(tmSeconds>0)
				{
					time_t* pLast5Day = CDataEngine::getLast5DayTime();
					float fVolume5 = 0.0;
					for(int i=0;i<5;++i)
					{
						if(mapHistorys.contains(pLast5Day[i]))
							fVolume5 = (fVolume5 + mapHistorys.value(pLast5Day[i])->fVolume);
					}
					fVolumeRatio = (pCurrentReport->fVolume)/((fVolume5/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60));
				}
			}
		}
	}

	CDataEngine::getDataEngine()->exportHistoryData(qsCode,mapHistorys.values());

	QMap<time_t,qRcvHistoryData*>::iterator iter = mapHistorys.begin();
	while(iter!=mapHistorys.end())
	{
		delete iter.value();
		++iter;
	}
	mapHistorys.clear();

	emit stockInfoItemChanged(qsCode);
}

QList<qRcvMinuteData*> CStockInfoItem::getMinuteList()
{
	return mapMinutes.values();
}

void CStockInfoItem::appendMinutes( const QList<qRcvMinuteData*>& list )
{
	foreach(qRcvMinuteData* p,list)
	{
		if(mapMinutes.contains(p->tmTime))
		{
			qRcvMinuteData* pBefore = mapMinutes[p->tmTime];
			if(pBefore!=p)
				delete pBefore;
		}
		mapMinutes[p->tmTime] = p;
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

void CStockInfoItem::setBaseInfo( const qRcvBaseInfoData& info )
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));


	//市盈率
	if(pCurrentReport&&baseInfo.fMgsy>0)
		fPERatio = pCurrentReport->fNewPrice/baseInfo.fMgsy/2;	//不除以2则跟飞狐的数据不匹配

	//流通市值
	if(pCurrentReport&&baseInfo.fLtAg>0)
		fLTSZ = baseInfo.fLtAg*pCurrentReport->fNewPrice;

	emit stockInfoItemChanged(qsCode);
}


QString CStockInfoItem::getCode() const
{
	return qsCode;
}

WORD CStockInfoItem::getMarket() const
{
	return wMarket;
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
	//流通市值 总市值就是以目前股票市价乘以总股本，流通值是以目前股票市价乘以流[1]通股本。
	return fLTSZ;
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

void CStockInfoItem::updateItemInfo()
{
//设置股票名称
	if(qsName.isEmpty())
		qsName = pCurrentReport->qsName;
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
	if(baseInfo.fZgb>0)
		fTurnRatio = pCurrentReport->fVolume/baseInfo.fZgb*100;

	//市盈率
	if(baseInfo.fMgsy>0)
		fPERatio = pCurrentReport->fNewPrice/baseInfo.fMgsy/2;	//不除以2则跟飞狐的数据不匹配
	//流通市值
	if(baseInfo.fLtAg>0)
		fLTSZ = baseInfo.fLtAg*pCurrentReport->fNewPrice;

	//涨跌，价格波动
	fPriceFluctuate = (pCurrentReport->fNewPrice-pCurrentReport->fLastClose);

	//振幅
	fAmplitude = (pCurrentReport->fHigh-pCurrentReport->fLow)/pCurrentReport->fLastClose;

	//均价
	fAvePrice = (pCurrentReport->fAmount/pCurrentReport->fVolume)/100;

	{
		//委买量计算
		fBuyVolume = 0.0;
		fBuyVolume += pCurrentReport->fBuyVolume[0];
		fBuyVolume += pCurrentReport->fBuyVolume[1];
		fBuyVolume += pCurrentReport->fBuyVolume[2];
		fBuyVolume += pCurrentReport->fBuyVolume4;
		fBuyVolume += pCurrentReport->fBuyVolume5;

		//委卖量计算
		fSellVolume = 0.0;
		fSellVolume += pCurrentReport->fSellVolume[0];
		fSellVolume += pCurrentReport->fSellVolume[1];
		fSellVolume += pCurrentReport->fSellVolume[2];
		fSellVolume += pCurrentReport->fSellVolume4;
		fSellVolume += pCurrentReport->fSellVolume5;

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

	if(pLastReport->fNewPrice <= 0.0)
	{
		//内外盘计算
		if(pCurrentReport->fNewPrice>=pCurrentReport->fLastClose)
			fSellVOL = pCurrentReport->fVolume;
		else
			fBuyVOL = pCurrentReport->fVolume;
	}
	else
	{
		//两个Report的对比计算
		fNowVolume = (pCurrentReport->fVolume)-(pLastReport->fVolume);
		fIncreaseSpeed = (pCurrentReport->fNewPrice-pLastReport->fNewPrice)/pLastReport->fNewPrice;

		//内外盘计算
		if(pCurrentReport->fNewPrice>=pLastReport->fNewPrice)
			fSellVOL += fNowVolume;
		else
			fBuyVOL += fNowVolume;
	}

	emit stockInfoItemChanged(qsCode);
}
