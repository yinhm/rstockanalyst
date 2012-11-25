#include "StdAfx.h"
#include "BaseLiner.h"

//static QScriptEngine g_se;


QScriptValue toScriptValue(QScriptEngine *engine, const stLinerItem &s)
{
	QScriptValue obj = engine->newObject();
	obj.setProperty("time", engine->newDate(QDateTime::fromTime_t(s.time)));
	obj.setProperty("open", s.fOpen);
	obj.setProperty("high", s.fHigh);
	obj.setProperty("low", s.fLow);
	obj.setProperty("close", s.fClose);
	obj.setProperty("volume", s.fVolume);
	obj.setProperty("amount", s.fAmount);
	obj.setProperty("advance", s.wAdvance);
	obj.setProperty("decline", s.wDecline);
	return obj;
}

void fromScriptValue(const QScriptValue &obj, stLinerItem &s)
{
	s.time = obj.property("time").toDateTime().toTime_t();
	s.fOpen = obj.property("open").toNumber();
	s.fHigh = obj.property("high").toNumber();
	s.fLow = obj.property("low").toNumber();
	s.fClose = obj.property("close").toNumber();
	s.fVolume = obj.property("volume").toNumber();
	s.fAmount = obj.property("amount").toNumber();
	s.wAdvance = obj.property("advance").toUInt16();
	s.wDecline = obj.property("decline").toUInt16();
}

QScriptValue createLinerItem(QScriptContext *, QScriptEngine *engine)
{
	stLinerItem s;
	return engine->toScriptValue(s);
}

QScriptValue getHighValue(QScriptContext *, QScriptEngine *engine)
{
//	engine->property("index");
	return 10;
}

void getMinAndMax(float& fMin,float& fMax,const QList<stLinerItem*>& list,int iCount)
{
	fMin = 9999999.0;
	fMax = -9999999.0;

	int c = 0;
	for(int i=(list.size()-1);(i>0&&c<=iCount);--i)
	{
		stLinerItem* p = list[i];
		if(p->fHigh>fMax)
			fMax = p->fHigh;
		if(p->fLow<fMin)
			fMin = p->fLow;

		++c;
	}
}


CBaseLiner::CBaseLiner(void)
	: m_pScriptEngine(0)
{
	m_qsExpression = "HIGH";
	m_pScriptEngine = new QScriptEngine;
	initScriptEnging();
}

CBaseLiner::~CBaseLiner(void)
{
	delete m_pScriptEngine;
}

void CBaseLiner::initScriptEnging()
{
//	qScriptRegisterMetaType<stLinerItem>(m_pScriptEngine, toScriptValue, fromScriptValue);
//	QScriptValue ctor = m_pScriptEngine->newFunction(createLinerItem);
//	m_pScriptEngine->globalObject().setProperty("stLinerItem", ctor);
	QScriptValue ctorHigh = m_pScriptEngine->newFunction(getHighValue);
	m_pScriptEngine->globalObject().setProperty("HIGH",ctorHigh);
}

void CBaseLiner::Draw( QPainter& p,const QList<stLinerItem*>& d,const QRectF& rtClient, int iShowCount  )
{
	QScriptProgram program("HIGH");
	QScriptValue result = m_pScriptEngine->evaluate(program);
	QString aa = result.toString();


	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);


	//设置画笔颜色
	p.setPen(QColor(255,0,0));

	float fItemWidth = rtClient.width()/iShowCount;			//单列宽度

	int iIndex = d.size()-1;
	float fBeginX = rtClient.right()-(fItemWidth/2);
	float fHighMax = fMaxPrice-fMinPrice;

	int iCount = 0;
	QPointF ptBegin(0.0,0.0);
	QPointF ptEnd(0.0,0.0);
	while(iCount<iShowCount&&iIndex>=0)
	{
		stLinerItem* pItem = d[iIndex];
		float fHighY = rtClient.bottom() - (((pItem->fHigh-fMinPrice)/fHighMax)*rtClient.height());
		if(ptBegin.isNull())
		{
			ptBegin = QPointF(fBeginX,fHighY);
		}
		else if(ptEnd.isNull())
		{
			ptEnd = ptBegin;
			ptBegin = QPointF(fBeginX,fHighY);
		}
		else
		{
			ptEnd = ptBegin;
			ptBegin = QPointF(fBeginX,fHighY);
			p.drawLine(ptBegin,ptEnd);
		}
	//	drawKGrid(pItem,p,QRectF(fBeginX,rtClient.top(),fItemWidth,rtClient.height()));

		fBeginX-=fItemWidth;
		--iIndex;
		++iCount;
	}
}



CKLineLiner::CKLineLiner( void )
{

}

CKLineLiner::~CKLineLiner( void )
{

}

void CKLineLiner::Draw( QPainter& p,const QList<stLinerItem*>& d,const QRectF& rtClient, int iShowCount )
{
	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);


	//设置画笔颜色
	p.setPen(QColor(255,0,0));

	float fItemWidth = rtClient.width()/iShowCount;			//单列宽度

	int iIndex = d.size()-1;
	float fBeginX = rtClient.right()-fItemWidth;
	int iCount = 0;
	while(iCount<iShowCount&&iIndex>=0)
	{
		stLinerItem* pItem = d[iIndex];
		drawKGrid(pItem,p,QRectF(fBeginX,rtClient.top(),fItemWidth,rtClient.height()));

		fBeginX-=fItemWidth;
		--iIndex;
		++iCount;
	}
}

void CKLineLiner::drawKGrid( stLinerItem* pHistory,QPainter& p,const QRectF& rtItem )
{
	float fHighMax = fMaxPrice-fMinPrice;
	float fHighY = ((pHistory->fHigh-fMinPrice)/fHighMax)*rtItem.height();
	float fLowY = ((pHistory->fLow-fMinPrice)/fHighMax)*rtItem.height();
	float fOpenY = ((pHistory->fOpen-fMinPrice)/fHighMax)*rtItem.height();
	float fCloseY = ((pHistory->fClose-fMinPrice)/fHighMax)*rtItem.height();

	if(pHistory->fClose>pHistory->fOpen)
	{
		//增长，绘制红色色块
		p.setPen(QColor(255,0,0));
		if(int(rtItem.width())%2==0)
		{
			QRectF rt = QRectF(rtItem.left()+0.5,rtItem.bottom()-fCloseY,rtItem.width()-1.0,fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
			p.fillRect(rt,QColor(255,0,0));
		}
		else
		{
			QRectF rt = QRectF(rtItem.left(),rtItem.bottom()-fCloseY,rtItem.width(),fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
			p.fillRect(rt,QColor(255,0,0));
		}
	}
	else
	{
		//降低，绘制蓝色色块
		p.setPen(QColor(0,255,255));
		if(int(rtItem.width())%2==0)
		{
			QRectF rt = QRectF(rtItem.left()+0.5,rtItem.bottom()-fOpenY,rtItem.width()-1.0,fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
			p.fillRect(rt,QColor(0,255,255));
		}
		else
		{
			QRectF rt = QRectF(rtItem.left(),rtItem.bottom()-fOpenY,rtItem.width(),fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
			p.fillRect(rt,QColor(0,255,255));
		}
	}

	p.drawLine(rtItem.center().x(),rtItem.bottom()-fHighY,rtItem.center().x(),rtItem.bottom()-fLowY);		//画最高价到最低价的线
}



CMultiLiner::CMultiLiner( MultiLinerType type )
	: m_type(type)
{

}

CMultiLiner::~CMultiLiner( void )
{

}

void CMultiLiner::Draw( QPainter& p, const QList<stLinerItem*>& d,const QRectF& rtClient, int iShowCount )
{
	float fMinPrice,fMaxPrice;
	getMinAndMax(fMinPrice,fMaxPrice,d,iShowCount);
	drawCoordY(p,QRectF(rtClient.right()+2,rtClient.top(),50,rtClient.height()),fMinPrice,fMaxPrice);

	foreach(CBaseLiner* pLiner,m_listLiner)
	{
		pLiner->setMaxPrice(fMaxPrice);
		pLiner->setMinPrice(fMinPrice);
		pLiner->Draw(p,d,rtClient,iShowCount);
	}
}

void CMultiLiner::setExpression( const QString& exp )
{
	foreach(CBaseLiner* p,m_listLiner)
		delete p;
	m_listLiner.clear();

	if(m_type==Main)
	{
		m_listLiner.push_back(new CKLineLiner());
		m_listLiner.push_back(new CBaseLiner());
	}
}

void CMultiLiner::drawCoordY( QPainter& p,const QRectF& rtClient,float fMinPrice,float fMaxPrice )
{
	if(!rtClient.isValid())
		return;
	//绘制Y坐标轴
	//设置画笔颜色
	p.setPen(QColor(255,0,0));

	//Y坐标（价格）
	p.drawLine(rtClient.topLeft(),rtClient.bottomLeft());			//主线

	int iBeginPrice = fMinPrice*10;
	int iEndPrice = fMaxPrice*10;
	float fGridHeight = rtClient.height()/(iEndPrice-iBeginPrice);
	int iGridSize = 1;
	while((fGridHeight*iGridSize)<10)
		++iGridSize;
	fGridHeight = fGridHeight*iGridSize;

	float fY = rtClient.bottom();
	float fX = rtClient.left();

	int iGridCount = 0;
	for (int i=(iBeginPrice+iGridSize); i<iEndPrice; i=i+iGridSize)
	{
		fY = fY-fGridHeight;
		if(iGridCount%10 == 0)
		{
			p.drawLine(fX,fY,fX+4,fY);
			p.setPen(QColor(0,255,255));
			p.drawText(fX+7,fY+4,QString("%1").arg(float(float(i)/10),0,'f',2));
			p.setPen(QColor(255,0,0));
		}
		else if(iGridCount%5 == 0)
			p.drawLine(fX,fY,fX+4,fY);
		else
			p.drawLine(fX,fY,fX+2,fY);
		++iGridCount;
	}
}