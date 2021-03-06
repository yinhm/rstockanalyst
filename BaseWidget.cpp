/************************************************************************/
/* 文件名称：BaseWidget.cpp
/* 创建时间：2012-11-16 10:59
/*
/* 描    述：所有绘制窗口的基类，支持拆分等操作
/************************************************************************/
#include "StdAfx.h"
#include "BaseWidget.h"
#include "KLineWidget.h"
#include "MarketTrendWidget.h"
#include "ColorBlockWidget.h"
#include "BColorBlockWidget.h"
#include "StockInfoWidget.h"
#include "RadarWidget.h"
#include "StockDealWidget.h"
#include "AllStockWidget.h"
#include "FlashWidget.h"
#include "KeyWizard.h"

CBaseWidget* CBaseWidget::createBaseWidget( CBaseWidget* parent/*=0*/, RWidgetType type/*=Basic*/ )
{
	switch(type)
	{
	case WidgetBasic:				//基础图
		return new CBaseWidget(parent);
		break;
	case WidgetKLine:					//K线图
		return new CKLineWidget(parent);
		break;
	case WidgetMarketTrend:			//市场行情图
		return new CMarketTrendWidget(parent);
		break;
	case WidgetSColorBlock:
		return new CColorBlockWidget(parent);
		break;
	case WidgetBColorBlock:
		return new CBColorBlockWidget(parent);
		break;
	case WidgetStockInfo:
		return new CStockInfoWidget(parent);
		break;
	case WidgetRadar:
		return new CRadarWidget(parent);
		break;
	case WidgetStockDeal:
		return new CStockDealWidget(parent);
		break;
	case WidgetAllStock:
		return new CAllStockWidget(parent);
		break;
	case WidgetFlash:
		return new CFlashWidget(parent);
		break;
	}

	return new CBaseWidget(parent);
}

CBaseWidget::CBaseWidget( CBaseWidget* parent /*= 0*/, RWidgetType type /*= Basic*/ )
	: QWidget(parent)
	, m_pParent(parent)
	, m_type(type)
	, m_bClearMode(false)
{
	setMouseTracking(true);
	//将Splitter放入到Layout中，这样使其充满整个窗口
	m_pLayout = new QVBoxLayout();
	m_pLayout->setMargin(0);		//更改此处可设置Panel的边框大小
	m_pSplitter = new QSplitter(Qt::Vertical,this);
	m_pSplitter->setHandleWidth(1);
	m_pSplitter->setFrameShadow(QFrame::Sunken);
	m_pSplitter->setOrientation(Qt::Horizontal);
	m_pSplitter->setMouseTracking(true);
	m_pLayout->addWidget(m_pSplitter);
	setLayout(m_pLayout);

	if(m_pParent == 0 && m_type==WidgetBasic)
	{
		//确保所有可操作到的窗口都是有父窗口的
		m_pSplitter->addWidget(new CBaseWidget(this));
	}

	{
		//初始化显示周期的快速查找表
		m_listWidget.push_back(RWidgetOpData(WidgetBasic,"vb","基础窗口"));
		m_listWidget.push_back(RWidgetOpData(WidgetKLine,"vk","K线图"));
		m_listWidget.push_back(RWidgetOpData(WidgetMarketTrend,"vm","市场行情图"));
		m_listWidget.push_back(RWidgetOpData(WidgetSColorBlock,"vs","股票色块图"));
		m_listWidget.push_back(RWidgetOpData(WidgetBColorBlock,"vl","板块色块图"));
		m_listWidget.push_back(RWidgetOpData(WidgetStockInfo,"vi","行情信息"));
		m_listWidget.push_back(RWidgetOpData(WidgetRadar,"vr","雷达监视"));
		m_listWidget.push_back(RWidgetOpData(WidgetStockDeal,"vd","成交详情图"));
		m_listWidget.push_back(RWidgetOpData(WidgetFlash,"vf","快速切换图"));
	}

	{
		m_listOperate.push_back(RWidgetOpData(InsertLeft,"il","左插入窗口"));
		m_listOperate.push_back(RWidgetOpData(InsertRight,"ir","右插入窗口"));
		m_listOperate.push_back(RWidgetOpData(InsertTop,"it","上插入窗口"));
		m_listOperate.push_back(RWidgetOpData(InsertBottom,"ib","下插入窗口"));
		m_listOperate.push_back(RWidgetOpData(InsertRemove,"id","删除窗口"));
	}
	setFocusPolicy(Qt::WheelFocus);

	initMenu();			//初始化菜单
}


CBaseWidget::~CBaseWidget(void)
{
	clearChildren();
	delete m_pActRelate;
	delete m_pMenu;
	delete m_pSplitter;
}

void CBaseWidget::initMenu()
{
	m_pMenu = new QMenu(tr("定制版面菜单"));
	{
		//设置版面类型
		QMenu* pMenuType = m_pMenu->addMenu(tr("设置版面类型"));
		{
			foreach(const RWidgetOpData& _d,m_listWidget)
			{
				pMenuType->addAction(_d.desc,this,SLOT(onResetWidget()))->setData(_d.value);
			}
		}
		m_pMenu->addSeparator();
	}

	{
		//设置插入方式
		foreach(const RWidgetOpData& _d,m_listOperate)
		{
			m_pMenu->addAction(_d.desc,this,SLOT(onInsertWidget()))->setData(_d.value);
		}
		m_pMenu->addSeparator();
		m_pActRelate = new QAction(tr("和其它窗口关联"),m_pMenu);
		m_pMenu->addAction(m_pActRelate);
		m_pActRelate->setCheckable(true);
		m_pActRelate->setChecked(true);
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

QList<CBaseWidget*> CBaseWidget::getChildren()
{
	QList<CBaseWidget*> list;
	for(int i=0;i<m_pSplitter->count();++i)
	{
		list.push_back(reinterpret_cast<CBaseWidget*>(m_pSplitter->widget(i)));
	}
	return list;
}

void CBaseWidget::clearChildren()
{
	while(m_pSplitter->count()>0)
	{
		CBaseWidget* pPanel = static_cast<CBaseWidget*>(m_pSplitter->widget(0));
		if(pPanel)
			delete pPanel;
	}
}

int CBaseWidget::getSize()
{
	CBaseWidget* pParent = getParent();
	if(!pParent)
		return 100;
	QSplitter* pParentSplitter = pParent->getSplitter();
	if(!pParentSplitter)
		return 100;

	int iTotal = 0;
	int iSize = 0;
	QList<int> sizes = pParentSplitter->sizes();
	if(sizes.size()<2)
		return 100;

	for(int i=0;i<sizes.size();++i)
	{
		if(pParentSplitter->widget(i)==this)
			iSize = sizes[i];
		iTotal = iTotal + sizes[i];
	}

	return (iSize*100)/iTotal;
}

int CBaseWidget::getWidgetIndex( CBaseWidget* widget ) const
{
	return m_pSplitter->indexOf(widget);
}

void CBaseWidget::replaceWidget( int index, CBaseWidget* widget )
{
	QList<int> sizes = m_pSplitter->sizes();
	CBaseWidget* pWidgetPre = reinterpret_cast<CBaseWidget*>(m_pSplitter->widget(index));
	pWidgetPre->setParent(NULL);
	pWidgetPre->deleteLater();
	m_pSplitter->insertWidget(index,widget);
	QApplication::flush();
	m_pSplitter->setSizes(sizes);
	
	//设置新窗口获取焦点
	if(::GetForegroundWindow() != widget->winId())
		::SetForegroundWindow(widget->winId());
}

bool CBaseWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	//读取Widget信息
	if(!eleWidget.isElement())
		return false;

	//获取名称
	QDomElement eleName = eleWidget.firstChildElement("name");
	if(eleName.isElement())
		setWidgetName(eleName.text());

	//清爽模式
	QDomElement eleClearMode = eleWidget.firstChildElement("ClearMode");
	if(eleClearMode.isElement())
		m_bClearMode = static_cast<bool>(eleClearMode.text().toInt());

	//获取控件的排列方式
	QDomElement eleAlign = eleWidget.firstChildElement("align");
	if(eleAlign.isElement())
		m_pSplitter->setOrientation(static_cast<Qt::Orientation>(eleAlign.text().toInt()));

	//获取关联方式
	QDomElement eleRelate = eleWidget.firstChildElement("relate");
	if(eleRelate.isElement())
		m_pActRelate->setChecked(eleRelate.text().toInt());

	QList<int> sizes;
	QDomElement eleChild = eleWidget.firstChildElement("widget");
	while(eleChild.isElement())
	{
		QDomElement eleChildSize = eleChild.firstChildElement("size");
		if(eleChildSize.isElement())
			sizes.push_back(eleChildSize.text().toInt());

		QDomElement eleChildType = eleChild.firstChildElement("type");
		RWidgetType typeChild = WidgetBasic;
		if(eleChildType.isElement())
			typeChild = static_cast<RWidgetType>(eleChildType.text().toInt());

		CBaseWidget* pWidget = createBaseWidget(this,typeChild);
		m_pSplitter->addWidget(pWidget);
		pWidget->loadPanelInfo(eleChild);

		eleChild = eleChild.nextSiblingElement("widget");
	}
	m_pSplitter->setSizes(sizes);

	return true;
}

bool CBaseWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	//保存Panel信息
	if(!eleWidget.isElement())
		return false;

	//获取名称
	QDomElement eleName = doc.createElement("name");
	eleName.appendChild(doc.createTextNode(getWidgetName()));
	eleWidget.appendChild(eleName);

	//清爽模式
	QDomElement eleClearMode = doc.createElement("ClearMode");
	eleClearMode.appendChild(doc.createTextNode(QString("%1").arg(static_cast<int>(m_bClearMode))));
	eleWidget.appendChild(eleClearMode);

	//获取控件类型
	QDomElement eleType = doc.createElement("type");
	eleType.appendChild(doc.createTextNode(QString("%1").arg(m_type)));
	eleWidget.appendChild(eleType);

	//获取关联方式
	QDomElement eleRelate = doc.createElement("relate");
	eleRelate.appendChild(doc.createTextNode(QString("%1").arg(isRelate())));
	eleWidget.appendChild(eleRelate);

	//获取控件中Splitter的排列方式
	QDomElement eleAlgin = doc.createElement("align");
	eleAlgin.appendChild(doc.createTextNode(QString("%1").arg(m_pSplitter->orientation())));
	eleWidget.appendChild(eleAlgin);

	//获取控件在父窗口中的大小
	QDomElement eleSize = doc.createElement("size");
	eleSize.appendChild(doc.createTextNode(QString("%1").arg(getSize())));
	eleWidget.appendChild(eleSize);

	//加载子Panel
	QList<CBaseWidget*> listChildren = getChildren();
	foreach(CBaseWidget* pChild,listChildren)
	{
		QDomElement eleChild = doc.createElement("widget");
		eleWidget.appendChild(eleChild);
		pChild->savePanelInfo(doc,eleChild);
	}

	return true;
}

void CBaseWidget::setStockCode( const QString& code )
{
	QList<CBaseWidget*> children = getChildren();
	foreach(CBaseWidget* p,children)
	{
		if(p->isRelate())
			p->setStockCode(code);
	}
}

void CBaseWidget::setBlock( const QString& block )
{
	QList<CBaseWidget*> children = getChildren();
	foreach(CBaseWidget* p,children)
	{
		if(p->isRelate())
			p->setBlock(block);
	}
}

void CBaseWidget::onBlockClicked( CBlockInfoItem* /*pBlock*/,int /*iCmd*/ )
{

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
	QMenu* pMenuCustom = getCustomMenu();
	if(pMenuCustom)
		pMenuCustom->exec(e->globalPos());
	else
		m_pMenu->exec(e->globalPos());
	return e->accept();
}

void CBaseWidget::keyPressEvent( QKeyEvent* e )
{
	//屏蔽不需要的按键
	int iKey = e->key();
	if( Qt::Key_Escape == iKey || Qt::Key_Backspace == iKey)
		return;

	QString _t = e->text().trimmed();
	if(!_t.isEmpty())
	{
		//弹出键盘精灵对话框
		CKeyWizard::getKeyWizard()->showWizard(this,_t);
	}
}

void CBaseWidget::mousePressEvent( QMouseEvent* /*e*/ )
{
	CKeyWizard* pWizard = CKeyWizard::getKeyWizard();
	if(pWizard->isVisible())
	{
		if(pWizard->getCurrentWidget() == this)
		{
			pWizard->showWizard(this);
		}
		else
		{
			pWizard->hide();
		}
	}
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
		m_pParent->realignSplitter();
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
		m_pParent->realignSplitter();
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
		m_pParent->realignSplitter();
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
		m_pParent->realignSplitter();
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

void CBaseWidget::onInsertWidget()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	insertWidget(static_cast<RWidgetInsert>(pAct->data().toInt()));
}

void CBaseWidget::insertWidget(RWidgetInsert _i)
{
	switch (_i)
	{
	case InsertLeft:
		onLeftInsert();
		break;
	case InsertRight:
		onRightInsert();
		break;
	case InsertTop:
		onTopInsert();
		break;
	case InsertBottom:
		onBottomInsert();
		break;
	case InsertRemove:
		deleteLater();
		break;
	default:
		break;
	}
}

void CBaseWidget::onResetWidget()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	//获取并重置窗口类型
	setWidgetType(static_cast<RWidgetType>(pAct->data().toInt()));
}

void CBaseWidget::setWidgetType(RWidgetType _t)
{
	int iIndex = m_pParent->getWidgetIndex(this);						//获取当前窗口所在的索引
	if(iIndex>=0)
	{
		m_pParent->replaceWidget(iIndex,createBaseWidget(m_pParent,_t));	//替换窗口
	}
}

void CBaseWidget::getKeyWizData( const QString& keyword,QList<KeyWizData*>& listRet )
{
	foreach(const RWidgetOpData& _d,m_listWidget)
	{
		if(_d.key.indexOf(keyword)>-1)
		{
			KeyWizData* pData = new KeyWizData;
			pData->cmd = CKeyWizard::CmdWidget;
			pData->arg = (void*)_d.value;
			pData->desc = _d.desc;
			listRet.push_back(pData);
			if(listRet.size()>20)
				return;
		}
	}
	
	foreach(const RWidgetOpData& _d,m_listOperate)
	{
		if(_d.key.indexOf(keyword)>-1)
		{
			KeyWizData* pData = new KeyWizData;
			pData->cmd = CKeyWizard::CmdInsert;
			pData->arg = (void*)_d.value;
			pData->desc = _d.desc;
			listRet.push_back(pData);
			if(listRet.size()>20)
				return;
		}
	}
}

void CBaseWidget::keyWizEntered( KeyWizData* pData )
{
	if(pData->cmd == CKeyWizard::CmdWidget)
	{
		setWidgetType(static_cast<RWidgetType>((int)(pData->arg)));
		return;
	}
	else if(pData->cmd == CKeyWizard::CmdInsert)
	{
		insertWidget(static_cast<RWidgetInsert>((int)(pData->arg)));
		return;
	}
}

