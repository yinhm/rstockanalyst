/************************************************************************/
/* 文件名称：RadarWatcher.h
/* 创建时间：2013-06-05 19:51
/*
/* 描    述：雷达监视类，用来监视数据的异常
/************************************************************************/
#ifndef	RADAR_WATCHER_H
#define	RADAR_WATCHER_H
#include <QtCore>
#include "rsd_global.h"

class CStockInfoItem;
class CBlockInfoItem;
class CRadarWatcher;

enum RadarType
{
	BigVolumn = 1,		//大成交量
	BigIncrease = 2,	//涨幅
	MaxPrice = 4,		//创新高
	MinPrice = 8,		//创新低
};

typedef struct tagRRadarData
{
	RadarType tpType;				//雷达类型
	time_t tmTime;					//生成时间
	int iWatcher;					//来源雷达ID
	CStockInfoItem* pStock;			//股票指针
	QString qsDesc;					//描述信息
} RRadarData;


class RSDSHARED_EXPORT CRadarWatcher : public QObject
{
	Q_OBJECT
public:
	CRadarWatcher(int _id,CBlockInfoItem* pBlock,RadarType _t,int iSec,float _hold,CBlockInfoItem* pDestBlock);
	~CRadarWatcher(void);

public:
	//获取该雷达的id
	int getId(){ return m_id; }
	RadarType getType(){return m_type;}
	float getHold(){ return m_fHold; }
	void setHold(float _hold){ m_fHold=_hold; }
	int getSec(){ return m_iSec; }
	void setSec(int _sec){ m_iSec = _sec; }
	CBlockInfoItem* getBlock(){ return m_pWatcherBlock; }
	void setBlock(CBlockInfoItem* _b);
	CBlockInfoItem* getDestBlock(){ return m_pDestBlock; }
	void setDestBlock(CBlockInfoItem* _b);

protected slots:
	virtual void onStockReportComing(CStockInfoItem* pItem) = 0;

protected:
	int m_iSec;									//监视间隔
	float m_fHold;								//阈值
	int m_id;									//雷达id
	CBlockInfoItem* m_pDestBlock;				//输出到的板块

private:
	RadarType m_type;							//雷达类型
	CBlockInfoItem* m_pWatcherBlock;			//监视的板块
};

class RSDSHARED_EXPORT CRadarManager : public QObject
{
	Q_OBJECT
public:
	static CRadarManager* getRadarManager();
	static QString getTypeName(RadarType _t);

public:
	/*
		创建雷达监视
		pBlock 监视的板块
		_t 雷达类型
		iSec 监视的时间间隔
		_hold 阈值
	*/
	CRadarWatcher* createRadarWatcher(CBlockInfoItem* pBlock,
		RadarType _t,int iSec,float _hold,CBlockInfoItem* pDestBlock=NULL,int iId=-1);
	
	//获取所有的监视雷达
	QList<CRadarWatcher*> getRadarWatchers();

	//获取指定ID的监视雷达
	CRadarWatcher* getWatcher(const int& _id);

	//删除指定ID的监视雷达
	void removeWatcher(const int& _id);

	//追加产生的监视信息
	void appendRadar(RRadarData* pRadar);

	//加载监视雷达
	void loadRadars();
	//保存监视雷达
	void saveRadars();
	//析构监视雷达
	void releaseRadars();

signals:
	void radarAlert(RRadarData* pRadar);

protected:
	CRadarManager(){};
	~CRadarManager(){};

private:
	QMap<int,CRadarWatcher*> m_listWatchers;	//所有的监视器
	QList<RRadarData*> m_listRadar;				//预警雷达数据

private:
	static CRadarManager* m_pSelf;
};


/*成交量监视，监视成交量的变化*/
class CVolumnWatcher : public CRadarWatcher
{
	struct tagVolumnData
	{
		time_t tmTime;		//最后形成时间
		float fLastV;		//上一次的成交量
		float fNewV;		//时间所表示的成交量
	};
	Q_OBJECT
public:
	CVolumnWatcher(int _id,CBlockInfoItem* pBlock,int iSec,float _hold,CBlockInfoItem* pDestBlock);
	~CVolumnWatcher(void);
protected slots:
	void onStockReportComing(CStockInfoItem* pItem);

private:
	QMap<CStockInfoItem*,tagVolumnData*> m_mapVolume;		//上一轮成交量
};

class CIncreaseWatcher : public CRadarWatcher
{
	struct tagPriceData
	{
		time_t tmTime;		//最后形成时间
		float fMaxPrice;	//周期中的最高价格
		float fNewPrice;	//时间所表示的价格
	};
	Q_OBJECT
public:
	CIncreaseWatcher(int _id,CBlockInfoItem* pBlock,int iSec,float _hold,CBlockInfoItem* pDestBlock);
	~CIncreaseWatcher(void);
protected slots:
	void onStockReportComing(CStockInfoItem* pItem);

private:
	QMap<CStockInfoItem*,tagPriceData*> m_mapPrice;		//上一轮成交量
};

class CMaxPriceWatcher : public CRadarWatcher
{
	struct tagPriceData
	{
		time_t tmTime;		//最后形成时间
		float fMaxPrice;	//周期中的最高价格
		float fNewPrice;	//时间所表示的价格
	};
	Q_OBJECT
public:
	CMaxPriceWatcher(int _id,CBlockInfoItem* pBlock,int iSec,float _hold,CBlockInfoItem* pDestBlock);
	~CMaxPriceWatcher(void);
protected slots:
	void onStockReportComing(CStockInfoItem* pItem);

private:
	QMap<CStockInfoItem*,tagPriceData*> m_mapPrice;		//上一轮成交量
};

class CMinPriceWatcher : public CRadarWatcher
{
	struct tagPriceData
	{
		time_t tmTime;		//最后形成时间
		float fMinPrice;	//周期中的最高价格
		float fNewPrice;	//时间所表示的价格
	};
	Q_OBJECT
public:
	CMinPriceWatcher(int _id,CBlockInfoItem* pBlock,int iSec,float _hold,CBlockInfoItem* pDestBlock);
	~CMinPriceWatcher(void);
protected slots:
	void onStockReportComing(CStockInfoItem* pItem);

private:
	QMap<CStockInfoItem*,tagPriceData*> m_mapPrice;		//上一轮成交量
};

#endif	//RADAR_WATCHER_H