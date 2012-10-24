#pragma once
#include <QtGui>
#include "BaseMarketTreeView.h"

//专门用于绘制类似于TabWidget 表头的类
class CTabButton : public QLabel
{
	Q_OBJECT
public:
	CTabButton(const QString& text,QWidget* parent = 0);
	~CTabButton(){}

public:
	void setBtnDown(bool bDown = true){ bButtonDown = bDown;}
protected:
	virtual void paintEvent(QPaintEvent* e);
	virtual void mousePressEvent(QMouseEvent *ev);

signals:
	void clicked();

private:
	bool bButtonDown;
};

//专门用于绘制类似于TabWidget 表头的类，配合CTabButton使用
class CTabButtonList : public QWidget
{
	Q_OBJECT
public:
	CTabButtonList(QWidget* parent = 0);
	~CTabButtonList(){}

public:
	void addTabButton(const QString& text,QWidget* widget);

protected slots:
	void tabBtnClicked();

private:
	QMap<CTabButton*,QWidget*> mapButtons;
	CTabButton* pDownBtn;
	QHBoxLayout* pLayout;
};


//显示基本行情的窗口。
class CBaseMarketWidget : public QScrollArea
{
	Q_OBJECT
public:
	CBaseMarketWidget();
	~CBaseMarketWidget(void);


public slots:
	void treeItemClicked(const QModelIndex& index);

private:
	//上证A股
	CBaseMarketTreeView* m_pViewSHA;
	//深圳指数
	CBaseMarketTreeView* m_pViewSZ;
};

