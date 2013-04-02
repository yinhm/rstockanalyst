#ifndef _FUNC_HELPER_H_
#define _FUNC_HELPER_H_
#include <QtGui>

class CFuncHelper : public QDialog
{
	Q_OBJECT
public:
	CFuncHelper(QWidget* parent = NULL);
	~CFuncHelper(void);

protected slots:
	void onRefreshAll();
	void onFuncsItemChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/);

private:
	QListWidget m_listFuncs;
	QLabel m_labelFunc;
};


#endif	//_FUNC_HELPER_H_