#include "FuncBase.h"
#include "RStockFunc.h"

int ExportAllFuncs(QMap<QString,lua_CFunction>& mapFuncs)
{
	mapFuncs.insert("RAdd",&my_lua_add);
	mapFuncs.insert("HIGH",&my_lua_high);

	return mapFuncs.size();
}

static int my_lua_add(lua_State* _L)
{
	int sum = lua_tonumber(_L,1) + lua_tonumber(_L,2);
	lua_pushnumber(_L,sum);
	return 1;
}

int my_lua_high( lua_State* _L )
{
	lua_getglobal(_L,"_calc");
	RCalcInfo* pCalc = reinterpret_cast<RCalcInfo*>(lua_touserdata(_L,-1));
	if(pCalc)
	{
		luaL_dostring(_L,"return Array.create()");
		int _t = lua_type(_L,-1);
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
