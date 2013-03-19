#include "FuncBase.h"
#include "RStockFunc.h"

int ExportAllFuncs(QMap<QString,lua_CFunction>& mapFuncs)
{
	mapFuncs.insert("HIGH",&my_lua_high);
	mapFuncs.insert("LOW",&my_lua_low);
	mapFuncs.insert("OPEN",&my_lua_open);
	mapFuncs.insert("CLOSE",&my_lua_close);

	return mapFuncs.size();
}

int my_lua_high( lua_State* _L )
{
	lua_getglobal(_L,"_calc");
	RCalcInfo* pCalc = reinterpret_cast<RCalcInfo*>(lua_touserdata(_L,-1));
	if(pCalc)
	{
		luaL_dostring(_L,"return Array.create()");
		int table_index = lua_gettop(_L);

		QMap<time_t,RStockData>::iterator iter = pCalc->mapData->begin();
		int _i = 1;
		while(iter!=pCalc->mapData->end())
		{
			lua_pushnumber(_L,iter->fHigh);
			lua_rawseti(_L,table_index,_i);
			++iter;
			++_i;
		}
	}

	return 1;
}

int my_lua_low( lua_State* _L )
{
	lua_getglobal(_L,"_calc");
	RCalcInfo* pCalc = reinterpret_cast<RCalcInfo*>(lua_touserdata(_L,-1));
	if(pCalc)
	{
		luaL_dostring(_L,"return Array.create()");
		int table_index = lua_gettop(_L);

		QMap<time_t,RStockData>::iterator iter = pCalc->mapData->begin();
		int _i = 1;
		while(iter!=pCalc->mapData->end())
		{
			lua_pushnumber(_L,iter->fLow);
			lua_rawseti(_L,table_index,_i);
			++iter;
			++_i;
		}
	}

	return 1;
}

int my_lua_open( lua_State* _L )
{
	lua_getglobal(_L,"_calc");
	RCalcInfo* pCalc = reinterpret_cast<RCalcInfo*>(lua_touserdata(_L,-1));
	if(pCalc)
	{
		luaL_dostring(_L,"return Array.create()");
		int table_index = lua_gettop(_L);

		QMap<time_t,RStockData>::iterator iter = pCalc->mapData->begin();
		int _i = 1;
		while(iter!=pCalc->mapData->end())
		{
			lua_pushnumber(_L,iter->fOpen);
			lua_rawseti(_L,table_index,_i);
			++iter;
			++_i;
		}
	}

	return 1;
}

int my_lua_close( lua_State* _L )
{
	lua_getglobal(_L,"_calc");
	RCalcInfo* pCalc = reinterpret_cast<RCalcInfo*>(lua_touserdata(_L,-1));
	if(pCalc)
	{
		luaL_dostring(_L,"return Array.create()");
		int table_index = lua_gettop(_L);

		QMap<time_t,RStockData>::iterator iter = pCalc->mapData->begin();
		int _i = 1;
		while(iter!=pCalc->mapData->end())
		{
			lua_pushnumber(_L,iter->fClose);
			lua_rawseti(_L,table_index,_i);
			++iter;
			++_i;
		}
	}

	return 1;
}
