#include "FuncDraw.h"
#include "RStockFunc.h"
#include "ColorManager.h"
#include <QtGui>

QMap<const char*,lua_CFunction>* ExportAllFuncs()
{
	QMap<const char*,lua_CFunction>* pFuncs = new QMap<const char*,lua_CFunction>;
	pFuncs->insert("DrawK",&my_lua_drawk);
	pFuncs->insert("DrawLine",&my_lua_drawLine);
	pFuncs->insert("DrawHist",&my_lua_drawHistogram);
	pFuncs->insert("DrawCross",&my_lua_drawCross);
	pFuncs->insert("Color",&my_lua_color);

	return pFuncs;
}

int my_lua_color(lua_State* _L)
{
	int iArgc = lua_gettop(_L);
	if(iArgc<3)
	{
		lua_settop(_L,0);
		lua_pushinteger(_L,rRGB(255,255,255));
		return 1;
	}
	if(lua_type(_L,-1)!=LUA_TNUMBER ||
		lua_type(_L,-2)!=LUA_TNUMBER ||
		lua_type(_L,-3)!=LUA_TNUMBER)
	{
		lua_settop(_L,0);
		lua_pushinteger(_L,rRGB(255,255,255));
		return 1;
	}

	if(iArgc>3)
		lua_pop(_L,iArgc-3);
		
	int iR = lua_tointeger(_L,1);
	int iG = lua_tointeger(_L,2);
	int iB = lua_tointeger(_L,3);
	
	lua_settop(_L,0);
	lua_pushinteger(_L,rRGB(iR,iG,iB));
	return 1;
}

bool getMinAndMax(QVector<float>& _v, float& fMin, float& fMax)
{
	if(_v.size()<1)
		return false;

	if(fMin>fMax)
	{
		fMin = _v.first();
		fMax = _v.first();
	}

	foreach(const float& _f,_v)
	{
		if(fMax<_f)
			fMax = _f;
		if(fMin>_f)
			fMin = _f;
	}

	return true;
}

int my_lua_drawk( lua_State* _L )
{
	int iArgc = lua_gettop(_L);
	if(iArgc<4)
		return 0;
	if(lua_type(_L,-1)!=LUA_TTABLE ||
		lua_type(_L,-2)!=LUA_TTABLE ||
		lua_type(_L,-3)!=LUA_TTABLE ||
		lua_type(_L,-4)!=LUA_TTABLE)
		return 0;

	QVector<float> vOpen,vClose,vHigh,vLow;

	{
		//获取最低价
		int table_index = lua_gettop(_L);
		lua_pushnil(_L);  /* 第一个 key */
		while (lua_next(_L, table_index) != 0) {
			vLow.push_back(lua_tonumber(_L, -1));
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
			/* 移除 'value' ；保留 'key' 做下一次迭代 */
			lua_pop(_L, 1);
		}

		lua_pop(_L,1);
	}
	{
		//获取最高价
		int table_index = lua_gettop(_L);
		lua_pushnil(_L);  /* 第一个 key */
		while (lua_next(_L, table_index) != 0) {
			vHigh.push_back(lua_tonumber(_L, -1));
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
			/* 移除 'value' ；保留 'key' 做下一次迭代 */
			lua_pop(_L, 1);
		}

		lua_pop(_L,1);
	}
	{
		//获取收盘价
		int table_index = lua_gettop(_L);
		lua_pushnil(_L);  /* 第一个 key */
		while (lua_next(_L, table_index) != 0) {
			vClose.push_back(lua_tonumber(_L, -1));
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
			/* 移除 'value' ；保留 'key' 做下一次迭代 */
			lua_pop(_L, 1);
		}

		lua_pop(_L,1);
	}
	{
		//获取开盘价
		int table_index = lua_gettop(_L);
		lua_pushnil(_L);  /* 第一个 key */
		while (lua_next(_L, table_index) != 0) {
			vOpen.push_back(lua_tonumber(_L, -1));
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
			/* 移除 'value' ；保留 'key' 做下一次迭代 */
			lua_pop(_L, 1);
		}

		lua_pop(_L,1);
	}


	lua_getglobal(_L,"_draw");
	RDrawInfo* pDraw = reinterpret_cast<RDrawInfo*>(lua_touserdata(_L,-1));
	lua_pop(_L,1);
	if(!pDraw||(!pDraw->pPainter))
		return 0;


	//读取数据
	QPainter& p = *(pDraw->pPainter);
	QRectF rtClient = pDraw->rtClient;
	float fItemWidth = pDraw->fItemWidth;
	int iEndIndex = pDraw->iEndIndex;
	int iCount = rtClient.width()/fItemWidth + 2;

	//获取最大值和最小值
	float fMin = pDraw->fMin;
	float fMax = pDraw->fMax;
	if(fMin>fMax)
	{
		int iBeginIndex = iEndIndex-iCount;
		if(iBeginIndex<0)
			iBeginIndex = 0;
		getMinAndMax(vHigh.mid(iBeginIndex),fMin,fMax);
		getMinAndMax(vLow.mid(iBeginIndex),fMin,fMax);

		//将最大值和最小值分别扩大5%，方便查看
		fMin = fMin - (fMax-fMin)*0.05;
		fMax = fMax + (fMax-fMin)*0.05;
	}

	{
		p.setPen(QColor(127,0,0));
		p.drawRect(rtClient);

		//设置画笔颜色
		p.setPen(QColor(255,0,0));

		
		float fHighMax = fMax-fMin;
		if(fHighMax<0.00001)
			return 0;
		float fBeginX = rtClient.right()-fItemWidth;
		int iIndex = iEndIndex;
		float fItemHeight = rtClient.height();
		while(fBeginX>rtClient.left() && iIndex>=0)
		{
			//绘制K线
			//float fH = vHigh[iIndex];
			//float fL = vLow[iIndex];
			//float fO = vOpen[iIndex];
			//float fC = vClose[iIndex];
			float fHighY = ((vHigh[iIndex]-fMin)/fHighMax)*fItemHeight;
			float fLowY = ((vLow[iIndex]-fMin)/fHighMax)*fItemHeight;
			float fOpenY = ((vOpen[iIndex]-fMin)/fHighMax)*fItemHeight;
			float fCloseY = ((vClose[iIndex]-fMin)/fHighMax)*fItemHeight;


			if(vClose[iIndex]>=vOpen[iIndex])
			{
				//增长，绘制红色色块
				p.setPen(QColor(255,0,0));

				float fCenterX = fBeginX+fItemWidth/2;
				p.drawLine(fCenterX,rtClient.bottom()-fHighY,fCenterX,rtClient.bottom()-fLowY);		//画最高价到最低价的线

				QRectF rt = QRectF(fBeginX,rtClient.bottom()-fCloseY,fItemWidth,fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);

				if(fItemWidth>5.0)
				{
					rt.adjust(1.0,0,-1.0,0);
				}
				p.fillRect(rt,QColor(0,0,0));
				p.drawRect(rt);
			}
			else
			{
				//降低，绘制蓝色色块
				p.setPen(QColor(0,255,255));

				QRectF rt = QRectF(fBeginX,rtClient.bottom()-fOpenY,fItemWidth,fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));

				if(fItemWidth>3.0)
				{
					rt.adjust(1.0,0,-1.0,0);
				}
				p.fillRect(rt,QColor(0,255,255));
				float fCenterX = fBeginX+fItemWidth/2;
				p.drawLine(fCenterX,rtClient.bottom()-fHighY,fCenterX,rtClient.bottom()-fLowY);		//画最高价到最低价的线
			}

			--iIndex;
			fBeginX-=fItemWidth;
		}
	}


	//将最大值和最小值重新写回去
	pDraw->fMin = fMin;
	pDraw->fMax = fMax;
	return 0;
}

int my_lua_drawLine( lua_State* _L )
{
	int iArgc = lua_gettop(_L);
	if(iArgc<1)
		return 0;
	if(lua_type(_L,-1)!=LUA_TTABLE)
		return 0;

	QVector<float> vValues;
	{
		//获取最低价
		int table_index = lua_gettop(_L);
		lua_pushnil(_L);  /* 第一个 key */
		while (lua_next(_L, table_index) != 0) {
			vValues.push_back(lua_tonumber(_L, -1));
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
			/* 移除 'value' ；保留 'key' 做下一次迭代 */
			lua_pop(_L, 1);
		}

		lua_pop(_L,1);
	}

	lua_getglobal(_L,"_draw");
	RDrawInfo* pDraw = reinterpret_cast<RDrawInfo*>(lua_touserdata(_L,-1));
	lua_pop(_L,1);
	if(!pDraw||(!pDraw->pPainter))
		return 0;


	//获取数据
	QPainter& p = *(pDraw->pPainter);
	QRectF rtClient = pDraw->rtClient;
	float fItemWidth = pDraw->fItemWidth;
	int iEndIndex = pDraw->iEndIndex;
	int iCount = rtClient.width()/fItemWidth + 2;
	QColor clPen = QColor(255,0,0);
	if(pDraw->lsColors.size()>0)
	{
		clPen = QColor::fromRgb(pDraw->lsColors[pDraw->iCurColor%(pDraw->lsColors.size())]);
		pDraw->iCurColor = pDraw->iCurColor+1;
	}

	//获取最大值和最小值
	float fMin = pDraw->fMin;
	float fMax = pDraw->fMax;
	if(fMin>fMax)
	{
		int iBeginIndex = iEndIndex-iCount;
		if(iBeginIndex<0)
			iBeginIndex = 0;
		getMinAndMax(vValues.mid(iBeginIndex),fMin,fMax);

		//将最大值和最小值分别扩大5%，方便查看
		fMin = fMin - (fMax-fMin)*0.05;
		fMax = fMax + (fMax-fMin)*0.05;
	}

	{
		p.setPen(QColor(127,0,0));
		p.drawRect(rtClient);

		//设置画笔颜色
		p.setPen(clPen);

		
		float fHighMax = fMax-fMin;
		if(fHighMax<0.00001)
			return 0;
		float fBeginX = rtClient.right()-fItemWidth;
		int iIndex = iEndIndex;
		float fItemHeight = rtClient.height();
		float fLastValue = rtClient.bottom()+100;
		while(fBeginX>rtClient.left() && iIndex>=0)
		{
			//绘制K线
			float fValue = ((vValues[iIndex]-fMin)/fHighMax)*fItemHeight;

			if(fLastValue<rtClient.bottom())
			{
				float fCenterX = fBeginX+fItemWidth/2;
				p.drawLine(fCenterX+fItemWidth,rtClient.bottom()-fLastValue,fCenterX,rtClient.bottom()-fValue);		//画最高价到最低价的线
			}
			fLastValue = fValue;

			--iIndex;
			fBeginX-=fItemWidth;
		}
	}


	//将最大值和最小值重新写回去
	pDraw->fMin = fMin;
	pDraw->fMax = fMax;
	return 0;
}

int my_lua_drawHistogram( lua_State* _L )
{
	int iArgc = lua_gettop(_L);
	if(iArgc<1)
		return 0;
	if(lua_type(_L,-1)!=LUA_TTABLE)
		return 0;

	QVector<float> vValues;
	{
		//获取最低价
		int table_index = lua_gettop(_L);
		lua_pushnil(_L);  /* 第一个 key */
		while (lua_next(_L, table_index) != 0) {
			vValues.push_back(lua_tonumber(_L, -1));
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
			/* 移除 'value' ；保留 'key' 做下一次迭代 */
			lua_pop(_L, 1);
		}

		lua_pop(_L,1);
	}

	lua_getglobal(_L,"_draw");
	RDrawInfo* pDraw = reinterpret_cast<RDrawInfo*>(lua_touserdata(_L,-1));
	lua_pop(_L,1);
	if(!pDraw||(!pDraw->pPainter))
		return 0;
	
	//获取数据
	QPainter& p = *(pDraw->pPainter);
	QRectF rtClient = pDraw->rtClient;
	float fItemWidth = pDraw->fItemWidth;
	int iEndIndex = pDraw->iEndIndex;
	int iCount = rtClient.width()/fItemWidth + 2;

	//获取最大值和最小值
	float fMin = pDraw->fMin;
	float fMax = pDraw->fMax;
	if(fMin>fMax)
	{
		int iBeginIndex = iEndIndex-iCount;
		if(iBeginIndex<0)
			iBeginIndex = 0;
		getMinAndMax(vValues.mid(iBeginIndex),fMin,fMax);

		//将最大值扩大5%，方便查看
		fMax = fMax + (fMax-fMin)*0.05;
	}

	{
		p.setPen(QColor(127,0,0));
		p.drawRect(rtClient);
		
		float fHighMax = fMax-fMin;
		if(fHighMax<0.00001)
			return 0;

		float fBeginX = rtClient.right()-fItemWidth;
		int iIndex = iEndIndex;
		float fItemHeight = rtClient.height();
		while(fBeginX>rtClient.left() && iIndex>=0)
		{
			//绘制K线
			float fValue = ((vValues[iIndex]-fMin)/fHighMax)*fItemHeight;

			p.fillRect(QRectF(fBeginX+0.5,rtClient.bottom()-fValue,fItemWidth-1,fValue),QColor(255,0,0));
			--iIndex;
			fBeginX-=fItemWidth;
		}
	}

	//将最大值和最小值重新写回去
	pDraw->fMin = fMin;
	pDraw->fMax = fMax;
	return 0;
}

int my_lua_drawCross( lua_State* _L )
{
	int iArgc = lua_gettop(_L);
	if(iArgc<2)
		return 0;
	if(lua_type(_L,1)!=LUA_TTABLE ||
		lua_type(_L,2)!=LUA_TTABLE)
		return 0;

	QColor clDot(255,255,255);
	if(iArgc>2)
	{
		if(lua_type(_L,3) == LUA_TNUMBER)
			clDot = QColor::fromRgb(lua_tointeger(_L,3));
		lua_pop(_L,iArgc-2);
	}

	QVector<float> vL1,vL2;

	{
		//获取第一条线
		int table_index = lua_gettop(_L);
		lua_pushnil(_L);  /* 第一个 key */
		while (lua_next(_L, table_index) != 0) {
			vL2.push_back(lua_tonumber(_L, -1));
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
			/* 移除 'value' ；保留 'key' 做下一次迭代 */
			lua_pop(_L, 1);
		}

		lua_pop(_L,1);
	}
	{
		//获取第二条线
		int table_index = lua_gettop(_L);
		lua_pushnil(_L);  /* 第一个 key */
		while (lua_next(_L, table_index) != 0) {
			vL1.push_back(lua_tonumber(_L, -1));
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
			/* 移除 'value' ；保留 'key' 做下一次迭代 */
			lua_pop(_L, 1);
		}

		lua_pop(_L,1);
	}

	lua_getglobal(_L,"_draw");
	RDrawInfo* pDraw = reinterpret_cast<RDrawInfo*>(lua_touserdata(_L,-1));
	lua_pop(_L,1);
	if(!pDraw||(!pDraw->pPainter))
		return 0;


	//读取数据
	QPainter& p = *(pDraw->pPainter);
	QRectF rtClient = pDraw->rtClient;
	float fItemWidth = pDraw->fItemWidth;
	int iEndIndex = pDraw->iEndIndex;
	int iCount = rtClient.width()/fItemWidth + 2;

	//获取最大值和最小值
	float fMin = pDraw->fMin;
	float fMax = pDraw->fMax;
	if(fMin>fMax)
	{
		int iBeginIndex = iEndIndex-iCount;
		if(iBeginIndex<0)
			iBeginIndex = 0;
		getMinAndMax(vL1.mid(iBeginIndex),fMin,fMax);
		getMinAndMax(vL2.mid(iBeginIndex),fMin,fMax);

		//将最大值和最小值分别扩大5%，方便查看
		fMin = fMin - (fMax-fMin)*0.05;
		fMax = fMax + (fMax-fMin)*0.05;
	}

	{
		p.setPen(QColor(127,0,0));
		p.drawRect(rtClient);

		//设置画笔颜色
		p.setPen(QColor(255,0,0));


		float fBeginX = rtClient.right()-fItemWidth;
		int iIndex = iEndIndex;
		float fItemHeight = rtClient.height();

		float fHighMax = fMax-fMin;
		if(fHighMax<0.00001)
			return 0;

		float fLastL1 = 0.0;
		if(vL1.size()>0)
			fLastL1 = ((vL1.first()-fMin)/fHighMax)*fItemHeight;
		float fLastL2 = 0.0;
		if(vL2.size()>0)
			fLastL2 = ((vL2.first()-fMin)/fHighMax)*fItemHeight;

		float fLastX = fBeginX + fItemWidth/2;
		while(fBeginX>rtClient.left() && iIndex>=0)
		{
			//绘制K线
			float fL1Y = ((vL1[iIndex]-fMin)/fHighMax)*fItemHeight;
			float fL2Y = ((vL2[iIndex]-fMin)/fHighMax)*fItemHeight;
			float fCenterX = fBeginX+fItemWidth/2;

			if((fL1Y>fL2Y && fLastL1<=fLastL2) || (fL2Y>fL1Y && fLastL2<=fLastL1))
			{
				//cross;
				double x = ((fLastX - fCenterX) * (fLastX * fL2Y - fCenterX * fLastL2) - (fLastX - fCenterX) * (fLastX * fL1Y - fCenterX * fLastL1))
					/ ((fLastX - fCenterX) * (fLastL1 - fL1Y) - (fLastX - fCenterX) * (fLastL2 - fL2Y));
				double y = ((fLastL1 - fL1Y) * (fLastX * fL2Y - fCenterX * fLastL2) - (fLastX * fL1Y - fCenterX * fLastL1) * (fLastL2 - fL2Y))
					/ ((fLastL1 - fL1Y) * (fLastX - fCenterX) - (fLastX - fCenterX) * (fLastL2 - fL2Y));

				if(!qIsNaN(x) && !qIsNaN(y))
				{
					QPainterPath _path;
					_path.addEllipse(x-3,rtClient.bottom()-y-3,6,6);
					p.fillPath(_path,QBrush(clDot));
				}
			}

			fLastX = fCenterX;
			//
			fLastL1 = fL1Y;
			fLastL2 = fL2Y;

			--iIndex;
			fBeginX-=fItemWidth;
		}
	}


	//将最大值和最小值重新写回去
	pDraw->fMin = fMin;
	pDraw->fMax = fMax;
	return 0;
}
