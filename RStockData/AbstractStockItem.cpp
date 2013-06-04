#include "StdAfx.h"
#include "AbstractStockItem.h"
#include "StockInfoItem.h"
#include "BlockInfoItem.h"
#include "DataEngine.h"


CAbstractStockItem::CAbstractStockItem(void)
	: pCurrentReport(NULL)
	, pMap5Min(NULL)
{

}


CAbstractStockItem::~CAbstractStockItem(void)
{
	QMultiMap<time_t,qRcvFenBiData*>::iterator iter = mapFenBis.begin();
	while(iter!=mapFenBis.end())
	{
		delete iter.value();
		++iter;
	}
	mapFenBis.clear();

	{
		if(pMap5Min)
		{
			QMap<time_t,RStockData*>::iterator iter = pMap5Min->begin();		//5分钟历史数据
			while(iter!=pMap5Min->end())
			{
				delete iter.value();
				++iter;
			}
			pMap5Min->clear();
			delete pMap5Min;
		}
	}
}

qRcvReportData* CAbstractStockItem::getCurrentReport() const
{
	return pCurrentReport;
}

QList<qRcvFenBiData*> CAbstractStockItem::getFenBiList()
{
	//获取分笔数据，未完工
	return mapFenBis.values();
}

QList<qRcvHistoryData*> CAbstractStockItem::getHistoryList()
{
	return CDataEngine::getDataEngine()->getHistoryList(qsCode);
}

QList<qRcvHistoryData*> CAbstractStockItem::getLastHistory( int count )
{
	return CDataEngine::getDataEngine()->getHistoryList(qsCode,count);
}

void CAbstractStockItem::appendHistorys( const QList<qRcvHistoryData*>& list )
{

}



QList<RStockData*> CAbstractStockItem::get5MinList()
{
	if(pMap5Min == NULL)
		pMap5Min = CDataEngine::getDataEngine()->get5MinData(qsCode);

	return pMap5Min->values();
}

void CAbstractStockItem::appendFenBis( const QList<qRcvFenBiData*>& list )
{
	//追加分笔数据，未完工
	if(mapFenBis.size()>0&&list.size()>0)
	{
		QDate myDate = QDateTime::fromTime_t(mapFenBis.begin().value()->tmTime).date();
		QDate theDate = QDateTime::fromTime_t(list.first()->tmTime).date();
		if(theDate>myDate)
		{
			//如果新来的数据的时间大于当前的时间，则清空当前的数据
			foreach(qRcvFenBiData* p,mapFenBis.values())
				delete p;

			mapFenBis.clear();
		}
	}

	foreach(qRcvFenBiData* p,list)
	{
		bool bInsert = true;
		if(mapFenBis.contains(p->tmTime))
		{
			QList<qRcvFenBiData*> listV = mapFenBis.values(p->tmTime);
			foreach(qRcvFenBiData* p1,listV)
			{
				if(p1->fVolume == p->fVolume)
				{
					if(p1->fBuyPrice[0]>0.0)
					{
						//如果之前的数据不包换挂单数据，则删除之前的
						delete p1;
						mapFenBis.remove(p->tmTime,p1);
						break;
					}
					else
					{
						//删除现在的数据
						bInsert = false;
						delete p;
						break;
					}
				}
			}
		}
		if(bInsert)
			mapFenBis.insert(p->tmTime,p);
	}

	emit stockItemFenBiChanged(qsCode);
}

bool CAbstractStockItem::isInstanceOfStock()
{
	CStockInfoItem* p = dynamic_cast<CStockInfoItem*>(this);
	return p ? true : false;
}

bool CAbstractStockItem::isInstanceOfBlock()
{
	CBlockInfoItem* p = dynamic_cast<CBlockInfoItem*>(this);
	return p ? true : false;
}
