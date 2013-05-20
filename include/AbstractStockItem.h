/************************************************************************/
/* 文件名称：AbstractStockItem.h
/* 创建时间：2013-05-20 14:14
/*
/* 描    述：股票数据的纯虚基类
/************************************************************************/
#ifndef ABSTRACT_STOCK_ITEM_H
#define ABSTRACT_STOCK_ITEM_H
#include <QtCore>
#include "rsd_global.h"

class RSDSHARED_EXPORT CAbstractStockItem : public QObject
{
	Q_OBJECT
public:
	CAbstractStockItem(void);
	~CAbstractStockItem(void);

public:
	/*属性类字段，只读*/
	virtual QString getCode() const = 0;		//股票代码
	virtual WORD getMarket() const = 0;			//股票市场
	virtual QString getName() const = 0;		//股票名称
	virtual float getIncrease() const = 0;		//涨幅
	virtual float getVolumeRatio() const = 0;	//量比
	virtual float getTurnRatio() const = 0;		//换手率
	virtual float getLastClose() const = 0;		//上一天收盘价
	virtual float getOpenPrice() const = 0;		//今日开盘价
	virtual float getHighPrice() const = 0;		//今日最高价
	virtual float getLowPrice() const = 0;		//今日最低价
	virtual float getNewPrice() const = 0;		//最新价
	virtual float getTotalVolume() const = 0;	//获取总手
	virtual float getTotalAmount() const = 0;	//获取总量
	virtual float getNowVolume() const = 0;		//获取现手
	virtual float getIncSpeed() const = 0;		//涨速
	virtual float getPriceFluctuate() const = 0;//涨跌，价格波动
	virtual float getAmplitude() const = 0;		//振幅
	virtual float getAvePrice() const = 0;		//均价
	virtual float getPERatio() const = 0;		//市盈率
	virtual float getLTSZ() const = 0;			//流通市值
	virtual float getZGB() const = 0;			//总股本
	virtual float getSellVOL() const = 0;		//外盘量
	virtual float getBuyVOL() const = 0;		//内盘量
	virtual float getBIDVOL() const = 0;		//委买量
	virtual float getASKVOL() const = 0;		//委卖量
	virtual float getCommRatio() const = 0;		//委比
	virtual float getCommSent() const = 0;		//委差

	virtual float getLast5Volume() = 0;			//获取过去5日的成交量（用于计算量比）

	virtual float getMgjzc() = 0;				//每股净资产
	virtual float getMgsy() = 0;				//获取每股收益


	/*判断关键词_key是否匹配*/
	virtual bool isMatch(const QString& _key) = 0;

	bool isInstanceOfStock();
	bool isInstanceOfBlock();
signals:
	void stockItemReportChanged(const QString&);	//行情数据更新
	void stockItemHistoryChanged(const QString&);	//历史数据更新
	void stockItemFenBiChanged(const QString&);		//分笔数据更新
};

#endif //ABSTRACT_STOCK_ITEM_H