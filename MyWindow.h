#pragma once
#include <QtGui>


class CMainWindow : public QWidget
{
	Q_OBJECT
public:
	CMainWindow();
protected slots:
	void onBtnOK();

private:
	QLineEdit editRegExp;
	QTextEdit editContent;
	QTextEdit editMatch;
	QPushButton btnOK;
};
