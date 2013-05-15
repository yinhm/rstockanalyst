/************************************************************************/
/* 文件名称：BlockInfoItem.cpp
/* 创建时间：2013-01-08 09:28
/*
/* 描    述：用于管理板块数据
/************************************************************************/
#include "StdAfx.h"
#include "BlockInfoItem.h"
#include "DataEngine.h"

CBlockInfoItem::CBlockInfoItem( const QString& _file,const QString& _parent )
	: CStockInfoItem("123",1)
{
	QFileInfo _info(_file);
	if(!_info.exists())
		return;

	parentName = _parent;
	blockFilePath = _file;
	blockName = _info.baseName();

	if(_info.isDir())
	{
		QDir dir(blockFilePath);
		QFileInfoList listEntity = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
		foreach(const QFileInfo& _f,listEntity)
		{
			appendBlock(new CBlockInfoItem(_f.absoluteFilePath(),parentName+"|"+blockName));
		}
	}
	else
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
						addStock(CDataEngine::getDataEngine()->getStockInfoItem(code));
					}
				}
			}
			file.close();
		}
	}
}

CBlockInfoItem::~CBlockInfoItem(void)
{
	clearTmpData();
}

QString CBlockInfoItem::getAbsPath()
{
	if(parentName.isEmpty())
		return blockName;
	return parentName+"|"+blockName;
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

QList<CStockInfoItem*> CBlockInfoItem::getStockList()
{
	return stocksInBlock;
}

QList<CBlockInfoItem*> CBlockInfoItem::getBlockList()
{
	return blocksInBlock.values();
}

bool CBlockInfoItem::hasBlocks()
{
	return blocksInBlock.size()>0 ? true : false;
}

bool CBlockInfoItem::appendStocks( QList<CStockInfoItem*> list )
{
	QStringList listCodes;
	foreach(CStockInfoItem* pItem,list)
		listCodes.push_back(pItem->getCode());

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
		listCodes.push_back(pItem->getCode());

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

void CBlockInfoItem::stockFenbiChanged( const QString& _code )
{

}

void CBlockInfoItem::stockHistoryChanged( const QString& _code )
{

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
}
