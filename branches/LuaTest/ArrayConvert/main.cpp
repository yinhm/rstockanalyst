#include <QtCore>
#include <QApplication>
#include <iostream>
#include <string>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};
using namespace std;


int createtable( lua_State* _L )
{
	cout<<"create"<<endl;
	lua_newtable(_L);
	lua_pushnumber(_L,-1);
	lua_rawseti(_L,-2,0);

	for(int i=0;i<2;++i)
	{
		lua_pushnumber(_L,i+10);
		lua_rawseti(_L,-2,i+1);
	}
		
	lua_getglobal(_L,"Array");
	lua_setmetatable(_L,-2);
	return 1;
}

void LuaPopArray( lua_State* _L,const char* _t,QVector<float>& _v )
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

int main(int argc, char *argv[])
{
	lua_State* L;
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L,"D:\\lua.lua");

	lua_register(L,"Create",&createtable);
	char* a1 = new char[10];
	char* a2 = new char[10];
	
	for(int i=0;i<2000;++i)
	{
		lua_pushlightuserdata(L,a1);
		lua_setglobal(L,"_calc");

		lua_pushlightuserdata(L,a2);
		lua_setglobal(L,"_draw");

		lua_getglobal(L,"InitValues");
		lua_call(L,0,0);

		luaL_dostring(L,"p1 = (Create()-REF(Create(),1))/Create()");
		int _t = lua_type(L,-1);
		if(_t==LUA_TSTRING)
		{
			cout<<"lua runtime error:"<<lua_tostring(L,-1);
		}
		QVector<float> _vvv;
		LuaPopArray(L,"p1",_vvv);
		if(_vvv.size()>0)
			qDebug()<<_vvv[0];
	}
	return 0;
}


//验证结果：lua的效率完全可以满足要求！！！