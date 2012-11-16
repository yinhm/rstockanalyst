#include "StdAfx.h"
#include "BaseMarketWidget.h"
#include "DataEngine.h"
#include "KLineWidget.h"

CTabButton::CTabButton( const QString& text,QWidget* parent /*= 0*/ )
	: QLabel(text,parent)
	, bButtonDown(false)
{

}

void CTabButton::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRect rtClient= this->rect();
	p.fillRect(this->rect(),QColor(0,0,0));

	QPainterPath path;
	path.lineTo(5,rtClient.bottom());
	path.lineTo(rtClient.right()-5,rtClient.bottom());
	path.lineTo(rtClient.right(),0);

	if(bButtonDown)
		p.fillPath(path,QColor(0,0,255));

	p.setPen(QColor(255,0,0));
	p.drawPath(path);

	p.setPen(QColor(255,255,255));
	p.drawText(rtClient,Qt::AlignCenter,this->text());
}

void CTabButton::mousePressEvent( QMouseEvent* )
{
	emit clicked();
}

CTabButtonList::CTabButtonList( QWidget* parent /*= 0*/ )
	: QWidget(parent)
	, pDownBtn(NULL)
{
	pLayout=new QHBoxLayout;
	pLayout->setMargin(0);
	pLayout->setSpacing(0);
	setLayout(pLayout);
}

void CTabButtonList::addTabButton( const QString& text,QWidget* widget )
{
	CTabButton* pBtn = new CTabButton(text);
	connect(pBtn,SIGNAL(clicked()),this,SLOT(tabBtnClicked()));
	if(mapButtons.size()==0)
	{
		pDownBtn = pBtn;
		pBtn->setBtnDown(true);
		widget->show();
	}
	else
	{
		widget->hide();
	}

	mapButtons[pBtn] = widget;
	pLayout->addWidget(pBtn);
}

void CTabButtonList::tabBtnClicked()
{
	CTabButton* pBtn = reinterpret_cast<CTabButton*>(sender());
	if(pBtn)
	{
		if(pDownBtn)
		{
			pDownBtn->setBtnDown(false);
			pDownBtn->update();
			if(mapButtons.contains(pDownBtn))
			{
				mapButtons[pDownBtn]->hide();
			}
		}
		pBtn->setBtnDown(true);
		pDownBtn = pBtn;
		pBtn->update();
		if(mapButtons.contains(pBtn))
			mapButtons[pBtn]->show();
	}
}


CBaseMarketWidget::CBaseMarketWidget()
	: QScrollArea()
{
	m_pViewSHA = new CBaseMarketTreeView(SH_MARKET_EX,this);
	m_pViewSZ = new CBaseMarketTreeView(SZ_MARKET_EX,this);

	connect(m_pViewSHA,SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(treeItemClicked(const QModelIndex&)));
	connect(m_pViewSZ,SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(treeItemClicked(const QModelIndex&)));

	connect(horizontalScrollBar(),SIGNAL(valueChanged(int)),m_pViewSHA,SLOT(horizonScolled(int)));
	connect(horizontalScrollBar(),SIGNAL(valueChanged(int)),m_pViewSZ,SLOT(horizonScolled(int)));

	QVBoxLayout* pLayout = new QVBoxLayout;
	pLayout->setMargin(0);

	viewport()->setLayout(pLayout);
	pLayout->addWidget(m_pViewSHA);
	pLayout->addWidget(m_pViewSZ);

//	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	CTabButtonList* pBtnList = new CTabButtonList();
	pBtnList->addTabButton("上海A股",m_pViewSHA);
	pBtnList->addTabButton("深圳指数",m_pViewSZ);
	this->addScrollBarWidget(pBtnList,Qt::AlignLeft);

	horizontalScrollBar()->setRange(0,20);
	horizontalScrollBar()->setPageStep(3);
	horizontalScrollBar()->setValue(0);
}

CBaseMarketWidget::~CBaseMarketWidget(void)
{
}

void CBaseMarketWidget::treeItemClicked( const QModelIndex& index )
{
	CBaseMarketTreeView* pTreeView = reinterpret_cast<CBaseMarketTreeView*>(sender());

	CStockInfoItem* pItem = reinterpret_cast<CStockInfoItem*>(pTreeView->getModel()->data(index,Qt::UserRole).toUInt());
	if(pItem)
	{
		QDialog dlg;
		QVBoxLayout layout;
		dlg.setLayout(&layout);

		CBaseWidget base(0);
		CKLineWidget widget(&base);
		base.replaceWidget(0,&widget);
		layout.addWidget(&base);
		widget.setStockCode(pItem->getCode());
		dlg.exec();
	}
}
