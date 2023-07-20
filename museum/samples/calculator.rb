rem +-------------------------------------------------------------------+
rem | RapidCalc                                                         |
rem +-------------------------------------------------------------------+
rem | This script is a simple demonstration on how to implement a sim-  |
rem | ple calculator application using individual dialog programming in |
rem | RapidBATCH 5.0.                                                   |
rem +-------------------------------------------------------------------+

rem *************
rem ** GLOBALS **
rem *************
dec [calc], [op], [delete]

rem *********************
rem ** IMPLEMENTATIONS **
rem *********************
proc createCalcButton: [name], [caption], [x], [y]
    newdialog 'Calc:' # [name], 'button', [x] # '|' # [y] # '|35|35'
    letdialog 'Calc:' # [name], 'caption', [caption]
endproc

proc addCalcDigit: [digit]
    dec [data], [cnt]
    if [delete] = [false] getdialog [data] = 'Calc:Value', 'text'
    cntvar [cnt] = [data], '.'

    if [cnt] = '1' & [digit] = '.' ret

    letdialog 'Calc:Value', 'text', [data] # [digit]
    [delete] = [false]
endproc

proc performCalc
    dec [value]
    getdialog [value] = 'Calc:Value', 'text'
    if [op] = '+'
        [calc] + [value]
    elseif [op] = '-'
        [calc] - [value]
    elseif [op] = '*'
        [calc] * [value]
    elseif [op] = '/'
        [calc] / [value]
    endif

    letdialog 'Calc:Value', 'text', [calc]
    [op] = ''
endproc

proc setCalcOperation: [operate]
    if [op] ! '' & [delete] = [false]
        performCalc
    else
        getdialog [calc] = 'Calc:Value', 'text'
    endif

    [op] = [operate]
    [delete] = [true]
endproc

rem *****************
rem ** MAIN SCRIPT **
rem *****************

rem create the main window (centered on the screen)
[x] = ([ScreenWidth] / '2') - ('185' / '2')
[y] = ([ScreenHeight] / '2') - ('195' / '2')
newdialog 'Calc', 'dialog', [x] # '|' # [y] # '|185|195'
letdialog 'Calc', 'caption', 'RapidCalc'
letdialog 'Calc', 'style', 'single'

rem create the input window
newdialog 'Calc:Value', 'input', '0|0|193|25'
letdialog 'Calc:Value', 'readonly', [true]
letdialog 'Calc:Value', 'text', '0'

rem create the buttons from 1 to 9
[i] = '0'
[x] = '0'
[y] = '26'
repeat
    [i] + '1'
    createCalcButton 'Num' # [i], [i], [x], [y]
    if ([i] ~ '3') = '0'
        [x] = '0'
        [y] + '35' + '1'
    else
        [x] + '35' + '1'
    endif
until [i] = '9'

rem the 0 button
createCalcButton 'Num0', '0', [x], [y]

rem the comma button
createCalcButton 'Comma', ',', ([x] + '35' + '1'), [y]

rem create the other buttons
[i] = '0'
[x] = ('3' * ('35' + '1'))
[y] = '26'
repeat
    [i] + '1'
    if [i] = '1'
        createCalcButton 'Clear', 'C', [x], [y]
    elseif [i] = '2'
        createCalcButton 'Equal', '=', [x], [y]
    elseif [i] = '3'
        createCalcButton 'Add', '+', [x], [y]
    elseif [i] = '4'
        createCalcButton 'Sub', '-', [x], [y]
    elseif [i] = '5'
        createCalcButton 'Mul', '*', [x], [y]
    elseif [i] = '6'
        createCalcButton 'Div', '/', [x], [y]
    elseif [i] = '7'
        createCalcButton 'Info', 'Info', [x], [y]
    elseif [i] = '8'
        createCalcButton 'Quit', 'Quit', [x], [y]
    endif

    if ([i] ~ '2') = '0'
        [x] = ('3' * ('35' + '1'))
        [y] + '35' + '1'
    else
        [x] + '35' + '1'
    endif
until [i] = '8'

letdialog 'Calc', 'visible', [true]
[delete] = [true]
repeat
    rundialog [event] = '0'
    [num] = '0'  
    
    rem perform a dynamic label call based on the event identifier
    goto [event]

    %click_Calc:Num9
    [num] + '1'
    %click_Calc:Num8
    [num] + '1'
    %click_Calc:Num7
    [num] + '1'
    %click_Calc:Num6
    [num] + '1'
    %click_Calc:Num5
    [num] + '1'
    %click_Calc:Num4
    [num] + '1'
    %click_Calc:Num3
    [num] + '1'
    %click_Calc:Num2
    [num] + '1'
    %click_Calc:Num1
    [num] + '1'
    %click_Calc:Num0
    addCalcDigit [num]
    cont

    %click_Calc:Comma
    addcalcdigit '.'
    cont

    %click_Calc:Clear
    [calc] = '0'
    [delete] = [true]
    letdialog 'Calc:Value', 'text', [calc]
    cont

    %click_Calc:Equal
    if [op] ! '' performCalc
    cont

    %click_Calc:Add
    setcalcoperation '+'
    cont
    %click_Calc:Sub
    setcalcoperation '-'
    cont
    %click_Calc:Mul
    setcalcoperation '*'
    cont
    %click_Calc:Div
    setcalcoperation '/'
    cont

    %click_Calc:Info
    echo 'RapidCalc' # [new_line] # [new_line] # 'Example script written in RapidBATCH 5.0'
    cont

    %click_Calc:Quit
    %close_Calc
until [event] = 'close_Calc' | [event] = 'click_Calc:Quit'
end