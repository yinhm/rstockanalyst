#include "StdAfx.h"
#include "RLuaEx.h"

void RLuaEx::LuaPushArray( lua_State* _L,const char* _t,QVector<float>& _v )
{
	luaL_dostring(_L,"return Array.create()");
	int table_index = lua_gettop(_L);

	for(int i = 0;i<_v.size();++i)
	{
		lua_pushnumber(_L,_v[i]);
		lua_rawseti(_L,table_index,i+1);
	}
	lua_setglobal(_L,_t);
}
