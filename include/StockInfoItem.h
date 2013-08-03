/************************************************************************/
/* 文件名称：StockInfoItem.h
/* 创建时间：2012-11-08 09:28
/*
/* 描    述：用于存储单只股票数据
/************************************************************************/
#ifndef STOCK_INFO_ITEM_H
#define STOCK_INFO_ITEM_H
#include <QtCore>
#include "rsd_global.h"
#include "RStockFunc.h"

class RSDSHARED_EXPORT CStockInfoItem : public CAbstractStockItem
{
	Q_OBJECT
public:
	CStockInfoItem(const QString& code, WORD market);
	CStockInfoItem(const qRcvBaseInfoData& info);
	~CStockInfoItem(void);

public:
	//补充Report数据
	void setReport(qRcvReportData* p);
	void setReport(RCV_REPORT_STRUCTExV3* p);

	//补充除权数据
	QList<qRcvPowerData*> getPowerList();
	void appendPowers(const QList<qRcvPowerData*>& list);

	//补充分笔数据
	virtual void appendFenBis(const QList<qRcvFenBiData*>& list);

	//补充竞价数据
	virtual void appendJingJias(qRcvFenBiData* pJingJia);

	//补充5分钟数据
	virtual QList<tagRStockData*> get5MinList();	//追加最后不够5分钟的数据
	//获取某时间的5分钟数据
	RStockData* get5MinData(const time_t& tmTime);
	//重新计算当日5Min数据
	virtual void recalc5MinData();

	//设置F10数据
	void setBaseInfo(const qRcvBaseInfoData& info);
	qRcvBaseInfoData* getBaseInfo(){ return &baseInfo; }

	//设置最后5日的总成交量
	void setLast5Volume(float fValue){ fLast5Volume = fValue; }

public:
	/*属性类字段，只读*/
	QString getName() const;		//股票名称
	float getIncrease() const;	//涨幅
	float getVolumeRatio() const;	//量比
	float getTurnRatio() const;	//换手率
	float getLastClose() const;	//上一天收盘价
	float getOpenPrice() const;	//今日开盘价
	float getHighPrice() const;	//今日最高价
	float getLowPrice() const;	//今日最低价
	float getNewPrice() const;	//最新价
	float getTotalVolume() const;	//获取总手
	float getTotalAmount() const;	//获取总量
	float getNowVolume() const;	//获取现手
	float getIncSpeed() const;	//涨速
	float getPriceFluctuate() const;//涨跌，价格波动
	float getAmplitude() const;	//振幅
	float getAvePrice() const;	//均价
	float getPERatio() const;		//市盈率
	float getLTSZ() const;			//流通市值
	float getZGB() const;			//总股本
	float getSellVOL() const;		//外盘量
	float getBuyVOL() const;		//内盘量
	float getBIDVOL() const;		//委买量
	float getASKVOL() const;		//委卖量
	float getBIDVOL3() const;		//委买量
	float getASKVOL3() const;		//委卖量
	float getCommRatio() const;	//委比
	float getCommSent() const;	//委差

	float getLast5Volume(); //获取过去5日的成交量（用于计算量比）

	float getMgjzc();		//每股净资产
	float getMgsy();		//获取每股收益
	float getLtag();		//流通A股

	QVector<int> getLast5CmpPrices();		//获取最近5分钟的价格变化
	/*判断关键词_key是否匹配*/
	bool isMatch(const QString& _key);

signals:
	void stockItemReportComing(CStockInfoItem*);//行情数据更新

protected:
	virtual void updateItemInfo();
	void resetBuySellVOL();		//重新计算内外盘数据

private:
	QString qsName;					//股票名称
	float fIncrease;				//涨幅
	float fVolumeRatio;				//量比
	float fTurnRatio;				//换手率

	qRcvReportData* pLastReport;	//最近的Report
	RStockData* pCurrent5Min;		//当前的5分钟数据
	float fLast5MinVolume;			//最后5分钟数据的成交量
	float fLast5MinAmount;			//最后5分钟数据的成交额


	float fNowVolume;				//现手
	float fIncreaseSpeed;			//增长速度  (NewPrice-OldPrice)/OldPrice
	float fPriceFluctuate;			//涨跌，价格波动
	float fAmplitude;				//振幅
	float fAvePrice;				//均价

	float fPERatio;					//市盈率
	float fLTSZ;					//流通市值
	float fZGB;						//总股本
	float fSellVOL;					//外盘量
	float fBuyVOL;					//内盘量

	float fBuyVolume;				//委买量
	float fSellVolume;				//委卖量
	float fBuyVolume3;				//3个委买量
	float fSellVolume3;				//3个委卖量
	float fCommRatio;				//委比
	float fCommSent;				//委差

	QList<QList<QChar>> shortName;	//简拼表

	float fLastCmpPrice;			//上一个用来比较的价格

private:
	QMap<time_t,qRcvPowerData*> mapPowers;			//除权数据
	QMultiMap<time_t,qRcvFenBiData*> mapJingJias;	//9:25前的竞价数据
	qRcvBaseInfoData baseInfo;
	QVector<int> vCmpPrices;						//经过比较后的涨跌表
};

#endif	//STOCK_INFO_ITEM_H