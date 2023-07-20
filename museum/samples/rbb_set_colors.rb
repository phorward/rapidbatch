rem +-------------------------------------------------------------------+
rem | RBB Set Colors                                                    |
rem +-------------------------------------------------------------------+
rem | This script demonstrates the use of registry-handling functions   |
rem | and build-in as well as extended dialogs. It is a nice tool to    |
rem | change RapidBATCH Builder's standard syntax color theme to a more |
rem | individual or special color theme.                                |
rem +-------------------------------------------------------------------+

include_once 'dialog.rb'

rem *************
rem ** GLOBALS **
rem *************

rem *******************************
rem ** PROCEDURE IMPLEMENTATIONS **
rem *******************************
proc setClassColor: [class_name], [color]
	letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', [class_name], [color]
endproc

func getClassColor: [class_name]
	getregval [val] = 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', [class_name]
	ret [val]
endfunc

btnmenu [style] = 'RBB Color Themes', 'Standard|Ocean|Moonlight|Fire|Individual|Quit'
if [style] = '' halt

if [style] = 'Standard'
	setclasscolor 'CLASS_KEYWORD', '000000'
	setclasscolor 'CLASS_VALUE', '0000ff'
	setclasscolor 'CLASS_VARIABLE', '000080'
	setclasscolor 'CLASS_COMMENT', '808080'
	setclasscolor 'CLASS_SPECIAL_COMMENT', '008000'
	setclasscolor 'CLASS_LABEL', 'ff0000'
	setclasscolor 'CLASS_OPERATOR', '008000'
	setclasscolor 'CLASS_TEXT', '000000'
	setclasscolor 'CLASS_BKGND', 'ffffff'
elseif [style] = 'Ocean'
	setclasscolor 'CLASS_KEYWORD', 'A8D3FF'
	setclasscolor 'CLASS_VALUE', '5FAFAF'
	setclasscolor 'CLASS_VARIABLE', '00b0ff'
	setclasscolor 'CLASS_COMMENT', '000080'
	setclasscolor 'CLASS_SPECIAL_COMMENT', '00ff00'
	setclasscolor 'CLASS_LABEL', 'ffff00'
	setclasscolor 'CLASS_OPERATOR', '00bd00'
	setclasscolor 'CLASS_TEXT', 'A8D3FF'
	setclasscolor 'CLASS_BKGND', '00458A'
elseif [style] = 'Moonlight'
	setclasscolor 'CLASS_KEYWORD', '75BAFF'
	setclasscolor 'CLASS_VALUE', 'FF952B'
	setclasscolor 'CLASS_VARIABLE', 'CACA00'
	setclasscolor 'CLASS_COMMENT', '464646'
	setclasscolor 'CLASS_SPECIAL_COMMENT', '77FFBB'
	setclasscolor 'CLASS_LABEL', 'ff0000'
	setclasscolor 'CLASS_OPERATOR', '0074E8'
	setclasscolor 'CLASS_TEXT', '75BAFF'
	setclasscolor 'CLASS_BKGND', '000000'
elseif [style] = 'Fire'
	setclasscolor 'CLASS_KEYWORD', 'ff3900'
	setclasscolor 'CLASS_VALUE', 'ff7b00'
	setclasscolor 'CLASS_VARIABLE', 'ffff00'
	setclasscolor 'CLASS_COMMENT', '710000'
	setclasscolor 'CLASS_SPECIAL_COMMENT', '00ff00'
	setclasscolor 'CLASS_LABEL', 'ffff00'
	setclasscolor 'CLASS_OPERATOR', 'bd0000'
	setclasscolor 'CLASS_TEXT', 'ff3900'
	setclasscolor 'CLASS_BKGND', '000000'
elseif [style] = 'Individual'
	getclasscolor [pre] = 'CLASS_KEYWORD'
	inputbox [new_value] = 'CLASS_KEYWORD', 'Enter hexadecimal color-value for CLASS_KEYWORD', [pre]
	if [new_value] = [null] halt
	setclasscolor 'CLASS_KEYWORD', [pre]

	getclasscolor [pre] = 'CLASS_VALUE'
	inputbox [new_value] = 'CLASS_VALUE', 'Enter hexadecimal color-value for CLASS_VALUE', [pre]
	if [new_value] = [null] halt
	setclasscolor 'CLASS_VALUE', [new_value]

	getclasscolor [pre] = 'CLASS_VARIABLE'
	inputbox [new_value] = 'CLASS_VARIABLE', 'Enter hexadecimal color-value for CLASS_VARIABLE', [pre]
	if [new_value] = [null] halt
	setclasscolor 'CLASS_VARIABLE', [new_value]

	getclasscolor [pre] = 'CLASS_COMMENT'
	inputbox [new_value] = 'CLASS_COMMENT', 'Enter hexadecimal color-value for CLASS_COMMENT', [pre]
	if [new_value] = [null] halt
	setclasscolor 'CLASS_COMMENT', [new_value]

	getclasscolor [pre] = 'CLASS_SPECIAL_COMMENT'
	inputbox [new_value] = 'CLASS_SPECIAL_COMMENT', 'Enter hexadecimal color-value for CLASS_SPECIAL_COMMENT', [pre]
	if [new_value] = [null] halt
	setclasscolor 'CLASS_SPECIAL_COMMENT', [new_value]

	getclasscolor [pre] = 'CLASS_LABEL'
	inputbox [new_value] = 'CLASS_LABEL', 'Enter hexadecimal color-value for CLASS_LABEL', [pre]
	if [new_value] = [null] halt
	setclasscolor 'CLASS_LABEL', [new_value]

	getclasscolor [pre] = 'CLASS_OPERATOR'
	inputbox [new_value] = 'CLASS_OPERATOR', 'Enter hexadecimal color-value for CLASS_OPERATOR', [pre]
	if [new_value] = [null] halt
	setclasscolor 'CLASS_OPERATOR', [new_value]

	getclasscolor [pre] = 'CLASS_TEXT'
	inputbox [new_value] = 'CLASS_TEXT', 'Enter hexadecimal color-value for CLASS_TEXT', [pre]
	if [new_value] = [null] halt
	setclasscolor 'CLASS_TEXT', [new_value]

	getclasscolor [pre] = 'CLASS_BKGND'
	inputbox [new_value] = 'CLASS_BKGND', 'Enter hexadecimal color-value for CLASS_BKGND', [pre]
	if [new_value] = [null] halt
	setclasscolor 'CLASS_BKGND', [new_value]
else
	halt
endif

echo 'Please restart the RapidBATCH Builder.'
end
