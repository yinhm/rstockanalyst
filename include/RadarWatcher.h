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
	CRadarWatcher(void);
	~CRadarWatcher(void);

signals:
	void radarAlert(RRadarData* pRadar);

private:
	QList<RRadarData*> m_listRadar;							//预警雷达数据
};

#endif	//RADAR_WATCHER_H