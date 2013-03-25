/************************************************************************/
/* 文件名称：BaseBlockWidget.h
/* 创建时间：2013-01-14 13:38
/*
/* 描    述：用于色块图的抽象，继承自CCoordXBaseWidget
/*           创造性的使用不同的色块来表示股票的升降走势，从而在相同的区域中，
/*           展现更大的信息量
/************************************************************************/

#include "StdAfx.h"
#include "BaseBlockWidget.h"
#include "ColorManager.h"
#include "DataEngine.h"


CBaseBlockWidget::CBaseBlockWidget( CBaseWidget* parent /*= 0*/, RWidgetType type /*= CBaseWidget::Basic*/ )
	: CCoordXBaseWidget(parent,type)
	, m_typeBlock(BlockCircle)
	, m_pMenuColorMode(0)
	, m_pMenuBlockMode(0)
	, m_iCBHeight(16)
	, m_iCBWidth(16)
	, m_qsColorMode("")
{
	m_typeCircle = Min1;				//设置初始显示周期为1分钟
	//初始化菜单
	{
		//颜色显示模式菜单
		m_pMenuColorMode = m_pMenuCustom->addMenu("设置颜色模式");
		//设置显示方式（圆形/方块）
		m_pMenuBlockMode = m_pMenuCustom->addMenu("设置显示形状");
		m_pMenuBlockMode->addAction("圆形",this,SLOT(onSetBlockMode()))->setData(BlockCircle);
		m_pMenuBlockMode->addAction("方形",this,SLOT(onSetBlockMode()))->setData(BlockRect);
		//设置色块的大小
		m_pMenuCustom->addAction(tr("设置色块大小"),this,SLOT(onSetBlockSize()));

	}

	m_pMenuCustom->addSeparator();
}


CBaseBlockWidget::~CBaseBlockWidget(void)
{
	delete m_pMenuCustom;
}

bool CBaseBlockWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CCoordXBaseWidget::loadPanelInfo(eleWidget))
		return false;

	//色块的宽度
	if(eleWidget.hasAttribute("CBWidth"))
	{
		m_iCBWidth = eleWidget.attribute("CBWidth").toInt();
	}
	//色块的高度
	if(eleWidget.hasAttribute("CBHeight"))
	{
		m_iCBHeight = eleWidget.attribute("CBHeight").toInt();
	}

	//当前的颜色模式
	QDomElement eleColorMode = eleWidget.firstChildElement("color");
	if(eleColorMode.isElement())
	{
		m_qsColorMode = eleColorMode.text();
	}


	//当前的显示形状模式
	QDomElement eleBlockMode = eleWidget.firstChildElement("mode");
	if(eleBlockMode.isElement())
	{
		m_typeBlock = static_cast<CBaseBlockWidget::BlockMode>(eleBlockMode.text().toInt());
	}
	
	return true;
}

bool CBaseBlockWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CCoordXBaseWidget::savePanelInfo(doc,eleWidget))
		return false;

	eleWidget.setAttribute("CBWidth",m_iCBWidth);
	eleWidget.setAttribute("CBHeight",m_iCBHeight);

	//当前的颜色模式
	QDomElement eleColorMode = doc.createElement("color");
	eleColorMode.appendChild(doc.createTextNode(m_qsColorMode));
	eleWidget.appendChild(eleColorMode);

	//当前的显示形状模式
	QDomElement eleBlockMode = doc.createElement("mode");
	eleBlockMode.appendChild(doc.createTextNode(QString("%1").arg(m_typeBlock)));
	eleWidget.appendChild(eleBlockMode);

	return true;
}

void CBaseBlockWidget::updateData()
{
	return CCoordXBaseWidget::updateData();
}


void CBaseBlockWidget::setColorMode( const QString& mode )
{
	m_qsColorMode = mode;
	//QList<QAction*> listActs = m_pMenuColorMode->actions();
	//foreach(QAction* pAct,listActs)
	//{
	//	pAct->setChecked((pAct->data().toString() == mode) ? true : false);
	//}

	update();
}

void CBaseBlockWidget::onSetColorMode()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	setColorMode(pAct->data().toString());
}

void CBaseBlockWidget::onSetBlockMode()
{
	//设置当前的显示周期
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	m_typeBlock = static_cast<BlockMode>(pAct->data().toInt());
	update();
}

void CBaseBlockWidget::onSetBlockSize()
{
	//弹出设置色块大小的对话框，用来设置色块的大小
	QDialog dlg(this);
	QGridLayout layout(&dlg);
	QLabel label1(tr("宽度"),&dlg);
	QLabel label2(tr("高度"),&dlg);
	QLineEdit editW(&dlg);
	QLineEdit editH(&dlg);
	QPushButton btnOk(&dlg);
	dlg.setLayout(&layout);
	layout.addWidget(&label1,0,0,1,1);
	layout.addWidget(&label2,1,0,1,1);
	layout.addWidget(&editW,0,1,1,1);
	layout.addWidget(&editH,1,1,1,1);
	layout.addWidget(&btnOk,2,0,1,2);
	btnOk.setText(tr("确定"));
	QIntValidator intValidator(1,100);
	editW.setValidator(&intValidator);
	editH.setValidator(&intValidator);
	editW.setText(QString("%1").arg(m_iCBWidth));
	editH.setText(QString("%1").arg(m_iCBHeight));
	dlg.setWindowTitle(tr("色块大小设置"));
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	int iW = editW.text().toInt();
	int iH = editH.text().toInt();
	if((iW>0&&iH>0))
	{
		if(iW!=m_iCBWidth||iH!=m_iCBHeight)
		{
			m_iCBHeight = iH;
			m_iCBWidth = iW;
			update();
		}
	}
	else
	{
		QMessageBox::information(this,tr("提示"),tr("设置色块大小失败，请检查你的输入！"));
	}
}

QMenu* CBaseBlockWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	{
		//设置当前选中的周期模式
		QList<QAction*> listAct = m_pMenuCircle->actions();
		foreach(QAction* pAct,listAct)
		{
			pAct->setCheckable(true);
			pAct->setChecked(pAct->data().toInt() == m_typeCircle);
		}
	}

	{
		//添加当前所有的支持的颜色模式菜单
		m_pMenuColorMode->clear();

		QStringList listColors = CColorManager::getBlockColorList();
		foreach(const QString& clr,listColors)
		{
			QAction* pAct = m_pMenuColorMode->addAction(clr,this,SLOT(onSetColorMode()));
			pAct->setData(clr);
			pAct->setCheckable(true);
			if(clr == m_qsColorMode)
				pAct->setChecked(true);
		}
	}
	{
		QList<QAction*> listAct = m_pMenuBlockMode->actions();
		foreach(QAction* pAct,listAct)
		{
			pAct->setCheckable(true);
			pAct->setChecked(pAct->data().toInt() == m_typeBlock);
		}
	}


	return m_pMenuCustom;
}

void CBaseBlockWidget::drawColocBlock(QPainter& p,int iY,QVector<float>& vValue)
{
	int nTimes = 1;
	if(m_typeCircle<=Min60)
		nTimes = 10;
	else if(m_typeCircle<=Week)
		nTimes = 1;
	else
		nTimes = 0.1;

	QMap<time_t,float>::iterator iter = m_mapShowTimes.begin();
	
	int iMapSize = m_mapTimes.size()-1;
	while(iter!=m_mapShowTimes.end())
	{
		QRectF rtCB = QRectF(iter.value(),iY,m_iCBWidth,m_iCBHeight);
		if(m_mapTimes.contains(iter.key()))
		{
			float f = vValue[iMapSize - m_mapTimes[iter.key()]];
			switch(m_typeBlock)
			{
			case BlockRect:
				{
					rtCB.adjust(1,1,-1,-1);
					p.fillRect(rtCB,CColorManager::getBlockColor(m_qsColorMode,f*nTimes));
				}
				break;
			case BlockCircle:
				{
					QPainterPath path;
					path.addEllipse(rtCB);
					p.fillPath(path,CColorManager::getBlockColor(m_qsColorMode,f));
				}
				break;
			}
		}
		++iter;
	}
	/*
	QMap<time_t,RStockData>::iterator iter = pMapCBs->begin();
	float fLastPrice = pItem->getCurrentReport()->fLastClose;
	while(iter!=pMapCBs->end())
	{
		float f = FLOAT_NAN;
		QRect rtB;
		if(m_typeCircle<Day)
		{
			time_t tmCur = iter.key()/(m_typeCircle)*(m_typeCircle);		//向下取整
			if(m_mapTimes.contains(tmCur))
			{
				float fCurX = fBeginX - ((m_mapTimes[tmCur])*m_iCBWidth);
				if(fCurX>=fEndX)
				{
					//计算增长百分比
					f = (iter->fClose - fLastPrice)/fLastPrice*10.0;
					rtB = QRect(fCurX,rtCB.top(),m_iCBWidth,m_iCBHeight);
				}
			}
		}
		else
		{
			time_t tmCur = iter.key();
			QMap<time_t,int>::iterator iterTime = m_mapTimes.upperBound(tmCur);
			if(iterTime!=m_mapTimes.begin())
			{
				--iterTime;
				float fCurX = fBeginX - ((iterTime.value())*m_iCBWidth);
				if(fCurX>=fEndX)
				{
					//计算增长百分比
					f = (iter->fClose - fLastPrice)/fLastPrice;
					if(m_typeCircle>DayN)
						f = f/10.0;							//大于周线的，则对比例进行缩小
					rtB = QRect(fCurX,rtCB.top(),m_iCBWidth,m_iCBHeight);
				}
			}
		}
		if(f!=FLOAT_NAN)
		{
			switch(m_typeBlock)
			{
			case BlockRect:
				{
					rtB.adjust(1,1,-1,-1);
					p.fillRect(rtB,CColorManager::getBlockColor(m_qsColorMode,f));
				}
				break;
			case BlockCircle:
				{
					QPainterPath path;
					path.addEllipse(rtB);
					p.fillPath(path,CColorManager::getBlockColor(m_qsColorMode,f));
				}
				break;
			}
		}
		fLastPrice = iter->fClose;
		++iter;
	}*/
}
