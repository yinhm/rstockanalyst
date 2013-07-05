/************************************************************************/
/* 文件名称：ColorManager.h
/* 创建时间：2012-11-30 14:38
/*
/* 描    述：颜色管理类
/*           用于各种常用的颜色管理
/************************************************************************/

#ifndef COLOR_MANAGER_H
#define COLOR_MANAGER_H
#include <QtCore>
#include "rsd_global.h"

#define	COLOR_BLOCK_SIZE	21

inline uint rRGB(int r, int g, int b)// set RGB value
{ return (0xffu << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff); }

class RSDSHARED_EXPORT CColorManager
{
public:
	static void initAll();
	static void reloadBlockColors();		//重新加载色块颜色表
private:
	static void initCommonColor();
	static void initBlockColors();

public:
	/*获取当前所支持的色块颜色列表*/
	static QStringList getBlockColorList();
	/*通过值fVal来获取种类mode中所对应的值*/
	static uint getBlockColor(const QString& mode,float fVal);
	/*通过索引值来查找种类为mode的对应颜色值*/
	static uint getBlockColor(const QString& mode,int index);
	//获取颜色表
	static bool getBlockColor(const QString& mode,QVector<uint>& vColors);

public:
	/*获取常用的颜色，通过索引值*/
	static uint getCommonColor(int index);

private:
	static QVector<uint> DefaultColor;	//默认色块表
	static QMap<QString,QVector<uint>> BlockColors;	//色块颜色种类。（通过百分比来所获取颜色的种类数）
	static QVector<uint> CommonColor;		//常用的颜色表
};


#endif	//COLOR_MANAGER_H