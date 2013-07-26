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

class RSDSHARED_EXPORT CBlockInfoItem : public CAbstractStockItem
{
	Q_OBJECT
public:
	CBlockInfoItem(const QString& _file,CBlockInfoItem* parent=0);						//构造函数。只传递名称，然后通过名称去获取文件中的列表
	~CBlockInfoItem(void);

protected:
	//初始化子节点
	virtual void initStockItem();

public:
	//补充5分钟数据
	virtual QList<tagRStockData*> get5MinList();	//追加最后不够5分钟的数据
	//重新计算当日5Min数据
	virtual void recalc5MinData();

public:
	QString getBlockName() const{ return blockName; }			//获取板块名称
	QString getAbsPath();										//获取板块的绝对路径
	QList<CAbstractStockItem*> getAbsStockList();				//获取全部列表
	QList<CStockInfoItem*> getStockList();						//获取股票列表
	QList<CBlockInfoItem*> getBlockList();						//获取板块列表

	int getStockCount() const;									//获取股票的数量
	int getBlockCount() const;									//获取板块的数量

	bool hasBlocks();											//返回该板块是否含有子板块
	bool hasBlock(const QString& _code);						//是否包含此block

	bool appendStocks(QList<CStockInfoItem*> list);		//向板块中添加股票
	bool removeStocks(QList<CStockInfoItem*> list);		//从板块中删除股票

	CBlockInfoItem* querySubBlock(const QStringList& _parent);	//查找子板块
	bool isChildOf(CBlockInfoItem* parent);

	CBlockInfoItem* parentBlock(){ return m_pParent;}

	void addStockInfo(CStockInfoItem* _p);
	void removeStockInfo(CStockInfoItem* _p);

	void addBlockInfo(CBlockInfoItem* _p);
	void removeBlockInfo(CBlockInfoItem* _p);

	QString getFilePath();					//获取此板块的文件路径

public:
	/*属性类字段，只读*/
	QString getName() const;		//股票名称
	float getIncrease() const;		//涨幅
	float getVolumeRatio() const;	//量比
	float getTurnRatio() const;		//换手率
	float getLastClose() const;		//上一天收盘价
	float getOpenPrice() const;		//今日开盘价
	float getHighPrice() const;		//今日最高价
	float getLowPrice() const;		//今日最低价
	float getNewPrice() const;		//最新价
	float getTotalVolume() const;	//获取总手
	float getTotalAmount() const;	//获取总量
	float getNowVolume() const;		//获取现手
	float getIncSpeed() const;		//涨速
	float getPriceFluctuate() const;//涨跌，价格波动
	float getAmplitude() const;		//振幅
	float getAvePrice() const;		//均价
	float getPERatio() const;		//市盈率
	float getLTSZ() const;			//流通市值
	float getZGB() const;			//总股本
	float getSellVOL() const;		//外盘量
	float getBuyVOL() const;		//内盘量
	float getBIDVOL() const;		//委买量
	float getASKVOL() const;		//委卖量
	float getCommRatio() const;		//委比
	float getCommSent() const;		//委差

	float getLast5Volume();			//获取过去5日的成交量（用于计算量比）

	float getMgjzc();				//每股净资产
	float getMgsy();				//获取每股收益


	/*判断关键词_key是否匹配*/
	bool isMatch(const QString& _key);

protected:
	virtual void updateItemInfo(){}

private:
	bool appendStocks(QList<QString> list);				//向板块中添加股票
	bool removeStocks(QList<QString> list);				//从板块中删除股票
	bool appendBlock(CBlockInfoItem* pBlock);			//添加板块
	bool removeBlock(CBlockInfoItem* pBlock);			//移除板块


protected slots:
	void stockFenbiChanged(const QString& _code);	//某只股票的分笔数据发生改变
	void stockHistoryChanged(const QString& _code);	//某只股票的历史数据发生改变
	void updateData();								//更新数据

private:
	void addStock(CStockInfoItem* p);				//添加股票
	void removeStock(CStockInfoItem* p);			//删除股票
	void saveBlockFile();							//保存到板块文件
	void clearTmpData();							//清理临时内存。

private:
	QString blockName;							//板块名称
	QString blockFilePath;						//板块数据的存储路径
	QList<CStockInfoItem*> stocksInBlock;		//该板块下的股票信息
	QMap<QString,CBlockInfoItem*> blocksInBlock;//该板块下的子板块

	QTimer timerUpdate;


	CBlockInfoItem* m_pParent;

	RBlockData* pCurrent5Min;		//当前的分笔数据

	QMap<CStockInfoItem*,float> mapLastPrice;	//最后进行比较的数据
	QMap<CStockInfoItem*,float> mapLast5Price;	//最新的5分钟价格
	float fLast5MinVolume;						//最后5分钟的成交量
	float fLast5MinAmount;						//最后5分钟的成交额

	QString qsRegExp;							//匹配的正则表达式
private:
	float fLastClose;				//昨日收盘价
	float fOpenPrice;				//今日开盘价
	float fNewPrice;				//最新价
	float fLowPrice;				//最低价
	float fHighPrice;				//最高价
	float fVolume;					//成交量
	float fAmount;					//成交额

	float fIncrease;				//涨幅

	float fLTSZ;					//流通市值
	float fLTG;						//总流通股

	QList<QList<QChar>> shortName;	//简拼表
};

#endif	//BLOCK_INFO_ITEM_H