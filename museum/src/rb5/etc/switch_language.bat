@echo off
if exist rb5_ger.lng goto german

move rb5.lng rb5_ger.lng
move rb5_en.lng rb5.lng
echo English set
pause
exit

:german
move rb5.lng rb5_en.lng
move rb5_ger.lng rb5.lng
echo German set
pause
cls