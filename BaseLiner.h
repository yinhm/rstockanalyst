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

struct DrawData
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
};


class CBaseLiner
{
public:
	CBaseLiner(void);
	~CBaseLiner(void);

	virtual void Draw(const QPainter& p,const QList<DrawData>& d);

public:
	void setLineColor(const QColor& clr){ m_clrLine = clr; }
	void setDrawRect(const QRect& rtClient){ m_rtClient = rtClient; }

private:
	QColor m_clrLine;		//线条颜色
	QRect m_rtClient;		//绘制的区域
};

#endif	//BASE_LINER_H