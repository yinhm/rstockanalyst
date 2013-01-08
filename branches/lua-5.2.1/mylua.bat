mkdir include
mkdir lib
mkdir bin
cd src 
cl /O2 /W3 /c l*.c 
del lua.obj luac.obj 
link /LIB /out:../lib/lua52.lib l*.obj 
cl /O2 /W3 /c lua.c 
link /out:../bin/lua.exe lua.obj ../lib/lua52.lib 
cd .. 
copy /y src\lua.h include\
copy /y src\lualib.h include\
copy /y src\lauxlib.h include\
copy /y src\luaconf.h include\
copy /y src\lauxlib.h include\
copy /y src\luaconf.h include\