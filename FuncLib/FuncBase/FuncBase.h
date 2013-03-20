/************************************************************************/
/* 文件名称：FuncBase.h
/* 创建时间：2013-02-26 03:54
/*
/* 描    述：函数扩展库，实现了基本的函数计算
/************************************************************************/
#include <QtCore>
#include "StockInfoItem.h"

extern "C"{_declspec(dllexport) QMap<const char*,lua_CFunction>* ExportAllFuncs();}

int my_lua_high(lua_State* _L);
int my_lua_low(lua_State* _L);
int my_lua_open(lua_State* _L);
int my_lua_close(lua_State* _L);
