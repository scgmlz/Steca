set QTDIR=C:\Qt\5.11.2\msvc2017_64
set PATH=C:\Qt\5.11.2\msvc2017_64\bin;C:\yaml.win64\local_x64;C:\Qt\QtIFW-3.0.4\bin;%PATH%
cmake -G "Visual Studio 15 2017" -A x64 -T host=x64 .. -DBUILD_SHARED_LIBS=OFF
cmake --build . --config Release
ctest -C Release
cpack
