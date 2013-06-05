/************************************************************************/
/* 文件名称：RadarWatcher.h
/* 创建时间：2013-06-05 19:51
/*
/* 描    述：雷达监视类，用来监视数据的异常
/************************************************************************/
#ifndef	RADAR_WATCHER_H
#define	RADAR_WATCHER_H
#include <QtCore>
class CAbstractStockItem;
enum RadarType
{
	BigVolumn = 1,		//大成交量
	BigAmount = 2,		//大成交额
	MaxPrice = 4,		//创新高
};

typedef struct tagRRadarData
{
	RadarType tpType;				//雷达类型
	time_t tmTime;					//生成时间
	CAbstractStockItem* pStock;		//股票指针
	QString qsDesc;					//描述信息
} RRadarData;

class CRadarWatcher : public QObject
{
	Q_OBJECT
public:
	/*
		创建雷达监视
		_t 雷达类型
		iSec 监视的时间间隔
	*/
	static CRadarWatcher* createRadarWatcher(RadarType _t,int iSec);

public:
	CRadarWatcher(int _id,RadarType _t,int iSec);
	~CRadarWatcher(void);

public:
	int getId(){ return m_id; }

signals:
	void radarAlert(RRadarData* pRadar);

private:
	QList<RRadarData*> m_listRadar;				//预警雷达数据
	int m_id;									//雷达id
	RadarType m_type;							//雷达类型
	int m_iSec;									//监视间隔

private:
	static QMap<int,CRadarWatcher*> m_listWatchers;		//所有的监视器
};

class CVolumnWatcher : public CRadarWatcher
{
public:
	CVolumnWatcher(int _id,int iSec);
	~CVolumnWatcher(void);
};

class CAmountWatcher : public CRadarWatcher
{
public:
	CAmountWatcher(int _id,int iSec);
	~CAmountWatcher(void);
};

class CMaxPriceWatcher : public CRadarWatcher
{
public:
	CMaxPriceWatcher(int _id,int iSec);
	~CMaxPriceWatcher(void);
};

#endif	//RADAR_WATCHER_H