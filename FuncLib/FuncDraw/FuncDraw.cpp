#include "FuncDraw.h"
#include "RStockFunc.h"
#include <QtGui>

QMap<const char*,lua_CFunction>* ExportAllFuncs()
{
	QMap<const char*,lua_CFunction>* pFuncs = new QMap<const char*,lua_CFunction>;
	pFuncs->insert("DrawK",&my_lua_drawk);

	return pFuncs;
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

	//获取最大值和最小值
	float fMin = pDraw->fMin;
	float fMax = pDraw->fMax;
	if(fMin>fMax)
	{
		getMinAndMax(vHigh,fMin,fMax);
		getMinAndMax(vLow,fMin,fMax);

		//将最大值和最小值分别扩大5%，方便查看
		fMin = fMin - (fMax-fMin)*0.05;
		fMax = fMax + (fMax-fMin)*0.05;
	}

	{
		QPainter& p = *(pDraw->pPainter);
		QRectF rtClient = pDraw->rtClient;
		float fItemWidth = pDraw->fItemWidth;
		int iEndIndex = pDraw->iEndIndex;

		p.setPen(QColor(127,0,0));
		p.drawRect(rtClient);

		//设置画笔颜色
		p.setPen(QColor(255,0,0));


		float fBeginX = rtClient.right()-fItemWidth;
		int iIndex = iEndIndex;
		float fItemHeight = rtClient.height();
		while(fBeginX>rtClient.left() && iIndex>=0)
		{
			//绘制K线
			float fHighMax = fMax-fMin;
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
				if(int(fItemWidth)%2==0)
				{
					QRectF rt = QRectF(fBeginX+0.5,rtClient.bottom()-fCloseY,fItemWidth-1.0,fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
					p.fillRect(rt,QColor(255,0,0));
				}
				else
				{
					QRectF rt = QRectF(fBeginX,rtClient.bottom()-fCloseY,fItemWidth,fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
					p.fillRect(rt,QColor(255,0,0));
				}
			}
			else
			{
				//降低，绘制蓝色色块
				p.setPen(QColor(0,255,255));
				if(int(fItemWidth)%2==0)
				{
					QRectF rt = QRectF(fBeginX+0.5,rtClient.bottom()-fOpenY,fItemWidth-1.0,fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
					p.fillRect(rt,QColor(0,255,255));
				}
				else
				{
					QRectF rt = QRectF(fBeginX,rtClient.bottom()-fOpenY,fItemWidth,fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
					p.fillRect(rt,QColor(0,255,255));
				}
			}

			float fCenterX = fBeginX+fItemWidth/2;
			p.drawLine(fCenterX,rtClient.bottom()-fHighY,fCenterX,rtClient.bottom()-fLowY);		//画最高价到最低价的线

			--iIndex;
			fBeginX-=fItemWidth;
		}
	}


	//将最大值和最小值重新写回去
	pDraw->fMin = fMin;
	pDraw->fMax = fMax;
	return 0;
}
