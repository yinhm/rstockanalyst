#ifndef _RSTOCK_FUNC_H_
#define _RSTOCK_FUNC_H_
#include "StockInfoItem.h"

#ifndef FLOAT_NAN
#define	FLOAT_NAN	(std::numeric_limits<float>::quiet_NaN())
#endif

/*
RStockAnalyst的扩展函数定义
*/


// 分析周期
enum RStockCircle			//分析周期
{
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
	WidgetMarketTrend,			//市场行情图
	WidgetSColorBlock,		//股票的色块图
	WidgetBColorBlock,			//板块的色块图
	WidgetStockInfo,				//单只股票的行情信息
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
	CStockInfoItem* pItem;

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
	QRectF rtClient;
	int iEndIndex;				//结束的数组索引
	float fItemWidth;			//单个数据的绘制宽度

	float fMin;
	float fMax;

	tagRDrawInfo()
	{
		memset(&dwVersion,0,sizeof(tagRDrawInfo));
		fMin = 1.0;
		fMax = -1.0;
	}
} RDrawInfo;

#endif	//_RSTOCK_FUNC_H_