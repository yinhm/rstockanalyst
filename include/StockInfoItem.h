/************************************************************************/
/* 文件名称：StockInfoItem.h
/* 创建时间：2013-11-08 09:28
/*
/* 描    述：用于存储单只股票数据
/************************************************************************/
#ifndef STOCK_INFO_ITEM_H
#define STOCK_INFO_ITEM_H
#include <QtCore>
#include "rsd_global.h"

#pragma   pack(push,1)					//设置内存对齐方式为 1字节


struct qRcvReportData
{
	time_t	tmTime;			//成交时间
	WORD		wMarket;		//股票市场类型
	QString		qsCode;			//股票代码
	QString		qsName;			//股票名称

	float	fLastClose;			//昨收
	float	fOpen;				//今开
	float	fHigh;				//最高
	float	fLow;				//最低
	float	fNewPrice;			//最新
	float	fVolume;			//成交量
	float	fAmount;			//成交额
	float	fBuyPrice[3];		//申买价1,2,3
	float	fBuyVolume[3];		//申买量1,2,3
	float	fSellPrice[3];		//申卖价1,2,3
	float	fSellVolume[3];		//申卖量1,2,3

	float	fBuyPrice4;			//申买价4
	float	fBuyVolume4;		//申买量4
	float	fSellPrice4;		//申卖价4
	float	fSellVolume4;		//申卖量4

	float	fBuyPrice5;			//申买价5
	float	fBuyVolume5;		//申买量5
	float	fSellPrice5;		//申卖价5
	float	fSellVolume5;		//申卖量5

	qRcvReportData()
	{
		tmTime = NULL;
		wMarket = NULL;
		memset(&fLastClose,0,sizeof(float)*27);
	}
	qRcvReportData(RCV_REPORT_STRUCTExV3* p)
	{
		tmTime = p->m_time;
		wMarket = p->m_wMarket;
		qsCode = QString::fromLocal8Bit(p->m_szLabel);
		qsName = QString::fromLocal8Bit(p->m_szName);

		//直接拷贝剩余的所有float数据
		memcpy(&fLastClose,&p->m_fLastClose,sizeof(float)*27);
	}

	void resetItem(RCV_REPORT_STRUCTExV3* p)
	{
		tmTime = p->m_time;
		wMarket = p->m_wMarket;
		qsCode = QString::fromLocal8Bit(p->m_szLabel);
		qsName = QString::fromLocal8Bit(p->m_szName);

		//直接拷贝剩余的所有float数据
		memcpy(&fLastClose,&p->m_fLastClose,sizeof(float)*27);
	}
	void resetItem(qRcvReportData* p)
	{
		tmTime = p->tmTime;
		wMarket = p->wMarket;
		qsCode = p->qsCode;
		qsName = p->qsName;

		//直接拷贝剩余的所有float数据
		memcpy(&fLastClose,&p->fLastClose,sizeof(float)*27);
	}
};

struct qRcvHistoryData
{
	time_t	time;			//UCT
	float	fOpen;			//开盘
	float	fHigh;			//最高
	float	fLow;				//最低
	float	fClose;			//收盘
	float	fVolume;			//量
	float	fAmount;			//额
	WORD	wAdvance;			//涨数,仅大盘有效
	WORD	wDecline;			//跌数,仅大盘有效

	qRcvHistoryData()
	{

	}
	qRcvHistoryData(RCV_HISTORY_STRUCTEx* p)
	{
		memcpy(&time,&p->m_time,sizeof(qRcvHistoryData));
	}
	qRcvHistoryData(qRcvReportData* p)
	{
		time = QDateTime(QDateTime::fromTime_t(p->tmTime).date()).toTime_t();
		fAmount = p->fAmount;
		fClose = p->fNewPrice;
		fHigh = p->fHigh;
		fLow = p->fLow;
		fOpen = p->fOpen;
		fVolume = p->fVolume;
	}
};

struct qRcvBaseInfoData
{
	char code[STKLABEL_LEN];
	WORD wMarket;

	float fZgb;                    //总股本(万股)			0
	float fGjg;                    //国家股(万股)
	float fFqrfrg;                  //发起人法人股(万股)
	float fFrg;                    //法人股(万股)
	float fZgg;                   //职工股(万股)
	float fGzAg;                  //公众A股(万股)			5
	float fLtAg;                  //流通A股(万股)
	float fHg;                    //Ｈ股(万股)
	float fBg;                    //B股(万股)
	float fZpg;                   //转配股(万股)
	float fZzc;                   //总资产(万元)			10
	float fLdzc;                  //流动资产(万元)
	float fCqtz;                  //长期投资(万元)
	float fGdzc;                  //固定资产(万元)
	float fWxzc;                 //无形资产(万元)
	float fLdfz;                  //流动负债(万元)		15
	float fCqfz;                 //长期负债(万元)
	float fGgqy;                 //股东权益(万元)
	float fZbgjj;                 //资本公积金(万元)
	float fYygjj;                 //盈余公积金(万元)
	float fMgjz;                 //每股净值(元)			20
	float fGdqybl;               //股东权益比率(%)
	float fMggjj;                //每股公积金(元)
	float fZyywsr;               //主营业务收入(万元)
	float fZyywlr;               //主营业务利润(万元)
	float fQtywlr;               //其它业务利润(万元)		25
	float fLrze;                 //利润总额(万元)
	float fJlr;                  //净利润(万元)
	float fWfplr;               //未分配利润(万元)
	float fMgsy;                //每股收益(元)
	float fJzcsyl;                //净资产收益率(%)		30
	float fMgwfplr;              //每股未分配利润(元)
	float fMgjzc;                //每股净资产(元)			32

	qRcvBaseInfoData()
	{
		memset(code,0,sizeof(qRcvBaseInfoData));
	}

	qRcvBaseInfoData( float* fVal )
	{
		fVal = fVal+1;
		fZgb = fVal[0];
		fGjg = fVal[1];
		fFqrfrg = fVal[2];
		fFrg = fVal[3];
		fBg = fVal[4];

		fHg = fVal[5];
		fLtAg = fVal[6];
		fZgg = fVal[7];

		fZzc = fVal[9]/10;

		fLdzc = fVal[10]/10;
		fGdzc = fVal[11]/10;
		fWxzc = fVal[12]/10;
		fCqtz = fVal[13]/10;
		fLdfz = fVal[14]/10;

		fCqfz = fVal[15]/10;
		fZbgjj = fVal[16]/10;
		fMggjj = fVal[17];
		fGgqy = fVal[18]/10;
		fZyywsr = fVal[19]/10;

		fZyywlr = fVal[20]/10;		//?
		fQtywlr = fVal[21]/10;
		fZyywlr = fVal[22]/10;		//?

		fLrze = fVal[27]/10;

		fJlr = fVal[29]/10;

		fWfplr = fVal[30]/10;
		fMgwfplr  = fVal[31];
		fMgsy = fVal[32];
		fMgjzc = fVal[33];

		fGdqybl  = fVal[35];
		fJzcsyl = fVal[36];
	}
};

struct qRcvPowerData
{
	time_t	tmTime;				// UCT
	float	fGive;			// 每股送
	float	fPei;				// 每股配
	float	fPeiPrice;		// 配股价,仅当 m_fPei!=0.0f 时有效
	float	fProfit;			// 每股红利

	qRcvPowerData(RCV_POWER_STRUCTEx* p)
	{
		memcpy(&tmTime,&p->m_time,sizeof(qRcvPowerData));
	}
};

//RCV_FENBI_STRUCTEx
struct qRcvFenBiData
{
	time_t	tmTime;				// hhmmss 例：93056 表明9:30:56的盘口数据
	float	fPrice;			// 最新
	float	fVolume;			// 成交量
	float	fAmount;			// 成交额

	float	fBuyPrice[5];		// 申买价1,2,3,4,5
	float	fBuyVolume[5];		// 申买量1,2,3,4,5
	float	fSellPrice[5];		// 申卖价1,2,3,4,5
	float	fSellVolume[5];		// 申卖量1,2,3,4,5
	qRcvFenBiData()
	{
		memset(&tmTime,0,sizeof(qRcvFenBiData));
	}
	qRcvFenBiData(RCV_FENBI_STRUCTEx* p, const long& lDate)
	{
		//将原来的时间转换为time_t
		long l = lDate;
		int iD = l%100;
		l = l/100;
		int iM = l%100;
		l = l/100;
		int iY = l;

		l = p->m_lTime;
		int iS = l%100;
		l = l/100;
		int iMin = l%100;
		l = l/100;
		int iH = l;

		QDateTime tmT(QDate(iY,iM,iD),QTime(iH,iMin,iS));
		tmTime = tmT.toTime_t();
		fPrice = p->m_fNewPrice;
		fVolume = p->m_fVolume;
		fAmount = p->m_fAmount;

		memcpy(&fBuyPrice[0],&p->m_fBuyPrice[0],sizeof(float)*20);
	}
	qRcvFenBiData(RCV_MINUTE_STRUCTEx* p)
	{
		memset(&tmTime,0,sizeof(qRcvFenBiData));
		//通过分钟数据构建 分笔数据
		tmTime = p->m_time;
		fPrice = p->m_fPrice;
		fVolume = p->m_fVolume;
		fAmount = p->m_fAmount;
	}
	qRcvFenBiData(qRcvReportData* p)
	{
		tmTime = p->tmTime;
		fPrice = p->fNewPrice;
		fVolume = p->fVolume;
		fAmount = p->fAmount;

		memcpy(&fBuyPrice[0],&p->fBuyPrice[0],sizeof(float)*3);
		memcpy(&fBuyVolume[0],&p->fBuyVolume[0],sizeof(float)*3);
		memcpy(&fSellPrice[0],&p->fSellPrice[0],sizeof(float)*3);
		memcpy(&fSellVolume[0],&p->fSellVolume[0],sizeof(float)*3);
		fBuyPrice[3] = p->fBuyPrice4; fBuyPrice[4] = p->fBuyPrice5;
		fBuyVolume[3] = p->fBuyVolume4; fBuyVolume[4] = p->fBuyVolume5;
		fSellPrice[3] = p->fSellPrice4; fSellPrice[4] = p->fSellPrice5;
		fSellVolume[3] = p->fSellVolume4; fSellVolume[4] = p->fSellVolume5;
	}
};

#pragma   pack(pop)					//去除内存对齐方式设置

class RSDSHARED_EXPORT CStockInfoItem : public QObject
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
	qRcvReportData* getCurrentReport() const;

	//补充日线数据
	QList<qRcvHistoryData*> getHistoryList();
	QList<qRcvHistoryData*> getLastHistory(int count);
	void appendHistorys(const QList<qRcvHistoryData*>& list);

	//补充除权数据
	QList<qRcvPowerData*> getPowerList();
	void appendPowers(const QList<qRcvPowerData*>& list);

	//补充分笔数据
	QList<qRcvFenBiData*> getFenBiList();
	void appendFenBis(const QList<qRcvFenBiData*>& list);

	//设置F10数据
	void setBaseInfo(const qRcvBaseInfoData& info);
	qRcvBaseInfoData* getBaseInfo(){ return &baseInfo; }

	//设置最后5日的总成交量
	void setLast5Volume(float fValue){ fLast5Volume = fValue; }

public:
	/*属性类字段，只读*/
	QString getCode() const;		//股票代码
	WORD getMarket() const;			//股票市场
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
	float getCommRatio() const;	//委比
	float getCommSent() const;	//委差

	float getLast5Volume(); //获取过去5日的成交量（用于计算量比）

	float getMgjzc();		//每股净资产
	float getMgsy();		//获取每股收益


protected:
	void updateItemInfo();
	void resetBuySellVOL();		//重新计算内外盘数据

signals:
	void stockItemReportChanged(const QString&);	//行情数据更新
	void stockItemHistoryChanged(const QString&);	//历史数据更新
	void stockItemFenBiChanged(const QString&);		//分笔数据更新

private:
	QString qsCode;					//代码
	WORD wMarket;					//市场类型
	QString qsName;					//股票名称
	float fIncrease;				//涨幅
	float fVolumeRatio;				//量比
	float fTurnRatio;				//换手率


	qRcvReportData* pLastReport;	//最近的Report
	qRcvReportData* pCurrentReport;	//当前的Report

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
	float fCommRatio;				//委比
	float fCommSent;				//委差

	float fLast5Volume;				//过去5日的成交总量（用于计算量比）

private:
	QMap<time_t,qRcvPowerData*> mapPowers;			//除权数据
	QMultiMap<long,qRcvFenBiData*> mapFenBis;		//分笔数据
	qRcvBaseInfoData baseInfo;
};

#endif	//STOCK_INFO_ITEM_H