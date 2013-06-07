/************************************************************************/
/* 文件名称：BlockInfoItem.cpp
/* 创建时间：2013-01-08 09:28
/*
/* 描    述：用于管理板块数据
/*
/* 更新间隔：暂时定为30秒更新一次
/* 计算方式：取前十只股票进行加权平均
/************************************************************************/
#include "StdAfx.h"
#include "BlockInfoItem.h"
#include "DataEngine.h"
#include "BlockCodeManager.h"
#include "Hz2Py.h"
#define	MAX_STOCK_IN_BLOCK	3000
#define UPDATE_BLOCK_TIME	60*1000

CBlockInfoItem::CBlockInfoItem( const QString& _file,CBlockInfoItem* parent/*=0*/ )
	: fIncrease(FLOAT_NAN)
	, fLTSZ(FLOAT_NAN)
	, fLTG(FLOAT_NAN)
	, fLastClose(FLOAT_NAN)
	, fOpenPrice(FLOAT_NAN)
	, fNewPrice(FLOAT_NAN)
	, fLowPrice(FLOAT_NAN)
	, fHighPrice(FLOAT_NAN)
	, m_pParent(parent)
	, blockFilePath(_file)
	, m_pCurFenBi(0)
{
	pCurrentReport = new qRcvReportData;
	QFileInfo _info(blockFilePath);
	if(!_info.exists())
		return;
	blockFilePath = _file;
	blockName = _info.baseName();

	//更新词库表中的简拼
	shortName = CHz2Py::getHzFirstLetter(blockName);


	/*设置板块代码、市场类型、唯一标识*/
	qsCode = CBlockCodeManager::getBlockCode(getAbsPath());
	wMarket = BB_MARKET_EX;
	qsMarket = CDataEngine::getMarketStr(wMarket);
	qsOnly = qsCode+qsMarket;

	pCurrentReport->tmTime = QDateTime::currentDateTime().toTime_t();
	pCurrentReport->qsCode = qsCode;
	pCurrentReport->wMarket = BB_MARKET_EX;

	connect(&timerUpdate,SIGNAL(timeout()),this,SLOT(updateData()));
	timerUpdate.start(UPDATE_BLOCK_TIME);
	updateData();

	CDataEngine::getDataEngine()->setBlockInfoItem(this);

	initBlock();
}

CBlockInfoItem::~CBlockInfoItem(void)
{
	CDataEngine::getDataEngine()->removeBlockInfoItem(this);
	clearTmpData();
	delete pCurrentReport;
}

void CBlockInfoItem::initBlock()
{
	clearTmpData();
	QFileInfo _info(blockFilePath);
	if(!_info.exists())
		return;

	if(_info.isFile())
	{
		QFile file(blockFilePath);
		if(file.open(QFile::ReadOnly))
		{
			QString qsType = file.readLine().trimmed();
			if(qsType == "RegExp")
			{
				QString qsExp = file.readLine().trimmed();
				if(!qsExp.isEmpty())
				{
					QRegExp _exp(qsExp);
					_exp.setPatternSyntax(QRegExp::Wildcard);
					QList<CStockInfoItem*> listStocks = CDataEngine::getDataEngine()->getStockInfoList();
					foreach(CStockInfoItem* p,listStocks)
					{
						if(_exp.exactMatch(p->getCode()))
						{
							addStock(p);
						}
					}
				}
			}
			else
			{
				file.seek(0);
				while(!file.atEnd())
				{
					QString code = file.readLine();
					code = code.trimmed();
					if(!code.isEmpty())
					{
						addStock(CDataEngine::getDataEngine()->getStockInfoItemByCode(code));
					}
				}
			}
			file.close();
		}
	}
	else if(_info.isDir())
	{
		QDir dir(blockFilePath);
		QFileInfoList listEntity = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
		foreach(const QFileInfo& _f,listEntity)
		{
			CBlockInfoItem* pChild = new CBlockInfoItem(_f.absoluteFilePath(),this);
			appendBlock(pChild);
		}
	}
}

QList<qRcvFenBiData*> CBlockInfoItem::getFenBiList()
{
	QList<qRcvFenBiData*> list = mapFenBis.values();
	if(m_pCurFenBi)
		list.append(m_pCurFenBi);
	return list;
}

void CBlockInfoItem::appendHistorys( const QList<qRcvHistoryData*>& list )
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
	//{
	//	//最近5日的全部成交量（用于量比的计算）
	//	fLast5Volume = 0.0;
	//	for (int i=1;i<=5;++i)
	//	{
	//		int iIndex = listHistory.size()-i;
	//		if(iIndex<0)
	//			break;
	//		fLast5Volume = fLast5Volume + listHistory[iIndex]->fVolume;
	//	}
	//	updateItemInfo();
	//}

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


QString CBlockInfoItem::getAbsPath()
{
	if(m_pParent)
	{
		return m_pParent->getAbsPath()+"|"+blockName;
	}
	else
	{
		return blockName;
	}
}

CBlockInfoItem* CBlockInfoItem::querySubBlock( const QStringList& _parent )
{
	QStringList listBlocks = _parent;
	if(listBlocks.size()<0)
		return 0;

	if(blocksInBlock.contains(listBlocks[0]))
	{
		CBlockInfoItem* pBlockItem = blocksInBlock[listBlocks[0]];
		if(listBlocks.size()==1)
			return pBlockItem;
		else
		{
			listBlocks.removeAt(0);
			return pBlockItem->querySubBlock(listBlocks);
		}
	}
	return 0;
}

bool CBlockInfoItem::isChildOf( CBlockInfoItem* parent)
{
	return getAbsPath().indexOf(parent->getAbsPath())>-1;
}

QList<CAbstractStockItem*> CBlockInfoItem::getAbsStockList()
{
	QList<CAbstractStockItem*> list;
	foreach(CBlockInfoItem* _p,blocksInBlock)
		list.push_back(_p);
	foreach(CStockInfoItem* _p,stocksInBlock)
		list.push_back(_p);

	return list;
}

QList<CStockInfoItem*> CBlockInfoItem::getStockList()
{
	return stocksInBlock;
}

QList<CBlockInfoItem*> CBlockInfoItem::getBlockList()
{
	return blocksInBlock.values();
}

int CBlockInfoItem::getStockCount() const
{
	return stocksInBlock.size();
}

bool CBlockInfoItem::hasBlocks()
{
	return blocksInBlock.size()>0 ? true : false;
}

bool CBlockInfoItem::appendStocks( QList<CStockInfoItem*> list )
{
	QStringList listCodes;
	foreach(CStockInfoItem* pItem,list)
		listCodes.push_back(pItem->getOnly());

	if(appendStocks(listCodes))
	{
		foreach(CStockInfoItem* pItem,list)
		{
			if(!stocksInBlock.contains(pItem))
				stocksInBlock.push_back(pItem);
		}
		return true;
	}
	else
		return false;
}

bool CBlockInfoItem::appendStocks( QList<QString> list )
{
	if(blockFilePath.isEmpty())
		return false;

	QFile file(blockFilePath);
	if(!file.open(QFile::Append|QFile::WriteOnly))
		return false;
	foreach(const QString& e,list)
	{
		file.write(QString(e+"\r\n").toAscii());
	}
	file.close();

	return true;
}

bool CBlockInfoItem::removeStocks( QList<CStockInfoItem*> list )
{
	QStringList listCodes;
	foreach(CStockInfoItem* pItem,list)
		listCodes.push_back(pItem->getOnly());

	if(removeStocks(listCodes))
	{
		foreach(CStockInfoItem* pItem,list)
		{
			stocksInBlock.removeOne(pItem);
		}
		return true;
	}
	else
		return false;
}

bool CBlockInfoItem::removeStocks( QList<QString> list )
{
	if(blockFilePath.isEmpty())
		return false;

	QMap<QString,QString> mapStocks;
	{
		//读取文件中的股票代码
		QFile file(blockFilePath);
		if(!file.open(QFile::ReadOnly))
			return false;

		while(!file.atEnd())
		{
			QString code = file.readLine();
			code = code.trimmed();
			if(!code.isEmpty())
			{
				mapStocks[code] = code;
			}
		}
		file.close();
	}
	{
		//删除对应的股票代码
		foreach(const QString& e,list)
		{
			mapStocks.remove(e);
		}
	}
	{
		//重新写回去
		QFile file(blockFilePath);
		if(!file.open(QFile::Truncate|QFile::WriteOnly))
			return false;

		QMap<QString,QString>::iterator iter = mapStocks.begin();
		while(iter!=mapStocks.end())
		{
			file.write(QString(iter.value()+"\r\n").toAscii());
			++iter;
		}
		file.close();
	}

	return true;
}

bool CBlockInfoItem::appendBlock( CBlockInfoItem* pBlock )
{
	blocksInBlock[pBlock->getBlockName()] = pBlock;
	return true;
}

void CBlockInfoItem::stockFenbiChanged( const QString& /*_code*/ )
{

}

void CBlockInfoItem::stockHistoryChanged( const QString& /*_code*/ )
{

}

void CBlockInfoItem::updateData()
{
	/*
	更新间隔：暂时定为1分钟更新一次
	计算方式：取前十只股票进行加权平均
	*/
	if((QDateTime::currentDateTime().toTime_t()-CDataEngine::getDataEngine()->getCurrentTime())>70)
	{
		//如果当前时间和最后的时间相差大于70秒(取10秒的延迟)，直接忽略
		return;
	}

	if(m_pCurFenBi==0)
	{
		//一次性计算的值
		//计算总流通股、昨日收盘价、今日开盘价
		double dLTG = 0.0;			//流通股（万）
		double dLastClose = 0.0;	//昨日收盘价
		double dOpen = 0.0;			//今日开盘价

		int iCount = stocksInBlock.size();
		if(iCount>MAX_STOCK_IN_BLOCK)
			iCount = MAX_STOCK_IN_BLOCK;
		for(int i=0;i<iCount;++i)
		{
			CStockInfoItem* pStock = stocksInBlock[i];
			if(pStock->getNewPrice()>0.1)
			{
				float fLTAG = pStock->getBaseInfo()->fLtAg;		//流通股
				dLTG += fLTAG;
				dLastClose += pStock->getLastClose()*fLTAG;
				dOpen += pStock->getOpenPrice()*fLTAG;
			}

			//设置初始值
			mapLast5Price[pStock] = pStock->getNewPrice();
		}

		if(dLTG<0.1)
		{
			return;
		}

		fLastClose = dLastClose/dLTG;
		fOpenPrice = dOpen/dLTG;
		fLTG = dLTG;
	}

	double dNew = 0.0;			//最新价
	double dLow = 0.0;			//最低价
	double dHigh = 0.0;			//最高价
	fVolume = 0.0;				//成交量
	fAmount = 0.0;				//成交额
	float fInc[20];		//20个增长
	memset(&fInc,0,sizeof(float)*20);
	int iCount = stocksInBlock.size();
	if(iCount>MAX_STOCK_IN_BLOCK)
		iCount = MAX_STOCK_IN_BLOCK;
	for(int i=0;i<iCount;++i)
	{
		CStockInfoItem* pStock = stocksInBlock[i];
		float _new = pStock->getNewPrice();
		float _last = mapLast5Price.value(pStock,0.0);
		if(_new>0.1)
		{
			float fLTAG = pStock->getBaseInfo()->fLtAg;		//流通股
			dNew += _new*fLTAG;
			dLow += pStock->getLowPrice()*fLTAG;
			dHigh += pStock->getHighPrice()*fLTAG;
			fVolume += pStock->getTotalVolume();
			fAmount += pStock->getTotalAmount();
			if(_last>0.1)
			{
				int _index = float((_new-_last)/_last)*1000.0;
				if(_index>9)
				{
					++fInc[0];
				}
				else if(_index>0)
				{
					++fInc[10-_index];
				}
				else if(_index<0&&_index>-10)
				{
					++fInc[9-_index];
				}
				else
				{
					++fInc[19];
				}
			}

			mapLast5Price[pStock] = _new;
		}
	}

	fNewPrice = dNew/fLTG;
	fLowPrice = dLow/fLTG;
	fHighPrice = dHigh/fLTG;
	fLTSZ = dNew;

	//涨幅
	if(fNewPrice>0.0 && fLastClose>0.0)
		fIncrease = (fNewPrice-fLastClose)*100.0/fLastClose;

	{
		//将新数据追加到分笔数据中
		qRcvFenBiData* pFenbi = new qRcvFenBiData();
		pFenbi->fAmount = fAmount;
		pFenbi->fVolume = fVolume;
		pFenbi->fPrice = fNewPrice;
		pFenbi->tmTime = QDateTime::currentDateTime().toTime_t();
		memcpy(&pFenbi->fBuyPrice[0],&fInc,sizeof(float)*20);

		m_pCurFenBi = pFenbi;
		appendFenBis(QList<qRcvFenBiData*>()<<pFenbi);
	}

	pCurrentReport->tmTime = QDateTime::currentDateTime().toTime_t();
	pCurrentReport->fOpen = fOpenPrice;
	pCurrentReport->fNewPrice = fNewPrice;
	pCurrentReport->fLow = fLowPrice;
	pCurrentReport->fHigh = fHighPrice;
	pCurrentReport->fAmount = fAmount;
	pCurrentReport->fVolume = fVolume;

	emit stockItemReportChanged(qsOnly);
	emit stockItemFenBiChanged(qsOnly);
}

void CBlockInfoItem::addStock( CStockInfoItem* _p )
{
	if(!_p)
		return;

	if(!stocksInBlock.contains(_p))
	{
		stocksInBlock.push_back(_p);
		connect(_p,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(stockHistoryChanged(const QString&)));
		connect(_p,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(stockFenbiChanged(const QString&)));
	}
}

void CBlockInfoItem::removeStock( CStockInfoItem* _p )
{
	if(!_p)
		return;
	stocksInBlock.removeOne(_p);
	disconnect(_p,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(stockHistoryChanged(const QString&)));
	disconnect(_p,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(stockFenbiChanged(const QString&)));
}

void CBlockInfoItem::clearTmpData()
{
	foreach(CStockInfoItem* _p,stocksInBlock)
	{
		disconnect(_p,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(stockHistoryChanged(const QString&)));
		disconnect(_p,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(stockFenbiChanged(const QString&)));
	}
	stocksInBlock.clear();

	foreach(CBlockInfoItem* _p,blocksInBlock)
	{
		delete _p;
	}
	blocksInBlock.clear();
}


QString CBlockInfoItem::getName() const
{
	return blockName;
}

float CBlockInfoItem::getIncrease() const
{
	//增长
	return fIncrease;
}

float CBlockInfoItem::getVolumeRatio() const
{
	//
	return 0.0;
}

float CBlockInfoItem::getTurnRatio() const
{
	return 0.0;
}

float CBlockInfoItem::getLastClose() const
{
	return fLastClose;
}

float CBlockInfoItem::getOpenPrice() const
{
	return fOpenPrice;
}

float CBlockInfoItem::getHighPrice() const
{

	return fHighPrice;
}

float CBlockInfoItem::getLowPrice() const
{
	return fLowPrice;
}

float CBlockInfoItem::getNewPrice() const
{
	return fNewPrice;
}

float CBlockInfoItem::getTotalVolume() const
{

	return 0.0;
}

float CBlockInfoItem::getTotalAmount() const
{

	return 0.0;
}

float CBlockInfoItem::getNowVolume() const
{

	return 0.0;
}

float CBlockInfoItem::getIncSpeed() const
{

	return 0.0;
}

float CBlockInfoItem::getPriceFluctuate() const
{

	return 0.0;
}

float CBlockInfoItem::getAmplitude() const
{

	return 0.0;
}

float CBlockInfoItem::getAvePrice() const
{

	return 0.0;
}

float CBlockInfoItem::getPERatio() const
{

	return 0.0;
}

float CBlockInfoItem::getLTSZ() const
{

	return 0.0;
}

float CBlockInfoItem::getZGB() const
{

	return 0.0;
}

float CBlockInfoItem::getSellVOL() const
{

	return 0.0;
}

float CBlockInfoItem::getBuyVOL() const
{

	return 0.0;
}

float CBlockInfoItem::getBIDVOL() const
{

	return 0.0;
}

float CBlockInfoItem::getASKVOL() const
{

	return 0.0;
}

float CBlockInfoItem::getCommRatio() const
{

	return 0.0;
}

float CBlockInfoItem::getCommSent() const
{

	return 0.0;
}

float CBlockInfoItem::getLast5Volume()
{

	return 0.0;
}

float CBlockInfoItem::getMgjzc()
{

	return 0.0;
}

float CBlockInfoItem::getMgsy()
{

	return 0.0;
}

bool CBlockInfoItem::isMatch( const QString& _key )
{
	//判断代码是否匹配
	if(qsCode.indexOf(_key)>-1)
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
