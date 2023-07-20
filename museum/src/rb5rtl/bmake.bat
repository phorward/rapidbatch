@echo off
rem Pseudo-Make
setlocal
set PATH=C:\Borland\bcc55\bin;%PATH%
set INCLUDE=C:\Borland\bcc55\include;%INCLUDE%
set LIB=C:\Borland\bcc55\lib;%LIB%
make -frb5rtl.bpr -DBCB=C:\Borland\bcc55
if "%ERRORLEVEL%"=="0" testcopy
endlocal
