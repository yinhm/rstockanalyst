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
#include <QColor>

class CColorManager
{
public:
	static void initAll();
private:
	static void initCommonColor();
	static void initBlockColors();

public:
	/*通过值fVal来获取种类iType中所对应的值*/
	static QColor getBlockColor(int iType,float fVal);

public:
	static QVector<QColor> CommonColor;		//常用的颜色表
	static QMap<QString,QVector<QColor>> BlockColors;	//色块颜色种类。（通过百分比来所获取颜色的种类数）
};


#endif	//COLOR_MANAGER_H