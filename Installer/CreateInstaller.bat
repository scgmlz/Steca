set QTINSTALLERFRAMEWORKBIN=
set PATH=%QTINSTALLERFRAMEWORKBIN%;%PATH%
set EXEDIR=..\..\build-Steca\main\Release\
set EXENAME=Steca.exe
set EXEPATH=%EXEDIR%\%EXENAME%
set PACKAGES=.\packages
set PACK1=fzj.jcns.scg.steca

set DATA=%PACKAGES%\%PACK1%\data
copy %EXEPATH% %DATA%
windeployqt %DATA%\%EXENAME%

echo "Installer is being created..." 
binarycreator --offline-only -c .\config.xml -p %PACKAGES% Install_%EXENAME%

@RD /S /Q %DATA%
mkdir %DATA%
