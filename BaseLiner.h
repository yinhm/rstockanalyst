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

Q_DECLARE_METATYPE(stLinerItem)
Q_DECLARE_METATYPE(QVector<float>)
Q_DECLARE_METATYPE(QVector<stLinerItem>)

class CBaseLiner
{
public:
	CBaseLiner(QScriptEngine* pEngine,const QString& exp);
	~CBaseLiner(void);
public:
	virtual void updateData();				//更新显示的数据
	virtual void getMinAndMax(float& fMin,float& fMax,int iCount);
	virtual void Draw(QPainter& p,const QRectF& rtClient,int iShowCount);

public:
	void setLineColor(const QColor& clr){ m_clrLine = clr; }
	void setMinPrice(float f){ fMinPrice=f; }
	void setMaxPrice(float f){ fMaxPrice=f; }

protected:
	QColor m_clrLine;		//线条颜色
	QRect m_rtClient;		//绘制的区域
	float fMaxPrice;
	float fMinPrice;
	QVector<float> m_vals;
	QString m_qsExp;			//表达式
	QScriptEngine* m_pEngine;	//脚本引擎
};

//K线图的线条绘制
class CKLineLiner : public CBaseLiner
{
public:
	CKLineLiner(QScriptEngine* pEngine);
	~CKLineLiner(void);

public:
	virtual void updateData();				//更新显示的数据
	virtual void getMinAndMax(float& fMin,float& fMax,int iCount);
	virtual void Draw(QPainter& p,const QRectF& rtClient,int iShowCount);

private:
	void drawKGrid( const int& iIndex,QPainter& p,const QRectF& rtItem );

private:
	QVector<float> m_vOpen;
	QVector<float> m_vHigh;
	QVector<float> m_vLow;
	QVector<float> m_vClose;
};


class CVolumeLiner : public CBaseLiner
{
public:
	CVolumeLiner(QScriptEngine* pEngine);
	~CVolumeLiner(void);

public:
	virtual void updateData();				//更新显示的数据
	virtual void getMinAndMax(float& fMin,float& fMax,int iCount);
	virtual void Draw(QPainter& p,const QRectF& rtClient,int iShowCount);

private:
	QVector<float> m_vOpen;
	QVector<float> m_vClose;
//	QVector<float> m_vVolume;
};

class CMultiLiner
{
public:
	enum MultiLinerType
	{
		MainKLine = 1,	//主图(主要指K线图)
		VolumeLine,		//副图中的量数据
		Deputy,			//副图
	};
public:
	CMultiLiner(MultiLinerType type,QScriptEngine* pEngine,const QString& exp);
	~CMultiLiner(void);

public:
	void updateData();			//更新数据
	void Draw(QPainter& p, const QRectF& rtClient,int iShowCount);
	void setExpression(const QString& exp);
	QString getExpression() const { return m_qsExp; }
	QRectF getRect() const { return m_rtClient; }

private:
	void drawCoordY(QPainter& p,const QRectF& rtClient,float fMinPrice,float fMaxPrice);

private:
	QList<CBaseLiner*> m_listLiner;	//所拥有的绘制列表
	MultiLinerType m_type;			//用于区分是主图还是副图
	QScriptEngine* m_pEngine;		//数据引擎
	QRectF m_rtClient;				//当前窗口的大小
	QString m_qsExp;				//该窗口中的表达式
};

#endif	//BASE_LINER_H