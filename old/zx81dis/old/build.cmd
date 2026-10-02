mkdir build\Debug
mkdir build\Release
make
for %%f in (t\input\*.input) do ..\zx81bas\build\Release\z88dk-zx81bas %%f
