#include "StdAfx.h"
#include "FuncHelper.h"
#include <QtXml>

CFuncHelper::CFuncHelper( QWidget* parent /*= NULL*/ )
	: QDialog(parent)
{
	QPushButton* pBtnRefresh = new QPushButton(this);
	pBtnRefresh->setText("Ë¢ÐÂ");

	m_listFuncs.setMinimumWidth(100);
	QGridLayout* pLayout = new QGridLayout();
	pLayout->addWidget(&m_listFuncs,0,0,10,3);
	pLayout->addWidget(&m_labelFunc,0,4,10,7);
	pLayout->addWidget(pBtnRefresh,11,0,2,10);
	setLayout(pLayout);

	connect(&m_listFuncs,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
		this,SLOT(onFuncsItemChanged(QListWidgetItem*,QListWidgetItem*)));
	connect(pBtnRefresh,SIGNAL(clicked()),this,SLOT(onRefreshAll()));

	onRefreshAll();
}

CFuncHelper::~CFuncHelper(void)
{
}

void CFuncHelper::onRefreshAll()
{
	m_listFuncs.clear();

	QString qsPath = qApp->applicationDirPath() + "/Funcs.xml";
	QFile file(qsPath);
	if(!file.open(QFile::ReadOnly))
		return;

	QDomDocument doc;
	doc.setContent(file.readAll());
	file.close();

	QDomElement eleRoot = doc.firstChildElement("funcs");
	if(!eleRoot.isElement())
		return;

	QDomElement eleFunc = eleRoot.firstChildElement("func");
	while(eleFunc.isElement())
	{
		QDomElement eleName = eleFunc.firstChildElement("name");
		QDomElement eleDesc = eleFunc.firstChildElement("desc");
		if(eleName.isElement() && eleDesc.isElement())
		{
			QListWidgetItem* pItem = new QListWidgetItem(eleName.text());
			pItem->setData(Qt::UserRole,eleDesc.text());

			m_listFuncs.addItem(pItem);
		}

		eleFunc = eleFunc.nextSiblingElement("func");
	}
}

void CFuncHelper::onFuncsItemChanged( QListWidgetItem* current, QListWidgetItem* /*previous*/ )
{
	if(current)
	{
		m_labelFunc.setText(current->data(Qt::UserRole).toString());
	}
}
