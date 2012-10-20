#include "StdAfx.h"
#include "DataEngine.h"
#include "STKDRV.h"

CDataEngine* CDataEngine::m_pDataEngine = NULL;

CDataEngine::CDataEngine(void)
{
}

CDataEngine::~CDataEngine(void)
{
}

void CDataEngine::initAllReport()
{
	int iTotal = CSTKDRV::GetTotalNumber();
	for(int i=0;i<iTotal;++i)
	{
		RCV_REPORT_STRUCTExV3 report;
		if(CSTKDRV::GetStockByNoEx(0,&report)>0)
		{
			qRcvReportData* pReport = CDataEngine::getDataEngine()->getBaseMarket(QString::fromLocal8Bit(report.m_szLabel));
			if(pReport)
			{
				pReport->resetItem(&report);
			}
			else
			{
				CDataEngine::getDataEngine()->setBaseMarket(new qRcvReportData(&report));
			}
		}
	}
}
