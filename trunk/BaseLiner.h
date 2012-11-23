/************************************************************************/
/* 文件名称：BaseLiner.h
/* 创建时间：2012-11-22 16:03
/*
/* 描    述：绘制各种线的基类
/*           主要用于走势图中的线条绘制
/************************************************************************/

#ifndef BASE_LINER_H
#define BASE_LINER_H
#include <QtGui>
#include <QtScript>

struct stLinerItem
{
	time_t	time;			//UCT
	float	fOpen;			//开盘
	float	fHigh;			//最高
	float	fLow;			//最低
	float	fClose;			//收盘
	float	fVolume;		//量
	float	fAmount;		//额
	WORD	wAdvance;		//涨数,仅大盘有效
	WORD	wDecline;		//跌数,仅大盘有效
	stLinerItem()
	{
		memset(&time,0,sizeof(stLinerItem));
	}
};


class CBaseLiner
{
public:
	CBaseLiner(void);
	~CBaseLiner(void);

	virtual void Draw(QPainter& p,const QList<stLinerItem*>& d,const QRectF& rtClient,int iShowCount);

public:
	void setLineColor(const QColor& clr){ m_clrLine = clr; }
	void setDrawRect(const QRect& rtClient){ m_rtClient = rtClient; }
	void setMinPrice(float f){ fMinPrice=f; }
	void setMaxPrice(float f){ fMaxPrice=f; }

protected:
	QColor m_clrLine;		//线条颜色
	QRect m_rtClient;		//绘制的区域
	float fMaxPrice;
	float fMinPrice;
};

//K线图的线条绘制
class CKLineLiner : public CBaseLiner
{
public:
	CKLineLiner(void);
	~CKLineLiner(void);

public:
	virtual void Draw(QPainter& p,const QList<stLinerItem*>& d,const QRectF& rtClient,int iShowCount);

private:
	void drawKGrid( stLinerItem* pHistory,QPainter& p,const QRectF& rtItem );
};

class CMultiLiner
{
public:
	enum MultiLinerType
	{
		Main = 1,		//主图
		Deputy,			//副图
	};
public:
	CMultiLiner(MultiLinerType type);
	~CMultiLiner(void);

public:
	void Draw(QPainter& p, const QList<stLinerItem*>& d,const QRectF& rtClient,int iShowCount);
	void setExpression(const QString& exp);

private:
	void drawCoordY(QPainter& p,const QRectF& rtClient,float fMinPrice,float fMaxPrice);

private:
	QList<CBaseLiner*> m_listLiner;	//所拥有的绘制列表
	MultiLinerType m_type;			//用于区分是主图还是副图
};

#endif	//BASE_LINER_H