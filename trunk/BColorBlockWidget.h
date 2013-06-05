#ifndef B_COLOR_BLOCK_WIDGET_H
#define	B_COLOR_BLOCK_WIDGET_H

#include "BaseBlockWidget.h"
#include "BlockInfoItem.h"

class CBColorBlockWidget : public CBaseBlockWidget
{
	Q_OBJECT
public:
	CBColorBlockWidget(CBaseWidget* parent = 0);
	~CBColorBlockWidget(void);

public:
	//加载该K线图的配置信息
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//保存该K线图的配置信息
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

	//更新数据
	virtual void updateData();
	//清理当前的内存
	virtual void clearTmpData();

	//更新当前的排序方式
	virtual void updateSortMode(bool bSelFirst);
public:
	//通过查找keyword获取需要在按键精灵上显示的数据
	virtual void getKeyWizData(const QString& keyword,QList<KeyWizData*>& listRet);
	//键盘精灵窗口确认后触发
	virtual void keyWizEntered(KeyWizData* pData);

public slots:
	virtual void setBlock(const QString& block);

protected slots:
	void updateColorBlockData();							//更新当前需要显示的数据
	void updateShowMap();									//更新要显示的数据，不删除之前的

private:
	void clickedBlock(CBlockInfoItem* pItem);				//当点击股票时触发

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void mouseMoveEvent(QMouseEvent* e);			//鼠标移动事件
	virtual void mousePressEvent(QMouseEvent* e);			//鼠标点击事件
	virtual void wheelEvent(QWheelEvent* e);				//鼠标中键滚动事件
	virtual void keyPressEvent(QKeyEvent* e);				//键盘操作

	//绘制色块
	//vColor:取0-21
	//vHeight:百分比0-100%;
	//vWidth:百分比0%-100%;
	void drawColocBlock(QPainter& p,int iY,
		QVector<float>& vColor,QVector<float>& vHeight,QVector<float>& vWidth);

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

	/*相关绘图函数*/
private:
	void drawHeader(QPainter& p,const QRect& rtHeader);			//绘制头部信息
	void drawClient(QPainter& p,const QRect& rtClient);			//绘制主区域
	void drawBottom(QPainter& p,const QRect& rtBottom);			//绘制底部区域

	void drawBlock(QPainter& p,const QRect& rtCB,CBlockInfoItem* pItem);	//绘制单只股票

	QRect rectOfBlock(CBlockInfoItem* pItem);					//获取某只股票显示的位置
	CBlockInfoItem* hitTestStock(const QPoint& ptPoint) const;		//测试某点所指向的股票信息
	RStockData* hitTestCBItem(const QPoint& ptPoint) const;//测试某点所指向的色块信息

private:
	CBlockInfoItem* m_pSelectedStock;

	/*用于绘制操作的成员变量*/
private:
	int m_iTitleHeight;						//头部高度
	int m_iBottomHeight;					//底部的高度
	int showStockIndex;						//当前显示的起始位置（列）

	QRect m_rtHeader;						//头部Header区域
	QRect m_rtBottom;						//底部区域，用于鼠标操作等信息

	QTimer m_timerUpdateUI;					//界面更新的timer
};

#endif // !B_COLOR_BLOCK_WIDGET_H
