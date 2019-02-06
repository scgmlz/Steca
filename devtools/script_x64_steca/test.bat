cd .build
SET PATH=C:\Qt\5.11.2\msvc2017_64\bin;C:\opt\local_x64\lib;%CD%\bin\Release;%PATH%
ctest -LE Fullcheck -C Release
