/************************************************************************/
/* 文件名称：BlockCodeManager.cpp
/* 创建时间：2013-05-21 10:43
/*
/* 描    述：用于管理板块的股票代码
/************************************************************************/
#include "StdAfx.h"
#include "BlockCodeManager.h"


QSettings* CBlockCodeManager::m_pRegApp = 0;

QString CBlockCodeManager::getBlockCode( const QString& qsCodePath )
{
	if(m_pRegApp == 0)
	{
		m_pRegApp = new QSettings("HKEY_CURRENT_USER\\Software\\RStockAnalyst\\", QSettings::NativeFormat);
	}

	QString qsCurrentCode = m_pRegApp->value("\\").toString();
	qsCurrentCode = QString("%1").arg(qsCurrentCode.toInt()+1);

	if(m_pRegApp->contains(qsCodePath))
	{
		return m_pRegApp->value(qsCodePath).toString();
	}
	else
	{
		m_pRegApp->setValue(qsCodePath,qsCurrentCode);
		m_pRegApp->setValue("\\",qsCurrentCode);
	}
	return "";
}

CBlockCodeManager::CBlockCodeManager(void)
{
}


CBlockCodeManager::~CBlockCodeManager(void)
{
}
