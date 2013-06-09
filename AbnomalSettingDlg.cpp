#include "StdAfx.h"
#include "AbnomalSettingDlg.h"

CAbnomalSettingDlg::CAbnomalSettingDlg( const QMap<RAbnomalType,float>& map,QWidget* parent /*= 0*/ )
	: QDialog(parent)
	, m_mapAbnomal(map)
{
	initDlg();
}


CAbnomalSettingDlg::~CAbnomalSettingDlg(void)
{
}

void CAbnomalSettingDlg::initDlg()
{
	m_ckHighIncrease.setText(tr("涨幅>="));
	m_ckHighTurnRatio.setText(tr("换手率>="));
	m_ckHighVolumeRatio.setText(tr("量比>="));
	if(m_mapAbnomal.contains(HighIncrease))
	{
		m_ckHighIncrease.setCheckState(Qt::Checked);
		m_editHighIncrease.setText(QString("%1").arg(m_mapAbnomal[HighIncrease]));
	}
	else
	{
		m_editHighIncrease.setText("0.05");
	}

	if(m_mapAbnomal.contains(HighTurnRatio))
	{
		m_ckHighTurnRatio.setCheckState(Qt::Checked);
		m_editHighTurnRatio.setText(QString("%1").arg(m_mapAbnomal[HighTurnRatio]));
	}
	else
	{
		m_editHighTurnRatio.setText("0.05");
	}

	if(m_mapAbnomal.contains(HighVolumeRatio))
	{
		m_ckHighVolumeRatio.setCheckState(Qt::Checked);
		m_editHighVolumeRatio.setText(QString("%1").arg(m_mapAbnomal[HighVolumeRatio]));
	}
	else
	{
		m_editHighVolumeRatio.setText("0.05");
	}


	QPushButton* pBtnOk = new QPushButton(tr("确定"),this);
	QPushButton* pBtnCancel = new QPushButton(tr("取消"),this);
	connect(pBtnOk,SIGNAL(clicked()),this,SLOT(onBtnOk()));
	connect(pBtnCancel,SIGNAL(clicked()),this,SLOT(onBtnCancel()));

	QGridLayout* pLayout = new QGridLayout(this);
	pLayout->addWidget(&m_ckHighIncrease,0,0);
	pLayout->addWidget(&m_ckHighTurnRatio,1,0);
	pLayout->addWidget(&m_ckHighVolumeRatio,2,0);

	pLayout->addWidget(&m_editHighIncrease,0,1);
	pLayout->addWidget(&m_editHighTurnRatio,1,1);
	pLayout->addWidget(&m_editHighVolumeRatio,2,1);

	pLayout->addWidget(new QLabel("%"),0,2);
	pLayout->addWidget(new QLabel("%"),1,2);
	pLayout->addWidget(new QLabel("%"),2,2);

	QHBoxLayout* pBtnLayout = new QHBoxLayout(this);
	pBtnLayout->addWidget(pBtnOk,1);
	pBtnLayout->addWidget(pBtnCancel,1);

	pLayout->addLayout(pBtnLayout,3,0,1,2);
	setLayout(pLayout);
}

void CAbnomalSettingDlg::onBtnOk()
{
	m_mapAbnomal.clear();
	if(m_ckHighIncrease.isChecked())
	{
		m_mapAbnomal[HighIncrease] = m_editHighIncrease.text().toFloat();
	}

	if(m_ckHighTurnRatio.isChecked())
	{
		m_mapAbnomal[HighTurnRatio] = m_editHighTurnRatio.text().toFloat();
	}

	if(m_ckHighVolumeRatio.isChecked())
	{
		m_mapAbnomal[HighVolumeRatio] = m_editHighVolumeRatio.text().toFloat();
	}

	accept();
}

void CAbnomalSettingDlg::onBtnCancel()
{
	reject();
}

QMap<RAbnomalType,float> CAbnomalSettingDlg::getAbnomalMap()
{
	return m_mapAbnomal;
}
