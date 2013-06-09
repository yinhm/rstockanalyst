#include "ConfigSettings.h"
#include <QCoreApplication>

using namespace DilideCode;

DConfigSettings* DConfigSettings::m_pConfigSettings = 0;
DConfigSettings* DConfigSettings::getConfigSettings()
{
	if(!m_pConfigSettings)
		m_pConfigSettings = new DConfigSettings;

	return m_pConfigSettings;
}

DConfigSettings::DConfigSettings(void)
{
	m_qsFileName = qApp->applicationDirPath()+"/config.ini";
	m_pSettings = new QSettings(m_qsFileName,QSettings::IniFormat);
}


DConfigSettings::~DConfigSettings(void)
{
}

QVariant DConfigSettings::getValue( const QString& key )
{
	return m_pSettings->value(key);
}

void DConfigSettings::setValue( const QString& key, const QVariant& value )
{
	m_pSettings->setValue(key,value);
}

