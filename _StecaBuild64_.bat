set QTDIR=C:\Qt\5.10.1\msvc2017_64
set PATH=C:\Qt\5.10.1\msvc2017_64\bin;%PATH%
cmake -G "Visual Studio 15 2017" -A x64 -T host=x64 ..\Steca-juanmcloaiza -DBUILD_SHARED_LIBS=OFF
cmake --build . --config Release