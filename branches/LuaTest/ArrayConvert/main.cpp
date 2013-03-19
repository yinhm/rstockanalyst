#include <QtCore>
#include <QApplication>
#include <iostream>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};



int main(int argc, char *argv[])
{
	QApplication app(argc,argv);

	qDebug()<<"Begin:"<<QTime::currentTime().toString("s:z");
	lua_State* L;
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L,QString(qApp->applicationDirPath()+"/lua.lua").toLocal8Bit());


	qDebug()<<"DoFile:"<<QTime::currentTime().toString("s:z");
	int _t = lua_type(L,-1);

	luaL_dostring(L,"return Array.create()");
	_t = lua_type(L,-1);

	int table_index = lua_gettop(L);
	for(int i=0;i<100000;++i)
	{
		lua_pushnumber(L,i);
		lua_rawseti(L,table_index,i+1);
	}
	lua_setglobal(L,"All");

	qDebug()<<"Insert:"<<QTime::currentTime().toString("s:z");

	luaL_dostring(L,"return (All+All)*All");


	qDebug()<<"Calc:"<<QTime::currentTime().toString("s:z");
	_t = lua_type(L,-1);
	table_index = lua_gettop(L);

	lua_pushnil(L);  /* 第一个 key */

	QVector<double> vAll;
	while (lua_next(L, table_index) != 0) {
//		lua_tonumber(L, -1);
		vAll.push_back(lua_tonumber(L, -1));
		/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
		/* 移除 'value' ；保留 'key' 做下一次迭代 */
		lua_pop(L, 1);
	}


	qDebug()<<"Popup:"<<QTime::currentTime().toString("s:z");
//	lua_setmetatable(L,-1);


//	lua_pushinteger(L2,123);
//	lua_setglobal(L,"L2");
//	lua_setfield(L2,LUA_REGISTRYINDEX,"L2");
	/*

	lua_Reader(L2,)
	lua_pushinteger(L2,1);
	lua_setfield(L2,lua_upvalueindex,"L2");

	lua_pushinteger(L2,1);
	lua_pushinteger(L2,2);

	lua_pushinteger(L2,3);


//	lua_getfield(L2,LUA_REGISTRYINDEX,"L2");
	lua_gettable(L2,1);
	int ii = lua_tointeger(L2,-1);


	lua_getfield(L,LUA_REGISTRYINDEX,"L2");
	int ii1 = lua_tointeger(L,-1);


	lua_close(L);*/
	return 0;
}


//验证结果：lua的效率完全可以满足要求！！！