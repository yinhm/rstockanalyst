#ifndef DATA_ENGINE_H
#define DATA_ENGINE_H

#include "StockInfoItem.h"
#include "BlockInfoItem.h"
#include "rsd_global.h"
#include "RStockFunc.h"

#define	R_TIME_ZONE	8

class RSDSHARED_EXPORT CDataEngine : public QObject
{
	Q_OBJECT
public:
	CDataEngine(void);
	~CDataEngine(void);
	static CDataEngine* getDataEngine();
	static void releaseDataEngine();

public:
	//通过市场类型获取市场名称
	static QString getMarketStr(WORD wMarket);
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
	/*导入分笔数据*/
	static int importFenBisData(const QString& qsFile);
	/*导入板块数据*/
	static int importBlocksData(const QString& qsPath);



	/*数据导出函数*/
	/*导出基本财务数据，F10数据*/
	static int exportBaseInfo(const QString& qsFile);
	/*导出Reports数据*/
	static int exportReportsInfo(const QString& qsFile);
	/*导出分笔数据*/
	static int exportFenBisData(const QString& qsFile);
	/*收盘后数据整理*/
	static int exportCloseData();

public:
	static bool isStockOpenDay(time_t tmDay);	//判断tmDay是否开市（某天的日期，不含时间）
	static bool isStockOpenTime(time_t tmMin);	//判断tmMin是否开市（某天的时间，不含日期）
	static time_t getOpenSeconds();				//获取每天的开市时间（秒）；一般为4小时
	static time_t getOpenSeconds(time_t tmTime);//获取当天相对于tmTime的开市时间（秒）
	static time_t getCurrentTime();				//获取最后一个report数据的时间
	static void setCurrentTime(const time_t& t);//设置最后一个report数据的时间

	static QMap<time_t,int> getTodayTimeMap(RStockCircle _c);	//获取当天的时间轴
	/*将分笔数据按照时间轴排序*/
	static QMap<time_t,RStockData*>* getColorBlockItems(const QMap<time_t,int>& mapTimes, const QList<qRcvFenBiData*>& minutes);
	/*将日线按照时间轴排序*/
	static QMap<time_t,RStockData*>* getColorBlockItems(const QMap<time_t,int>& mapTimes, const QList<qRcvHistoryData*>& minutes);

public:
	QString getStockBlockDir() const{ return m_qsBlocksDir; }

public:
	//获取板块数据
	QList<CBlockInfoItem*> getTopLevelBlocks();	//获取顶层板块列表
	QList<CBlockInfoItem*> getStockBlocks();	//获取所有板块列表
	CBlockInfoItem* getStockBlock(const QString& qsCode);		//通过板块名称获取板块
	bool isHadBlock(const QString& block);		//是否存在某板块
	void setBlockInfoItem(CBlockInfoItem* _p);						//设置板块数据
	void removeBlockInfoItem(CBlockInfoItem* _p);				//删除板块数据

	//获取单只股票数据
	QList<CStockInfoItem*> getStocksByMarket(WORD wMarket);			//根据市场类型获取股票列表
	QList<CStockInfoItem*> getStockInfoList();
	CStockInfoItem* getStockInfoItem(const QString& qsCode);
	void setStockInfoItem(CStockInfoItem* p);

	//获取股票（含板块）数据
	CAbstractStockItem* getStockItem(const QString& qsCode);
	QList<CAbstractStockItem*> getStockItems();

	//新闻数据
	void appendNews(const QString& title, const QString& content);		//添加新闻
	//F10数据
	void appendF10(const QString& title, const QString& content);		//添加F10数据
	//显示F10数据
	bool showF10Data(const QString& code);

	/*导出日线数据*/
	bool exportHistoryData(const QString& qsCode, const QList<qRcvHistoryData*>& list);
	bool exportHistoryData(const QString& qsCode, const QList<qRcvHistoryData*>& list, int iOffset);
	/*获取某只股票的日线数据*/
	QList<qRcvHistoryData*> getHistoryList(const QString& code);
	/*获取某只股票最近count条的日线数据*/
	QList<qRcvHistoryData*> getHistoryList(const QString& code, int count);

	/*导出收盘后当天5分钟数据*/
	bool export5MinData(const QString& qsCode, const QMap<time_t,int>& mapTimes);
	/*获取5分钟数据*/
	QMap<time_t,RStockData*>* get5MinData(const QString& qsCode);


	/*导出分笔数据*/
	bool exportFenBiData(const QString& qsCode, const long& lDate, const QList<qRcvFenBiData*>& list);

private:
	QMap<QString,CStockInfoItem*> m_mapStockInfos;			//股票数据
	QMap<QString,CBlockInfoItem*> m_mapBlockInfos;			//板块数据
	QList<CBlockInfoItem*> m_listTopLevelBlocks;			//顶层板块

private:
	static CDataEngine* m_pDataEngine;
	static time_t m_tmCurrentDay;
	static time_t m_tmCurrent;					//最后一个report数据的时间
	QString m_qsHistroyDir;						//日线数据存储的路径 AppDir/data/history/...
	QString m_qsBlocksDir;						//板块数据的存储路径 AppDir/data/blocks/...
	QString m_qsNewsDir;						//新闻数据的存储路径 AppDir/data/news/...
	QString m_qsF10Dir;							//F10数据的存储路径 AppDir/data/F10/...
	QString m_qs5Min;							//5分钟数据的存储路径 AppDir/data/5Min/...
	QString m_qsFenBiDir;						//分笔数据的存储路径 AppDir/data/FenBi/Date/...
};


#endif	//DATA_ENGINE_H