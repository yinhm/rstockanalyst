#ifndef _Hz_2_Py_H_
#define _Hz_2_Py_H_
#include <QtCore>

class CHz2Py
{
public:
	/*初始化汉字->拼音对照表*/
	static int initHz2PyTable(const QString& qsFile);
	/*获取汉字的首字母（小写）*/
	static QList<QList<QChar>> getHzFirstLetter(const QString& qsWord);

private:
	static QMap<QChar,QList<QString>> m_mapHanZi2PY;				//汉字拼音对照表
};

#endif	//_Hz_2_Py_H_