#include "StdAfx.h"
#include "MainWindow.h"
#include "STKDRV.h"
#include "DataEngine.h"
#include "KLineWidget.h"
#include "BlockColorSettingDlg.h"

#define RSTOCK_ANALYST_MAINMSG (WM_USER+1)

CMainWindow* CMainWindow::m_pMainWindow = 0;

CMainWindow* CMainWindow::getMainWindow()
{
	if(m_pMainWindow == 0)
		m_pMainWindow = new CMainWindow();

	return m_pMainWindow;
}


CMainWindow::CMainWindow()
	: QMainWindow()
{
	m_pTabWidget = new QTabWidget();
	setCentralWidget(m_pTabWidget);
	m_qsTemplateDir = qApp->applicationDirPath()+"/config/template/";
	QDir().mkpath(m_qsTemplateDir);
	{
		//初始化Menu
		m_pMenuBar = new QMenuBar(this);
		//版面管理
		QMenu* pMenuTemplate = m_pMenuBar->addMenu(tr("版面管理"));
		pMenuTemplate->addAction(tr("添加版面"),this,SLOT(onAddTemplate()));
		pMenuTemplate->addAction(tr("保存所有"),this,SLOT(onSaveTemplate()));
		pMenuTemplate->addAction(tr("删除当前版面"),this,SLOT(onRemoveTemplate()));
		setMenuBar(m_pMenuBar);

		//设置
		QMenu* pMenuSettings = m_pMenuBar->addMenu(tr("设置"));
		pMenuSettings->addAction(tr("设置色块颜色"),CBlockColorSettingDlg::getDialog(),SLOT(exec()));
	}
}

CMainWindow::~CMainWindow()
{

}

bool CMainWindow::setupStockDrv()
{
	if(CSTKDRV::Stock_Init(this->winId(),RSTOCK_ANALYST_MAINMSG,RCV_WORK_MEMSHARE)>0)
	{
		if(CSTKDRV::SetupReceiver(TRUE)>0)
		{
			return true;
		}
	}

	return false;
}

void CMainWindow::initTemplates()
{
	QDir dir(m_qsTemplateDir);
	QFileInfoList list = dir.entryInfoList(QDir::Files,QDir::Time);
	foreach(const QFileInfo& info,list)
	{
		QFile file(info.absoluteFilePath());
		if(!file.open(QFile::ReadOnly))
			continue;
		QDomDocument doc;
		doc.setContent(file.readAll());
		file.close();

		QDomElement eleRoot = doc.firstChildElement("ROOT");
		if(!eleRoot.isElement())
			continue;

		QString qsTitle = eleRoot.attribute("title");
		if(!getSubWindows(qsTitle))
		{
			CBaseWidget* pWidget = new CBaseWidget(0);
			QDomElement eleWidget = eleRoot.firstChildElement("widget");
			if(eleWidget.isElement())
			{
				pWidget->clearChildren();
				pWidget->loadPanelInfo(eleWidget);
			}
			m_pTabWidget->addTab(pWidget,qsTitle);
		}
	}
}

void CMainWindow::saveTemplates()
{
	for(int i=0;i<m_pTabWidget->count();++i)
	{
		CBaseWidget* pWidget = reinterpret_cast<CBaseWidget*>(m_pTabWidget->widget(i));
		QString qsTitle = m_pTabWidget->tabText(i);
		QDomDocument doc("template");
		QDomElement eleRoot = doc.createElement("ROOT");
		eleRoot.setAttribute("title",qsTitle);
		doc.appendChild(eleRoot);
		QDomElement eleWidget = doc.createElement("widget");
		eleRoot.appendChild(eleWidget);
		pWidget->savePanelInfo(doc,eleWidget);

		QFile file(m_qsTemplateDir+qsTitle+".xml");
		if(!file.open(QFile::Truncate|QFile::WriteOnly))
		{
			QMessageBox::warning(this,tr("错误"),QString("保存模板'%1'失败！").arg(qsTitle));
			continue;
		}
		file.write(doc.toByteArray());
		file.close();
	}
}

void CMainWindow::clickedStock( const QString& code )
{
	CBaseWidget* pWidget = reinterpret_cast<CBaseWidget*>(m_pTabWidget->currentWidget());
	if(pWidget)
	{
		pWidget->setStockCode(code);
	}
}

void CMainWindow::clickedBlock( const QString& block )
{
	CBaseWidget* pWidget = reinterpret_cast<CBaseWidget*>(m_pTabWidget->currentWidget());
	if(pWidget)
	{
		pWidget->setBlock(block);
	}
}

bool CMainWindow::winEvent( MSG* message, long* result )
{
	if(message->message == RSTOCK_ANALYST_MAINMSG)
	{
		*result = OnStockDrvMsg(message->wParam,message->lParam);
	}

	return QMainWindow::winEvent(message,result);
}

long CMainWindow::OnStockDrvMsg( WPARAM wParam,LPARAM lParam )
{
	RCV_DATA* pHeader;
	pHeader = (RCV_DATA*)lParam;

	//  对于处理速度慢的数据类型,最好将 pHeader->m_pData 指向的数据备份,再作处理

	/*
		补分笔和补分时其实补的都是分笔数据，只是补分时补的每分钟的最后一笔，而补分笔补的每分钟的所有分笔数据。
	*/

	switch( wParam )
	{
	case RCV_REPORT:                        // 共享数据引用方式,股票行情
		{
			qDebug()<< "####Comming reports"<<QTime::currentTime().toString()<<"####";
			if(pHeader->m_nPacketNum<1)
				break;

			RCV_REPORT_STRUCTExV3* pReport = pHeader->m_pReportV3;
			for(int i=0;i<pHeader->m_nPacketNum;++i)
			{
				pReport = (pHeader->m_pReportV3+i);

				QString qsCode = QString::fromLocal8Bit(pReport->m_szLabel);

				CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
				if(pItem==NULL)
				{
					pItem = new CStockInfoItem(qsCode,pReport->m_wMarket);
					CDataEngine::getDataEngine()->setStockInfoItem(pItem);
				}

				pItem->setReport(pReport);
			}
		}
		break;
	case RCV_FILEDATA:                      // 共享数据引用方式,文件
		{
			switch(pHeader->m_wDataType)
			{
			case FILE_HISTORY_EX:               // 补日线数据
				{
					QTime timeBegin = QTime::currentTime();
					RCV_HISTORY_STRUCTEx* pHistory = pHeader->m_pDay;
					QString qsCode;

					QList<qRcvHistoryData*> listHistory;
 					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pHistory = (pHeader->m_pDay+i);
						if(pHistory->m_head.m_dwHeadTag == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
							if(pItem==NULL)
							{
								//删除指针，防止内存泄漏
								foreach(qRcvHistoryData* p,listHistory)
									delete p;
							}
							else
							{
								pItem->appendHistorys(listHistory);
							}
							qsCode = QString::fromLocal8Bit(pHistory->m_head.m_szLabel);
							listHistory.clear();
						}
						else
						{
							listHistory.append(new qRcvHistoryData(pHistory));
						}
					}

					CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
					if(pItem==NULL)
					{
						//删除指针，防止内存泄漏
						foreach(qRcvHistoryData* p,listHistory)
							delete p;
					}
					else
					{
						pItem->appendHistorys(listHistory);
					}

					qDebug()<<"Packet cout:"<<pHeader->m_nPacketNum;
					qDebug()<<"UseTime:"<<qAbs(QTime::currentTime().msecsTo(timeBegin))<<"m secs";
				}
				break;

			case FILE_MINUTE_EX:                // 补分钟线数据
				{
					qDebug()<<"Minute Packet cout:"<<pHeader->m_nPacketNum;

					RCV_MINUTE_STRUCTEx* pMinute = pHeader->m_pMinute;
					QString qsCode;

					QList<qRcvMinuteData*> listMinute;
					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pMinute = (pHeader->m_pMinute+i);

						if(pMinute->m_time == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
							if(pItem==NULL)
							{
								//删除指针，防止内存泄漏
								foreach(qRcvMinuteData* p,listMinute)
									delete p;
							}
							else
							{
								pItem->appendMinutes(listMinute);
							}
							qsCode = QString::fromLocal8Bit(pMinute->m_head.m_szLabel);
							listMinute.clear();
						}
						else
						{
							listMinute.append(new qRcvMinuteData(pMinute));
						}
					}
				}
				break;

			case FILE_BASE_EX:                  // 钱龙兼容基本资料文件,m_szFileName仅包含文件名
				{
					int i = 0;
				}
				break;

			case FILE_NEWS_EX:                  // 新闻类,其类型由m_szFileName中子目录名来定
				{
					QString qsNewsTitle = QString::fromLocal8Bit(pHeader->m_File.m_szFileName);
					QString qsContent = QString::fromLocal8Bit((char*)pHeader->m_pData,pHeader->m_File.m_dwLen);
					qDebug()<<"Comming News "<< qsNewsTitle;
					CDataEngine::getDataEngine()->appendNews(qsNewsTitle,qsContent);
				}
				break;
			case FILE_POWER_EX:
				{
					qDebug()<<"Power Packet cout:"<<pHeader->m_nPacketNum;

					RCV_POWER_STRUCTEx* pPower = pHeader->m_pPower;
					QString qsCode;

					QList<qRcvPowerData*> listPower;
					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pPower = (pHeader->m_pPower+i);

						if(pPower->m_time == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
							if(pItem==NULL)
							{
								//删除指针，防止内存泄漏
								foreach(qRcvPowerData* p,listPower)
									delete p;
							}
							else
							{
								pItem->appendPowers(listPower);
							}
							qsCode = QString::fromLocal8Bit(pPower->m_head.m_szLabel);
							listPower.clear();
						}
						else
						{
							listPower.append(new qRcvPowerData(pPower));
						}
					}
				}
				break;
			default:
				{
					int i = 0;
				}
				break;
			}
		}
		break;
	case RCV_FENBIDATA:
		{
			qDebug()<< "####Comming FenBi Data"<<QTime::currentTime().toString()<<"####";

			RCV_FENBI* pFb = reinterpret_cast<RCV_FENBI*>(lParam);
			int iCount = 0;
			RCV_FENBI_STRUCTEx* p = pFb->m_Data;

			while(iCount<pFb->m_nCount)
			{
				//数据处理

				++p;
				++iCount;
			}
		}
		break;

	default:
		return 0;                           // unknown data
	}
	return 1;
}

void CMainWindow::onAddTemplate()
{
	QDialog dlg(this);
	QVBoxLayout layout;
	dlg.setLayout(&layout);
	QLineEdit edit;
	layout.addWidget(&edit);
	QPushButton btnOk(tr("确定"));
	layout.addWidget(&btnOk);
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted == dlg.exec())
	{
		QString qsName = edit.text().trimmed();
		if(!getSubWindows(qsName))
		{
			m_pTabWidget->addTab(new CBaseWidget(0),qsName);
//			m_pMdiArea->setActiveSubWindow(pSubWin);
		}
		else
		{
			QMessageBox::information(this,tr("提示"),tr("该模板名称已经存在！"));
		}
	}
}

void CMainWindow::onSaveTemplate()
{
	saveTemplates();
}

void CMainWindow::onRemoveTemplate()
{
	int iCurIndex = m_pTabWidget->currentIndex();
	QString qsText = m_pTabWidget->tabText(iCurIndex);
	if(QFile::remove(m_qsTemplateDir+qsText+".xml"))
		m_pTabWidget->removeTab(iCurIndex);
}


CBaseWidget* CMainWindow::getSubWindows( const QString& title )
{
	for(int i=0;i<m_pTabWidget->count();++i)
	{
		if(m_pTabWidget->tabText(i) == title)
			return reinterpret_cast<CBaseWidget*>(m_pTabWidget->widget(i));
	}

	return 0;
}
