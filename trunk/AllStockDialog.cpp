#include "StdAfx.h"
#include "AllStockDialog.h"
#include "DataEngine.h"

CAllStockDialog::CAllStockDialog( QWidget* parent /*= 0*/ )
	: QDialog(parent)
{
	QSplitter* pSplitter = new QSplitter(Qt::Vertical,this);
	QVBoxLayout* pLayout = new QVBoxLayout(this);
	pLayout->addWidget(pSplitter);
	pLayout->setSpacing(0);
	setLayout(pLayout);

	m_pWidgetAll = new CAllStockWidget(0);
	pSplitter->addWidget(m_pWidgetAll);

	QWidget* pWidgetCtrl = new QWidget(pSplitter);
	pSplitter->addWidget(pWidgetCtrl);

	pSplitter->setSizes(QList<int>()<<70<<30);

	QHBoxLayout* pHLayout = new QHBoxLayout(pWidgetCtrl);
	pHLayout->setSpacing(0);
	pWidgetCtrl->setLayout(pHLayout);

	m_pWidgetKLine = new CKLineWidget(0);
	m_pWidgetKLine->setStockCode("1A0001");
	connect(m_pWidgetAll,SIGNAL(stockFocus(CStockInfoItem*)),this,SLOT(onStockFocusChanged(CStockInfoItem*)));
	pHLayout->addWidget(m_pWidgetKLine,70);

	{
		QGridLayout* pGLayout = new QGridLayout();
		{
			//初始化周期按钮
			QPushButton* pBtn5Min = new QPushButton(pWidgetCtrl);
			pBtn5Min->setText(tr("5分"));
			connect(pBtn5Min,SIGNAL(clicked()),this,SLOT(onCircle5Min()));
			pGLayout->addWidget(pBtn5Min,0,0,1,1);

			QPushButton* pBtnDay = new QPushButton(pWidgetCtrl);
			pBtnDay->setText(tr("日线"));
			connect(pBtnDay,SIGNAL(clicked()),this,SLOT(onCircleDay()));
			pGLayout->addWidget(pBtnDay,0,1,1,1);

			QPushButton* pBtnWeek = new QPushButton(pWidgetCtrl);
			pBtnWeek->setText(tr("周线"));
			connect(pBtnWeek,SIGNAL(clicked()),this,SLOT(onCircleWeek()));
			pGLayout->addWidget(pBtnWeek,0,2,1,1);

			QPushButton* pBtnMonth = new QPushButton(pWidgetCtrl);
			pBtnMonth->setText(tr("月线"));
			connect(pBtnMonth,SIGNAL(clicked()),this,SLOT(onCircleMonth()));
			pGLayout->addWidget(pBtnMonth,0,3,1,1);
		}
		{
			//初始化排序按钮
//			SortByCode = 1,		//按股票代码排序
//			SortByZGB,			//按总股本排序
//			SortByIncrease,		//按涨幅排序
//			SortByTurnRatio,	//按换手率排序
//			SortByVolumeRatio,	//按量比排序

			QPushButton* pBtnStCode = new QPushButton(pWidgetCtrl);
			pBtnStCode->setText(tr("代码"));
			connect(pBtnStCode,SIGNAL(clicked()),this,SLOT(onSortCode()));
			pGLayout->addWidget(pBtnStCode,1,0,1,1);

			QPushButton* pBtnStZGB = new QPushButton(pWidgetCtrl);
			pBtnStZGB->setText(tr("总股本"));
			connect(pBtnStZGB,SIGNAL(clicked()),this,SLOT(onSortZGB()));
			pGLayout->addWidget(pBtnStZGB,1,1,1,1);

			QPushButton* pBtnStInc = new QPushButton(pWidgetCtrl);
			pBtnStInc->setText(tr("涨幅"));
			connect(pBtnStInc,SIGNAL(clicked()),this,SLOT(onSortInc()));
			pGLayout->addWidget(pBtnStInc,1,2,1,1);

			QPushButton* pBtnStTurnRatio = new QPushButton(pWidgetCtrl);
			pBtnStTurnRatio->setText(tr("换手率"));
			connect(pBtnStTurnRatio,SIGNAL(clicked()),this,SLOT(onSortTurnRatio()));
			pGLayout->addWidget(pBtnStTurnRatio,1,3,1,1);

			QPushButton* pBtnStVolumeRatio = new QPushButton(pWidgetCtrl);
			pBtnStVolumeRatio->setText(tr("量比"));
			connect(pBtnStVolumeRatio,SIGNAL(clicked()),this,SLOT(onSortVolumeRatio()));
			pGLayout->addWidget(pBtnStVolumeRatio,1,4,1,1);
		}

		pHLayout->addLayout(pGLayout,30);
	}

	showMaximized();
}

CAllStockDialog::~CAllStockDialog(void)
{
	if(m_pWidgetAll)
		delete m_pWidgetAll;
	if(m_pWidgetKLine)
		delete m_pWidgetKLine;
}

void CAllStockDialog::onCircle5Min()
{
	m_pWidgetAll->setCircle(Min5);
}

void CAllStockDialog::onCircleDay()
{
	m_pWidgetAll->setCircle(Day);
}

void CAllStockDialog::onCircleWeek()
{
	m_pWidgetAll->setCircle(Week);
}

void CAllStockDialog::onCircleMonth()
{
	m_pWidgetAll->setCircle(Month);
}

void CAllStockDialog::onSortCode()
{
	m_pWidgetAll->setSortType(CAllStockWidget::SortByCode);
}

void CAllStockDialog::onSortZGB()
{
	m_pWidgetAll->setSortType(CAllStockWidget::SortByZGB);
}

void CAllStockDialog::onSortInc()
{
	m_pWidgetAll->setSortType(CAllStockWidget::SortByIncrease);
}

void CAllStockDialog::onSortTurnRatio()
{
	m_pWidgetAll->setSortType(CAllStockWidget::SortByTurnRatio);
}

void CAllStockDialog::onSortVolumeRatio()
{
	m_pWidgetAll->setSortType(CAllStockWidget::SortByVolumeRatio);
}

void CAllStockDialog::onStockFocusChanged( CStockInfoItem* pStock )
{
	m_pWidgetKLine->setStockCode(pStock->getOnly());
}
