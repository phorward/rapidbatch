@echo off
if exist rbatch_ger.lng goto german

move rbatch.lng rbatch_ger.lng
move rbatch_en.lng rbatch.lng
echo English set
pause
exit

:german
move rbatch.lng rbatch_en.lng
move rbatch_ger.lng rbatch.lng
echo German set
pause
cls