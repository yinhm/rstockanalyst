#include <QtCore>
#include <iostream>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

static int pppp(lua_State* _L)
{
	int n = lua_gettop(_L);
	const char* aaa = lua_tostring(_L,0);
	lua_getglobal(_L,"QString");
	QString* p = (QString*)(lua_topointer(_L,-1));
	qDebug()<<*p;
	return 1;
	for (int i=1;i<=n;++i)
	{
		int _type = lua_type(_L,i);
		printf(lua_tostring(_L,i));
	}
	return 0;
}

static int my_lua_add(lua_State* _L)
{
	int sum = lua_tonumber(_L,1) + lua_tonumber(_L,2);
	lua_pushnumber(_L,sum);
	return 1;
}


int main(int argc, char *argv[])
{
	lua_CFunction pFun = &pppp;
	lua_State* L;
	L = luaL_newstate();
	QString qsTitle = "test for pointer to lua";

	lua_register(L,"print",pFun);
	lua_register(L,"RAdd",&my_lua_add);
//	lua_pushlightuserdata(L,&qsTitle);
//	lua_setglobal(L,"QString");
//	luaL_dostring(L,"print(\"Lua worked!!!\\r\\n\")");
	QTime tmBegin = QTime::currentTime();
	luaL_dostring(L,"\
	local sum=0\
	for cnt=1, 10000000, 1 do\
		sum = RAdd(sum,1)\
	end\
	return sum\
		");
	int j = lua_tointeger(L,-1);
	//luaL_error(L,aaa);
	qDebug()<<"result:"<<j<<"\tuse time:"<<QTime::currentTime().msecsTo(tmBegin)<<"ms(lua with c++)";
	tmBegin = QTime::currentTime();
	luaL_dostring(L,"\
	local sum=0\
	for cnt=1, 10000000, 1 do\
		sum = sum + 1\
	end\
	return sum\
		");
	int i = lua_tointeger(L,-1);
	//luaL_error(L,aaa);
	qDebug()<<"result:"<<i<<"\tuse time:"<<QTime::currentTime().msecsTo(tmBegin)<<"ms(only lua)";
	
	tmBegin = QTime::currentTime();
	qint64 sum = 0;
	for(qint64 i=0;i<10000000;++i)
		sum = sum + 1;
	qDebug()<<"result:"<<sum<<"\tuse time:"<<QTime::currentTime().msecsTo(tmBegin)<<"ms(only c++)";

	lua_close(L);
	return 0;
}