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

inline uint rRGB(int r, int g, int b)// set RGB value
{ return (0xffu << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff); }

class RSDSHARED_EXPORT CColorItem : public QObject
{
public:
	CColorItem(const QVector<uint>& vClrs);
	~CColorItem();

public:
	uint getColor(const float& _f,const float& _abs=0.1);
	uint getColor(const int& _v);

private:
	QVector<uint> m_vColors;
};


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
	/*获取指定的颜色列表*/
	static CColorItem* getColorItem(const QString& mode);

public:
	/*获取常用的颜色，通过索引值*/
	static uint getCommonColor(int index);

private:
	static CColorItem* m_pDefaultColor;	//默认色块表
	static QMap<QString,CColorItem*> m_mapColors;	//色块颜色种类。（通过百分比来所获取颜色的种类数）
	static QVector<uint> CommonColor;		//常用的颜色表
};


#endif	//COLOR_MANAGER_H