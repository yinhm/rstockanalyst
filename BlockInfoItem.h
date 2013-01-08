/************************************************************************/
/* 文件名称：BlockInfoItem.h
/* 创建时间：2013-01-08 09:28
/*
/* 描    述：用于管理板块数据
/************************************************************************/
#ifndef	BLOCK_INFO_ITEM_H
#define BLOCK_INFO_ITEM_H
#include <QtCore>
#include "StockInfoItem.h"

class CBlockInfoItem : public QObject
{
	Q_OBJECT
public:
	CBlockInfoItem(const QString& _name);						//构造函数。只传递名称，然后通过名称去获取文件中的列表
	CBlockInfoItem(const QString& _name,const QRegExp& _exp);	//构造函数。通用的列表，通过正则表达式去匹配股票代码
	~CBlockInfoItem(void);

private:
	QString blockName;
	QList<CStockInfoItem*> stocksInBlock;
};

#endif	//BLOCK_INFO_ITEM_H