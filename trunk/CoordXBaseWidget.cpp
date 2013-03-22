#include "StdAfx.h"
#include "CoordXBaseWidget.h"
#include "DataEngine.h"
extern lua_State* g_Lua;
extern QString g_native;
extern luaL_Reg* g_pFuncs;

CCoordXBaseWidget::CCoordXBaseWidget(CBaseWidget* parent /*= 0*/, RWidgetType type /*= CBaseWidget::Basic*/)
	: CBaseWidget(parent,type)
	, m_typeCircle(Min1)

{
	{
		m_pL = luaL_newstate();
		luaL_openlibs(m_pL);
		luaL_dostring(m_pL,g_native.toLocal8Bit());
		if(lua_type(m_pL,-1)==LUA_TSTRING)
		{
			const char* aa = lua_tostring(m_pL,-1);
			QString ss = QString::fromLocal8Bit(aa);
			qDebug()<<"Load native.lua error:"<<ss;
		}
		lua_pushglobaltable(m_pL);
		luaL_setfuncs(m_pL, g_pFuncs,0);
		lua_pop(m_pL,1);
	}
	//m_pL = lua_newthread(g_Lua);
}


CCoordXBaseWidget::~CCoordXBaseWidget(void)
{
}
