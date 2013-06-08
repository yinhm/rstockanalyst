#include "StdAfx.h"
#include "RadarWatcher.h"
#include "StockInfoItem.h"
#include "BlockInfoItem.h"
#include "DataEngine.h"
#include <QtXml>

CRadarManager* CRadarManager::m_pSelf = 0;

CRadarManager* CRadarManager::getRadarManager()
{
	if(m_pSelf==0)
		m_pSelf = new CRadarManager();

	return m_pSelf;
}

QString CRadarManager::getTypeName( RadarType _t )
{
	switch(_t)
	{
	case BigVolumn:		//大成交量
		return QString("成交量");
		break;
	case BigIncrease:	//涨幅
		return QString("涨幅");
		break;
	case MaxPrice:		//创新高
		return QString("创新高");
		break;
	case MinPrice:		//创新低
		return QString("创新低");
		break;
	}

	return QString("未知");
}

CRadarWatcher* CRadarManager::createRadarWatcher( CBlockInfoItem* pBlock, RadarType _t,int iSec,float _hold,int iId/*=-1*/ )
{
	QList<int> listKey = m_listWatchers.keys();
	int iMaxId = iId;
	if(iMaxId<0)
	{
		iMaxId = 0;
		//生成最新的id
		foreach(int i,listKey)
		{
			if(i>iMaxId)
				iMaxId = i;
		}
		++iMaxId;
	}

	CRadarWatcher* pWatcher = NULL;
	switch(_t)
	{
	case BigVolumn:
		pWatcher = new CVolumnWatcher(iMaxId,pBlock,iSec,_hold);
		break;
	case BigIncrease:
		pWatcher = new CIncreaseWatcher(iMaxId,pBlock,iSec,_hold);
		break;
	case MaxPrice:
		pWatcher = new CMaxPriceWatcher(iMaxId,pBlock,iSec,_hold);
		break;
	case MinPrice:
		pWatcher = new CMinPriceWatcher(iMaxId,pBlock,iSec,_hold);
		break;
	}

	if(pWatcher)
	{
		m_listWatchers[pWatcher->getId()] = pWatcher;
	}

	return pWatcher;
}

QList<CRadarWatcher*> CRadarManager::getRadarWatchers()
{
	return m_listWatchers.values();
}

CRadarWatcher* CRadarManager::getWatcher( const int& _id )
{
	if(m_listWatchers.contains(_id))
		return m_listWatchers[_id];

	return 0;
}

void CRadarManager::removeWatcher( const int& _id )
{
	if(m_listWatchers.contains(_id))
	{
		CRadarWatcher* pWatcher = m_listWatchers[_id];
		m_listWatchers.remove(_id);
		if(pWatcher)
		{
			delete pWatcher;
		}
	}
}

void CRadarManager::appendRadar( RRadarData* pRadar )
{
	m_listRadar.append(pRadar);
	emit radarAlert(pRadar);
}

void CRadarManager::loadRadars()
{
	QFile file(qApp->applicationDirPath()+"/config/radars.xml");
	if(!file.open(QFile::ReadOnly))
		return;
	QDomDocument doc;
	doc.setContent(file.readAll());
	file.close();

	QDomElement eleRoot = doc.firstChildElement("ROOT");
	if(!eleRoot.isElement())
		return;
	QDomElement eleRadar = eleRoot.firstChildElement("radar");
	while(eleRadar.isElement())
	{
		int iId = eleRadar.attribute("id").toInt();
		RadarType iType = static_cast<RadarType>(eleRadar.attribute("type").toInt());
		float fHold = eleRadar.attribute("hold").toFloat();
		int iSec = eleRadar.attribute("sec").toInt();
		QString qsCode = eleRadar.attribute("block");
		CBlockInfoItem* pBlock = CDataEngine::getDataEngine()->getStockBlock(qsCode);

		createRadarWatcher(pBlock,iType,iSec,fHold,iId);

		eleRadar = eleRadar.nextSiblingElement("radar");
	}
}

void CRadarManager::saveRadars()
{
	QDomDocument doc("template");
	QDomElement eleRoot = doc.createElement("ROOT");
	doc.appendChild(eleRoot);
	QMap<int,CRadarWatcher*>::iterator iter = m_listWatchers.begin();
	while (iter!=m_listWatchers.end())
	{
		CRadarWatcher* pWatcher = iter.value();
		QDomElement eleRadar = doc.createElement("radar");
		eleRadar.setAttribute("id",iter.key());
		eleRadar.setAttribute("type",pWatcher->getType());
		eleRadar.setAttribute("hold",pWatcher->getHold());
		eleRadar.setAttribute("sec",pWatcher->getSec());
		CBlockInfoItem* pBlock = pWatcher->getBlock();
		eleRadar.setAttribute("block",pBlock==0 ? "" : pBlock->getOnly());
		eleRoot.appendChild(eleRadar);
		++iter;
	}

	QFile file(qApp->applicationDirPath()+"/config/radars.xml");
	if(!file.open(QFile::Truncate|QFile::WriteOnly))
	{
		return;
	}
	file.write(doc.toByteArray());
	file.close();
}

void CRadarManager::releaseRadars()
{
	QMap<int,CRadarWatcher*>::iterator iter = m_listWatchers.begin();
	while (iter!=m_listWatchers.end())
	{
		delete iter.value();
		++iter;
	}
	m_listWatchers.clear();

	foreach(RRadarData* pData,m_listRadar)
	{
		delete pData;
	}
	m_listRadar.clear();
	delete m_pSelf;
	m_pSelf = NULL;
}



CRadarWatcher::CRadarWatcher( int _id,CBlockInfoItem* pBlock,RadarType _t,int iSec,float _hold )
	: m_id(_id)
	, m_pWatcherBlock(pBlock)
	, m_type(_t)
	, m_iSec(iSec)
	, m_fHold(_hold)
{
	if(m_pWatcherBlock)
	{
		//设置监视的股票列表
		QList<CStockInfoItem*> list = m_pWatcherBlock->getStockList();
		foreach(CStockInfoItem* pItem,list)
		{
			connect(pItem,SIGNAL(stockItemReportComing(CStockInfoItem*)),this,SLOT(onStockReportComing(CStockInfoItem*)));
		}
	}
}


CRadarWatcher::~CRadarWatcher(void)
{
}

void CRadarWatcher::setBlock( CBlockInfoItem* _b )
{
	if(m_pWatcherBlock==_b)
		return;

	if(m_pWatcherBlock)
	{
		//删除之前监视的股票列表
		QList<CStockInfoItem*> list = m_pWatcherBlock->getStockList();
		foreach(CStockInfoItem* pItem,list)
		{
			disconnect(pItem,SIGNAL(stockItemReportComing(CStockInfoItem*)),this,SLOT(onStockReportComing(CStockInfoItem*)));
		}
	}

	m_pWatcherBlock = _b;

	if(m_pWatcherBlock)
	{
		//设置新监视的股票列表
		QList<CStockInfoItem*> list = m_pWatcherBlock->getStockList();
		foreach(CStockInfoItem* pItem,list)
		{
			connect(pItem,SIGNAL(stockItemReportComing(CStockInfoItem*)),this,SLOT(onStockReportComing(CStockInfoItem*)));
		}
	}
}


CVolumnWatcher::CVolumnWatcher( int _id,CBlockInfoItem* pBlock,int iSec,float _hold )
	: CRadarWatcher(_id,pBlock,BigVolumn,iSec,_hold)
{

}

CVolumnWatcher::~CVolumnWatcher( void )
{
}

void CVolumnWatcher::onStockReportComing( CStockInfoItem* pItem )
{
	qRcvReportData* pReport = pItem->getCurrentReport();
	if(!pReport)
		return;

	tagVolumnData* pData = m_mapVolume[pItem];
	if(pData)
	{
		if(_isnan(pData->fLastV))
		{
			if((pReport->tmTime-pData->tmTime)>m_iSec)
			{
				pData->tmTime = pReport->tmTime;
				pData->fLastV = pData->fNewV;
				pData->fNewV = pReport->fVolume;
			}
		}
		else
		{
			//过去一个周期的成交量
			float fLastV = pData->fNewV-pData->fLastV;
			float fNewV = pReport->fVolume-pData->fNewV;
			if((fNewV-fLastV)/fLastV > m_fHold)
			{
				//超过阈值，进行抛出警报
				RRadarData* pRadar = new RRadarData;
				pRadar->pStock = pItem;
				pRadar->tmTime = pReport->tmTime;
				pRadar->tpType = BigVolumn;
				pRadar->qsDesc = QString("大笔成交量出现，超过上一周期:%1").arg((fNewV-fLastV)/fLastV);
				pRadar->iWatcher = m_id;
				CRadarManager::getRadarManager()->appendRadar(pRadar);
			}

			if((pReport->tmTime-pData->tmTime)>m_iSec)
			{
				pData->tmTime = pReport->tmTime;
				pData->fLastV = pData->fNewV;
				pData->fNewV = pReport->fVolume;
			}
		}
	}
	else
	{
		pData = new tagVolumnData();
		pData->tmTime = pReport->tmTime;
		pData->fLastV = FLOAT_NAN;
		pData->fNewV = pReport->fVolume;
		m_mapVolume[pItem] = pData;
	}
}

CIncreaseWatcher::CIncreaseWatcher( int _id,CBlockInfoItem* pBlock,int iSec,float _hold )
	: CRadarWatcher(_id,pBlock,BigIncrease,iSec,_hold)
{

}

CIncreaseWatcher::~CIncreaseWatcher( void )
{

}

void CIncreaseWatcher::onStockReportComing( CStockInfoItem* pItem )
{
	qRcvReportData* pReport = pItem->getCurrentReport();
	if(!pReport)
		return;

	tagPriceData* pData = m_mapPrice[pItem];
	if(pData)
	{
		//过去一个周期的最高价
		if(pData->fMaxPrice>0.0)
		{
			if((pReport->fNewPrice-pData->fMaxPrice)/pData->fMaxPrice > m_fHold)
			{
				//超过阈值，进行抛出警报
				RRadarData* pRadar = new RRadarData;
				pRadar->pStock = pItem;
				pRadar->tmTime = pReport->tmTime;
				pRadar->tpType = BigIncrease;
				pRadar->qsDesc = QString("大的成交价出现，超过上一周期:%1").arg((pReport->fNewPrice-pData->fMaxPrice)/pData->fMaxPrice);
				pRadar->iWatcher = m_id;
				CRadarManager::getRadarManager()->appendRadar(pRadar);
			}
		}

		if((pReport->tmTime-pData->tmTime)>m_iSec)
		{
			pData->tmTime = pReport->tmTime;
			pData->fMaxPrice = pData->fNewPrice;
			pData->fNewPrice = pReport->fNewPrice;
		}
		else if(pReport->fNewPrice>pData->fNewPrice)
		{
			//取本周期中的最大值。
			pData->fNewPrice = pReport->fNewPrice;
		}
	}
	else
	{
		pData = new tagPriceData();
		pData->tmTime = pReport->tmTime;
		pData->fMaxPrice = pReport->fNewPrice;
		pData->fNewPrice = pReport->fNewPrice;
		m_mapPrice[pItem] = pData;
	}
}


CMaxPriceWatcher::CMaxPriceWatcher( int _id,CBlockInfoItem* pBlock,int iSec,float _hold )
	: CRadarWatcher(_id,pBlock,MaxPrice,iSec,_hold)
{

}

CMaxPriceWatcher::~CMaxPriceWatcher( void )
{

}

void CMaxPriceWatcher::onStockReportComing( CStockInfoItem* pItem )
{
	qRcvReportData* pReport = pItem->getCurrentReport();
	if(!pReport)
		return;

	tagPriceData* pData = m_mapPrice[pItem];
	if(pData)
	{
		if((pReport->tmTime-pData->tmTime)>m_iSec)
		{
			pData->tmTime = pReport->tmTime;
			pData->fMaxPrice = pData->fNewPrice;
			pData->fNewPrice = pReport->fNewPrice;
		}

		if(!_isnan(pData->fMaxPrice))
		{
			//过去一个周期的最高价
			if(pReport->fNewPrice > pData->fMaxPrice && pReport->fNewPrice > pData->fNewPrice)
			{
				//超过阈值，进行抛出警报
				RRadarData* pRadar = new RRadarData;
				pRadar->pStock = pItem;
				pRadar->tmTime = pReport->tmTime;
				pRadar->tpType = MaxPrice;
				pRadar->qsDesc = QString("创新高，成交价:%1").arg(pReport->fNewPrice);
				pRadar->iWatcher = m_id;
				CRadarManager::getRadarManager()->appendRadar(pRadar);
			}
		}
		if(pReport->fNewPrice>pData->fNewPrice)
		{
			pData->fNewPrice = pReport->fNewPrice;
		}
	}
	else
	{
		pData = new tagPriceData();
		pData->tmTime = pReport->tmTime;
		pData->fMaxPrice = FLOAT_NAN;
		pData->fNewPrice = pReport->fNewPrice;
		m_mapPrice[pItem] = pData;
	}
}

CMinPriceWatcher::CMinPriceWatcher( int _id,CBlockInfoItem* pBlock,int iSec,float _hold )
	: CRadarWatcher(_id,pBlock,MinPrice,iSec,_hold)
{

}

CMinPriceWatcher::~CMinPriceWatcher( void )
{

}

void CMinPriceWatcher::onStockReportComing( CStockInfoItem* pItem )
{
	qRcvReportData* pReport = pItem->getCurrentReport();
	if(!pReport)
		return;

	tagPriceData* pData = m_mapPrice[pItem];
	if(pData)
	{
		if((pReport->tmTime-pData->tmTime)>m_iSec)
		{
			pData->tmTime = pReport->tmTime;
			pData->fMinPrice = pData->fNewPrice;
			pData->fNewPrice = pReport->fNewPrice;
		}

		if(!_isnan(pData->fMinPrice))
		{
			//过去一个周期的最高价
			if((pReport->fNewPrice < pData->fMinPrice) && (pReport->fNewPrice < pData->fNewPrice))
			{
				//超过阈值，进行抛出警报
				RRadarData* pRadar = new RRadarData;
				pRadar->pStock = pItem;
				pRadar->tmTime = pReport->tmTime;
				pRadar->tpType = MinPrice;
				pRadar->qsDesc = QString("创新高，成交价:%1").arg(pReport->fNewPrice);
				pRadar->iWatcher = m_id;
				CRadarManager::getRadarManager()->appendRadar(pRadar);
			}
		}
		if(pReport->fNewPrice<pData->fNewPrice)
		{
			pData->fNewPrice = pReport->fNewPrice;
		}
	}
	else
	{
		pData = new tagPriceData();
		pData->tmTime = pReport->tmTime;
		pData->fMinPrice = FLOAT_NAN;
		pData->fNewPrice = pReport->fNewPrice;
		m_mapPrice[pItem] = pData;
	}
}
