#include "FuncBase.h"

int ExportAllFuncs(QMap<QString,lua_CFunction>& mapFuncs)
{
	mapFuncs.insert("RAdd",&my_lua_add);

	CStockInfoItem* pItem = new CStockInfoItem("aaa",1);


	return mapFuncs.size();
}

static int my_lua_add(lua_State* _L)
{
	int sum = lua_tonumber(_L,1) + lua_tonumber(_L,2);
	lua_pushnumber(_L,sum);
	return 1;
}