#include "StdAfx.h"
#include <QtNetwork>
#include "BaseInfoGetter.h"
#include "DataEngine.h"

float getFloatByRegExp(const QString& qsContent,const QString& qsExp, int iIndex = 0)
{
	QRegExp rx(qsExp);
	qsContent.indexOf(rx);
	QStringList listCaps = rx.capturedTexts();
	if(listCaps.size()>iIndex)
	{
		bool bOk = false;
		float fRet = listCaps[1].toFloat(&bOk);
		if(bOk)
		{
//			qDebug()<<fRet;
			return fRet;
		}
	}
	return FLOAT_NAN;
}


QString CBaseInfoGetter::getWebContentByUrl( const QString& qsUrl )
{
	QNetworkAccessManager manager;
	QNetworkRequest req(qsUrl);
	QEventLoop loop;
	QNetworkReply* pReply = manager.get(req);
	QObject::connect(pReply,SIGNAL(finished()),&loop,SLOT(quit()));
	loop.exec();
	QString qsRet = pReply->readAll();


	pReply->deleteLater();
	return qsRet;
}


void CBaseInfoGetter::updateBaseInfoFromEastMoney( qRcvBaseInfoData* pBaseInfo )
{
	QString qsUrl = QString("http://quote.eastmoney.com/%1%2.html").arg(CDataEngine::getMarketStr(pBaseInfo->wMarket)).arg(pBaseInfo->code);
	QString qsContent = getWebContentByUrl(qsUrl);
	if(qsContent.contains("您访问的页面不存在"))
	{
		return;
	}

	{
		//去除“上市时间”前的数据
//		qsContent.remove(QRegExp(".*上市时间"));
	}
	//
	{
		//上市时间 \d{4}年\d{2}月\d{2}日
		QRegExp rx("\\d{4}年\\d{2}月\\d{2}日");
		qsContent.indexOf(rx);
		QStringList listCaps = rx.capturedTexts();
		if(listCaps.size()>0)
		{
			QDate dtTime = QDate::fromString(listCaps[0],"yyyy年MM月dd日");
		}
	}
	{
		//总股本  (?:prozgb.{2,15})(\d+\.\d+)
		float _v = getFloatByRegExp(qsContent,"(?:prozgb.{2,15})(\\d+\\.\\d+)",1);
		if(!_isnan(_v))
		{
			pBaseInfo->fZgb = _v*10000;
		}
	}
	{
		//流通股 (?:proltgb.{2,15})(\d+\.\d+)
		float _v = getFloatByRegExp(qsContent,"(?:proltgb.{2,15})(\\d+\\.\\d+)",1);
		if(!_isnan(_v))
		{
			pBaseInfo->fLtAg = _v*10000;
		}
	}
	{
		//每股收益 (?:每股收益.{2,100})(\d+\.\d+)
		float _v = getFloatByRegExp(qsContent,"(?:每股收益.{2,100})(\\d+\\.\\d+)",1);
		if(!_isnan(_v))
		{
			pBaseInfo->fMgsy = _v;
		}
	}
	{
		//每股净资产 (?:每股净资产.{2,100})(\d+\.\d+)
		float _v = getFloatByRegExp(qsContent,"(?:每股净资产.{2,100})(\\d+\\.\\d+)",1);
		if(!_isnan(_v))
		{
			pBaseInfo->fMgjzc = _v;
		}
	}
	{
		//净资产收益率 (?:净资产收益率.{2,100})(\d+\.\d+)
		float _v = getFloatByRegExp(qsContent,"(?:净资产收益率.{2,100})(\\d+\\.\\d+)",1);
		if(!_isnan(_v))
		{
			pBaseInfo->fJzcsyl = _v;
		}
	}
	{
		//营业收入 (?:营业收入.{2,100})(\d+\.\d+)
		float _v = getFloatByRegExp(qsContent,"(?:营业收入.{2,100})(\\d+\\.\\d+)",1);
		if(!_isnan(_v))
		{
			pBaseInfo->fZyywsr = _v*10000;
		}
	}
	{
		//营业收入增长率 (?:营业收入增长率.{2,100})(\d+\.\d+)
		float _v = getFloatByRegExp(qsContent,"(?:营业收入增长率.{2,100})(\\d+\\.\\d+)",1);
		if(!_isnan(_v))
		{
//			pBaseInfo->fZyywsr = _v*10000;
		}
	}
	{
		//销售毛利率 (?:销售毛利率.{2,100})(\d+\.\d+)
		float _v = getFloatByRegExp(qsContent,"(?:销售毛利率.{2,100})(\\d+\\.\\d+)",1);
		if(!_isnan(_v))
		{
//			pBaseInfo->fZyywsr = _v*10000;
		}
	}
	{
		//净利润 (?:净利润.{2,100})(\d+\.\d+)
		float _v = getFloatByRegExp(qsContent,"(?:净利润.{2,100})(\\d+\\.\\d+)",1);
		if(!_isnan(_v))
		{
			pBaseInfo->fJlr = _v;
		}
	}
	{
		//净利润增长率 (?:净利润增长率.{2,100})(\d+\.\d+)
		float _v = getFloatByRegExp(qsContent,"(?:净利润增长率.{2,100})(\\d+\\.\\d+)",1);
		if(!_isnan(_v))
		{
//			pBaseInfo->fJlr = _v;
		}
	}
	{
		//每股未分配利润 (?:每股未分配利润.{2,100})(\d+\.\d+)
		float _v = getFloatByRegExp(qsContent,"(?:每股未分配利润.{2,100})(\\d+\\.\\d+)",1);
		if(!_isnan(_v))
		{
//			pBaseInfo->fJlr = _v;
		}
	}
}
