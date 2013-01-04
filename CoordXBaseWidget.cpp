#include "StdAfx.h"
#include "CoordXBaseWidget.h"
#include "DataEngine.h"

//计算分时数据的横坐标时间
int getTimeMapByMin(QMap<time_t,int>& mapTimes,time_t& tmBegin, time_t& tmEnd, int iSize = 60/*second*/)
{
	if(tmBegin>tmEnd)
		return 0;

	time_t tmCur = tmEnd;
	while(tmCur>=tmBegin)
	{
//		QString qsTime = QDateTime::fromTime_t(tmCur).toString();
		mapTimes.insert(tmCur,mapTimes.size());
		tmCur = tmCur-iSize;
	}
	return 1;
}


CCoordXBaseWidget::CCoordXBaseWidget(CBaseWidget* parent /*= 0*/, WidgetType type /*= CBaseWidget::Basic*/)
	: CBaseWidget(parent,type)
	, m_typeCircle(Min1)

{

}


CCoordXBaseWidget::~CCoordXBaseWidget(void)
{
}


void CCoordXBaseWidget::updateTimesH()
{
	//更新当前的横坐标数据，从后向前计算时间
	m_mapTimes.clear();

//	int iCount = 1024;				//计算1024个时间
	if(m_typeCircle<Day)
	{
		time_t tmCur = CDataEngine::getCurrentTime();
		time_t tmLast = ((tmCur/(3600*24))*3600*24)+3600*(9-8)+60*25;	//9：25开盘
		time_t tmCurrent = (tmCur+m_typeCircle)/m_typeCircle*m_typeCircle;//向上对分钟取整
		time_t tmNoon1 = ((tmCur/(3600*24))*3600*24)+3600*(11-8)+60*30;
		time_t tmNoon2 = ((tmCur/(3600*24))*3600*24)+3600*(13-8);

		if((tmCurrent%(3600*24))>3600*7)
		{
			tmCurrent = (tmCurrent/(3600*24))*3600*24 + 3600*7 + m_typeCircle;		//3点收盘(多加一个周期)
		}
		/*需向上和向下多计算一个周期*/
		if(tmCurrent>tmNoon2)
		{
			time_t tmBegin = tmNoon2-m_typeCircle;
			//time_t tmEnd = tmCurrent+m_typeCircle*2;
			getTimeMapByMin(m_mapTimes,tmBegin,tmCurrent,m_typeCircle);
		}
		if(tmCurrent>tmNoon1)
		{
			time_t tmBegin = tmLast-m_typeCircle;
			time_t tmEnd = tmNoon1+m_typeCircle;
			getTimeMapByMin(m_mapTimes,tmBegin,tmEnd,m_typeCircle);
		}
	}
	else
	{
		if(m_typeCircle == Day)
		{
		}
		else if(m_typeCircle == DayN)
		{
			//目前未使用
		}
		else if(m_typeCircle == Week)
		{
		}
		else if(m_typeCircle == Month)
		{
		}
		else if(m_typeCircle == Month3)
		{
		}
		else if(m_typeCircle == Year)
		{
		}
	}
}


void CCoordXBaseWidget::drawCoordX(QPainter& p,const QRect& rtCoordX, int iGridSize)	//绘制X坐标轴
{
		//从右向左绘制横坐标
	float fBeginX = rtCoordX.right();
	float fEndX = rtCoordX.left();
	float fCBWidth = fBeginX-fEndX;
	if(fCBWidth<0)
		return;

	QList<time_t> listTimes = m_mapTimes.keys();
	float fCurX = fBeginX;
	float fLastX = fCurX;
	int iCount = listTimes.size()-1;

	int iTimeCount = 0;				//只是用来区分时间的颜色（隔开颜色，便于查看）
	while(fCurX>fEndX && iCount>=0)
	{
		if(m_typeCircle<Day)
		{
			if((fLastX-fCurX)>30)
			{
				p.setPen( iTimeCount%2 ? QColor(255,0,0) : QColor(0,255,255));
				p.drawLine(fCurX,rtCoordX.top(),fCurX,rtCoordX.top()+2);
				p.drawText(fCurX-14,rtCoordX.top()+2,30,rtCoordX.height()-2,
					Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("hh:mm"));
				fLastX = fCurX;
				++iTimeCount;
			}
		}

		--iCount;
		fCurX = fCurX-iGridSize;
	}
	return;
}