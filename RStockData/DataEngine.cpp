#include "StdAfx.h"
#include <time.h>
#include <QApplication>
#include <QtXml>
#include "DataEngine.h"
#include "STKDRV.h"
#include "Hz2Py.h"
#include "RadarWatcher.h"
#include "BaseInfoGetter.h"


//计算分时数据的横坐标时间
int getTimeMapBySec(QMap<time_t,int>& mapTimes,const time_t& tmBegin, const time_t& tmEnd, int iSize = 60/*second*/)
{
	if(tmBegin>tmEnd)
		return 0;

	time_t tmCur = tmEnd;
	while(tmCur>=tmBegin)
	{
		//		QString qsTime = QDateTime::fromTime_t(tmCur).toString();
		mapTimes.insert(tmCur,mapTimes.size());
		tmCur = tmCur-iSize;
	}
	return 1;
}

int getDayMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	time_t tmNow = (_tmEnd+3600*24);			//需多计算一个周期
	while(tmNow>=_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		tmNow -= 3600*24;
	}

	return 1;
}

int getWeekMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addDays(8-dtNow.dayOfWeek());	//多计算一个周期
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		tmNow -= (3600*24*7);
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getMonthMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addMonths(1);					//多计算一个周期
	dtNow = QDate(dtNow.year(),dtNow.month(),1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addMonths(-1);
		dtNow = QDate(dtNow.year(),dtNow.month(),1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getMonth3MapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();
	dtNow = dtNow.addMonths(3);					//多计算一个周期
	dtNow = QDate(dtNow.year(),(dtNow.month()-1)/3*3,1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addMonths(-3);
		dtNow = QDate(dtNow.year(),(dtNow.month()-1)/3*3,1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getYearMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addYears(1);					//多计算一个周期
	dtNow = QDate(dtNow.year(),1,1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addYears(-1);
		dtNow = QDate(dtNow.year(),1,1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

/*通过分钟数据获取指定周期内的数据*/
RStockData* getColorBlockItemByMins(const QList<qRcvFenBiData*>& list, qRcvFenBiData* pLastFenbi)
{
	if(list.size()<1)
		return NULL;

	RStockData* pData = new RStockData;
	//	qRcvFenBiData* pBegin = list.first();
	qRcvFenBiData* pLast = list.last();
	qRcvFenBiData* pFirst = list.first();
	pData->tmTime = pFirst->tmTime;
	pData->fOpen = pFirst->fPrice;
	pData->fClose = pLast->fPrice;
	pData->fHigh = pFirst->fPrice;
	pData->fLow = pFirst->fPrice;

	if(pLastFenbi)
	{
		pData->fAmount = pLast->fAmount-pLastFenbi->fAmount;
		pData->fVolume = pLast->fVolume-pLastFenbi->fVolume;
	}
	else
	{
		pData->fAmount = pLast->fAmount;
		pData->fVolume = pLast->fVolume;
	}

	foreach(qRcvFenBiData* p,list)
	{
		if(pData->fLow>p->fPrice)
			pData->fLow = p->fPrice;
		if(pData->fHigh<p->fPrice)
			pData->fHigh = p->fPrice;
	}
	return pData;
}

/*通过多天数据获取一个周期内的数据*/
RStockData* getColorBlockItemByDays(const QList<qRcvHistoryData*>& list)
{
	if(list.size()<1)
		return NULL;
	RStockData* pData = new RStockData();

	qRcvHistoryData* pFirst = list.first();
	qRcvHistoryData* pLast = list.last();
	pData->tmTime = pFirst->time;
	pData->fOpen = pFirst->fOpen;
	pData->fClose = pLast->fClose;


	pData->fLow = pFirst->fLow;
	pData->fHigh = pFirst->fHigh;
	pData->fAmount = 0;
	pData->fVolume = 0;
	foreach(qRcvHistoryData* p,list)
	{
		if(pData->fLow>p->fLow)
			pData->fLow = p->fLow;
		if(pData->fHigh<p->fHigh)
			pData->fHigh = p->fHigh;
		pData->fAmount+=p->fAmount;
		pData->fVolume+=p->fVolume;
	}
	return pData;
}

CDataEngine* CDataEngine::m_pDataEngine = 0;

time_t CDataEngine::m_tmCurrentDay = 0;
time_t CDataEngine::m_tmCurrent = 0;
QMap<QString,qRcvReportData*> CDataEngine::m_mapReportForBlock;


CDataEngine* CDataEngine::getDataEngine()
{
	if(m_pDataEngine == NULL)
		m_pDataEngine = new CDataEngine;
	return m_pDataEngine;
}

void CDataEngine::releaseDataEngine()
{
	if(m_pDataEngine)
		delete m_pDataEngine;
	CRadarManager::getRadarManager()->releaseRadars();
}

void CDataEngine::importData()
{
	m_bLoading = true;
	QString qsDir = qApp->applicationDirPath();
	{
		//导入F10 数据
		/*如果有本地数据，先导入本地数据*/
		QString qsBaseInfo = qsDir+"/data/baseinfo.rsqfin";
		if(QFile::exists(qsBaseInfo))
		{
			messageShowed("加载F10数据...",10);
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

		//如果有银江的F10数据，则尝试再次导入银江的F10 数据。
		QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\StockDrv",QSettings::NativeFormat);
		QString qsF10File = QFileInfo(settings.value("driver").toString()).absolutePath() + "/财务数据.fin";
		if(QFile::exists(qsF10File))
		{
			qDebug()<<"Import F10 Data from "<<qsF10File;
			CDataEngine::importBaseInfoFromFinFile(qsF10File);
		}
	}
	{
		//导入最后一组的Reports数据
		QString qsReportFile = QString("%1/data/reports").arg(qsDir);
		qDebug()<<"Import reports data from "<<qsReportFile;
		messageShowed("加载Reports数据...",20);
		int iCount = importReportsInfo(qsReportFile);
		if(iCount<1)
		{
			QFile::remove(qsReportFile);
			qDebug()<<"Import reports data from "<<qsReportFile<<" error!";
		}
		else
			qDebug()<<iCount<<" reports data had been imported.";
	}

	{
		//导入板块数据
		messageShowed("加载板块信息...",30);
		importBlocksData("");
		//清空 m_mapReportForBlock;
		QMap<QString,qRcvReportData*>::iterator iter = m_mapReportForBlock.begin();
		while(iter!=m_mapReportForBlock.end())
		{
			delete iter.value();
			++iter;
		}
		m_mapReportForBlock.clear();
	}

	{
		//导入当天的分笔数据
		messageShowed("加载当日分笔数据...",40);
		QString qsFenBiFile = QString("%1/data/FenBi/%2").arg(qsDir).arg(QDate::currentDate().toString("yyyyMMdd"));
		while(!QFile::exists(qsFenBiFile))
		{
			static int iAddDays = -1;
			qsFenBiFile = QString("%1/data/FenBi/%2").arg(qsDir).arg(QDate::currentDate().addDays(iAddDays).toString("yyyyMMdd"));
			if(iAddDays<-10)			//超过10以后则按不存在分笔数据处理
				break;
			--iAddDays;
		}

		qDebug()<<"Import FenBi data from "<<qsFenBiFile;
		int iCount = importFenBisData(qsFenBiFile);
		if(iCount<1)
		{
			qDebug()<<"Import FenBi data from "<<qsFenBiFile<<" error!";
		}
		else
			qDebug()<<iCount<<" FenBi data had been imported.";
	}
	{
		//加载监视雷达数据
		messageShowed("加载雷达信息...",90);
		CRadarManager::getRadarManager()->loadRadars();
	}

	m_bLoading = false;
}

void CDataEngine::exportData()
{
	QString qsDir = qApp->applicationDirPath();
	QString qsBaseInfo = qsDir+"/data/baseinfo.rsqfin";
	{
		QDir().mkpath(qsDir+"/data");
		qDebug()<<"Export F10 data...";
		int iCount = exportBaseInfo(qsBaseInfo);
		qDebug()<<iCount<<" F10 data had been exported.";
	}

	{
		//导出最后一组的Reports数据
		QString qsReportDir = qsDir + "/data";
		if(!QDir().exists(qsReportDir))
			QDir().mkpath(qsReportDir);
		QString qsReportFile = QString("%1/reports").arg(qsReportDir);
		qDebug()<<"Export reports data to "<<qsReportFile;
		int iCount = exportReportsInfo(qsReportFile);
		qDebug()<<iCount<<" reports data had been exported.";
	}

	{
		//导出当天的分笔数据
		QString qsFenBiDir = QString("%1/data/FenBi")
			.arg(qsDir);
		QDir().mkpath(qsFenBiDir);
		qDebug()<<"Export FenBis data to "<<qsFenBiDir;
		int iCount = exportFenBisData(QString("%1/%2").arg(qsFenBiDir).arg(QDateTime::fromTime_t(m_tmCurrent).toString("yyyyMMdd")));
		qDebug()<<iCount<<" FenBis data had been exported.";
	}
	{
		//导出监视雷达数据
		CRadarManager::getRadarManager()->saveRadars();
	}
}

bool CDataEngine::isLoading()
{
	return m_bLoading;
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

		QString qsOnly = QString::fromLocal8Bit(chCode) + CDataEngine::getMarketStr(wMarket);;

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsOnly);
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


		float fLast5Volume = 0.0;
		if(out.readRawData((char*)&fLast5Volume,sizeof(float))!=sizeof(float))
			break;

		QString qsCode = QString::fromLocal8Bit(baseInfo.code);
		if(qsCode.isEmpty())
			return -1;

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode+getMarketStr(baseInfo.wMarket));
		if(pItem)
		{
			pItem->setBaseInfo(baseInfo);
			pItem->setLast5Volume(fLast5Volume);
		}
		else
		{
//			CBaseInfoGetter::updateBaseInfoFromEastMoney(&baseInfo);
			CStockInfoItem* pItem = new CStockInfoItem(baseInfo);
			pItem->setLast5Volume(fLast5Volume);
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

	QDataStream in(&file);

	int iCount = 0;
	while(true)
	{
		qRcvReportData* pReport = new qRcvReportData;
		quint32 _t;
		in>>_t>>pReport->wMarket>>pReport->qsCode>>pReport->qsName;
		pReport->tmTime = _t;

		int iSize = in.readRawData((char*)&pReport->fLastClose,sizeof(float)*27);
		if(iSize!=(sizeof(float)*27))
			break;

		QString qsOnly = pReport->qsCode+getMarketStr(pReport->wMarket);
		if(pReport->wMarket == BB_MARKET_EX)
		{
			if(m_mapReportForBlock.contains(qsOnly))
			{
				delete m_mapReportForBlock[qsOnly];
			}

			m_mapReportForBlock[qsOnly] = pReport;
		}
		else
		{
			CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsOnly);
			if(pItem==NULL)
			{
				pItem = new CStockInfoItem(pReport->qsCode,pReport->wMarket);
				CDataEngine::getDataEngine()->setStockInfoItem(pItem);
			}
			pItem->setReport(pReport);
		}
		++iCount;
	}

	file.close();
	return iCount;
}

int CDataEngine::importFenBisData( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return -1;

	int iCount = 0;
	while(true)
	{
		char chCode[STKLABEL_LEN];
		int iSize;
		if(file.read(chCode,STKLABEL_LEN)!=STKLABEL_LEN)
			break;
		if(file.read((char*)&iSize,sizeof(iSize))!=sizeof(iSize))
			break;

		char* pFenBiData = new char[iSize];
		if(file.read(pFenBiData,iSize)!=iSize)
		{
			delete pFenBiData;
			break;
		}

		CAbstractStockItem* pItem = CDataEngine::getDataEngine()->getStockItem(QString::fromLocal8Bit(chCode));
		if(!pItem)
		{
			delete pFenBiData;
			continue;
		}

		messageShowed("导入\""+pItem->getName()+"\"的分笔数据...",40);

		int iIndex = 0;
		QList<qRcvFenBiData*> listFenBis;
	//	QString qsMin;
		while(iIndex<iSize)
		{
			qRcvFenBiData* pData = new qRcvFenBiData();
			memcpy(pData,&pFenBiData[iIndex],sizeof(qRcvFenBiData));
			listFenBis.push_back(pData);
		//	qsMin+= QString("%1\r\n").arg(QDateTime::fromTime_t(pData->tmTime).toString("hh:mm:ss"));

			iIndex += sizeof(qRcvFenBiData);
		}
		pItem->appendFenBis(listFenBis);

		if(pItem->isInstanceOfStock())
		{
			//重新计算该股票的5分钟数据
			pItem->recalcMinData();
			pItem->recalc5MinData();
		}
		delete pFenBiData;

		++iCount;
	}

	file.close();

	//重新计算板块的5分钟数据
	QList<CBlockInfoItem*> listBlocks = CDataEngine::getDataEngine()->getStockBlocks();
	foreach( CBlockInfoItem* pItem, listBlocks)
	{
		messageShowed("计算板块\""+pItem->getName()+"\"数据...",70);
		pItem->recalcMinData();
		pItem->recalc5MinData();
	}

	return iCount;
}

int CDataEngine::importBlocksData( const QString& /*qsPath*/ )
{
	QDir dir(CDataEngine::getDataEngine()->getStockBlockDir());
	QFileInfoList listEntity = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot,QDir::DirsLast);
	foreach(const QFileInfo& _f,listEntity)
	{
		new CBlockInfoItem(_f.absoluteFilePath());
	}
	return CDataEngine::getDataEngine()->getStockBlocks().size();
}

qRcvReportData* CDataEngine::getReportForInitBlock( const QString& qsOnly )
{
	if(m_mapReportForBlock.contains(qsOnly))
		return m_mapReportForBlock[qsOnly];
	return NULL;
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
		float fLast5Volume = pItem->getLast5Volume();
		if(out.writeRawData((char*)&fLast5Volume,sizeof(float))!=sizeof(float))
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

	int iCount = 0;
	{
		//导出Report数据
		QList<CAbstractStockItem*> listItem = CDataEngine::getDataEngine()->getStockItems();
		foreach( CAbstractStockItem* pItem, listItem)
		{
			//保存当天所有的Reports
			qRcvReportData* pReport = pItem->getCurrentReport();
			if(pReport&&(!(pReport->qsCode.isEmpty())))
			{
				out<<pReport->tmTime<<pReport->wMarket<<pReport->qsCode<<pReport->qsName;
				//直接拷贝剩余的所有float数据
				if(out.writeRawData((char*)&pReport->fLastClose,sizeof(float)*27)!=(sizeof(float)*27))
					break;

				++iCount;
			}
		}
	}

	file.close();
	return iCount;
}

int CDataEngine::exportFenBisData( const QString& qsFile )
{
	QList<CAbstractStockItem*> listItem = CDataEngine::getDataEngine()->getStockItems();
	int iCount = 0;

	if(QFile::exists(qsFile))
		QFile::remove(qsFile);
	if(QFile::exists(qsFile))
		return -1;

	QFile file(qsFile);
	if(!file.open(QFile::WriteOnly))
		return -1;

	foreach(CAbstractStockItem* pItem,listItem)
	{
		//保存当天所有的分钟数据
		QList<qRcvFenBiData*> listFenBis= pItem->getFenBiList();
		int iSize = listFenBis.size()*sizeof(qRcvFenBiData);
		char* pFenBiData = new char[iSize];
		for(int i = 0; i<listFenBis.size(); ++i)
		{
			qRcvFenBiData* pData = listFenBis[i];
			memcpy(pFenBiData+i*sizeof(qRcvFenBiData),pData,sizeof(qRcvFenBiData));
		}
		//foreach(qRcvMinuteData* pData,listMins)
		//{
		//	minData.append(pData->tmTime);
		//	minData.append(pData->fPrice);
		//	minData.append(pData->fVolume);
		//	minData.append(pData->fAmount);
		//	minData<<pData->tmTime<<pData->fPrice<<pData->fVolume<<pData->fAmount;
		//	out<<pData->tmTime<<pData->fPrice<<pData->fVolume<<pData->fAmount;
		//}
		char chCode[STKLABEL_LEN];
		memset(chCode,0,STKLABEL_LEN);
		QByteArray arrCode = pItem->getOnly().toLocal8Bit();
		memcpy(chCode,arrCode.data(),arrCode.size());
		file.write(chCode,STKLABEL_LEN);
		file.write((char*)&iSize,sizeof(int));

		file.write(pFenBiData,iSize);
		file.flush();

		delete pFenBiData;

		++iCount;
	}

	file.close();
	return iCount;
}

//收盘后数据的整理
int CDataEngine::exportCloseData()
{
	//导出单只股票数据
	QList<CStockInfoItem*> listStocks = CDataEngine::getDataEngine()->getStockInfoList();
	foreach(CStockInfoItem* pItem,listStocks)
	{
		//导出分钟数据
		CDataEngine::getDataEngine()->exportMinData(pItem);
		qRcvReportData* pReport = pItem->getCurrentReport();
		if(pReport)
		{
			qRcvHistoryData* pData = new qRcvHistoryData(pItem->getCurrentReport());

			//将当天的report追加为历史数据
			pItem->appendHistorys(QList<qRcvHistoryData*>()<<pData);
		}
	}

	//导出板块数据
	QList<CBlockInfoItem*> listBlocks = CDataEngine::getDataEngine()->getStockBlocks();
	foreach(CBlockInfoItem* pItem,listBlocks)
	{
		//导出5min数据
		pItem->recalcMinData();
		CDataEngine::getDataEngine()->exportMinData(pItem);
		qRcvReportData* pReport = pItem->getCurrentReport();
		if(pReport)
		{
			qRcvHistoryData* pData = new qRcvHistoryData(pItem->getCurrentReport());
			{
				//计算涨跌数
				float fAdv=0,fDec=0;
				for (int i=0;i<10;++i)
				{
					fAdv += pReport->fBuyPrice[i];
				}
				for (int i=10;i<19;++i)
				{
					fDec += pReport->fBuyPrice[i];
				}
				pData->wAdvance = fAdv + 0.5;
				pData->wDecline = fDec + 0.5;
			}
			//将当天的report追加为历史数据
			pItem->appendHistorys(QList<qRcvHistoryData*>()<<pData);
		}
	}

	return -1;
}


bool CDataEngine::isStockOpenDay( time_t tmDay )
{
	QDate tmDate = QDateTime::fromTime_t(tmDay).date();
	int iWeek = tmDate.dayOfWeek();
	if((iWeek == 6)||(iWeek == 7))
		return false;

	return true;
}

bool CDataEngine::isStockOpenTime( time_t tmMin )
{
	QTime tmTime = QDateTime::fromTime_t(tmMin).time();
	uint tmT = tmTime.hour()*60+tmTime.second();

	if(tmT<(9*60+25))
		return false;
	
	if(tmT>11*60+35 && tmT<12*60+55)
		return false;

	if(tmT>15*60+5)
		return false;

	return true;
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
	if(tmTime>tmBegin1)
		return tmTime-tmBegin1;


	return 0;
}

time_t CDataEngine::getCurrentTime()
{
	if(m_tmCurrent<1)
		return QDateTime::currentDateTime().toTime_t();
	return m_tmCurrent;
}
void CDataEngine::setCurrentTime(const time_t& t)
{
	if(m_tmCurrent<t)
		m_tmCurrent = t;
}

QMap<time_t,int> CDataEngine::getTodayTimeMap( RStockCircle _c )
{
//	int iCount = 1024;				//计算1024个时间
	QMap<time_t,int> mapTimes;
	if(AutoCircle == _c)
	{
		/*
		2分钟的10s周期（6*2=12）
		10分钟的1m周期（10）
		其余为5m周期
		*/
		time_t tmCur = CDataEngine::getCurrentTime();
		time_t tmLast = ((tmCur/(3600*24))*3600*24)+3600*(9-8)+60*25;	//9：25开盘
		time_t tmCurrent = (tmCur+Sec3)/Sec3*Sec3;						//向上对周期取整
		time_t tmNoon1 = ((tmCur/(3600*24))*3600*24)+3600*(11-8)+60*30;
		time_t tmNoon2 = ((tmCur/(3600*24))*3600*24)+3600*(13-8);


		/*需向上和向下多计算一个周期*/
		if(tmCurrent>tmNoon2)
		{
			time_t tmC2N = (tmCurrent-tmNoon2+Min5);
			int iSec3Count = (tmC2N/Sec3 - tmC2N/Sec6*2);
			int iSec6Count = (tmC2N/Sec6 - tmC2N/Sec12*2);
			int iSec12Count = (tmC2N/Sec12 - tmC2N/Min1*5);
			int iMin1Count = (tmC2N/Min1 - tmC2N/Min5*5);
			int iMin5Count = (tmC2N/Min5);

			//计算时间轴
			time_t tmEnd = tmCurrent;
			time_t tmBegin = tmEnd-(Sec3*iSec3Count);
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,Sec3);
			tmEnd = tmBegin;
			tmBegin = tmEnd-(Sec6*iSec6Count);
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,Sec6);
			tmEnd = tmBegin;
			tmBegin = tmEnd-(Sec12*iSec12Count);
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,Sec12);
			tmEnd = tmBegin;
			tmBegin = tmEnd-(Min1*iMin1Count);
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,Min1);
			tmEnd = tmBegin;
			tmBegin = tmEnd-(Min5*iMin5Count);
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,Min5);

			//计算上午的时间轴
			getTimeMapBySec(mapTimes,tmLast-Min5,tmNoon1+Min5,Min5);
		}
		else
		{
			time_t tmC2N = (tmCurrent-tmLast+Min5);
			int iSec3Count = (tmC2N/Sec3 - tmC2N/Sec6*2);
			int iSec6Count = (tmC2N/Sec6 - tmC2N/Sec12*2);
			int iSec12Count = (tmC2N/Sec12 - tmC2N/Min1*5);
			int iMin1Count = (tmC2N/Min1 - tmC2N/Min5*5);
			int iMin5Count = (tmC2N/Min5);

			//计算时间轴
			time_t tmEnd = tmCurrent;
			time_t tmBegin = tmEnd-(Sec3*iSec3Count);
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,Sec3);
			tmEnd = tmBegin;
			tmBegin = tmEnd-(Sec6*iSec6Count);
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,Sec6);
			tmEnd = tmBegin;
			tmBegin = tmEnd-(Sec12*iSec12Count);
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,Sec12);
			tmEnd = tmBegin;
			tmBegin = tmEnd-(Min1*iMin1Count);
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,Min1);
			tmEnd = tmBegin;
			tmBegin = tmEnd-(Min5*iMin5Count);
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,Min5);
		}
	}
	else if(_c<Day)
	{
		time_t tmCur = CDataEngine::getCurrentTime();
		time_t tmLast = ((tmCur/(3600*24))*3600*24)+3600*(9-8)+60*25;	//9：25开盘
		time_t tmCurrent = (tmCur+_c*2)/_c*_c;//向上对分钟取整
		time_t tmNoon1 = ((tmCur/(3600*24))*3600*24)+3600*(11-8)+60*30;
		time_t tmNoon2 = ((tmCur/(3600*24))*3600*24)+3600*(13-8);


		if((tmCurrent%(3600*24))>3600*7)
		{
			tmCurrent = (tmCurrent/(3600*24))*3600*24 + 3600*7 + _c;		//3点收盘(多加一个周期)
		}
		/*需向上和向下多计算一个周期*/
		if(tmCurrent>tmNoon2)
		{
			time_t tmBegin = tmNoon2-_c;
			//time_t tmEnd = tmCurrent+m_typeCircle*2;
			getTimeMapBySec(mapTimes,tmBegin,tmCurrent,_c);
		}

		if(tmCurrent>tmNoon1)
		{
			time_t tmBegin = tmLast-_c;
			time_t tmEnd = tmNoon1+_c;
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,_c);
		}
		else if(tmCurrent>tmLast)
		{
			time_t tmBegin = tmLast-_c;
			time_t tmEnd = tmCurrent;
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,_c);
		}
	}

	return mapTimes;
}

QMap<time_t,int> CDataEngine::getHistoryTimeMap( RStockCircle _c,int iCount )
{
	QMap<time_t,int> mapTimes;
	time_t tmEnd = (CDataEngine::getCurrentTime()/(3600*24))*3600*24 - 8*3600;
	time_t tmBegin = QDateTime::fromTime_t(tmEnd).addDays(-iCount).toTime_t();

	if(_c == Day)
	{
		getDayMapByHistory(mapTimes,tmBegin,tmEnd);
	}
	else if(_c == DayN)
	{
		//目前未使用
	}
	else if(_c == Week)
	{
		getWeekMapByHistory(mapTimes,tmBegin,tmEnd);
	}
	else if(_c == Month)
	{
		getMonthMapByHistory(mapTimes,tmBegin,tmEnd);
	}
	else if(_c == Month3)
	{
		getMonth3MapByHistory(mapTimes,tmBegin,tmEnd);
	}
	else if(_c == Year)
	{
		getYearMapByHistory(mapTimes,tmBegin,tmEnd);
	}

	return mapTimes;
}

QMap<time_t,RStockData*>* CDataEngine::getColorBlockItems( const QMap<time_t,int>& mapTimes, const QList<qRcvFenBiData*>& minutes )
{
	QMap<time_t,RStockData*>* pMap = new QMap<time_t,RStockData*>();
	if(mapTimes.size()<1)
	{
		return pMap;
	}

	QMap<time_t,int>::iterator iter = mapTimes.begin();

	time_t tmBegin = iter.key();
	if(mapTimes.size()<2)
	{
		QList<qRcvFenBiData*> listFenBi;
		foreach(qRcvFenBiData* pFenBi,minutes)
		{
			if(pFenBi->tmTime>tmBegin)
				listFenBi.push_back(pFenBi);
		}
		pMap->insert(tmBegin,getColorBlockItemByMins(listFenBi,NULL));
		return pMap;
	}

	time_t tmEnd = (iter+1).key();

	QList<qRcvFenBiData*> listPer;
	qRcvFenBiData* pLastFenBi = NULL;
	foreach(qRcvFenBiData* pFenBi,minutes)
	{
		if(pFenBi->tmTime<tmBegin)
		{
			pLastFenBi = pFenBi;
			continue;
		}
		else if(pFenBi->tmTime>=tmBegin&&pFenBi->tmTime<=tmEnd)
		{
			listPer.push_back(pFenBi);
		}
		else
		{
			pMap->insert(tmBegin,getColorBlockItemByMins(listPer,pLastFenBi));
			if(listPer.size()>0)
				pLastFenBi = listPer.last();
			listPer.clear();

			++iter;
			while (iter!=mapTimes.end())
			{
				tmBegin = iter.key();
				if(iter!=mapTimes.end())
					tmEnd = (iter+1).key();
				else
					tmEnd = tmBegin+3600*24*1000;		//加1000天的时间

				if(pFenBi->tmTime>=tmBegin&&pFenBi->tmTime<=tmEnd)
				{
					listPer.push_back(pFenBi);
					break;
				}
				else
				{
					pMap->insert(tmBegin,NULL);
				}
				++iter;
			}

			if(iter==mapTimes.end())
				break;
		}
	}

	//获取最后一个数据块，并进行计算
	RStockData* pLastData = getColorBlockItemByMins(listPer,pLastFenBi);
	while(iter!=mapTimes.end())
	{
		tmBegin = iter.key();
		if(pLastData && pLastData->tmTime>=tmBegin)
		{
			pMap->insert(tmBegin,pLastData);
			pLastData = NULL;
		}
		else
		{
			pMap->insert(tmBegin,NULL);
		}
		++iter;
	}
	delete pLastData;

	if(pMap->size()!=mapTimes.size())
	{
		qDebug()<<"FenBi Map is not enouph";
	}

	return pMap;
}

QMap<time_t,RStockData*>* CDataEngine::getColorBlockItems( const QMap<time_t,int>& mapTimes, const QList<qRcvHistoryData*>& minutes )
{
	QMap<time_t,RStockData*>* pMap = new QMap<time_t,RStockData*>();
	if(mapTimes.size()<1)
		return pMap;

	QMap<time_t,int>::iterator iter = mapTimes.begin();

	time_t tmBegin = iter.key();
	if(mapTimes.size()<2)
	{
		QList<qRcvHistoryData*> listFenBi;
		foreach(qRcvHistoryData* pFenBi,minutes)
		{
			if(pFenBi->time>tmBegin)
				listFenBi.push_back(pFenBi);
		}
		pMap->insert(tmBegin,getColorBlockItemByDays(listFenBi));
		return pMap;
	}

	time_t tmEnd = (iter+1).key();

	QList<qRcvHistoryData*> listPer;
	qRcvHistoryData* pLastFenBi = NULL;
	foreach(qRcvHistoryData* pFenBi,minutes)
	{
		if(pFenBi->time<tmBegin)
		{
			pLastFenBi = pFenBi;
			continue;
		}
		else if(pFenBi->time>=tmBegin&&pFenBi->time<=tmEnd)
			listPer.push_back(pFenBi);
		else
		{
			if(listPer.size()>0)
				pLastFenBi = listPer.last();
			pMap->insert(tmBegin,getColorBlockItemByDays(listPer));
			listPer.clear();

			++iter;
			while (iter!=mapTimes.end())
			{
				tmBegin = iter.key();
				if(iter!=mapTimes.end())
					tmEnd = (iter+1).key();
				else
					tmEnd = tmBegin+3600*24*1000;		//加1000天的时间

				if(pFenBi->time>=tmBegin&&pFenBi->time<=tmEnd)
				{
					listPer.push_back(pFenBi);
					break;
				}
				else
				{
					pMap->insert(tmBegin,NULL);
				}
				++iter;
			}

			if(iter==mapTimes.end())
				break;
		}
	}

	//获取最后一个数据块，并进行计算
	RStockData* pLastData = getColorBlockItemByDays(listPer);
	while(iter!=mapTimes.end())
	{
		tmBegin = iter.key();
		if(pLastData && pLastData->tmTime>=tmBegin)
		{
			pMap->insert(tmBegin,pLastData);
			pLastData = NULL;
		}
		else
		{
			pMap->insert(tmBegin,NULL);
		}
		++iter;
	}
	delete pLastData;

	if(pMap->size()!=mapTimes.size())
	{
		qDebug()<<"Day Map is not enouph";
	}
	return pMap;
}


CDataEngine::CDataEngine(void)
	: m_pBlockCustom(0)
{
	m_qsHistroyDir = qApp->applicationDirPath()+"/data/history/";
	QDir().mkpath(m_qsHistroyDir);
	QDir().mkpath(m_qsHistroyDir+getMarketStr(SH_MARKET_EX));
	QDir().mkpath(m_qsHistroyDir+getMarketStr(SZ_MARKET_EX));
	QDir().mkpath(m_qsHistroyDir+getMarketStr(HK_MARKET_EX));
	QDir().mkpath(m_qsHistroyDir+getMarketStr(EB_MARKET_EX));
	QDir().mkpath(m_qsHistroyDir+getMarketStr(BB_MARKET_EX));

	m_qsBlocksDir = qApp->applicationDirPath()+"/config/blocks/";
	QDir().mkpath(m_qsBlocksDir);

	m_qsNewsDir = qApp->applicationDirPath()+"/data/news/";
	QDir().mkpath(m_qsNewsDir);

	m_qsF10Dir = qApp->applicationDirPath()+"/data/F10/";
	QDir().mkpath(m_qsF10Dir);
/*	QDir().mkpath(m_qsF10Dir+getMarketStr(SH_MARKET_EX));
	QDir().mkpath(m_qsF10Dir+getMarketStr(SZ_MARKET_EX));
	QDir().mkpath(m_qsF10Dir+getMarketStr(HK_MARKET_EX));
	QDir().mkpath(m_qsF10Dir+getMarketStr(EB_MARKET_EX));
	QDir().mkpath(m_qsF10Dir+getMarketStr(BB_MARKET_EX));*/

	m_qsMin = qApp->applicationDirPath()+"/data/min/";
	QDir().mkpath(m_qsMin);
	QDir().mkpath(m_qsMin+getMarketStr(SH_MARKET_EX));
	QDir().mkpath(m_qsMin+getMarketStr(SZ_MARKET_EX));
	QDir().mkpath(m_qsMin+getMarketStr(HK_MARKET_EX));
	QDir().mkpath(m_qsMin+getMarketStr(EB_MARKET_EX));
	QDir().mkpath(m_qsMin+getMarketStr(BB_MARKET_EX));

	m_qsFenBiDir = qApp->applicationDirPath()+"/data/FenBi/";
	QDir().mkpath(m_qsFenBiDir);


	//导入汉字->拼音对照表
	CHz2Py::initHz2PyTable(qApp->applicationDirPath()+"/config/PY.hz");
}

CDataEngine::~CDataEngine(void)
{
#ifndef _DEBUG
	QMap<QString,CStockInfoItem*>::iterator iter = m_mapStockInfos.begin();
	while(iter!=m_mapStockInfos.end())
	{
		delete iter.value();
		++iter;
	}
#endif // _DEBUG
	m_mapStockInfos.clear();
}

QList<CBlockInfoItem*> CDataEngine::getTopLevelBlocks()
{
	return m_listTopLevelBlocks;
}

QList<CBlockInfoItem*> CDataEngine::getStockBlocks()
{
	return m_mapBlockInfos.values();
}

CBlockInfoItem* CDataEngine::getStockBlock( const QString& qsCode )
{
	if(m_mapBlockInfos.contains(qsCode))
	{
		return m_mapBlockInfos[qsCode];
	}
	return NULL;
}

bool CDataEngine::isHadBlock( const QString& block )
{
	return m_mapBlockInfos.contains(block);
}

void CDataEngine::setBlockInfoItem( CBlockInfoItem* _p )
{
	if(_p->parentBlock()==0)
	{
		if(_p->getName()=="自选板块")
		{
			m_pBlockCustom = _p;
		}
		m_listTopLevelBlocks.push_back(_p);
	}
	m_mapBlockInfos[_p->getOnly()] = _p;
}

void CDataEngine::removeBlockInfoItem( CBlockInfoItem* _p )
{
	if(_p->parentBlock()==0)
		m_listTopLevelBlocks.removeOne(_p);
	m_mapBlockInfos.remove(_p->getOnly());
}

CBlockInfoItem* CDataEngine::getCustomBlock()
{
	return m_pBlockCustom;
}



QList<CStockInfoItem*> CDataEngine::getStocksByMarket( WORD wMarket )
{
	QList<CStockInfoItem*> listStocks;
	QMap<QString,CStockInfoItem*>::iterator iter = m_mapStockInfos.begin();
	while(iter!=m_mapStockInfos.end())
	{
		if((*iter)->getMarket() == wMarket)
			listStocks.push_back(iter.value());
		++iter;
	}

	return listStocks;
}

QList<CStockInfoItem*> CDataEngine::getStockInfoList()
{
	return m_mapStockInfos.values();
}

CStockInfoItem* CDataEngine::getStockInfoItem( const QString& qsOnly )
{
	if(m_mapStockInfos.contains(qsOnly))
	{
		return m_mapStockInfos[qsOnly];
	}
	return NULL;
}

CStockInfoItem* CDataEngine::getStockInfoItemByCode( const QString& qsCode )
{
	//先从深证查找股票代码
	QString qsOnly = qsCode+getMarketStr(SZ_MARKET_EX);
	if(m_mapStockInfos.contains(qsOnly))
	{
		return m_mapStockInfos[qsOnly];
	}
	//再从上证中查找股票代码
	qsOnly = qsCode+getMarketStr(SH_MARKET_EX);
	if(m_mapStockInfos.contains(qsOnly))
	{
		return m_mapStockInfos[qsOnly];
	}
	return NULL;
}

void CDataEngine::setStockInfoItem( CStockInfoItem* p )
{
	m_mapStockInfos[p->getOnly()] = p;
}


CAbstractStockItem* CDataEngine::getStockItem(const QString& qsCode)
{
	if(m_mapStockInfos.contains(qsCode))
	{
		return m_mapStockInfos[qsCode];
	}

	if(m_mapBlockInfos.contains(qsCode))
	{
		return m_mapBlockInfos[qsCode];
	}

	return NULL;
}

QList<CAbstractStockItem*> CDataEngine::getStockItems()
{
	QMap<QString,CAbstractStockItem*> maps;

	QMap<QString,CStockInfoItem*>::iterator iterStock = m_mapStockInfos.begin();
	while(iterStock!=m_mapStockInfos.end())
	{
		maps.insert(iterStock.key(),iterStock.value());
		++iterStock;
	}

	QMap<QString,CBlockInfoItem*>::iterator iterBlock = m_mapBlockInfos.begin();
	while(iterBlock!=m_mapBlockInfos.end())
	{
		maps.insert(iterBlock.key(),iterBlock.value());
		++iterBlock;
	}

	return maps.values();
}


void CDataEngine::appendNews( const QString& title, const QString& content )
{
	QString qsNewsPath = m_qsNewsDir + title;

	QDir().mkpath(QFileInfo(qsNewsPath).absolutePath());
	QFile file(qsNewsPath);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return;
	file.write(content.toAscii());
	file.close();
}

void CDataEngine::appendF10( const QString& title, const QString& content )
{
	QString qsF10Path = m_qsF10Dir + title;

	QDir().mkpath(QFileInfo(qsF10Path).absolutePath());
	QFile file(qsF10Path);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return;
	file.write(content.toAscii());
	file.close();
}

bool CDataEngine::showF10Data( const QString& code )
{
	/*显示F10数据*/
	QString qsF10Path = QString("%1%2.txt").arg(m_qsF10Dir).arg(code);
	if(QFile::exists(qsF10Path))
	{
		return QProcess::startDetached("notepad.exe",QStringList()<<qsF10Path);
	}
	return false;
}

bool CDataEngine::exportHistoryData( CAbstractStockItem* pItem, const QList<qRcvHistoryData*>& list )
{
	if(!pItem)
		return false;
	QString qsDayData = QString("%1/%2/%3").arg(m_qsHistroyDir).arg(pItem->getMarketName()).arg(pItem->getCode());

	QFile file(qsDayData);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return false;

	QDataStream out(&file);

	foreach(qRcvHistoryData* pData, list)
	{
		int iSize = out.writeRawData((char*)pData,sizeof(qRcvHistoryData));
		if(iSize!=sizeof(qRcvHistoryData))
		{
			file.close();
			return false;
		}
	}

	file.close();
	return true;
}

bool CDataEngine::exportHistoryData( CAbstractStockItem* pItem, const QList<qRcvHistoryData*>& list, int iOffset )
{
	if(!pItem)
		return false;
	QString qsDayData = QString("%1/%2/%3").arg(m_qsHistroyDir).arg(pItem->getMarketName()).arg(pItem->getCode());

	QFile file(qsDayData);
	if(!file.open(QFile::ReadWrite))
		return false;

	int iPos = 0;
	if(iOffset>=0)
	{
		iPos = file.size()-sizeof(qRcvHistoryData)*iOffset;
	}
	if(iPos<0)
	{
		iPos = 0;
	}

	if(!(file.resize(iPos))||(!file.seek(iPos)))
		return false;

	foreach(qRcvHistoryData* pData, list)
	{
		int iSize = file.write((char*)pData,sizeof(qRcvHistoryData));
		if(iSize!=sizeof(qRcvHistoryData))
		{
			file.close();
			return false;
		}
	}

	file.close();
	return true;
}

QList<qRcvHistoryData*> CDataEngine::getHistoryList( CAbstractStockItem* pItem )
{
	QList<qRcvHistoryData*> list;
	
	QString qsDayData = QString("%1/%2/%3").arg(m_qsHistroyDir).arg(pItem->getMarketName()).arg(pItem->getCode());
	QFile file(qsDayData);
	if(!file.open(QFile::ReadOnly))
		return list;

	QDataStream inStream(&file);
	while(!inStream.atEnd())
	{
		qRcvHistoryData* pData = new qRcvHistoryData;
		int iSize = inStream.readRawData(reinterpret_cast<char*>(pData),sizeof(qRcvHistoryData));
		if(iSize!=sizeof(qRcvHistoryData))
		{
			delete pData;
			break;
		}
		list.push_back(pData);
	}

	file.close();
	return list;
}

QList<qRcvHistoryData*> CDataEngine::getHistoryList( CAbstractStockItem* pItem, int count )
{
	QList<qRcvHistoryData*> list;
	
	QString qsDayData = QString("%1/%2/%3").arg(m_qsHistroyDir).arg(pItem->getMarketName()).arg(pItem->getCode());
	QFile file(qsDayData);
	if(!file.open(QFile::ReadOnly))
		return list;
	int iDataSize = sizeof(qRcvHistoryData);

	int iPos = file.size()-iDataSize*count;
	if(iPos<0)
		iPos = 0;
	file.seek(iPos);

	while(true)
	{
		qRcvHistoryData* pData = new qRcvHistoryData;
		int iSize = file.read(reinterpret_cast<char*>(pData),iDataSize);
		if(iSize!=iDataSize)
		{
			delete pData;
			break;
		}
		list.push_back(pData);
	}

	file.close();
	return list;
}

bool CDataEngine::exportMinData( CAbstractStockItem* pItem )
{
	//导出1分钟数据，对于非今日的数据只以min为最小单位存储
	//60*sizeof(RStockData)
	if(!pItem)
		return false;

	pItem->recalcMinData();
	qRcvReportData* pReport = pItem->getCurrentReport();
	if(pReport->tmTime<1)
		return false;

	time_t tmDate = QDateTime(QDateTime::fromTime_t(pReport->tmTime).date()).toTime_t();
	tmDate = tmDate-3600*24*7;		//7天以前的数据不再保存

	QList<tagRStockData*> listData = pItem->getMinList();
	if(listData.size()>0)
	{
		if( ((listData.last()->tmTime+1)%60) != 0)
		{
			listData.removeLast();
		}
	}

	QString qsFileName = QString("%1/%2/%3").arg(m_qsMin).arg(pItem->getMarketName()).arg(pItem->getCode());
	QFile file(qsFileName);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
	{
		qDebug()<<"Open file\""<<qsFileName<<"\" error!";
		return false;
	}
	QDataStream out(&file);

	{
		int iSizeOfStruct = sizeof(RStockData);
		foreach(tagRStockData* pData,listData)
		{
			if(pData->tmTime>tmDate)
			{
				if(file.write(reinterpret_cast<char*>(pData),iSizeOfStruct)!=iSizeOfStruct)
				{
					break;
				}
			}
		}
	}

	file.close();
	return true;
}

void CDataEngine::importMinData( CAbstractStockItem* pItem, QMap<time_t,RStockData*>& mapDatas )
{
	QString qsFileName = QString("%1/%2/%3").arg(m_qsMin).arg(pItem->getMarketName()).arg(pItem->getCode());
	if(!QFile::exists(qsFileName))
		return;

	QFile file(qsFileName);
	if(!file.open(QFile::ReadOnly))
	{
		qDebug()<<"Open file\""<<qsFileName<<"\" error!";
		return;
	}

	QDataStream out(&file);
	{
		int iSizeOfStruct = sizeof(RStockData);
		while(true)
		{
			RStockData* pData = new RStockData();
			if(out.readRawData((char*)pData,iSizeOfStruct) == iSizeOfStruct)
			{
				if(!mapDatas.contains(pData->tmTime))
				{
					mapDatas.insert(pData->tmTime,pData);
				}
				else
					delete pData;
			}
			else
			{
				delete pData;
				break;
			}
		}
	}

	file.close();
	return;
}

bool CDataEngine::exportFenBiData( const QString& qsCode, const long& lDate, const QList<qRcvFenBiData*>& list )
{
	QString qsPath = QString("%1%2/").arg(m_qsFenBiDir).arg(lDate);
	QDir().mkpath(qsPath);
	QString qsFenBiData = QString("%1%2").arg(qsPath).arg(qsCode);

	QFile file(qsFenBiData);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return false;

	QDataStream out(&file);

	foreach(qRcvFenBiData* pData, list)
	{
		//int iSize = file.write((char*)pData,sizeof(qRcvFenBiData));
		//if(iSize!=sizeof(qRcvFenBiData))
		//{
		//	file.close();
		//	return false;
		//}
		QString qsLine = QString("time:%1	Price:%2	Buy1:%3	Sell1:%4\r\n")
			.arg(pData->tmTime).arg(pData->fPrice)
			.arg(pData->fBuyPrice[0]).arg(pData->fSellPrice[1]);
		file.write(qsLine.toLocal8Bit());
	}

	file.close();
	return true;
}



QString CDataEngine::getMarketStr( WORD wMarket )
{
	switch(wMarket)
	{
	case SH_MARKET_EX:
		return "SH";
	case SZ_MARKET_EX:
		return "SZ";
	case HK_MARKET_EX:
		return "HK";
	case EB_MARKET_EX:
		return "EB";
	case BB_MARKET_EX:
		return "BB";
	}

	//未知类型
	return "UN";
}

SplashMessage CDataEngine::messageShowed = NULL;

bool CDataEngine::m_bLoading = false;