#pragma once
#include <QtCore>

#define	R_TIME_ZONE	8

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

	QMap<time_t,qRcvHistoryData> mapHistorys;

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

Q_DECLARE_METATYPE(qRcvReportData*);
Q_DECLARE_METATYPE(qRcvHistoryData*);

class CDataEngine : public QObject
{
	Q_OBJECT
public:
	CDataEngine(void);
	~CDataEngine(void);
	static CDataEngine* getDataEngine()
	{
		if(m_pDataEngine == NULL)
			m_pDataEngine = new CDataEngine;
		return m_pDataEngine;
	}

public:
	static time_t* getLast5DayTime();			//获取最近5天的开市日期
	static bool isStockOpenDay(time_t tmDay);	//判断tmDay是否开市
	static time_t getOpenSeconds();				//获取每天的开市时间（秒）；一般为4小时
	static time_t getOpenSeconds(time_t tmTime);//获取当天相对于tmTime的开市时间（秒）

public:
	//获取基本行情数据
	QList<qRcvReportData*> getBaseMarket();
	qRcvReportData* getBaseMarket(const QString& qsCode);
	void setBaseMarket(qRcvReportData* p);

	//补充日线数据
	bool appendHistory(const QString& code, const qRcvHistoryData& p);
	void updateBaseMarket(const QString& code);

signals:
	void baseMarketChanged(const QString&);

private:
	QMap<QString,qRcvReportData*> m_mapBaseMarket;

private:
	static CDataEngine* m_pDataEngine;
	static time_t m_tmCurrentDay;
	static time_t* m_tmLast5Day;

};

