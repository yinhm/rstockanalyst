#include "StdAfx.h"
#include "StockInfoItem.h"
#include "DataEngine.h"

CStockInfoItem::CStockInfoItem( const QString& code, WORD market )
	: qsCode(code)
	, wMarket(market)
	, fNowVolume(NULL)
	, pLastReport(NULL)
	, fIncreaseSpeed(NULL)
	, fBuyVolume(NULL)
	, fSellVolume(NULL)
{

}

CStockInfoItem::CStockInfoItem( const qRcvBaseInfoData& info )
	: qsCode(QString::fromLocal8Bit(info.code))
	, wMarket(info.wMarket)
	, fNowVolume(NULL)
	, pLastReport(NULL)
	, fIncreaseSpeed(NULL)
	, fBuyVolume(NULL)
	, fSellVolume(NULL)
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));
}

CStockInfoItem::~CStockInfoItem(void)
{
}

void CStockInfoItem::appendReport( qRcvReportData* p )
{
	if(mapReports.contains(p->tmTime))
	{
		qRcvReportData* pBefore = mapReports[p->tmTime];
		if(pBefore!=p)
			delete pBefore;
	}
	mapReports[p->tmTime] = p;
	pLastReport = (mapReports.end()-1).value();

	{
		//委买量计算
		fBuyVolume = 0.0;
		fBuyVolume += pLastReport->fBuyVolume[0];
		fBuyVolume += pLastReport->fBuyVolume[1];
		fBuyVolume += pLastReport->fBuyVolume[2];
		fBuyVolume += pLastReport->fBuyVolume4;
		fBuyVolume += pLastReport->fBuyVolume5;

		//委卖量计算
		fSellVolume = 0.0;
		fSellVolume += pLastReport->fSellVolume[0];
		fSellVolume += pLastReport->fSellVolume[1];
		fSellVolume += pLastReport->fSellVolume[2];
		fSellVolume += pLastReport->fSellVolume4;
		fSellVolume += pLastReport->fSellVolume5;
	}

	if(mapReports.size()>2)
	{
		//两个Report的对比计算
		qRcvReportData* pReport2 = (mapReports.end()-2).value();
		fNowVolume = (pLastReport->fVolume)-(pReport2->fVolume);
		fIncreaseSpeed = (pLastReport->fNewPrice-pReport2->fNewPrice)/pReport2->fNewPrice;
	}

	emit stockInfoItemChanged(qsCode);
}

QList<qRcvHistoryData*> CStockInfoItem::getHistoryList()
{
	return mapHistorys.values();
}

void CStockInfoItem::appendHistorys( const QList<qRcvHistoryData*>& list )
{
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

	emit stockInfoItemChanged(qsCode);
}

void CStockInfoItem::setBaseInfo( const qRcvBaseInfoData& info )
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));

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
	if(pLastReport)
		return pLastReport->qsName;

	return QString();
}

QString CStockInfoItem::getIncrease() const
{
	if(pLastReport)
	{
		if(pLastReport->fNewPrice<=0.0 || pLastReport->fLastClose<=0.0)
			return QString();

		return QString("%1%").arg((pLastReport->fNewPrice-pLastReport->fLastClose)*100.0/pLastReport->fLastClose,0,'f',2);
	}
	else
	{
		return QString();
	}
}

QString CStockInfoItem::getVolumeRatio() const
{
	/*量比计算：
		量比＝现成交总手/（过去5日平均每分钟成交量*当日累计开市时间（分）） 
		当量比大于1时，说明当日每分钟的平均成交量要大于过去5日的平均数值，交易比过去5日火爆；
		而当量比小于1时，说明现在的成交比不上过去5日的平均水平。
	*/
	if(mapHistorys.size()<5)
		return QString();

	//判断最新的数据是否是今天开市后的数据
	if(!pLastReport)
		return QString();

	time_t tmSeconds = CDataEngine::getOpenSeconds(pLastReport->tmTime);
	if(tmSeconds<1)
		return QString();

	time_t* pLast5Day = CDataEngine::getLast5DayTime();
	float fVolume5 = 0.0;
	for(int i=0;i<5;++i)
	{
		if(!mapHistorys.contains(pLast5Day[i]))
			return QString();
		fVolume5 = (fVolume5 + mapHistorys.value(pLast5Day[i])->fVolume);
	}

	return QString("%1").arg((pLastReport->fVolume)/((fVolume5/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60)),0,'f',2);
}

QString CStockInfoItem::getTurnRatio() const
{
	/*换手率（仓差）
		换手率=某一段时期内的成交量/发行总股数*100%
		（在中国：成交量/流通总股数*100%）
	*/
	if(baseInfo.fZgb>0&&pLastReport)
	{
		return QString("%1%").arg(pLastReport->fVolume/baseInfo.fZgb*100,0,'f',2);
	}
	return QString();
}

QString CStockInfoItem::getLastClose() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fLastClose,0,'f',2);

	return QString();
}

QString CStockInfoItem::getOpenPrice() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fOpen,0,'f',2);

	return QString();
}

QString CStockInfoItem::getHighPrice() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fHigh,0,'f',2);

	return QString();
}

QString CStockInfoItem::getLowPrice() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fLow,0,'f',2);

	return QString();
}

QString CStockInfoItem::getNewPrice() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fNewPrice,0,'f',2);

	return QString();
}

QString CStockInfoItem::getTotalVolume() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fVolume,0,'f',0);

	return QString();
}

QString CStockInfoItem::getTotalAmount() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fAmount/10000,0,'f',0);

	return QString();
}

QString CStockInfoItem::getNowVolume() const
{
	return QString("%1").arg(fNowVolume,0,'f',0);
}

QString CStockInfoItem::getIncSpeed() const
{
	//例如，某个股票5分钟之前的股价是10元，而现在的价格是10.1元，则这个股票的5分钟涨速为：
	if(pLastReport)
		return QString("%1%").arg(fIncreaseSpeed*100,0,'f',2);

	return QString();
}

QString CStockInfoItem::getPriceFluctuate() const
{
	//今日涨跌幅（%）=（当前价-前收盘价）/ 前收盘价 * 100
	if(pLastReport)
		return QString("%1%").arg((pLastReport->fNewPrice-pLastReport->fLastClose)/pLastReport->fLastClose*100,0,'f',2);

	return QString();
}

QString CStockInfoItem::getAmplitude() const
{
	/*
		一、股票振幅=当期最高价/当期最低价×100%-100%
		二、股票振幅=（当期最高价－当期最低价)/上期收盘价×100%
		依据飞狐，采用第二种计算方法。
	*/
	if(pLastReport)
		return QString("%1%").arg((pLastReport->fHigh-pLastReport->fLow)/pLastReport->fLastClose*100,0,'f',2);

	return QString();
}

QString CStockInfoItem::getAvePrice() const
{
	if(pLastReport)
		return QString("%1").arg((pLastReport->fAmount/pLastReport->fVolume)/100,0,'f',2);
	return QString();
}

QString CStockInfoItem::getPERatio() const
{
	//市盈率（静态市盈率）=普通股每股市场价格÷普通股每年每股盈利

	return QString("UnKown");
}

QString CStockInfoItem::getLTSZ() const
{
	//流通市值 总市值就是以目前股票市价乘以总股本，流通值是以目前股票市价乘以流[1]通股本。
	if(pLastReport&&(baseInfo.fLtAg>0))
		return QString("%1").arg(baseInfo.fLtAg*pLastReport->fNewPrice,0,'f',0);
	return QString();
}

QString CStockInfoItem::getSellVOL() const
{
	//外盘量

	return QString("UnKown");
}

QString CStockInfoItem::getBuyVOL() const
{
	//内盘量

	return QString("UnKown");
}

QString CStockInfoItem::getBIDVOL() const
{
	//委买量
	if(pLastReport&&fBuyVolume>0)
	{
		return QString("%1").arg(fBuyVolume,0,'f',0);
	}

	return QString();
}

QString CStockInfoItem::getASKVOL() const
{
	//委卖量
	if(pLastReport&&fSellVolume>0)
	{
		return QString("%1").arg(fSellVolume,0,'f',0);
	}

	return QString();
}

QString CStockInfoItem::getCommRatio() const
{
	//委比
	//(委买手数－委卖手数)/(委买手数+委卖手数)*100
	if(pLastReport&&(fBuyVolume>0||fSellVolume>0))
		return QString("%1%").arg(((fBuyVolume-fSellVolume)/(fBuyVolume+fSellVolume))*100,0,'f',2);

	return QString();
}

QString CStockInfoItem::getCommSent() const
{
	//委差
	if(pLastReport&&(fBuyVolume>0||fSellVolume>0))
		return QString("%1").arg(fBuyVolume-fSellVolume,0,'f',0);

	return QString();
}
