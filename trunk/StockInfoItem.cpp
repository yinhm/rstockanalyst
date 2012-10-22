#include "StdAfx.h"
#include "StockInfoItem.h"
#include "DataEngine.h"

CStockInfoItem::CStockInfoItem( const QString& code, WORD market )
	: qsCode(code)
	, wMarket(market)
{

}

CStockInfoItem::CStockInfoItem( const qRcvBaseInfoData& info )
	: qsCode(QString::fromLocal8Bit(info.code))
	, wMarket(info.wMarket)
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
}

qRcvReportData* CStockInfoItem::getLastReport() const
{
	if(mapReports.size()<1)
		return NULL;

	return (mapReports.end()-1).value();
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
}

void CStockInfoItem::setBaseInfo( const qRcvBaseInfoData& info )
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));
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
	if(mapReports.size()>0)
		return (*mapReports.begin())->qsName;

	return QString();
}

QString CStockInfoItem::getIncrease() const
{
	qRcvReportData* pReport = this->getLastReport();
	if(pReport)
	{
		if(pReport->fNewPrice<=0.0 || pReport->fLastClose<=0.0)
			return QString();

		return QString("%1%").arg((pReport->fNewPrice-pReport->fLastClose)*100.0/pReport->fLastClose,0,'f',2);
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
	qRcvReportData* pReport = getLastReport();
	if(!pReport)
		return QString();

	time_t tmSeconds = CDataEngine::getOpenSeconds(pReport->tmTime);
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

	return QString("%1").arg((pReport->fVolume)/((fVolume5/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60)));
}

QString CStockInfoItem::getTurnRatio() const
{
	/*换手率（仓差）
		换手率=某一段时期内的成交量/发行总股数*100%
		（在中国：成交量/流通总股数*100%）
	*/
	return QString();
}

QString CStockInfoItem::getLastClose() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fLastClose,0,'f',2);

	return QString();
}

QString CStockInfoItem::getOpenPrice() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fOpen,0,'f',2);

	return QString();
}

QString CStockInfoItem::getHighPrice() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fHigh,0,'f',2);

	return QString();
}

QString CStockInfoItem::getLowPrice() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fLow,0,'f',2);

	return QString();
}

QString CStockInfoItem::getNewPrice() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fNewPrice,0,'f',2);

	return QString();
}

QString CStockInfoItem::getTotalVolume() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fVolume,0,'f',0);

	return QString();
}
