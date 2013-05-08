#include "StdAfx.h"
#include "KLineWidget.h"
#include "DataEngine.h"
#include "KeyWizard.h"
#include "ColorManager.h"

#define	KLINE_BORDER	2

CKLineWidget::CKLineWidget( CBaseWidget* parent /*= 0*/ )
	: CCoordXBaseWidget(parent,WidgetKLine)
	, m_pStockItem(0)
	, m_iShowCount(100)
	, m_iCurExp(0)
	, m_bShowMax(false)
	, m_iTitleHeight(16)
	, m_iCoorYWidth(50)
	, m_iCoorXHeight(16)
	, m_iMainLinerHeight(200)
	, m_mapData(NULL)
{
	m_typeCircle = Min1;

	m_vSizes.push_back(60);
	m_vExps.push_back("DrawK(OPEN,CLOSE,HIGH,LOW)");

	m_pMenuCustom->addAction(tr("设置股票代码"),this,SLOT(onSetStockCode()));
	{
		m_pMenuCustom->addSeparator();
		QMenu* pMenuDeputy = m_pMenuCustom->addMenu(tr("添加副图"));
		pMenuDeputy->addAction(tr("普通副图"),this,SLOT(onAddDeputy()));
		pMenuDeputy->addAction(tr("量副图"),this,SLOT(onAddVolume()));

	}
	m_pMenuCustom->addAction(tr("删除副图"),this,SLOT(onRemoveDeputy()));
	m_pMenuCustom->addAction(tr("设置表达式"),this,SLOT(onSetExpression()));
	m_pMenuCustom->addAction(tr("设置所有图的显示比例"),this,SLOT(onSetSizes()));

//	setMinimumSize(200,200);
}

CKLineWidget::~CKLineWidget(void)
{
	clearTmpData();

	delete m_pMenuCustom;
}

bool CKLineWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CCoordXBaseWidget::loadPanelInfo(eleWidget))
		return false;

	m_vSizes.clear();
	m_vExps.clear();

	QDomElement eleLiners = eleWidget.firstChildElement("liners");
	if(eleLiners.isElement())
	{
		//加载所有的Liner
		QDomElement eleLiner = eleLiners.firstChildElement("Liner");
		while(eleLiner.isElement())
		{
			m_vSizes.push_back(eleLiner.attribute("size").toInt());

			QDomElement eleExp = eleLiner.firstChildElement("exp");
			if(eleExp.isElement())
			{
				m_vExps.push_back(eleExp.text());
			}
			else
			{
				m_vExps.push_back("DrawK(OPEN,CLOSE,HIGH,LOW)");
			}

			eleLiner = eleLiner.nextSiblingElement("Liner");
		}
	}

	QDomElement eleCode = eleWidget.firstChildElement("code");
	if(eleCode.isElement())
		setStockCode(eleCode.text());

	return true;
}

bool CKLineWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CCoordXBaseWidget::savePanelInfo(doc,eleWidget))
		return false;
	if(m_pStockItem)
	{
		//当前的股票值
		QDomElement eleCode = doc.createElement("code");
		eleCode.appendChild(doc.createTextNode(m_pStockItem->getCode()));
		eleWidget.appendChild(eleCode);


		QDomElement eleLiners = doc.createElement("liners");
		eleWidget.appendChild(eleLiners);
		{
			//保存所有的副图信息
			for(int i=0;i<m_vSizes.size();++i)
			{
				QDomElement eleLiner = doc.createElement("Liner");
				eleLiners.appendChild(eleLiner);
				int iSize = m_vSizes[i];
				eleLiner.setAttribute("size",iSize);

				QDomElement eleExp = doc.createElement("exp");
				eleExp.appendChild(doc.createTextNode(m_vExps[i]));
				eleLiner.appendChild(eleExp);
			}
		}
	}

	return true;
}

void CKLineWidget::updateData()
{
	resetTmpData();
	return /*CCoordXBaseWidget::updateData()*/;
}

void CKLineWidget::setStockCode( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);
	if(pItem)
	{
		setStockItem(pItem);
	}

	return CBaseWidget::setStockCode(code);
}

void CKLineWidget::setStockItem( CStockInfoItem* pItem )
{
	if(pItem)
	{
		if(m_pStockItem)
		{
			//移除所有和 updateKLine关联的 信号/槽
			disconnect(m_pStockItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
			disconnect(m_pStockItem,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
			//disconnect(m_pStockItem,SIGNAL(stockItemReportChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
		}

		//设置默认显示100个K线
		if((pItem!=m_pStockItem)&&(m_iShowCount<50))
			m_iShowCount = 100;

		m_pStockItem = pItem;
		//建立更新机制
		connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
		connect(pItem,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
		//		connect(pItem,SIGNAL(stockItemReportChanged(const QString&)),this,SLOT(updateKLine(const QString&)));

		//更新K线图
		resetTmpData();
	}
}

void CKLineWidget::updateKLine( const QString& code )
{
	if(m_pStockItem&&m_pStockItem->getCode()!=code)
		return;

	resetTmpData();
}

void CKLineWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	p.setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing,true);
	QRect rtClient = this->rect();
	p.fillRect(rtClient,QColor(0,0,0));
	if(!m_pStockItem)
		return;

	if(m_iShowCount>m_mapData->size())
	{
		m_iShowCount = m_mapData->size();
	}

	/*画头部*/
	QRect rtTitle = QRect(rtClient.left(),rtClient.top(),rtClient.width(),m_iTitleHeight);
	drawTitle(p,rtTitle);

	/*画X坐标轴*/
	QRectF rtCoordX = QRectF(rtClient.left()+3,rtClient.bottom()-m_iCoorXHeight+1,rtClient.width()-m_iCoorYWidth-5,m_iCoorXHeight);
	m_fItemWidth = float(rtCoordX.width()-1)/float(m_iShowCount);
	updateShowTimes(rtCoordX,m_fItemWidth);
	CCoordXBaseWidget::drawCoordX(p,rtCoordX,m_fItemWidth);

	/*画右下角的两个按钮*/
	QRect rtShowBtns = QRect(rtClient.right()-m_iCoorYWidth,rtClient.bottom()-m_iCoorXHeight,m_iCoorYWidth,m_iCoorXHeight);
	drawShowBtns(p,rtShowBtns);


	rtClient.adjust(3,m_iTitleHeight,-m_iCoorYWidth-2,-m_iCoorXHeight);			//改变为可画图区域的大小
	m_rtClient = rtClient;

	if(m_bShowMax&&m_iCurExp<m_vExps.size())
	{
		QString qsExp = m_vExps[m_iCurExp];		//获取表达式

		//进行全屏绘制
		RDrawInfo draw;
		draw.dwVersion = RSTOCK_VER;
		draw.pPainter = &p;
		draw.rtClient = rtClient;
		draw.fItemWidth = m_fItemWidth;
		draw.iEndIndex = m_mapData->size()-1;
		draw.iCurColor = 0;
		drawExpArgs(p,rtClient,qsExp,draw.lsColors);

		lua_pushlightuserdata(m_pL,&draw);
		lua_setglobal(m_pL,"_draw");

		luaL_dostring(m_pL,qsExp.toLocal8Bit());

		drawCoordY(p,QRectF(rtClient.right(),rtClient.top(),50,rtClient.height()),draw.fMax,draw.fMin);
	}
	else
	{
		int iCurY = rtClient.top();		//当前绘制到的位置
		for(int i=0;i<m_vSizes.size();++i)
		{
			int iTotal = 0;					//比例总和
			for (int j=i;j<m_vSizes.size();++j)
				iTotal += m_vSizes[j];

			int iTotalHeight = rtClient.bottom()-iCurY;
			int iHeight = (float)((float)m_vSizes[i]/float(iTotal))*iTotalHeight + 0.5;

			QRectF rtArea = QRectF(rtClient.left(),iCurY,rtClient.width(),iHeight);
			{
				QString qsExp = m_vExps[i];		//获取表达式
				RDrawInfo draw;
				draw.dwVersion = RSTOCK_VER;
				draw.pPainter = &p;
				draw.rtClient = rtArea;
				draw.fItemWidth = m_fItemWidth;
				draw.iEndIndex = m_mapData->size()-1;
				draw.iCurColor = 0;
				drawExpArgs(p,rtArea.toRect(),qsExp,draw.lsColors);

				lua_pushlightuserdata(m_pL,&draw);
				lua_setglobal(m_pL,"_draw");

				luaL_dostring(m_pL,qsExp.toLocal8Bit());

				drawCoordY(p,QRectF(rtArea.right(),rtArea.top(),50,rtArea.height()),draw.fMax,draw.fMin);
			}

			iCurY += iHeight;
		}
	}
	return;
}

void CKLineWidget::mouseMoveEvent( QMouseEvent* e )
{
	if(!((qApp->mouseButtons())&Qt::LeftButton))
	{
		update();
		QToolTip::hideText();
		return CCoordXBaseWidget::mouseMoveEvent(e);
	}
	if(!m_pStockItem)
	{
		return QToolTip::hideText();
	}

	float fEnd = m_rtClient.right();
	int iLastN = (fEnd - e->posF().x())/m_fItemWidth;
	if(iLastN<0||iLastN>=m_iShowCount)
		return QToolTip::hideText();

	RStockData* pData = NULL;
	int iIndex = 0;
	QMap<time_t,RStockData*>::iterator iter = m_mapData->end();
	while(iter!=m_mapData->begin())
	{
		--iter;
		if(iIndex == iLastN)
		{
			pData = iter.value();
			break;
		}
		if(iIndex>iLastN)
			break;
		++iIndex;
	}
	
	if(pData)
	{
		QString qsTooltip;		//Tips
		QString qsTime;
		if(m_typeCircle<Day)
		{
			time_t tmToday = QDateTime(QDateTime::fromTime_t(m_pStockItem->getCurrentReport()->tmTime).date()).toTime_t();
			if(pData->tmTime>tmToday)
				qsTime = QDateTime::fromTime_t(pData->tmTime).toString("hh:mm:ss");
			else
				qsTime = QDateTime::fromTime_t(pData->tmTime).toString("MM/dd hh:mm");
		}
		else
		{
			QDate dtTmp = QDateTime::fromTime_t(pData->tmTime).date();
			if(m_typeCircle == Week)
				qsTime = QString("%1 %2").arg(dtTmp.year()).arg(dtTmp.weekNumber());
			else if(m_typeCircle == Month)
				qsTime = dtTmp.toString("yyyy/MM");
			else if(m_typeCircle == Month3)
				qsTime = dtTmp.toString("yyyy/MM");
			else if(m_typeCircle == Year)
				qsTime = dtTmp.toString("yyyy");
			else
				qsTime = dtTmp.toString("yyyy/MM/dd");
		}
	
		qsTooltip = QString("股票代码:%1\r\n时间:%2\r\n最新价:%7\r\n最高价:%3\r\n最低价:%4\r\n成交量:%5\r\n成交额:%6")
			.arg(m_pStockItem->getCode()).arg(qsTime).arg(pData->fHigh).arg(pData->fLow)
			.arg(pData->fVolume).arg(pData->fAmount).arg(pData->fClose);

		QToolTip::showText(e->globalPos(),qsTooltip,this);
	}
	else
	{
		QToolTip::hideText();
	}

	return CCoordXBaseWidget::mouseMoveEvent(e);
}

void CKLineWidget::leaveEvent( QEvent* e )
{
	update();
	return CBaseWidget::leaveEvent(e);
}

void CKLineWidget::mousePressEvent( QMouseEvent* e )
{
	QPoint ptCur = e->pos();
	QRect rtClient = rect();
	rtClient.adjust(3,m_iTitleHeight,-m_iCoorYWidth-2,-m_iCoorXHeight);			//改变为可画图区域的大小

	int iCurY = rtClient.top();		//当前绘制到的位置
	for(int i=0;i<m_vSizes.size();++i)
	{
		int iTotal = 0;					//比例总和
		for (int j=i;j<m_vSizes.size();++j)
			iTotal += m_vSizes[j];

		int iTotalHeight = rtClient.bottom()-iCurY;
		int iHeight = (float)((float)m_vSizes[i]/float(iTotal))*iTotalHeight + 0.5;

		QRectF rtArea = QRectF(rtClient.left(),iCurY,rtClient.width(),iHeight);
		if(rtArea.contains(ptCur))
		{
			m_iCurExp = i;
			break;
		}

		iCurY += iHeight;
	}

	if(e->button()==Qt::LeftButton)
	{
		if(m_rtAddShow.contains(ptCur))
		{
			onClickedAddShow();
		}
		else if(m_rtSubShow.contains(ptCur))
		{
			onClickedSubShow();
		}
	}

	return CCoordXBaseWidget::mousePressEvent(e);
}

void CKLineWidget::mouseDoubleClickEvent( QMouseEvent* e )
{
	QPoint ptCur = e->pos();
	QRect rtClient = rect();
	rtClient.adjust(3,m_iTitleHeight,-m_iCoorYWidth-2,-m_iCoorXHeight);			//改变为可画图区域的大小
	if(rtClient.contains(ptCur))
	{
		m_bShowMax = !m_bShowMax;
		update();
	}
}

void CKLineWidget::keyPressEvent(QKeyEvent* e)
{
	int iKey = e->key();
	if(iKey == Qt::Key_Up)
		return setShowCount(m_iShowCount-10);
	else if(iKey == Qt::Key_Down)
		return setShowCount(m_iShowCount+10);
	else if(Qt::Key_F10 == iKey)
	{
		//F10数据
		if(m_pStockItem)
		{
			if(!CDataEngine::getDataEngine()->showF10Data(m_pStockItem->getCode()))
			{
				//未打开F10数据 do something
			}
		}
	}

	return CBaseWidget::keyPressEvent(e);
}

QMenu* CKLineWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	{
		QList<QAction*> listAct = m_pMenuCircle->actions();
		foreach(QAction* pAct,listAct)
		{
			pAct->setCheckable(true);
			pAct->setChecked(pAct->data().toInt() == m_typeCircle);
		}
	}

	return m_pMenuCustom;
}

void CKLineWidget::onSetStockCode()
{
	QDialog dlg(this);
	QVBoxLayout layout(&dlg);
	QLineEdit edit(&dlg);
	QPushButton btnOk(&dlg);
	dlg.setLayout(&layout);
	layout.addWidget(&edit);
	layout.addWidget(&btnOk);
	btnOk.setText(tr("确定"));
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	QString code = edit.text();
	setStockCode(code);
}

void CKLineWidget::onSetExpression()
{
	if(m_iCurExp>=m_vExps.size())
		return;

	QDialog dlg(this);
	QVBoxLayout layout(&dlg);
	QTextEdit edit(&dlg);
	QPushButton btnOk(&dlg);
	layout.addWidget(&edit);
	layout.addWidget(&btnOk);
	dlg.setLayout(&layout);
	btnOk.setText(tr("确定"));
	edit.setText(m_vExps[m_iCurExp]);
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	m_vExps[m_iCurExp] = edit.toPlainText();
}

void CKLineWidget::onClickedAddShow()
{
	setShowCount(m_iShowCount+10);
}

void CKLineWidget::onClickedSubShow()
{
	setShowCount(m_iShowCount-10);
}

void CKLineWidget::onAddDeputy()
{
	m_vSizes.push_back(20);
	m_vExps.push_back("DrawLine(CLOSE)");
	update();
}

void CKLineWidget::onAddVolume()
{
	m_vSizes.push_back(20);
	m_vExps.push_back("DrawHist(VOLUME)");
	update();
}


void CKLineWidget::onRemoveDeputy()
{
	//删除当前选中的副图
	if(m_iCurExp<m_vSizes.size())
	{
		m_vSizes.remove(m_iCurExp);
		m_vExps.remove(m_iCurExp);
		m_iCurExp = 0;
		update();
	}
}

void CKLineWidget::onSetSizes()
{
	//弹出让用户编辑各自窗口比例的对话框
	QDialog dlg(this);
	QGridLayout layout(&dlg);
	QPushButton btnOk(this);
	dlg.setLayout(&layout);

	int iCount = m_vSizes.size();
	QVector<QLabel*> vLabels;
	QVector<QSpinBox*> vSpins;
	for (int i=0;i<iCount;++i)
	{
		//设置初始的比例
		QLabel* pLabel = new QLabel(&dlg);
		pLabel->setText(QString("%1").arg(i+1));
		vLabels.push_back(pLabel);
		QSpinBox* pSpin = new QSpinBox(&dlg);
		pSpin->setMinimum(1);
		pSpin->setMaximum(100);
		pSpin->setValue(m_vSizes[i]);
		vSpins.push_back(pSpin);

		layout.addWidget(pLabel,i,0);
		layout.addWidget(pSpin,i,1);
	}
	layout.addWidget(&btnOk,iCount,0,1,2);
	btnOk.setText(tr("确定"));
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	m_vSizes.clear();			//清空之前的比例数据

	for (int i=0;i<iCount;++i)
	{
		m_vSizes.push_back(vSpins[i]->value());
		delete vLabels[i];
		delete vSpins[i];
	}
}

void CKLineWidget::setShowCount(int _iShow)
{
	if(_iShow>m_mapData->size())
	{
		m_iShowCount = m_mapData->size();
	}
	else if(_iShow<1)
		m_iShowCount = 1;
	else
		m_iShowCount = _iShow;
	update();
}

void CKLineWidget::drawTitle( QPainter& p,const QRect& rtTitle )
{
	if(!rtTitle.isValid())
		return;

	QRect rtClient = rtTitle.adjusted(2,0,-2,0);
	QString qsName = m_pStockItem->getName();
	if(qsName.isEmpty())
		qsName = m_pStockItem->getCode();

	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);

	p.setPen(QColor(255,255,255));
	p.drawText(rtClient,Qt::AlignLeft|Qt::AlignVCenter,qsName);
}

void CKLineWidget::drawShowBtns( QPainter& p,const QRect& rtBtns )
{
	p.setPen(QColor(255,0,0));
	int iBtnWidth = 16;
	int iBtnHeight = rtBtns.height()-1;
	m_rtAddShow = QRect(rtBtns.right()-2*iBtnWidth,rtBtns.top()+1,iBtnWidth,iBtnHeight);
	m_rtSubShow = QRect(rtBtns.right()-iBtnWidth,rtBtns.top()+1,iBtnWidth,iBtnHeight);
	
	p.drawRect(m_rtAddShow);
	p.drawRect(m_rtSubShow);

	p.drawText(m_rtAddShow,Qt::AlignCenter,QString("+"));
	p.drawText(m_rtSubShow,Qt::AlignCenter,QString("-"));
}

void CKLineWidget::drawExpArgs( QPainter& p,const QRect& rtClient, 
							   const QString& e,QList<uint>& lsColor )
{
	QFontMetrics fm(p.font());
	int x = rtClient.left()+5;
	int y = rtClient.top() + fm.height();

	QStringList listExps = e.split("\n");
	foreach(const QString& _e,listExps)
	{
		int _i = _e.indexOf("DrawLine");
		if(_i>-1)
		{
			_i += 9;
			QString _arg = _e.mid(_i,_e.lastIndexOf(")")-_i);
			uint clPen = CColorManager::getCommonColor(lsColor.size());
			lsColor.push_back(clPen);
			p.setPen(QColor::fromRgb(clPen));
			p.drawText(x,y,_arg+";");
			x += (fm.width(_arg)+20);
		}
	}
}

void CKLineWidget::clearTmpData()
{
	//foreach(stLinerItem* p,listItems)
	//	delete p;
	time_t tmToday = QDateTime(QDateTime::fromTime_t(m_pStockItem->getCurrentReport()->tmTime).date()).toTime_t();
	if(m_mapData)
	{
		QMap<time_t,RStockData*>::iterator iter = m_mapData->begin();
		while(iter!=m_mapData->end())
		{
			if((*iter)->tmTime>tmToday)
				delete iter.value();
			++iter;
		}
		m_mapData->clear();
		delete m_mapData;
		m_mapData = NULL;
	}
	disconnect(this,SLOT(updateKLine(const QString&)));
}

void CKLineWidget::resetTmpData()
{
	clearTmpData();
	updateTimesH();			//更新最新的时间轴数据

	m_mapData = getColorBlockMap(m_pStockItem);

	{
		//对于K线图需去除空值
		QMap<time_t,RStockData*>::iterator iter = m_mapData->begin();
		while(iter != m_mapData->end())
		{
			time_t tmRemove = 0;
			if(iter.value()==NULL)
			{
				tmRemove = iter.key();
			}
			++iter;

			if(tmRemove!=0)
			{
				m_mapData->remove(tmRemove);
				m_mapTimes.remove(tmRemove);
			}
		}
	}
	{
		if(m_typeCircle<Day)
		{
			time_t tmToday = QDateTime(QDateTime::fromTime_t(m_pStockItem->getCurrentReport()->tmTime).date()).toTime_t();
			//过去5分钟数据
			QList<RStockData*> listData = m_pStockItem->get5MinList();
			int iCount = listData.count();
			for(int i=(iCount-1);i>=0;--i)
			{
				RStockData* _p = listData[i];
				if(_p->tmTime<tmToday)
				{
					m_mapData->insert(_p->tmTime,_p);
					m_mapTimes.insert(_p->tmTime,m_mapTimes.size());
				}
			}
		}
	}

	if(m_mapData->size()!=m_mapTimes.size())
	{
		qDebug()<<"Something wrong in 'CKLineWidget::resetTmpData()'!";
	}
	QTime tmNow = QTime::currentTime();
	/*将更新后的数据设置到脚本引擎中*/
	{
		RCalcInfo calc;
		calc.dwVersion = RSTOCK_VER;
		calc.emCircle = m_typeCircle;
		calc.mapData = m_mapData;
		calc.mapDataEx = NULL;
		calc.pItem = m_pStockItem;
		
		lua_pushlightuserdata(m_pL,&calc);
		lua_setglobal(m_pL,"_calc");

		lua_getglobal(m_pL,"InitValues");
		lua_call(m_pL,0,0);
	}

	if(m_pStockItem)
		qDebug()<<"set "<<m_pStockItem->getCode()<<" data to script, use ms:"<<tmNow.msecsTo(QTime::currentTime());

	//更新界面
	update();
}

void CKLineWidget::getKeyWizData( const QString& keyword,QList<KeyWizData*>& listRet )
{
	foreach(CStockInfoItem* pItem,CDataEngine::getDataEngine()->getStockInfoList())
	{
		if(pItem->isMatch(keyword))
		{
			KeyWizData* pData = new KeyWizData;
			pData->cmd = CKeyWizard::CmdStock;
			pData->arg = pItem;
			pData->desc = QString("%1 %2").arg(pItem->getCode()).arg(pItem->getName());
			listRet.push_back(pData);
			if(listRet.size()>20)
				return;
		}
	}

	return CCoordXBaseWidget::getKeyWizData(keyword,listRet);
}

void CKLineWidget::keyWizEntered( KeyWizData* pData )
{
	if(pData->cmd == CKeyWizard::CmdStock)
	{
		setStockItem(reinterpret_cast<CStockInfoItem*>(pData->arg));
		return;
	}

	return CCoordXBaseWidget::keyWizEntered(pData);
}
