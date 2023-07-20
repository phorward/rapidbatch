@echo off
echo Compiling interpreter...
rem cd rb5
call bmake

echo Compiling compiler...
cd ..\rb5c
call bmake
call garbage

echo Compiling runtime library...
cd ..\rb5rtl
call bmake

cd ..\rb5
call garbage

echo Done
pause