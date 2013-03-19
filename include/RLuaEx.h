#ifndef _RLUA_EX_H_
#define _RLUA_EX_H_
#include <QtCore>
#include "rsd_global.h"
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};


class RSDSHARED_EXPORT RLuaEx
{
public:
	static void LuaPushArray(lua_State* _L,const char* _t,QVector<float>& _v);
	static void LuaPopArray(lua_State* _L,const char* _t,QVector<float>& _v);
	static void LuaRetArray(lua_State* _L,QVector<float>& _v);
};

#endif	//_RLUA_EX_H_