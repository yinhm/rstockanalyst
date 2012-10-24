#include "StdAfx.h"
#include "DataEngine.h"
#include "STKDRV.h"
#include <time.h>
#include <QApplication>

CDataEngine* CDataEngine::m_pDataEngine = NULL;

time_t CDataEngine::m_tmCurrentDay = NULL;
time_t* CDataEngine::m_tmLast5Day = new time_t[5];


void CDataEngine::importData()
{
	QString qsDir = qApp->applicationDirPath();
	{
		//导入F10 数据
		QString qsBaseInfo = qsDir+"/baseinfo.rsqfin";
		if(QFile::exists(qsBaseInfo))
		{
			qDebug()<<"Import F10 data...";
			int iCount = importBaseInfo(qsBaseInfo);
			if(iCount<1)
			{
				QFile::remove(qsBaseInfo);
				qDebug()<<"Import F10 data from "<<qsBaseInfo<<" error!";
			}
			else
				qDebug()<<iCount<<" F10 data had been imported.";
		}
		else
		{
			//如果本机的F10数据，则尝试导入银江的F10 数据。
			QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\StockDrv",QSettings::NativeFormat);
			QString qsF10File = QFileInfo(settings.value("driver").toString()).absolutePath() + "/财务数据.fin";
			if(QFile::exists(qsF10File))
			{
				qDebug()<<"Import F10 Data from "<<qsF10File;
				CDataEngine::importBaseInfoFromFinFile(qsF10File);
			}
		}
	}
	{
		//导入当天的Reports数据
		QString qsReportFile = QString("%1/reports/%2").arg(qsDir).arg(QDate::currentDate().toJulianDay());
		qDebug()<<"Import reports data from "<<qsReportFile;
		int iCount = importReportsInfo(qsReportFile);
		if(iCount<1)
		{
			QFile::remove(qsReportFile);
			qDebug()<<"Import reports data from "<<qsReportFile<<" error!";
		}
		else
			qDebug()<<iCount<<" reports data had been imported.";
	}
}

void CDataEngine::exportData()
{
	QString qsDir = qApp->applicationDirPath();
	QString qsBaseInfo = qsDir+"/baseinfo.rsqfin";
	{
		qDebug()<<"Export F10 data...";
		int iCount = exportBaseInfo(qsBaseInfo);
		qDebug()<<iCount<<" F10 data had been exported.";
	}

	{
		//导出当天的Reports数据
		QString qsReportDir = qsDir + "/reports";
		if(!QDir().exists(qsReportDir))
			QDir().mkpath(qsReportDir);
		QString qsReportFile = QString("%1/%2").arg(qsReportDir).arg(QDate::currentDate().toJulianDay());
		qDebug()<<"Export reports data to "<<qsReportFile;
		int iCount = exportReportsInfo(qsReportFile);
		qDebug()<<iCount<<" reports data had been exported.";
	}
}

int CDataEngine::importBaseInfoFromFinFile( const QString& qsFile )
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

int CDataEngine::importBaseInfo( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return -1;

	QDataStream out(&file);

	int iCount = 0;
	while(true)
	{
		qRcvBaseInfoData baseInfo;
		int iSize = out.readRawData((char*)&baseInfo,sizeof(qRcvBaseInfoData));
		if(iSize!=sizeof(qRcvBaseInfoData))
			break;

		QString qsCode = QString::fromLocal8Bit(baseInfo.code);
		if(qsCode.isEmpty())
			return -1;

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
		if(pItem)
		{
			pItem->setBaseInfo(baseInfo);
		}
		else
		{
			CStockInfoItem* pItem = new CStockInfoItem(baseInfo);
			CDataEngine::getDataEngine()->setStockInfoItem(pItem);
		}

		++iCount;
	}

	file.close();
	return iCount;
}

int CDataEngine::importReportsInfo( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return -1;

	QDataStream out(&file);

	int iCount = 0;
	while(true)
	{
		qRcvReportData* pReport = new qRcvReportData;
		quint32 _t;
		out>>_t>>pReport->wMarket>>pReport->qsCode>>pReport->qsName;
		pReport->tmTime = _t;

		int iSize = out.readRawData((char*)&pReport->fLastClose,sizeof(float)*27);
		if(iSize!=(sizeof(float)*27))
			break;

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(pReport->qsCode);
		if(pItem==NULL)
		{
			pItem = new CStockInfoItem(pReport->qsCode,pReport->wMarket);
			CDataEngine::getDataEngine()->setStockInfoItem(pItem);
		}
		pItem->appendReport(pReport);

		++iCount;
	}

	file.close();
	return iCount;
}


int CDataEngine::exportBaseInfo( const QString& qsFile )
{
	if(QFile::exists(qsFile))
		QFile::remove(qsFile);
	if(QFile::exists(qsFile))
		return -1;

	QFile file(qsFile);
	if(!file.open(QFile::WriteOnly))
		return -1;

	QDataStream out(&file);

	QList<CStockInfoItem*> listItem = CDataEngine::getDataEngine()->getStockInfoList();
	int iCount = 0;

	foreach( CStockInfoItem* pItem, listItem)
	{
		qRcvBaseInfoData* pBaseInfo = pItem->getBaseInfo();

		int iSize = out.writeRawData((char*)pBaseInfo,sizeof(qRcvBaseInfoData));
		if(iSize!=sizeof(qRcvBaseInfoData))
			break;
		++iCount;
	}

	file.close();
	return iCount;
}

int CDataEngine::exportReportsInfo( const QString& qsFile )
{
	if(QFile::exists(qsFile))
		QFile::remove(qsFile);
	if(QFile::exists(qsFile))
		return -1;

	QFile file(qsFile);
	if(!file.open(QFile::WriteOnly))
		return -1;

	QDataStream out(&file);

	QList<CStockInfoItem*> listItem = CDataEngine::getDataEngine()->getStockInfoList();
	int iCount = 0;

	foreach( CStockInfoItem* pItem, listItem)
	{
		//只保存最近的report
		qRcvReportData* pReport = pItem->getLastReport();
		if(pReport)
		{
			out<<pReport->tmTime<<pReport->wMarket<<pReport->qsCode<<pReport->qsName;
			//直接拷贝剩余的所有float数据

			if(out.writeRawData((char*)&pReport->fLastClose,sizeof(float)*27)!=(sizeof(float)*27))
				break;
			//		int iSize = out.writeRawData((char*)pBaseInfo,sizeof(qRcvBaseInfoData));
			//		if(iSize!=sizeof(qRcvBaseInfoData))
			//			break;
			++iCount;
		}
	}

	file.close();
	return iCount;
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
	emit stockInfoChanged(p->getCode());
	connect(p,SIGNAL(stockInfoItemChanged(const QString&)),this,SIGNAL(stockInfoChanged(const QString&)));
}

CDataEngine* CDataEngine::getDataEngine()
{
	if(m_pDataEngine == NULL)
		m_pDataEngine = new CDataEngine;
	return m_pDataEngine;
}
