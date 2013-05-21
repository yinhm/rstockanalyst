/************************************************************************/
/* 文件名称：BlockCodeManager.h
/* 创建时间：2013-05-21 10:43
/*
/* 描    述：用于管理板块的股票代码
/************************************************************************/
#ifndef BLOCK_CODE_MANAGER_H
#define BLOCK_CODE_MANAGER_H
#include <QtCore>


class CBlockCodeManager
{

public:
	static QString getBlockCode(const QString& qsCodePath);

protected:
	CBlockCodeManager(void);
	~CBlockCodeManager(void);

private:
	static QSettings* m_pRegApp;
};


#endif	//BLOCK_CODE_MANAGER_H