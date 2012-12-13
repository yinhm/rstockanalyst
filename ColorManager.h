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

#define	COLOR_BLOCK_SIZE	21

class CColorManager
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
	static QColor getBlockColor(const QString& mode,float fVal);
	/*通过索引值来查找种类为mode的对应颜色值*/
	static QColor getBlockColor(const QString& mode,int index);

public:
	static QVector<QColor> CommonColor;		//常用的颜色表

private:
	static QVector<QColor> DefaultColor;	//默认色块表
	static QMap<QString,QVector<QColor>> BlockColors;	//色块颜色种类。（通过百分比来所获取颜色的种类数）
};


#endif	//COLOR_MANAGER_H