#include "StdAfx.h"
#include "KeyWizard.h"
#include "MainWindow.h"

class CWizardInput : public QLineEdit
{
public:
	CWizardInput(QWidget* parent = 0)
		: QLineEdit(parent)
	{
	}
	~CWizardInput(){}

protected:
	virtual void keyPressEvent(QKeyEvent* e)
	{
		QString _t = e->text();
		if(_t.size()>0)
		{
			QChar _c = _t[0];
			if(_c>=QChar('a')&&_c<=QChar('z'))
			{
				return setText(text()+e->text().toUpper());
			}
		}

		return QLineEdit::keyPressEvent(e);
	}
};


CKeyWizard* CKeyWizard::m_pSelf = NULL;

CKeyWizard* CKeyWizard::getKeyWizard()
{
	if(m_pSelf==NULL)
		m_pSelf = new CKeyWizard();

	return m_pSelf;
}

CKeyWizard::CKeyWizard( QWidget* parent /*= 0*/ )
	: QWidget(parent)
	, m_pCurWidget(NULL)
{
	setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint|Qt::Tool);
	setFixedSize(150,200);

	m_pInput = new CWizardInput(this);
	QVBoxLayout* pLayout = new QVBoxLayout();
	pLayout->setContentsMargins(0,0,0,0);
	pLayout->setMargin(0);
	pLayout->setSpacing(0);
	pLayout->addWidget(m_pInput);
	pLayout->addWidget(&m_viewList);
	setLayout(pLayout);

	m_viewList.setSelectionMode(QListView::SingleSelection);
	connect(m_pInput,SIGNAL(textChanged(const QString&)),this,SLOT(inputTextChanged(const QString&)));

	CMainWindow* pMain = CMainWindow::getMainWindow();
	if(pMain)
	{
		QPoint ptBR = pMain->mapToGlobal(pMain->childrenRect().bottomRight());
		this->move(ptBR.x()-162,ptBR.y()-230);
	}
}

CKeyWizard::~CKeyWizard(void)
{
	delete m_pInput;
}

bool CKeyWizard::showWizard( CBaseWidget* pWidget, const QString& text )
{
	m_pCurWidget = pWidget;
	if(!isVisible())
	{
		m_pInput->setText(text.toUpper());
	}
	this->show();
	if(::GetForegroundWindow() != winId())
		::SetForegroundWindow(winId());
	return true;
}

void CKeyWizard::hideEvent( QHideEvent* )
{
	m_pInput->clear();
	clearWizData();
}

void CKeyWizard::keyPressEvent( QKeyEvent* e )
{
	int iKey  = e->key();
	if(iKey == Qt::Key_Escape)
		return hide();
	else if(iKey == Qt::Key_Enter || iKey==Qt::Key_Return)
		return enterPressed();
	else if(iKey == Qt::Key_Down)
		return selectItemAbs(1);
	else if(iKey == Qt::Key_Up)
		return selectItemAbs(-1);

	return QWidget::keyPressEvent(e);
}

void CKeyWizard::inputTextChanged( const QString& text )
{
	if((text.isEmpty())||(!m_pCurWidget))
	{
		return hide();
	}

	clearWizData();
	m_pCurWidget->getKeyWizData(text.toLower(),m_listWizData);
	foreach(KeyWizData* pData,m_listWizData)
	{
		QListWidgetItem* pItem = new QListWidgetItem(pData->desc);
		pItem->setData(Qt::UserRole,(uint)pData);
		m_viewList.addItem(pItem);
	}
	selectItemAbs(0);
}

void CKeyWizard::clearWizData()
{
	m_viewList.clear();
	foreach(KeyWizData* p,m_listWizData)
		delete p;
	m_listWizData.clear();
}

void CKeyWizard::enterPressed()
{
	if(!m_pCurWidget)
		return;

	QList<QListWidgetItem*> list = m_viewList.selectedItems();
	if(list.size()>0)
	{
		KeyWizData* pData = reinterpret_cast<KeyWizData*>(list[0]->data(Qt::UserRole).toUInt());
		if(pData)
			m_pCurWidget->keyWizEntered(pData);
	}

	return hide();
}

void CKeyWizard::selectItemAbs(int iAbs)
{
	QList<QListWidgetItem*> list = m_viewList.selectedItems();
	QListWidgetItem* pItemBefore = NULL;
	QListWidgetItem* pItemAfter = NULL;
	if(list.size()>0)
	{
		pItemBefore = list[0];

		int iRow = m_viewList.row(pItemBefore) + iAbs;
		if(iRow<m_viewList.count())
		{
			pItemAfter = m_viewList.item(iRow);
		}
	}
	else
	{
		if(m_viewList.count()>0)
		{
			pItemAfter = m_viewList.item(0);
		}
	}
	
	if(pItemBefore)
	{
		pItemBefore->setBackgroundColor(QColor::fromRgb(255,255,255));
	}
	if(!pItemAfter)
		pItemAfter = pItemBefore;
	
	if(pItemAfter)
	{
		m_viewList.setItemSelected(pItemAfter,true);
		pItemAfter->setBackgroundColor(QColor::fromRgb(255,0,0));
	}
}