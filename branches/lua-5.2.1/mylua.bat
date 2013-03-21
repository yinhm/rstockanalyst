@setlocal
@set MYMT=mt /nologo

mkdir include
mkdir lib
mkdir bin
cd src 
cl /MD /O2 /W3 /c /DLUA_BUILD_AS_DLL *.c
del *.o
ren lua.obj lua.o
ren luac.obj luac.o
link /DLL /IMPLIB:../lib/lua52.lib /OUT:../lib/lua52.dll *.obj
%MYMT% -manifest ../lib/lua52.dll.manifest -outputresource:../lib/lua52.dll;2

link /OUT:../bin/lua.exe lua.o ../lib/lua52.lib
%MYMT% -manifest ../bin/lua.exe.manifest -outputresource:../bin/lua.exe

cd ..\lib
del *.manifest
cd ..\bin
del *.manifest

cd .. 
copy /y src\lua.h ..\..\include\
copy /y src\lualib.h ..\..\include\
copy /y src\lauxlib.h ..\..\include\
copy /y src\luaconf.h ..\..\include\
copy /y src\lauxlib.h ..\..\include\
copy /y src\luaconf.h ..\..\include\

copy /y lib\lua52.lib ..\..\bin\
copy /y lib\lua52.dll ..\..\bin\