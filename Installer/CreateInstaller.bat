set QTINSTALLERFRAMEWORKBIN=
set PATH=%QTINSTALLERFRAMEWORKBIN%;%PATH%
set EXEDIR=..\build-Steca\main\Release\
set EXENAME=Steca.exe
set EXEPATH=%EXEDIR%\%EXENAME%
set STARTMENU=.\startmenu
set DATA=%STARTMENU%\packages\fzj.jcns.scg.steca\data

copy %EXEPATH% %DATA%
windeployqt %DATA%\%EXENAME%

echo "Installer is being created..." 
binarycreator --offline-only -c %STARTMENU%\config\config.xml -p %STARTMENU%\packages %EXENAME%.Installer

@RD /S /Q %DATA%
mkdir %DATA%