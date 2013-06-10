#ifndef _RSTOCK_FUNC_H_
#define _RSTOCK_FUNC_H_
#include <QtCore>
#include "AbstractStockItem.h"

#ifndef FLOAT_NAN
#define	FLOAT_NAN	(std::numeric_limits<float>::quiet_NaN())
#endif

/*
RStockAnalyst的扩展函数定义
*/

#pragma   pack(push,1)					//设置内存对齐方式为 1字节
// 分析周期
enum RStockCircle			//分析周期
{
	AutoCircle = 0,			//自动模式
	FenShi = 1,				//分时模式（直接连线）
	Sec10 = 10,				//10秒
	Sec30 = 30,				//30秒
	Min1 = 60,				//1分钟
	Min5 = 5*60,			//5分钟
	Min15 = 15*60,			//15分钟
	Min30 = 30*60,			//30分钟
	Min60 = 60*60,			//60分钟
	MinN,					//N分钟
	Day,					//日线
	DayN,					//N日线
	Week,					//周线
	Month,					//月线
	Month3,					//季线
	Year,					//年线
};

//窗口类型
enum RWidgetType
{
	WidgetBasic = 0,				//基础图
	WidgetKLine,					//K线图
	WidgetMarketTrend,				//市场行情图
	WidgetSColorBlock,				//股票的色块图
	WidgetBColorBlock,				//板块的色块图
	WidgetStockInfo,				//单只股票的行情信息
	WidgetRadar,					//雷达监视图
	WidgetStockDeal,				//交易详情图
};


//窗口的方向
enum RWidgetInsert
{
	InsertLeft =1,			//左插入
	InsertRight,				//右插入
	InsertTop,				//上插入
	InsertBottom,			//下插入
	InsertRemove,			//删除
};

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

// 基本数据
typedef struct tagRStockData
{
	time_t	tmTime;			//UCT
	float	fOpen;			//开盘 注：分时数据中此数据表示最新价
	float	fHigh;			//最高
	float	fLow;				//最低
	float	fClose;			//收盘 注：分时数据中此数据表示上一周期最后的价格
	float	fVolume;			//量
	float	fAmount;			//额
	WORD	wAdvance;			//涨数,仅大盘有效
	WORD	wDecline;			//跌数,仅大盘有效

	tagRStockData()
	{
		memset(&tmTime,0,sizeof(tagRStockData));
	}
	tagRStockData(RCV_HISTORY_STRUCTEx* p)
	{
		memcpy(&tmTime,&p->m_time,sizeof(tagRStockData));
	}
	tagRStockData(const qRcvReportData* p)
	{
		tmTime = QDateTime(QDateTime::fromTime_t(p->tmTime).date()).toTime_t();
		fAmount = p->fAmount;
		fClose = p->fNewPrice;
		fHigh = p->fHigh;
		fLow = p->fLow;
		fOpen = p->fOpen;
		fVolume = p->fVolume;
	}
} RStockData;


// 扩展数据,用于描述分笔成交数据的买卖盘
typedef union tagRStockDataEx
{
	struct
	{
		float m_fBuyPrice[5];		//买1--买5价
		float m_fBuyVol[5];			//买1--买5量
		float m_fSellPrice[5];		//卖1--卖5价        
		float m_fSellVol[5];		//卖1--卖5量 
	};
	float m_fDataEx[20];
} RStockDataEx;


// 除权数据
typedef struct tagRSplitData
{
	time_t	m_time;	//时间,UCT
	float	m_fHg;	//红股
	float	m_fPg;	//配股
	float	m_fPgj;	//配股价
	float	m_fHl;	//红利
} RSpliteData;


// 调用接口信息数据结构
typedef struct tagRCalcInfo
{
	DWORD dwVersion;		//版本
	RStockCircle emCircle;		//分析周期
	CAbstractStockItem* pItem;

	QMap<time_t,RStockData*>* mapData;	//常规数据,可能为NULL
	QMap<time_t,RStockDataEx*>* mapDataEx;	//扩展数据,分笔成交买卖盘,注意:可能为 NULL

	tagRCalcInfo()
	{
		memset(&dwVersion,0,sizeof(tagRCalcInfo));
	}
} RCalcInfo;


typedef class QPainter QPainter;
typedef class QRect QRect;
typedef struct tagRDrawInfo
{
	DWORD dwVersion;
	QPainter* pPainter;
	int iEndIndex;				//结束的数组索引
	float fItemWidth;			//单个数据的绘制宽度
	float fMin;
	float fMax;
	int iCurColor;				//当前使用的颜色索引
	QList<uint> lsColors;		//绘制所需要的颜色表
	QRectF rtClient;

	tagRDrawInfo()
	{
		memset(&dwVersion,0,28);
		fMin = 1.0;
		fMax = -1.0;
	}
} RDrawInfo;

#pragma   pack(pop)					//去除内存对齐方式设置

#endif	//_RSTOCK_FUNC_H_