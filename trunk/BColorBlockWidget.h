#ifndef B_COLOR_BLOCK_WIDGET_H
#define	B_COLOR_BLOCK_WIDGET_H

#include "CoordXBaseWidget.h"
#include "BlockInfoItem.h"

class CBColorBlockWidget : public CCoordXBaseWidget
{
	Q_OBJECT
public:
	CBColorBlockWidget(CBaseWidget* parent = 0);
	~CBColorBlockWidget(void);


	/*用于绘制操作的成员变量*/
private:
	int m_iTitleHeight;						//头部高度
	int m_iCBHeight;						//单个色块的高度
	int m_iCBWidth;							//单个色块的宽度
	int m_iBottomHeight;					//底部的高度
	int showStockIndex;						//当前显示的起始位置（列）

	QRect m_rtHeader;						//头部Header区域
	QRect m_rtClient;						//实际色块绘制区域
	QRect m_rtBottom;						//底部区域，用于鼠标操作等信息
};

#endif // !B_COLOR_BLOCK_WIDGET_H
