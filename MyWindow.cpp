#include "MyWindow.h"

CMainWindow::CMainWindow()
{
	QVBoxLayout* playout = new QVBoxLayout();

	playout->addWidget(&editRegExp);
	playout->addWidget(&editContent);
	playout->addWidget(&editMatch);
	playout->addWidget(&btnOK);
	btnOK.setText("OK");
	connect(&btnOK,SIGNAL(clicked()),this,SLOT(onBtnOK()));

	this->setLayout(playout);
}

void CMainWindow::onBtnOK()
{
	QRegExp rx(editRegExp.text(),Qt::CaseSensitive,QRegExp::W3CXmlSchema11);
//	rx.setMinimal(true);

	QString str(editContent.toPlainText());
	str.indexOf(rx);              // 0, position of the first match.
	editMatch.clear();
	QStringList listMatchs = rx.capturedTexts();
	qDebug()<<listMatchs.length();
	foreach(const QString& _v, listMatchs)
	{
		editMatch.insertPlainText(_v+"\r\n");
	}
}
