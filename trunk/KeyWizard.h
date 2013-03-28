/************************************************************************/
/* 键盘精灵窗口
/************************************************************************/

#ifndef _KEY_WIZARD_H_
#define _KEY_WIZARD_H_
#include <QtGui>
#include "BaseWidget.h"

class CKeyWizard : public QWidget
{
	Q_OBJECT
public:
	enum CmdType
	{
		CmdStock = 1,		//选中某股票
	};

public:
	static CKeyWizard* getKeyWizard();

protected:
	CKeyWizard(QWidget* parent = 0);
	~CKeyWizard(void);

public:
	CBaseWidget* getCurrentWidget(){ return m_pCurWidget; }
	bool showWizard(CBaseWidget* pWidget, const QString& text = "");

protected:
	virtual void hideEvent(QHideEvent*);
	virtual void keyPressEvent(QKeyEvent* e);

protected slots:
	void inputTextChanged(const QString& text);

private:
	void enterPressed();
	void selectItemAbs(int iAbs);

private:
	void clearWizData();

private:
	static CKeyWizard* m_pSelf;
	CBaseWidget* m_pCurWidget;
	QList<KeyWizData*> m_listWizData;

private:
	QLineEdit m_editInput;
	QListWidget m_viewList;
};

#endif //_KEY_WIZARD_H_
