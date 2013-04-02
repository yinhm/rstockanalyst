/************************************************************************/
/* 文件名称：FuncBase.h
/* 创建时间：2013-02-26 03:54
/*
/* 描    述：函数扩展库，实现了基本的函数计算
/************************************************************************/
#include <QtCore>
#include "StockInfoItem.h"

extern "C"{_declspec(dllexport) QMap<const char*,lua_CFunction>* ExportAllFuncs();}

/*
别名: 绘制K线图
所属类别: 绘图函数  参数数量: 4

通过输入的参数绘制K线图
用法:
DrawK(OPEN,CLOSE,HIGH,LOW)
例如:
DrawK(OPEN,CLOSE,HIGH,LOW)
将开盘价，收盘价，最高价，最低价绘制成K线图
*/
int my_lua_drawk(lua_State* _L);

/*
别名: 绘制线段图
所属类别: 绘图函数  参数数量: 1

通过输入的参数绘制线段图
用法:
DrawLine(OPEN)
例如:
DrawK(OPEN)
将开盘价绘制为连线图
*/
int my_lua_drawLine(lua_State* _L);

/*
别名: 绘制柱状图
所属类别: 绘图函数  参数数量: 1

通过输入的参数绘制柱状图
用法:
DrawHist(VOLUME)
例如:
DrawHist(VOLUME)
绘制量的柱状图
*/
int my_lua_drawHistogram(lua_State* _L);

/*
别名: 绘制交叉点
所属类别: 绘图函数  参数数量: 2

通过输入的参数绘制柱状图
用法:
DrawCross(line1,line2)
例如:
DrawCross(line1,line2)
绘制line1和line2的交叉点
*/
int my_lua_drawCross(lua_State* _L);