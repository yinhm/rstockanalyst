#ifndef _KEY_WIZARD_H_
#define _KEY_WIZARD_H_
#include <QtGui>

class CKeyWizard : public QWidget
{
	Q_OBJECT
public:
	static CKeyWizard* getKeyWizard();
protected:
	CKeyWizard(QWidget* parent = 0);
	~CKeyWizard(void);

private:
	static CKeyWizard* m_pSelf;
};

#endif //_KEY_WIZARD_H_
