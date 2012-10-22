#include "StdAfx.h"
#include "DataEngine.h"
#include "STKDRV.h"
#include <time.h>

CDataEngine* CDataEngine::m_pDataEngine = NULL;

time_t CDataEngine::m_tmCurrentDay = NULL;
time_t* CDataEngine::m_tmLast5Day = new time_t[5];


int CDataEngine::importBaseInfoFromFile( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return 0;

	int iFlag;
	file.read((char*)&iFlag,4);
	int iTotal;
	file.read((char*)&iTotal,4);

	int iCout = 0;

	while(true)
	{
		WORD wMarket;
		if(file.read((char*)&wMarket,2)!=2) break;
		if(!file.seek(file.pos()+2)) break;

		char chCode[STKLABEL_LEN];
		if(file.read(chCode,STKLABEL_LEN)!=STKLABEL_LEN) break;


		float fVal[38];
		if(file.read((char*)fVal,sizeof(float)*38)!=(sizeof(float)*38)) break;

		qRcvBaseInfoData d(fVal);
		d.wMarket = wMarket;
		memcpy(d.code,chCode,STKLABEL_LEN);

		QString qsCode = QString::fromLocal8Bit(chCode);

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
		if(pItem)
		{
			pItem->setBaseInfo(d);
		}
		else
		{
			CStockInfoItem* pItem = new CStockInfoItem(d);
			CDataEngine::getDataEngine()->setStockInfoItem(pItem);
		}

		++iCout;
	}

	return iCout;
}

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



QList<CStockInfoItem*> CDataEngine::getStockInfoList()
{
	return m_mapStockInfos.values();
}

CStockInfoItem* CDataEngine::getStockInfoItem( const QString& qsCode )
{
	if(m_mapStockInfos.contains(qsCode))
	{
		return m_mapStockInfos[qsCode];
	}
	return NULL;
}

void CDataEngine::setStockInfoItem( CStockInfoItem* p )
{
	m_mapStockInfos[p->getCode()] = p;
	emit stockInfoAdded(p->getCode());
}
