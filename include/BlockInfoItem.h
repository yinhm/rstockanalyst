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
#include "rsd_global.h"

class RSDSHARED_EXPORT CBlockInfoItem : public CStockInfoItem
{
	Q_OBJECT
public:
	CBlockInfoItem(const QString& _file,const QString& _parent);						//构造函数。只传递名称，然后通过名称去获取文件中的列表
	~CBlockInfoItem(void);

public:
	QString getBlockName() const{ return blockName; }			//获取板块名称
	QString getAbsPath();										//获取板块的绝对路径
	QList<CStockInfoItem*> getStockList();						//获取股票列表
	QList<CBlockInfoItem*> getBlockList();						//获取板块列表

	bool hasBlocks();											//返回该板块是否含有子板块

	bool appendStocks(QList<CStockInfoItem*> list);		//向板块中添加股票
	bool removeStocks(QList<CStockInfoItem*> list);		//从板块中删除股票

	CBlockInfoItem* querySubBlock(const QStringList& _parent);	//查找子板块

private:
	bool appendStocks(QList<QString> list);				//向板块中添加股票
	bool removeStocks(QList<QString> list);				//从板块中删除股票
	bool appendBlock(CBlockInfoItem* pBlock);			//添加板块


protected slots:
	void stockFenbiChanged(const QString& _code);	//某只股票的分笔数据发生改变
	void stockHistoryChanged(const QString& _code);	//某只股票的历史数据发生改变

private:
	void addStock(CStockInfoItem* p);				//添加股票
	void removeStock(CStockInfoItem* p);			//删除股票
	void clearTmpData();							//清理临时内存。

private:
	QString parentName;							//板块的父板块路径
	QString blockName;							//板块名称
	QString blockFilePath;						//板块数据的存储路径
	QList<CStockInfoItem*> stocksInBlock;		//该板块下的股票信息
	QMap<QString,CBlockInfoItem*> blocksInBlock;		//该板块下的子板块
};

#endif	//BLOCK_INFO_ITEM_H