ECHO Storing current directory in VSCMD_START_DIR
SET "VSCMD_START_DIR=%CD%"
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
cd .build
SET QTDIR=C:\Qt\5.11.2\msvc2017_64
SET PATH=C:\Qt\5.11.2\msvc2017_64\bin;C:\opt\local_x64\lib;%PATH%
SET CMAKE_PREFIX_PATH=C:\opt\local_x64;%CMAKE_PREFIX_PATH%

cmake -G "Visual Studio 15 2017" -A x64 -T host=x64 ../../build -DBUILD_SHARED_LIBS=OFF
