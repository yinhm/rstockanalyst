/************************************************************************/
/* 文件名称：KLineWidget.h
/* 创建时间：2012-11-08 10:59
/*
/* 描    述：用于显示K线图的窗口，继承自CBaseWidget
/************************************************************************/

#ifndef K_LINE_WIDGET_H
#define K_LINE_WIDGET_H
#include <QtGui>
#include "CoordXBaseWidget.h"
#include "StockInfoItem.h"


class CKLineWidget : public CCoordXBaseWidget
{
	Q_OBJECT
public:
	CKLineWidget(CBaseWidget* parent = 0);
	~CKLineWidget(void);

public:
	//加载该K线图的配置信息
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//保存该K线图的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

	//更新数据
	virtual void updateData();

public slots:
	virtual void setStockCode(const QString& code);
	void updateKLine(const QString& code);

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void mouseMoveEvent(QMouseEvent* e);			//鼠标移动事件
	virtual void leaveEvent(QEvent* e);						//鼠标移出事件
	virtual void mousePressEvent(QMouseEvent* e);			//鼠标点击事件
	virtual void mouseDoubleClickEvent(QMouseEvent *e);		//鼠标双击事件
	virtual void keyPressEvent(QKeyEvent* e);				//键盘按下事件，用于处理快捷键设置

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

protected slots:
	void onSetStockCode();								//弹出对话框，让用户手动输入股票代码
	void onSetExpression();								//弹出对话框，让用户手动输入表达式
	void onClickedAddShow();							//减少显示个数
	void onClickedSubShow();							//增加显示个数
	void onAddDeputy();									//增加副图
	void onAddVolume();									//是否显示量视图
	void onRemoveDeputy();								//删除副图
	void onSetSizes();									//设置所有图的显示比例

private:
	void drawTitle(QPainter& p,const QRect& rtTitle);	//绘制头部
	void drawShowBtns(QPainter& p,const QRect& rtBtns);	//绘制右下角的两个按钮
private:
	void resetTmpData();					//重新计算数据。
	void clearTmpData();					//清理本窗口中创建的内存。

private:
	CStockInfoItem* m_pStockItem;			//当前K线图的股票数据指针
	QMap<time_t,RStockData*>* m_mapData;	//所有用于显示的数据
	int m_iShowCount;						//需要显示的数据个数（长度，理论上应小于listItems的size）

	int m_iCurExp;							//当前选中的表达式
	bool m_bShowMax;						//是否最大化显示（目前只对副图有效）

	int m_iTitleHeight;						//头部高度
	int m_iCoorYWidth;						//Y坐标轴的宽度
	int m_iCoorXHeight;						//X坐标轴的高度
	float m_fItemWidth;						//单个Item的宽度
	int m_iMainLinerHeight;					//主图的高度

	QVector<int> m_vSizes;					//显示比例(总和为100)
	QVector<QString> m_vExps;				//显示的表达式

	QRect m_rtAddShow;						//增加显示个数的按钮区域
	QRect m_rtSubShow;						//减少显示个数的按钮区域

	QRect m_rtClient;						//绘制K线图的区域
};


#endif	//K_LINE_WIDGET_H