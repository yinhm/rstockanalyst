/************************************************************************/
/* 获取单只股票的基本信息
/************************************************************************/
#ifndef	BASE_INFO_GETTER_H
#define	BASE_INFO_GETTER_H
#include "RStockFunc.h"

class CBaseInfoGetter
{
public:
	static QString getWebContentByUrl(const QString& qsUrl);
	static void updateBaseInfoFromEastMoney(qRcvBaseInfoData* pBaseInfo);		//从东方财富网更新数据
};

#endif	//BASE_INFO_GETTER_H