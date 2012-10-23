#ifndef STOCK_INFO_ITEM_H
#define STOCK_INFO_ITEM_H
#include <QtCore>


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
};

struct qRcvReportData
{
	time_t	tmTime;			//成交时间
	DWORD		wMarket;		//股票市场类型
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

	qRcvReportData(RCV_REPORT_STRUCTExV3* p)
	{
		tmTime = p->m_time;
		wMarket = p->m_wMarket;
		qsCode = QString::fromLocal8Bit(p->m_szLabel);
		qsName = QString::fromLocal8Bit(p->m_szName);

		//直接拷贝剩余的所有float数据
		memcpy(&fLastClose,&p->m_fLastClose,sizeof(float)*27);
/*		fLastClose = p->m_fLastClose;
		fOpen = p->m_fOpen;				//今开
		fHigh = p->m_fHigh;				//最高
		fLow = p->m_fLow;				//最低
		fNewPrice = p->m_fNewPrice;		//最新
		fVolume = p->m_fVolume;			//成交量
		fAmount = p->m_fAmount;			//成交额

		memcpy(fBuyPrice,p->m_fBuyPrice,sizeof(float)*3);	//申买价1,2,3
		memcpy(fBuyVolume,p->m_fBuyVolume,sizeof(float)*3);	//申买量1,2,3
		memcpy(fSellPrice,p->m_fSellPrice,sizeof(float)*3);	//申卖价1,2,3
		memcpy(fSellVolume,p->m_fSellVolume,sizeof(float)*3);	//申卖量1,2,3

		fBuyPrice4;			//申买价4
		fBuyVolume4;		//申买量4
		fSellPrice4;		//申卖价4
		fSellVolume4;		//申卖量4

		fBuyPrice5;			//申买价5
		fBuyVolume5;		//申买量5
		fSellPrice5;		//申卖价5
		fSellVolume5;		//申卖量5
		*/
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


class CStockInfoItem : public QObject
{
	Q_OBJECT
public:
	CStockInfoItem(const QString& code, WORD market);
	CStockInfoItem(const qRcvBaseInfoData& info);
	~CStockInfoItem(void);

public:
	//补充Report数据
	void appendReport(qRcvReportData* p);
	qRcvReportData* getLastReport() const;

	//补充日线数据
	QList<qRcvHistoryData*> getHistoryList();
	void appendHistorys(const QList<qRcvHistoryData*>& list);

	//设置F10数据
	void setBaseInfo(const qRcvBaseInfoData& info);

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
	float getSellVOL() const;		//外盘量
	float getBuyVOL() const;		//内盘量
	float getBIDVOL() const;		//委买量
	float getASKVOL() const;		//委卖量
	float getCommRatio() const;	//委比
	float getCommSent() const;	//委差



signals:
	void stockInfoItemChanged(const QString&);

private:
	QString qsCode;					//代码
	WORD wMarket;					//市场类型
	QString qsName;					//股票名称
	float fIncrease;				//涨幅
	float fVolumeRatio;				//量比
	float fTurnRatio;				//换手率


	qRcvReportData* pLastReport;	//最近的Report
	float fNowVolume;				//现手
	float fIncreaseSpeed;			//增长速度  (NewPrice-OldPrice)/OldPrice
	float fPriceFluctuate;			//涨跌，价格波动
	float fAmplitude;				//振幅
	float fAvePrice;				//均价

	float fPERatio;					//市盈率
	float fLTSZ;					//流通市值
	float fSellVOL;					//外盘量
	float fBuyVOL;					//内盘量

	float fBuyVolume;				//委买量
	float fSellVolume;				//委卖量
	float fCommRatio;				//委比
	float fCommSent;				//委差

private:
	QMap<time_t,qRcvReportData*> mapReports;
	QMap<time_t,qRcvHistoryData*> mapHistorys;
	qRcvBaseInfoData baseInfo;
};

#endif	//STOCK_INFO_ITEM_H