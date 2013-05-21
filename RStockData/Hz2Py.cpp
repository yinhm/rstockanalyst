#include "StdAfx.h"
#include "Hz2Py.h"

QMap<QChar,QList<QString>> CHz2Py::m_mapHanZi2PY;

//初始化汉字->拼音对照表
int CHz2Py::initHz2PyTable( const QString& qsFile )
{
	if(m_mapHanZi2PY.size()>0)
	{
		qDebug()<<"The Hz2Py had inited!";
		return -1;
	}
	QFile _f(qsFile);
	if(!_f.open(QFile::ReadOnly))
	{
		qDebug()<<"Open HanZi2PinYin DB Error!";
		return -1;
	}

	qDebug()<<"Load HanZi2PinYin DB...";
	while(!_f.atEnd())
	{
		QString strLine = QString::fromUtf8(_f.readLine()).trimmed();
		QStringList _l = strLine.split(" ");
		if(_l.size()<2)
			continue;
		m_mapHanZi2PY[_l[0].at(0)] = _l.mid(1);
	}

	return m_mapHanZi2PY.size();
}

QList<QList<QChar>> CHz2Py::getHzFirstLetter( const QString& qsWord )
{
	QList<QList<QChar>> listChar;

	foreach(const QChar& _c,qsWord)
	{
		ushort uni = _c.unicode();
		QHash<QChar,QChar> _chs;
		if(uni >= 0x4E00 && uni <= 0x9FA5)
		{
			if(m_mapHanZi2PY.contains(_c))
			{
				foreach(const QString& _str, m_mapHanZi2PY[_c])
				{
					if(_str.size()<0)
						continue;
					QChar _cc = _str[0];
					_chs[_cc] = _cc;
				}
			}
			else
			{
				_chs[_c.toLower()] = _c.toLower();
			}
		}
		else
		{
			_chs[_c.toLower()] = _c.toLower();
		}
		listChar.push_back(_chs.keys());
	}

	return listChar;
}

