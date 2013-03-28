#include "StdAfx.h"
#include "KeyWizard.h"

CKeyWizard* CKeyWizard::m_pSelf = NULL;

CKeyWizard* CKeyWizard::getKeyWizard()
{
	if(m_pSelf==NULL)
		m_pSelf = new CKeyWizard();

	return m_pSelf;
}

CKeyWizard::CKeyWizard( QWidget* parent /*= 0*/ )
	: QWidget(parent)
{

}

CKeyWizard::~CKeyWizard(void)
{
}
