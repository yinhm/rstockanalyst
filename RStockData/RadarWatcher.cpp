#include "StdAfx.h"
#include "RadarWatcher.h"


QMap<int,CRadarWatcher*> CRadarWatcher::m_listWatchers;

CRadarWatcher* CRadarWatcher::createRadarWatcher( RadarType _t,int iSec )
{
	QList<int> listKey = m_listWatchers.keys();
	int iMaxId = 0;
	{
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
		pWatcher = new CVolumnWatcher(iMaxId,iSec);
		break;
	case BigAmount:
		pWatcher = new CAmountWatcher(iMaxId,iSec);
		break;
	case MaxPrice:
		pWatcher = new CMaxPriceWatcher(iMaxId,iSec);
		break;
	}

	if(pWatcher)
	{
		m_listWatchers[pWatcher->getId()] = pWatcher;
	}

	return pWatcher;
}

CRadarWatcher::CRadarWatcher( int _id,RadarType _t,int iSec )
	: m_id(_id)
	, m_type(_t)
	, m_iSec(iSec)
{

}


CRadarWatcher::~CRadarWatcher(void)
{
}

CVolumnWatcher::CVolumnWatcher( int _id,int iSec )
	: CRadarWatcher(_id,BigVolumn,iSec)
{

}

CVolumnWatcher::~CVolumnWatcher( void )
{

}

CAmountWatcher::CAmountWatcher( int _id,int iSec )
	: CRadarWatcher(_id,BigAmount,iSec)
{

}

CAmountWatcher::~CAmountWatcher( void )
{

}

CMaxPriceWatcher::CMaxPriceWatcher( int _id,int iSec )
	: CRadarWatcher(_id,MaxPrice,iSec)
{

}

CMaxPriceWatcher::~CMaxPriceWatcher( void )
{

}
