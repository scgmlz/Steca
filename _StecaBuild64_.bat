set QTDIR=C:\Qt\5.11.2\msvc2017_64
set PATH=C:\Qt\5.11.2\msvc2017_64\bin;C:\Qt\QtIFW-3.0.4\bin;%PATH%
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config Release
ctest -C Release
cpack
