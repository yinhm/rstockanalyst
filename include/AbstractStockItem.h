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

struct qRcvFenBiData;
struct qRcvReportData;
struct qRcvHistoryData;
struct tagRStockData;
struct tagRcvBlockData;

class RSDSHARED_EXPORT CAbstractStockItem : public QObject
{
	Q_OBJECT
public:
	CAbstractStockItem(void);
	~CAbstractStockItem(void);

public:
	virtual void initStockItem();							//初始化股票

	//补充Report数据
	virtual qRcvReportData* getCurrentReport() const;

	//补充分笔数据
	virtual QList<qRcvFenBiData*> getTodayFenBiList();
	virtual QList<qRcvFenBiData*> getFenBiList();
	virtual void appendFenBis(const QList<qRcvFenBiData*>& list);

	//补充日线数据
	virtual QList<qRcvHistoryData*> getHistoryList();
	virtual QList<qRcvHistoryData*> getLastHistory(int count);
	virtual void appendHistorys(const QList<qRcvHistoryData*>& list);

	//补充历史分钟数据
	virtual void appendMinData(tagRStockData* pData);			//补充分钟数据
	virtual void appendToday5MinData(tagRStockData* pData);		//补充当日5分钟数据
	virtual QList<tagRStockData*> getMinList();			//追加最后不够1分钟的数据
	virtual QList<tagRStockData*> getMinList(int _c);	//获取日线以下的历史数据
	virtual void recalcMinData() = 0;					//重新计算当日1Min数据

	//获取今日的5分钟数据
	virtual QList<tagRStockData*> getToday5MinList() = 0;
	virtual tagRStockData* getToday5MinData( const time_t& tmTime );//获取指定时间的数据
	virtual void recalc5MinData() = 0;				//重新计算当日5Min数据

public:
	/*属性类字段，只读*/
	virtual QString getOnly() const;		//唯一标识 000009SZ
	virtual QString getCode() const;		//股票代码
	virtual WORD getMarket() const;			//股票市场
	virtual QString getMarketName() const;	//股票市场名称

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
	virtual float getBIDVOL3() const = 0;		//委买量3
	virtual float getASKVOL3() const = 0;		//委卖量3
	virtual float getASKVOL() const = 0;		//委卖量
	virtual float getCommRatio() const = 0;		//委比
	virtual float getCommSent() const = 0;		//委差

	virtual float getLast5Volume() = 0;			//获取过去5日的成交量（用于计算量比）

	virtual float getMgjzc() = 0;				//每股净资产
	virtual float getMgsy() = 0;				//获取每股收益
	virtual float getLtag(){ return -1.0; }		//流通A股

	virtual int getNewHighPriceCount();			//获取价格创新高次数，以一分钟为界
	virtual int getNewHighVolumeCount();		//获取成交量创新高次数，以一分钟为界


	/*判断关键词_key是否匹配*/
	virtual bool isMatch(const QString& _key) = 0;

	bool isInstanceOfStock();
	bool isInstanceOfBlock();

	virtual bool isIndex() = 0;							//是否是指数

protected:
	virtual void updateItemInfo() = 0;
	void loadHisMinData();
signals:
	void stockItemReportChanged(const QString&);	//行情数据更新
	void stockItemHistoryChanged(const QString&);	//历史数据更新
	void stockItemFenBiChanged(const QString&);		//分笔数据更新


protected:
	QString qsOnly;									//唯一标识
	QString qsCode;									//代码
	WORD wMarket;									//市场类型
	QString qsMarket;								//市场类型名称
	QMap<time_t,qRcvFenBiData*> mapFenBis;			//分笔数据
	QMap<time_t,tagRStockData*> mapToday5MinDatas;	//当天5分钟历史数据

	QMap<time_t,tagRStockData*> mapMinDatas;		//1分钟历史数据，一般存储7天的
	QList<tagRStockData*> list5MinDatas;			//5分钟历史数据，一般存储7天的
	QList<tagRStockData*> list15MinDatas;			//15分钟历史数据，一般存储7天的
	QList<tagRStockData*> list30MinDatas;			//30分钟历史数据，一般存储7天的
	QList<tagRStockData*> list60MinDatas;			//60分钟历史数据，一般存储7天的
	QList<qRcvHistoryData*> listHistories;			//历史数据
	qRcvReportData* pCurrentReport;					//当前的Report


	tagRStockData* pCurrentMin;		//当前的1分钟数据
	tagRStockData* pCurrent5Min;	//当前的5分钟数据
	tagRStockData* pCurrent15Min;	//当前的15分钟数据
	tagRStockData* pCurrent30Min;	//当前的30分钟数据
	tagRStockData* pCurrent60Min;	//当前的60分钟数据

	float fLast5Volume;				//过去5日的成交总量（用于计算量比）
	bool m_bLoadHisMin;				//是否已经加载历史分钟数据
};

#endif //ABSTRACT_STOCK_ITEM_H