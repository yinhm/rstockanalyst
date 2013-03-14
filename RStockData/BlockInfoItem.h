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

class Q_DECL_EXPORT CBlockInfoItem : public QObject
{
	Q_OBJECT
public:
	CBlockInfoItem(const QString& _name);						//构造函数。只传递名称，然后通过名称去获取文件中的列表
	CBlockInfoItem(const QString& _name,const QRegExp& _exp);	//构造函数。通用的列表，通过正则表达式去匹配股票代码
	~CBlockInfoItem(void);

public:
	QString getBlockName() const{ return blockName; }			//获取板块名称
	QList<CStockInfoItem*> getStockList();						//获取板块列表

	bool appendStocks(QList<CStockInfoItem*> list);		//向板块中添加股票
	bool appendStocks(QList<QString> list);				//向板块中添加股票
	bool removeStocks(QList<CStockInfoItem*> list);		//从板块中删除股票
	bool removeStocks(QList<QString> list);				//从板块中删除股票

protected slots:
	void stockFenbiChanged(const QString& _code);	//某只股票的分笔数据发生改变
	void stockHistoryChanged(const QString& _code);	//某只股票的历史数据发生改变

private:
	void addStock(CStockInfoItem* p);				//添加股票
	void removeStock(CStockInfoItem* p);			//删除股票
	void clearTmpData();							//清理临时内存。
private:
	QString blockName;
	QString blockFilePath;						//板块数据的存储路径
	QList<CStockInfoItem*> stocksInBlock;
};

#endif	//BLOCK_INFO_ITEM_H