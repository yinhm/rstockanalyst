#ifndef _CONFIG_SETTINGS_H_
#define _CONFIG_SETTINGS_H_

#include <QtCore>
#include <QSettings>

namespace DilideCode
{
	class DConfigSettings
	{
	public:
		DConfigSettings(void);
		~DConfigSettings(void);

	public:
		static DConfigSettings* getConfigSettings();
		QVariant getValue(const QString& key);
		void setValue(const QString& key, const QVariant& value);

	private:
		static DConfigSettings* m_pConfigSettings;
		QString m_qsFileName;
	//	QMap<QString,QVariant> m_mapValues;
		QSettings* m_pSettings;
	};
}
#endif	//_CONFIG_SETTINGS_H_