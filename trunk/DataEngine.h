#ifndef DATA_ENGINE_H
#define DATA_ENGINE_H

#include "StockInfoItem.h"

#define	R_TIME_ZONE	8

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
	/*数据导入函数*/

	//F10数据导入,返回值为导入的数据总条数
	static int importBaseInfoFromFile(const QString& qsFile);


public:
	static time_t* getLast5DayTime();			//获取最近5天的开市日期
	static bool isStockOpenDay(time_t tmDay);	//判断tmDay是否开市
	static time_t getOpenSeconds();				//获取每天的开市时间（秒）；一般为4小时
	static time_t getOpenSeconds(time_t tmTime);//获取当天相对于tmTime的开市时间（秒）

public:
	//获取基本行情数据
	QList<CStockInfoItem*> getStockInfoList();
	CStockInfoItem* getStockInfoItem(const QString& qsCode);
	void setStockInfoItem(CStockInfoItem* p);

signals:
	void stockInfoAdded(const QString&);

private:
	QMap<QString,CStockInfoItem*> m_mapStockInfos;

private:
	static CDataEngine* m_pDataEngine;
	static time_t m_tmCurrentDay;
	static time_t* m_tmLast5Day;
};


#endif	//DATA_ENGINE_H