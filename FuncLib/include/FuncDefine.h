#include <QtCore>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};



typedef QMap<QString,lua_CFunction> ExportFuncs;