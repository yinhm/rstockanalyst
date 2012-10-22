#include "StdAfx.h"
#include "DataEngine.h"
#include "STKDRV.h"
#include <time.h>

CDataEngine* CDataEngine::m_pDataEngine = NULL;

time_t CDataEngine::m_tmCurrentDay = NULL;
time_t* CDataEngine::m_tmLast5Day = new time_t[5];

bool CDataEngine::isStockOpenDay( time_t tmDay )
{
	QDate tmDate = QDateTime::fromTime_t(tmDay).date();
	int iWeek = tmDate.dayOfWeek();
	if((iWeek == 6)||(iWeek == 7))
		return false;

	return true;
}

time_t* CDataEngine::getLast5DayTime()
{
	time_t tmCur = time(NULL);
	tmCur = tmCur-(tmCur%(3600*24));
	tmCur = tmCur-(3600*R_TIME_ZONE);

	if(tmCur==m_tmCurrentDay)
		return m_tmLast5Day;

	m_tmCurrentDay = tmCur;

	for(int i=0;i<5;++i)
	{
		tmCur = tmCur-(3600*24);
		while(!isStockOpenDay(tmCur))
		{
			tmCur = tmCur-(3600*24);
		}
		m_tmLast5Day[i] = tmCur;
	}
	QDateTime tmDataTime = QDateTime::fromTime_t(tmCur);

	return m_tmLast5Day;
}

time_t CDataEngine::getOpenSeconds()
{
	return 3600*4;
}

time_t CDataEngine::getOpenSeconds( time_t tmTime )
{
	time_t tmEnd2 = m_tmCurrentDay+3600*15;
	if(tmTime>tmEnd2)
		return 3600*4;

	time_t tmBegin2 = m_tmCurrentDay+3600*13;
	if(tmTime>tmBegin2)
		return 3600*2+(tmTime-tmBegin2);

	time_t tmEnd1 = m_tmCurrentDay+1800*23;
	if(tmTime>tmEnd1)
		return 3600*2;

	time_t tmBegin1 = m_tmCurrentDay + 1800*19;
	if(tmTime>tmBegin2)
		return tmTime-tmBegin1;

	return 0;
}



CDataEngine::CDataEngine(void)
{
	getLast5DayTime();
}

CDataEngine::~CDataEngine(void)
{
}



QList<qRcvReportData*> CDataEngine::getBaseMarket()
{
	return m_mapBaseMarket.values();
}

qRcvReportData* CDataEngine::getBaseMarket( const QString& qsCode )
{
	if(m_mapBaseMarket.contains(qsCode))
	{
		return m_mapBaseMarket[qsCode];
	}
	return NULL;
}

void CDataEngine::setBaseMarket( qRcvReportData* p )
{
	m_mapBaseMarket[p->qsCode] = p;
	emit baseMarketChanged(p->qsCode);
}


bool CDataEngine::appendHistory( const QString& code, const qRcvHistoryData& p )
{
	if(!m_mapBaseMarket.contains(code))
		return false;
	qRcvReportData* pData = m_mapBaseMarket[code];
	pData->mapHistorys.insert(p.time,p);
	return true;
}

void CDataEngine::updateBaseMarket( const QString& code )
{
	if(m_mapBaseMarket.contains(code))
		emit baseMarketChanged(code);
}
