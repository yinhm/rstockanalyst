/************************************************************************/
/* 文件名称：BlockInfoItem.cpp
/* 创建时间：2013-01-08 09:28
/*
/* 描    述：用于管理板块数据
/************************************************************************/
#include "StdAfx.h"
#include "BlockInfoItem.h"
#include "DataEngine.h"


CBlockInfoItem::CBlockInfoItem( const QString& _name )
	: blockName(_name)
{
	QFile file(CDataEngine::getDataEngine()->getStockBlockDir()+_name);
	if(file.open(QFile::ReadOnly))
	{
		while(!file.atEnd())
		{
			QString code = file.readLine();
			code = code.trimmed();
			if(!code.isEmpty())
			{
				CStockInfoItem* p = CDataEngine::getDataEngine()->getStockInfoItem(code);
				if(p)
					stocksInBlock.push_back(p);
			}
		}
		file.close();
	}
}

CBlockInfoItem::CBlockInfoItem( const QString& _name,const QRegExp& _exp )
	: blockName(_name)
{
	QList<CStockInfoItem*> listStocks = CDataEngine::getDataEngine()->getStockInfoList();
	foreach(CStockInfoItem* p,listStocks)
	{
		if(_exp.exactMatch(p->getCode()))
		{
			stocksInBlock.push_back(p);
		}
	}
}


CBlockInfoItem::~CBlockInfoItem(void)
{
}
