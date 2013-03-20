#include "StdAfx.h"
#include "RLuaEx.h"

void RLuaEx::LuaPushArray( lua_State* _L,const char* _t,QVector<float>& _v )
{
	luaL_dostring(_L,"return Array.create()");
	if(lua_type(_L,-1)!=LUA_TTABLE)
		return;

	int table_index = lua_gettop(_L);

	for(int i = 0;i<_v.size();++i)
	{
		lua_pushnumber(_L,_v[i]);
		lua_rawseti(_L,table_index,i+1);
	}
	lua_setglobal(_L,_t);
}

void RLuaEx::LuaPopArray( lua_State* _L,const char* _t,QVector<float>& _v )
{
	lua_getglobal(_L,_t);
	if(lua_type(_L,-1)!=LUA_TTABLE)
	{
		lua_pop(_L,1);
		return;
	}

	int table_index = lua_gettop(_L);

	lua_pushnil(_L);  /* 第一个 key */

	while (lua_next(_L, table_index) != 0) {
		_v.push_back(lua_tonumber(_L, -1));
		/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
		/* 移除 'value' ；保留 'key' 做下一次迭代 */
		lua_pop(_L, 1);
	}
	lua_pop(_L,1);
}


void RLuaEx::LuaRetArray( lua_State* _L,QVector<float>& _v )
{
	if(lua_type(_L,-1)!=LUA_TTABLE)
		return;
	int table_index = lua_gettop(_L);

	lua_pushnil(_L);  /* 第一个 key */
	while (lua_next(_L, table_index) != 0) {
		_v.push_back(lua_tonumber(_L, -1));
		/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
		/* 移除 'value' ；保留 'key' 做下一次迭代 */
		lua_pop(_L, 1);
	}
}
