set QTDIR=C:\Qt5\5.11.2\msvc2017_64
set PATH=C:\Qt5\5.11.2\msvc2017_64\lib;C:\yaml.win64\local_x64;C:\Qt5\QtIFW-3.0.4\bin;%PATH%
set Qt5Core_DIR=C:\Qt5\5.11.2\msvc2017_64\lib\cmake
set Qt5Gui_DIR=C:\Qt5\5.11.2\msvc2017_64\lib\cmake
set Qt5Widgets_DIR=C:\Qt5\5.11.2\msvc2017_64\lib\cmake
set Qt5Network_DIR=C:\Qt5\5.11.2\msvc2017_64\lib\cmake
cmake -G "Visual Studio 15 2017" -A x64 -T host=x64 .. -DBUILD_SHARED_LIBS=OFF
cmake --build . --config Release
