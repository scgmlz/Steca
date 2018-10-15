set QTDIR=C:\Qt5\5.11.2\msvc2017_64
set PATH=C:\Qt5\5.11.2\msvc2017_64\bin;%PATH%
cmake -G "Visual Studio 15 2017" -A x64 -T host=x64 .. -DBUILD_SHARED_LIBS=OFF
cmake --build . --config Release
