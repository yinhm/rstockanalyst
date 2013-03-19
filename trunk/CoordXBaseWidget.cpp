#include "StdAfx.h"
#include "CoordXBaseWidget.h"
#include "DataEngine.h"
extern QMap<QString,lua_CFunction> g_func;
extern lua_State* g_Lua;
extern QString g_native;

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

int getDayMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	time_t tmNow = (_tmEnd+3600*24);			//需多计算一个周期
	while(tmNow>=_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		tmNow -= 3600*24;
	}

	return 1;
}

int getWeekMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addDays(8-dtNow.dayOfWeek());	//多计算一个周期
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		tmNow -= (3600*24*7);
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getMonthMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addMonths(1);					//多计算一个周期
	dtNow = QDate(dtNow.year(),dtNow.month(),1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addMonths(-1);
		dtNow = QDate(dtNow.year(),dtNow.month(),1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getMonth3MapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();
	dtNow = dtNow.addMonths(3);					//多计算一个周期
	dtNow = QDate(dtNow.year(),(dtNow.month()-1)/3*3,1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addMonths(-3);
		dtNow = QDate(dtNow.year(),(dtNow.month()-1)/3*3,1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getYearMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addYears(1);					//多计算一个周期
	dtNow = QDate(dtNow.year(),1,1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addYears(-1);
		dtNow = QDate(dtNow.year(),1,1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}




CCoordXBaseWidget::CCoordXBaseWidget(CBaseWidget* parent /*= 0*/, WidgetType type /*= CBaseWidget::Basic*/)
	: CBaseWidget(parent,type)
	, m_typeCircle(Min1)

{
	{
		m_pL = luaL_newstate();
		luaL_openlibs(m_pL);
		luaL_dostring(m_pL,g_native.toLocal8Bit());

		QMap<QString,lua_CFunction>::iterator iter = g_func.begin();
		while(iter!=g_func.end())
		{
			lua_register(m_pL,iter.key().toAscii(),iter.value());
			++iter;
		}
	}
	//m_pL = lua_newthread(g_Lua);
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
		time_t tmCurrent = (tmCur+m_typeCircle*2)/m_typeCircle*m_typeCircle;//向上对分钟取整
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
		else if(tmCurrent>tmLast)
		{
			time_t tmBegin = tmLast-m_typeCircle;
			time_t tmEnd = tmCurrent;
			getTimeMapByMin(m_mapTimes,tmBegin,tmEnd,m_typeCircle);
		}
	}
	else
	{
		time_t tmBegin = QDateTime(QDate(2000,1,1)).toTime_t();
		time_t tmEnd = (CDataEngine::getCurrentTime()/(3600*24))*3600*24 - 8*3600;

		if(m_typeCircle == Day)
		{
			getDayMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == DayN)
		{
			//目前未使用
		}
		else if(m_typeCircle == Week)
		{
			getWeekMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == Month)
		{
			getMonthMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == Month3)
		{
			getMonth3MapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == Year)
		{
			getYearMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
	}
}


void CCoordXBaseWidget::drawCoordX(QPainter& p,const QRect& rtCoordX, float fGridSize)	//绘制X坐标轴
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
		else
		{
			if((fLastX-fCurX)>80)
			{
				p.setPen( iTimeCount%2 ? QColor(255,0,0) : QColor(0,255,255));
				p.drawLine(fCurX,rtCoordX.top(),fCurX,rtCoordX.top()+2);
				switch(m_typeCircle)
				{
				case Week:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QString("%1 %2").arg(dtmTmp.date().year()).arg(dtmTmp.date().weekNumber()));
					}
					break;
				case Month:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy/MM"));
					}
					break;
				case Month3:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy/MM"));
					}
					break;
				case Year:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy"));
					}
					break;
				default:
					p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
						Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy/MM/dd"));
					break;
				}
				fLastX = fCurX;
				++iTimeCount;
			}
		}

		--iCount;
		fCurX = fCurX-fGridSize;
	}
	return;
}