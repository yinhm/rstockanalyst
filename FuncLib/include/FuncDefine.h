#include <QtCore>
#include "StockDrv.h"
#include "StockInfoItem.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};



typedef QMap<QString,lua_CFunction> ExportFuncs;