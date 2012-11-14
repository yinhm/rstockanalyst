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
	static CDataEngine* getDataEngine();

public:
	//程序启动时，数据的初始化
	static void importData();
	//程序退出时，数据的自动保存
	static void exportData();


	/*数据导入函数*/
	//F10数据导入,返回值为导入的数据总条数，从后缀名为.fin的文件导入。
	static int importBaseInfoFromFinFile(const QString& qsFile);
	//F10数据导入，从本应用支持的数据导入，后缀名为 .rsafin
	static int importBaseInfo(const QString& qsFile);
	/*导入Reports数据*/
	static int importReportsInfo(const QString& qsFile);


	/*数据导出函数*/
	/*导出基本财务数据，F10数据*/
	static int exportBaseInfo(const QString& qsFile);
	/*导出Reports数据*/
	static int exportReportsInfo(const QString& qsFile);

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

	/*导出日线数据*/
	bool exportHistoryData(const QString& qsCode, const QList<qRcvHistoryData*>& list);
	/*获取某只股票的日线数据*/
	QList<qRcvHistoryData*> getHistoryList(const QString& code);
	/*获取某只股票最近count条的日线数据*/
	QList<qRcvHistoryData*> getHistoryList(const QString& code, int count);

signals:
	void stockInfoChanged(const QString&);

private:
	QMap<QString,CStockInfoItem*> m_mapStockInfos;

private:
	static CDataEngine* m_pDataEngine;
	static time_t m_tmCurrentDay;
	static time_t* m_tmLast5Day;
	QString m_qsHistroyDir;						//日线数据存储的路径 Dir/code/day...
};


#endif	//DATA_ENGINE_H