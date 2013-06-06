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
	, fLast5Volume(0.0)
{
	pCurrentReport = new qRcvReportData;
	pLastReport = new qRcvReportData;
	qsCode = code;
	wMarket = market;
	qsMarket = CDataEngine::getMarketStr(wMarket);
	qsOnly = qsCode+qsMarket;
}

CStockInfoItem::CStockInfoItem( const qRcvBaseInfoData& info )
	: fNowVolume(FLOAT_NAN)
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
	, fLast5Volume(0.0)
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));
	pCurrentReport = new qRcvReportData;
	pLastReport = new qRcvReportData;

	qsCode = QString::fromLocal8Bit(info.code);
	wMarket = info.wMarket;
	qsMarket = CDataEngine::getMarketStr(wMarket);
	qsOnly = qsCode+qsMarket;
	//fLast5Volume = 0.0;
	//QList<qRcvHistoryData*> list = getLastHistory(5);
	//foreach(qRcvHistoryData* pHis,list)
	//{
	//	fLast5Volume = fLast5Volume+pHis->fVolume;
	//}
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
	if(p->tmTime<=pCurrentReport->tmTime)
		return;
	pLastReport->resetItem(pCurrentReport);
	pCurrentReport->resetItem(p);
	CDataEngine::setCurrentTime(pCurrentReport->tmTime);

	//将新的Report数据添加到分笔数据中
	qRcvFenBiData* pFenBi = new qRcvFenBiData(pCurrentReport);
	
	updateItemInfo();
	appendFenBis(QList<qRcvFenBiData*>()<<pFenBi);
}

void CStockInfoItem::setReport( RCV_REPORT_STRUCTExV3* p )
{
	if(p->m_time<=pCurrentReport->tmTime)
		return;
	pLastReport->resetItem(pCurrentReport);
	pCurrentReport->resetItem(p);
	CDataEngine::setCurrentTime(pCurrentReport->tmTime);

	//将新的Report数据添加到分笔数据中
	qRcvFenBiData* pFenBi = new qRcvFenBiData(pCurrentReport);

	updateItemInfo();
	appendFenBis(QList<qRcvFenBiData*>()<<pFenBi);
}

void CStockInfoItem::appendHistorys( const QList<qRcvHistoryData*>& list )
{
	QList<qRcvHistoryData*> listHistory;
	int iCountFromFile = -1;
	if(list.size()>130)
	{
		listHistory = CDataEngine::getDataEngine()->getHistoryList(this);
	}
	else
	{
		iCountFromFile = list.size();
		listHistory = CDataEngine::getDataEngine()->getHistoryList(this,iCountFromFile);
	}

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


	listHistory.clear();
	listHistory = mapHistorys.values();
	{
		//最近5日的全部成交量（用于量比的计算）
		fLast5Volume = 0.0;
		for (int i=1;i<=5;++i)
		{
			int iIndex = listHistory.size()-i;
			if(iIndex<0)
				break;
			fLast5Volume = fLast5Volume + listHistory[iIndex]->fVolume;
		}
		updateItemInfo();
	}

	CDataEngine::getDataEngine()->exportHistoryData(this,listHistory,iCountFromFile);

	QMap<time_t,qRcvHistoryData*>::iterator iter = mapHistorys.begin();
	while(iter!=mapHistorys.end())
	{
		delete iter.value();
		++iter;
	}
	mapHistorys.clear();

	emit stockItemHistoryChanged(qsOnly);
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
	if(baseInfo.fZgb>0)
		fTurnRatio = pCurrentReport->fVolume/baseInfo.fZgb*100;

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

