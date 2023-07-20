rem +-------------------------------------------------------------------+
rem | Bin/Oct/Hex Converter                                             |
rem +-------------------------------------------------------------------+
rem | This is the number systems conversation utility that is implemen- |
rem | ted in the RapidBATCH user's manual.                              |
rem +-------------------------------------------------------------------+

rem *************
rem ** GLOBALS **
rem *************

rem --- importing library help functions ---
include 'math.rb'
include 'dialog.rb'

rem --- global variables (bases and digits of the number systems) ---
dec [bin], [oct], [hex], [current_base]

rem basis 2 (binary number system)
[bin] = '01'
rem basis 8 (octal number system)
[oct] = '01234567'
rem basis 16 (hexadecimal number system)
[hex] = '0123456789ABCDEF'

rem *********************
rem ** IMPLEMENTATIONS **
rem *********************

rem --- implementation of the procedure "calc_target_value" ---
proc calc_target_value
	dec [decimal], [convert]
	
	rem calculate value in the target number system from the decimal value
	getdialog [decimal] = 'converter:decimal', 'text'
	if [decimal] ! ''
		rem calculating the value
		rem (using the decimal_to_base-library function)
		decimal_to_base [target] = [decimal], [current_base]
		if [target] = '-1' [target] = 'Error'
	endif
	
	rem set the calculated value to the target field
	letdialog 'converter:target', 'text', [target]
endproc

rem --- implementation of the procedure "calc_decimal_value" ---
proc calc_decimal_value
	dec [decimal], [target]
	
	rem calculating the decimal value from the currently activated number system
	getdialog [target] = 'converter:target', 'text'
	if [target] ! ''
		rem calculating the decimal value
		rem (using the base_to_decimal-library function)
		base_to_decimal [decimal] = [target], [current_base]
		if [decimal] = '-1' [decimal] = 'Error'
	endif
	
	rem set the calculated value to the decimal field
	letdialog 'converter:decimal', 'text', [decimal]
endproc

rem --- implementation of the procedure "set_number_system" ---
proc set_number_system: [number_system]

	rem removing the checkmarks from the menu items
	letdialog 'converter:Menu_Decimal -> &Octal', 'checked', [false]
	letdialog 'converter:Menu_Decimal -> &Hexadecimal', 'checked', [false]
	letdialog 'converter:Menu_Decimal -> &Binary', 'checked', [false]

	rem updating the label in front of the target fields, check the
	rem particular menu item again
	if [number_system] = [bin]
		letdialog 'converter:l_target', 'caption', 'Binary value:'
		letdialog 'converter:Menu_Decimal -> &Binary', 'checked', [true]
	elseif [number_system] = [oct]
		letdialog 'converter:l_target', 'caption', 'Octal value:'
		letdialog 'converter:Menu_Decimal -> &Octal', 'checked', [true]
	elseif [number_system] = [hex]
		letdialog 'converter:l_target', 'caption', 'Hexadecimal value:'
		letdialog 'converter:Menu_Decimal -> &Hexadecimal', 'checked', [true]
	endif

	rem update the currently used base to the assigned value
	[current_base] = [number_system]
	
	rem recalculate the target value
	calc_target_value
endproc

rem *****************
rem ** MAIN SCRIPT **
rem *****************

rem --- creating and setting up the main window ---
newdialog 'converter', 'dialog', '1|1|270|100'

rem set the dialog's title bar text and style
letdialog 'converter', 'caption', 'Bin/Oct/Hex Converter'
letdialog 'converter', 'style', 'single'

rem assign menu tree
[menu] = '&Mode:Decimal -> &Binary|Decimal -> &Octal|Decimal -> &Hexadecimal;;'
letdialog 'converter', 'menu', [menu]

rem --- creating input fields and labels ---

rem label and input-field for the decimal number
newdialog 'converter:l_decimal', 'label', '5|8|100|22'
letdialog 'converter:l_decimal', 'caption', 'Decimal value:'
newdialog 'converter:decimal', 'input', '110|4|150|25'
letdialog 'converter:decimal', 'font', 'Sans Serif|10|2'
letdialog 'converter:decimal', 'event_change', [true]

rem label and input-field for the number in the target number system
newdialog 'converter:l_target', 'label', '5|34|100|22'
newdialog 'converter:target', 'input', '110|30|150|25'
letdialog 'converter:target', 'font', 'Sans Serif|10|2'
letdialog 'converter:target', 'event_change', [true]

rem by default, use binary number system
set_number_system [bin]

rem --- centering the dialog on the screen and makeing it visible ---
centerdialog 'converter'
letdialog 'converter', 'visible', [true]
letdialog 'converter:decimal', 'focus', [true]

rem --- start the dialog event handling loop ---
repeat
	rundialog [event] = '0'
	
	if [event] = 'change_converter:decimal'
		calc_target_value
	elseif [event] = 'change_converter:target'
		calc_decimal_value
	elseif [event] = 'click_converter:Menu_Decimal -> &Binary'
		set_number_system [bin]
	elseif [event] = 'click_converter:Menu_Decimal -> &Octal'
		set_number_system [oct]
	elseif [event] = 'click_converter:Menu_Decimal -> &Hexadecimal'
		set_number_system [hex]	
	endif
	
until [event] = 'close_converter'
end