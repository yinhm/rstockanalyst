/************************************************************************/
/* 文件名称：BaseWidget.cpp
/* 创建时间：2012-11-16 10:59
/*
/* 描    述：所有绘制窗口的基类，支持拆分等操作
/************************************************************************/
#include "StdAfx.h"
#include "BaseWidget.h"
#include "KLineWidget.h"

CBaseWidget::CBaseWidget( CBaseWidget* parent /*= 0*/, WidgetType type /*= Basic*/ )
	: QWidget(parent)
	, m_pParent(parent)
	, m_type(type)
{
	initMenu();
	//将Splitter放入到Layout中，这样使其充满整个窗口
	QVBoxLayout* pLayout = new QVBoxLayout();
	pLayout->setMargin(0);		//更改此处可设置Panel的边框大小
	m_pSplitter = new QSplitter(Qt::Vertical,this);
	m_pSplitter->setHandleWidth(1);
	m_pSplitter->setFrameShadow(QFrame::Sunken);
	m_pSplitter->setOrientation(Qt::Horizontal);
	pLayout->addWidget(m_pSplitter);
	setLayout(pLayout);

	if(m_pParent == 0 && m_type==Basic)
	{
		//确保所有可操作到的窗口都是有父窗口的
		m_pSplitter->addWidget(new CBaseWidget(this));
	}
}


CBaseWidget::~CBaseWidget(void)
{
	delete m_pMenu;
	delete m_pSplitter;
}

void CBaseWidget::initMenu()
{
	m_pMenu = new QMenu(tr("定制版面菜单"));
	{
		//设置版面类型
		QMenu* pMenuType = m_pMenu->addMenu(tr("设置版面类型"));
		pMenuType->addAction(tr("基础窗口"),this,SLOT(onSetNormalWidget()));
		pMenuType->addAction(tr("K线图"),this,SLOT(onSetKLineWidget()));
		m_pMenu->addSeparator();
	}

	{
		//设置插入方式
		m_pMenu->addAction(tr("左插入"),this,SLOT(onLeftInsert()));
		m_pMenu->addAction(tr("右插入"),this,SLOT(onRightInsert()));
		m_pMenu->addAction(tr("上插入"),this,SLOT(onTopInsert()));
		m_pMenu->addAction(tr("下插入"),this,SLOT(onBottomInsert()));
		m_pMenu->addSeparator();
	}
}

void CBaseWidget::realignSplitter()
{
	QList<int> listSize;
	listSize.push_back(-1);
	m_pSplitter->setSizes(listSize);
}

void CBaseWidget::resetParent( CBaseWidget* parent )
{
	m_pParent = parent;
	QWidget::setParent(parent);
}

int CBaseWidget::getWidgetIndex( CBaseWidget* widget ) const
{
	return m_pSplitter->indexOf(widget);
}

void CBaseWidget::replaceWidget( int index, CBaseWidget* widget )
{
	CBaseWidget* pWidgetPre = reinterpret_cast<CBaseWidget*>(m_pSplitter->widget(index));
	pWidgetPre->deleteLater();
	m_pSplitter->insertWidget(index,widget);
}

bool CBaseWidget::loadPanelInfo( const QDomElement& elePanel )
{

	return true;
}

bool CBaseWidget::savePanelInfo( QDomDocument& doc,QDomElement& elePanel )
{

	return true;
}

void CBaseWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	p.fillRect(this->rect(),QColor(0,0,0));
	p.setPen(QColor(255,255,255));
	p.drawText(this->rect(),Qt::AlignCenter,QString(tr("单击右键选择版面类型")));
}

void CBaseWidget::contextMenuEvent( QContextMenuEvent* e )
{
	if(m_pParent)
	{
		QMenu* pMenuCustom = getCustomMenu();
		if(pMenuCustom)
			pMenuCustom->exec(e->globalPos());
		else
			m_pMenu->exec(e->globalPos());
	}
	return e->accept();
}

void CBaseWidget::onLeftInsert()
{
	//左插入
	QSplitter* pSplitterParent = m_pParent->getSplitter();		//父窗口的Splitter指针
	int iIndex = pSplitterParent->indexOf(this);				//所在的索引号
	if(iIndex<0)
		return;

	if((pSplitterParent->orientation() == Qt::Horizontal) ||
		(pSplitterParent->count() == 1))
	{
		//水平方向的Splitter 或者 垂直方向，但只有一个子窗口
		pSplitterParent->setOrientation(Qt::Horizontal);
		pSplitterParent->insertWidget(iIndex,new CBaseWidget(m_pParent));
	}
	else														//垂直方向的，并已经进行了垂直布局
	{
		CBaseWidget* pWidgetParent = new CBaseWidget(m_pParent);
		pSplitterParent->insertWidget(iIndex,pWidgetParent);
		resetParent(pWidgetParent);

		pWidgetParent->getSplitter()->setOrientation(Qt::Horizontal);
		pWidgetParent->getSplitter()->addWidget(new CBaseWidget(pWidgetParent));
		pWidgetParent->getSplitter()->addWidget(this);

		pWidgetParent->realignSplitter();
	}
}

void CBaseWidget::onRightInsert()
{
	//右插入
	QSplitter* pSplitterParent = m_pParent->getSplitter();		//父窗口的Splitter指针
	int iIndex = pSplitterParent->indexOf(this);				//所在的索引号
	if(iIndex<0)
		return;

	if((pSplitterParent->orientation() == Qt::Horizontal) ||
		(pSplitterParent->count() == 1))
	{
		//水平方向的Splitter 或者 垂直方向，但只有一个子窗口
		pSplitterParent->setOrientation(Qt::Horizontal);
		pSplitterParent->insertWidget(iIndex+1,new CBaseWidget(m_pParent));
	}
	else														//垂直方向的，并已经进行了垂直布局
	{
		CBaseWidget* pWidgetParent = new CBaseWidget(m_pParent);
		pSplitterParent->insertWidget(iIndex,pWidgetParent);
		resetParent(pWidgetParent);

		pWidgetParent->getSplitter()->setOrientation(Qt::Horizontal);
		pWidgetParent->getSplitter()->addWidget(this);
		pWidgetParent->getSplitter()->addWidget(new CBaseWidget(pWidgetParent));

		pWidgetParent->realignSplitter();
	}
}

void CBaseWidget::onTopInsert()
{
	//上插入
	QSplitter* pSplitterParent = m_pParent->getSplitter();		//父窗口的Splitter指针
	int iIndex = pSplitterParent->indexOf(this);				//所在的索引号
	if(iIndex<0)
		return;

	if((pSplitterParent->orientation() == Qt::Vertical) ||
		(pSplitterParent->count() == 1))
	{
		//水平方向的Splitter 或者 垂直方向，但只有一个子窗口
		pSplitterParent->setOrientation(Qt::Vertical);
		pSplitterParent->insertWidget(iIndex,new CBaseWidget(m_pParent));
	}
	else														//垂直方向的，并已经进行了垂直布局
	{
		CBaseWidget* pWidgetParent = new CBaseWidget(m_pParent);
		pSplitterParent->insertWidget(iIndex,pWidgetParent);
		resetParent(pWidgetParent);

		pWidgetParent->getSplitter()->setOrientation(Qt::Vertical);
		pWidgetParent->getSplitter()->addWidget(new CBaseWidget(pWidgetParent));
		pWidgetParent->getSplitter()->addWidget(this);

		pWidgetParent->realignSplitter();
	}
}

void CBaseWidget::onBottomInsert()
{
	//下插入
	QSplitter* pSplitterParent = m_pParent->getSplitter();		//父窗口的Splitter指针
	int iIndex = pSplitterParent->indexOf(this);				//所在的索引号
	if(iIndex<0)
		return;

	if((pSplitterParent->orientation() == Qt::Vertical) ||
		(pSplitterParent->count() == 1))
	{
		//水平方向的Splitter 或者 垂直方向，但只有一个子窗口
		pSplitterParent->setOrientation(Qt::Vertical);
		pSplitterParent->insertWidget(iIndex+1,new CBaseWidget(m_pParent));
	}
	else														//垂直方向的，并已经进行了垂直布局
	{
		CBaseWidget* pWidgetParent = new CBaseWidget(m_pParent);
		pSplitterParent->insertWidget(iIndex,pWidgetParent);
		resetParent(pWidgetParent);

		pWidgetParent->getSplitter()->setOrientation(Qt::Vertical);
		pWidgetParent->getSplitter()->addWidget(this);
		pWidgetParent->getSplitter()->addWidget(new CBaseWidget(pWidgetParent));

		pWidgetParent->realignSplitter();
	}
}

void CBaseWidget::onSetNormalWidget()
{
	int iIndex = m_pParent->getWidgetIndex(this);
	if(iIndex>=0)
	{
		m_pParent->replaceWidget(iIndex,new CBaseWidget(m_pParent));
	}
}

void CBaseWidget::onSetKLineWidget()
{
	int iIndex = m_pParent->getWidgetIndex(this);
	if(iIndex>=0)
	{
		m_pParent->replaceWidget(iIndex,new CKLineWidget(m_pParent));
	}
}
