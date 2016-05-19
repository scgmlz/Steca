@echo off
For /f "tokens=1-3 delims=/ " %%a in ('date /t') do (set mydate=%%c%%a%%b)
For /f "tokens=1-2 delims=/:" %%a in ('time /t') do (set mytime=%%a%%b)
ren build.exe STeCa2_%mydate%_%mytime%.exe
