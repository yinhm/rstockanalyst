#ifndef B_COLOR_BLOCK_WIDGET_H
#define	B_COLOR_BLOCK_WIDGET_H

#include "BaseBlockWidget.h"
#include "BlockInfoItem.h"
#include "ColorManager.h"

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
	virtual void setCircle(RStockCircle /*_c*/){};		//屏蔽设置当前的显示周期

protected slots:
	void updateUI();							//更新当前需要显示的数据
	void onBlockReportUpdate(const QString& qsOnly);		//当板块数据更新时触发
	void onRemoveBlock();								//删除板块
	void onRecalcBlock();								//重新计算该板块的数据

private:
	void clickedBlock(CBlockInfoItem* pItem);				//当点击股票时触发

protected:
	virtual void paintEvent(QPaintEvent* e);				//绘制事件
	virtual void mouseMoveEvent(QMouseEvent* e);			//鼠标移动事件
	virtual void mousePressEvent(QMouseEvent* e);			//鼠标点击事件
	virtual void wheelEvent(QWheelEvent* e);				//鼠标中键滚动事件
	virtual void keyPressEvent(QKeyEvent* e);				//键盘操作

	//虚函数，各个控件的自定义菜单。
	virtual QMenu* getCustomMenu();

	/*相关绘图函数*/
private:
	void drawHeader(QPainter& p,const QRect& rtHeader);			//绘制头部信息
	void drawClient(QPainter& p,const QRect& rtClient);			//绘制主区域
	void drawBottom(QPainter& p,const QRect& rtBottom);			//绘制底部区域

	void drawBlock(QPainter& p,const QRect& rtCB,CBlockInfoItem* pItem,CColorItem* pClrItem);	//绘制单只股票

	QRect rectOfBlock(CBlockInfoItem* pItem);					//获取某只股票显示的位置
	CBlockInfoItem* hitTestBlock(const QPoint& ptPoint) const;	//测试某点所指向的股票信息
	qRcvFenBiData* hitTestCBItem(const QPoint& ptPoint) const;	//测试某点所指向的色块信息

private:
	QList<CBlockInfoItem*> m_listBlocks;		//当前显示的所有股票列表
	QMap<CBlockInfoItem*,int> m_mapBlockIndex;	//用来快速查找某只股票所在的索引

	CBlockInfoItem* m_pSelectedBlock;				//选中的block
	CBlockInfoItem* m_pCurBlock;					//当前显示的block

	/*用于绘制操作的成员变量*/
private:
	QAction* m_pActRemoveBlock;				//删除选中板块
	int m_iTitleHeight;						//头部高度
	int m_iBottomHeight;					//底部的高度
	int m_iLeftLen;							//左侧的空闲区域
	int m_iRightLen;						//右侧的空闲区域
	int showStockIndex;						//当前显示的起始位置（列）

	QRect m_rtHeader;						//头部Header区域
	QRect m_rtBottom;						//底部区域，用于鼠标操作等信息

	QTimer m_timerUpdateUI;					//界面更新的timer


	QList<QPair<CBlockInfoItem*,QRect>> m_listBlockBtns;	//各个分类所在的矩形
};

#endif // !B_COLOR_BLOCK_WIDGET_H
