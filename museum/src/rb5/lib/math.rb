rem +-------------------------------------------------------------------+
rem | RapidBATCH v5.1                                                   |
rem +-------------------------------------------------------------------+
rem | Library: Mathematical functions                                   |
rem | Version: 1.2                                                      |
rem | Authors: Jan Max Meyer, Stephan Bernhardt                         |
rem +-------------------------------------------------------------------+
rem | Copyright (C) 2000-2006 by J.M.K S.F. Software Technologies	      |
rem +-------------------------------------------------------------------+
rem | This library is part of the RapidBATCH 5 software package.        |
rem | It may be used within any RapidBATCH script and for free of	      |
rem | charge. Changes in this library are allowed for personal purpose. |
rem |                                                                   |
rem | If you find any bugs or wrote own, extending functions, please    |
rem | send us a mail.                                                   |
rem +-------------------------------------------------------------------+

rem *************
rem ** GLOBALS **
rem *************
[pi] = '3.14159'

rem *********************
rem ** IMPLEMENTATIONS **
rem *********************

rem ===================================================================
rem FUNCTION:
rem pow
rem
rem DESCRIPTION:
rem Calculates the power of a value to the specified base.
rem
rem PARAMETERS:
rem + [val]
rem   Source value to be calculated.
rem + [power]
rem   The power that shoud be calculated.
rem
rem RETURN VALUE:
rem The calculated value. If [power] is lower than 0, the value -1
rem will be returned.
rem ===================================================================
func pow: [val], [power]
	dec [res]
	if [power] < '0'	ret '-1'
	if [power] = '0'	ret '1'
	if [power] < '2'	ret [val]

	gettok [power] = [power],'.','1'
	[res] = [val]
	repeat
		[res] = [res] * [val]
		[power] = [power] - '1'
	until [power] = '1'
	ret [res]
endfunc

rem ===================================================================
rem FUNCTION:
rem sgn
rem
rem DESCRIPTION:
rem Determines if a value is positive, negative or zero.
rem
rem PARAMETERS:
rem + [val]
rem   A value that should be tested.
rem
rem RETURN VALUE:
rem -1 = negative
rem  0 = zero
rem  1 = positive
rem ===================================================================
func sgn: [val]
	if [val] > '0'
		ret '1'
	elseif [val] < '0'
		ret '-1'
	endif
	
	ret '0'
endfunc

rem ===================================================================
rem FUNCTION:
rem floor
rem
rem DESCRIPTION:
rem Returns the lower integer number of a value.
rem
rem PARAMETERS:
rem + [val]
rem   The value base.
rem
rem RETURN VALUE:
rem The next lower integer number of the specified value.
rem ===================================================================
func floor: [val]
	dec [pos]
	getpos [pos] = [val], '.', '1'

	if [pos] > '0'
		gettok [val] = [val], '.', '1'
	endif

	ret [val]
endfunc

rem ===================================================================
rem FUNCTION:
rem ceil
rem
rem DESCRIPTION:
rem Returns the upper integer number of a value.
rem
rem PARAMETERS:
rem + [val]
rem   The value base.
rem
rem RETURN VALUE:
rem The next upper integer number of the specified value.
rem ===================================================================
func ceil: [val]
	dec [pos]

	getpos [pos] = [val], '.', '1'
	if [pos] > '0'
		gettok [val] = [val], '.', '1'
	endif
	
	ret [val] + '1'
endfunc

rem ===================================================================
rem FUNCTION:
rem base_to_decimal
rem
rem DESCRIPTION:
rem Converts a value of any number system to decimal.
rem Example to convert the hex-value 9C to decimal:
rem    base_to_decimal [val] = '9C', '0123456789ABCDEF'
rem will result: 156
rem
rem Which base the source number system uses is determined by the
rem number of digits (here: 0-F)
rem
rem PARAMETERS:
rem + [value]
rem   A value to be converted in the source number system.
rem + [digits]
rem   All possible digits of the source numeric system. The number of
rem   these digits define the base of the source number system, e.g.
rem   '0123456789ABCDEF' are 16 digits => Hexadecimal number system.
rem
rem RETURN VALUE:
rem The converted value in decimal.
rem '-1' is returned in case of an error.
rem ===================================================================
func base_to_decimal: [value], [digits]
	if [value] = '' | [digits] = '' ret '-1'
	
	upvar [value] = [value]
	upvar [digits] = [digits]
	
	dec [base], [vdigit], [result]
	dec [p], [i], [j]
	
	getlen [base] = [digits]
	getlen [i] = [value]
	[j] = '0'
	
	repeat
		getcharat [vdigit] = [value], [i]
		pow [p] = [base], [j]

		getpos [vdigit] = [digits], [vdigit], '1'
		if [vdigit] = '0' ret '-1'
		[result] + (([vdigit]-'1') * [p])
		
		[i] - '1'
		[j] + '1'
	until [i] = '0'

	ret [result]
endfunc

rem ===================================================================
rem FUNCTION:
rem decimal_to_base
rem
rem DESCRIPTION:
rem Converts a decimal value to any number system.
rem Example to convert the value 255 to hexadecimal:
rem    decimal_to_base [hex] = '255', '0123456789ABCDEF'
rem results: FF
rem Which base the source number system uses is determined by the
rem number of digits (here: 0-F)
rem
rem PARAMETERS:
rem + [value] 
rem   A value in decimal to be converted in the target number system.
rem + [digits]
rem   All possible digits of the source numeric system. The number of
rem   these digits define the base of the source number system, e.g.
rem   '0123456789ABCDEF' are 16 digits => Hexadecimal number system.
rem
rem RETURN VALUE:
rem The converted value in the target number system.
rem ===================================================================
func decimal_to_base: [value], [digits]
	if [value] = '' | [digits] = '' ret '-1'

	dec [base], [result], [rest], [digit], [i], [len]
	
	[i] = '0'
	getlen [len] = [value]
	repeat
		[i] + '1'
		getcharat [digit] = [value], [i]
		getpos [digit] = '0123456789', [digit], '1'
		if [digit] = '0' ret '-1'
	until [i] = [len]
	
	getlen [base] = [digits]
	
	repeat
		[value] = [value] / [base]
		gettok [rest] = [value], '.', '2'
		if [rest] ! ''
			[rest] = ('0.' # [rest]) * [base]
			getcharat [rest] = [digits], [rest]+'1'
			[result] = [rest] # [result]
			gettok [value] = [value], '.', '1'
		else
			getcharat [rest] = [digits], '0'+'1'
			[result] = [rest] # [result]
		endif
	until [value] = '0'

	ret [result]
endfunc

rem ===================================================================
rem FUNCTION:
rem fac
rem
rem DESCRIPTION:
rem Calculates the faculty.
rem
rem PARAMETERS:
rem + [value] 
rem   The value to be calculated.
rem
rem RETURN VALUE:
rem The faculty of the specified value.
rem ===================================================================
func fac: [val]
	dec [res]
	if [val] < '0' ret '-1'
	if [val] = '0' ret '0'
	if [val] < '2' ret '1'

	gettok [val] = [val],'.','1'
	[res] = '1'
	repeat
		[res] = [res] * [val]
		[val] - '1'
	until [val] = '1'
	ret [res]
endfunc

rem ===================================================================
rem FUNCTION:
rem sin
rem
rem DESCRIPTION:
rem Calculates the sinus-value.
rem
rem PARAMETERS:
rem + [val] 
rem   Angle in RAD
rem
rem RETURN VALUE:
rem The sinus-value of the specified angle.
rem ===================================================================
func sin: [val]
	dec [i], [vorz], [x], [v], [a], [b]

	if [val] > ('2'*[pi])
		repeat
			[val] - ('2'*[pi])
		until [val] < ('2'*[pi])
	elseif [val] < '0'
		repeat
			[val] + ('2'*[pi])
		until [val] > '0' | [val] = '0'
	endif
	
	[vorz] = '1'
	if [val] > ([pi]*'3'/'2')
		[vorz] = '-1'
		[val] = ('2'*[pi]) - [val]
	elseif [val] > [pi]
		[vorz] = '-1'
		[val] = [val] - [pi]
	elseif [val] > ([pi]/'2')
		[val] = [pi] - [val]
	endif
	if [val] = '0'	ret '0'
	if [val] = ([pi]/'2')	ret '1'*[vorz]
	
	[i] = '1'
	[v] = '1'
	[x] = '0'
	repeat
		fac [a] = [i]
		pow [b] = [val], [i]
		[x] + ([v]*[b]/[a])
		if [v]='1'
			[v]='-1'
		else
			[v]='1'
		endif
		[i] + '2'
	until [i] > '13'
	[x] * [vorz]
	ret [x]
endfunc

rem ===================================================================
rem FUNCTION:
rem cos
rem
rem DESCRIPTION:
rem Calculates the cosinus-value.
rem
rem PARAMETERS:
rem + [val] 
rem   Angle in RAD
rem
rem RETURN VALUE:
rem The cosinus-value of the specified angle.
rem ===================================================================
func cos: [val]
	dec [i], [vorz], [x], [v], [a], [b]

	if [val] > ('2'*[pi])
		repeat
			[val] - ('2'*[pi])
		until [val] < ('2'*[pi])
	elseif [val] < '0'
		repeat
			[val] + ('2'*[pi])
		until [val] > '0' | [val] = '0'
	endif

	[vorz] = '1'
	if [val] > ([pi]*'3'/'2')
		[val] = ('2'*[pi]) - [val]
	elseif [val] > [pi]
		[vorz] = '-1'
		[val] = [val] - [pi]
	elseif [val] > ([pi]/'2')
		[vorz] = '-1'
		[val] = [pi] - [val]
	endif
	if [val] = '0'	ret '1'*[vorz]
	if [val] = ([pi]/'2')	ret '0'

	[i] = '2'
	[v] = '-1'
	[x] = '1'
	repeat
		fac [a] = [i]
		pow [b] = [val], [i]
		[x] + ([v]*[b]/[a])
		if [v]='1'
			[v]='-1'
		else
			[v]='1'
		endif
		[i] + '2'
	until [i] > '12'
	[x] * [vorz]
	ret [x]
endfunc

rem ===================================================================
rem FUNCTION:
rem tan
rem
rem DESCRIPTION:
rem Calculates the tangens-value.
rem
rem PARAMETERS:
rem + [val] 
rem   Angle in RAD
rem
rem RETURN VALUE:
rem The tangens-value of the specified angle.
rem In case of an error, -1 is returned.
rem ===================================================================
func tan: [val]
	dec [s], [c]
	cos [c] = [val]
	if [c	] = '0' ret '-1'
	sin [s] = [val]
	ret ([s]/[c])
endfunc


rem ===================================================================
rem FUNCTION:
rem arcsin
rem
rem DESCRIPTION:
rem Calculates the arcussinus-value.
rem
rem PARAMETERS:
rem + [val] 
rem   Angle in RAD
rem
rem RETURN VALUE:
rem The arcussinus of the specified angle.
rem In case of an error, -1 is returned.
rem ===================================================================
func arcsin: [val]
	dec [i], [vorz], [x], [delta], [s], [c]

	if [val] > '1'	ret '-1'
	if [val] < '-1'	ret '-1'
	if [val] = '1'	ret ([pi]/'2')
	if [val] = '-1'	ret (([pi]*'-1')/'2')
	if [val] = '0'	ret '0'
	
	[x] = [val]
	repeat
		sin [s] = [x]
		cos [c] = [x]
		[delta] = ([s]-[val]) / [c]
		[x] - [delta]
		[i] + '1'
		rem 20 iterations are enough!
	until [i]='20' | [delta]='0'
	ret [x]
endfunc

rem ===================================================================
rem FUNCTION:
rem arccos
rem
rem DESCRIPTION:
rem Calculates the arcuscosinus-value. (Cosinus-value '-1' to '1')
rem
rem PARAMETERS:
rem + [val] 
rem   Angle in RAD
rem
rem RETURN VALUE:
rem Calculated value ('0' to [pi])
rem In case of an error, -1 is returned.
rem ===================================================================
func arccos: [val]
	dec [i], [vorz], [x], [delta], [s], [c]

	if [val] > '1'	ret '-1'
	if [val] < '-1'	ret '-1'
	if [val] = '1'	ret '0'
	if [val] = '0'	ret ([pi]/'2')
	if [val] = '-1'	ret [pi]
	
	[x] = [val]
	repeat
		sin [s] = [x]
		cos [c] = [x]
		[delta] = ([c]-[val]) / ('0'-[s])
		[x] - [delta]
		[i] + '1'
	until [i]='10' | [delta]='0'
	ret [x]
endfunc

rem ===================================================================
rem FUNCTION:
rem arctan
rem
rem DESCRIPTION:
rem Calculates the arcustangens-value. (tangens-value '-1' to '1')
rem
rem PARAMETERS:
rem + [val] 
rem   Angle in RAD
rem
rem RETURN VALUE:
rem Calculated value ('0' to [pi])
rem In case of an error, -1 is returned.
rem ===================================================================
func arctan: [val]
	dec [i], [vorz], [x], [delta], [t], [c]

	if [val] = '0'	ret '0'
	
	rem Warning: Do not modify the starting value [pi] / '2' - 1 Digit!!!
	[x] = '1.570795'	
	if [val] < '0'	[x] = '0'-[x]
	repeat
		tan [t] = [x]
		[delta] = ([t]-[val]) / ([t]*[t]+'1')
		[x] - [delta]
		[i] + '1'
		rem 30 iterations are enough!
	until [i]='30' | [delta]='0'
	ret [x]
endfunc

rem ===================================================================
rem FUNCTION:
rem deg_to_rad
rem
rem DESCRIPTION:
rem Converts an angle-value from RAD to DEG.
rem
rem PARAMETERS:
rem + [val] 
rem   Angle in RAD
rem
rem RETURN VALUE:
rem Angle in DEG.
rem ===================================================================
func deg_to_rad: [val]
	ret ([val]*[pi]/'180')
endfunc

rem ===================================================================
rem FUNCTION:
rem rad_to_deg
rem
rem DESCRIPTION:
rem Converts an angle-value from DEG to RAD.
rem
rem PARAMETERS:
rem + [val] 
rem   Angle in DEG
rem
rem RETURN VALUE:
rem Angle in RAD.
rem ===================================================================
func rad_to_deg: [val]
	ret ([val]/[pi]*'180')
endfunc

rem ===================================================================
rem FUNCTION:
rem sqrt
rem
rem DESCRIPTION:
rem Calculates the square root of a given value.
rem Uses the approximation procedure by Newton.
rem
rem PARAMETERS:
rem + [val] 
rem   Value
rem
rem RETURN VALUE:
rem The calculated square root of the given value.
rem In case of an error, '-1'.
rem ===================================================================
func sqrt: [val]
	dec [i], [x], [delta]
	if [val] < '0'	ret '-1'
	if [val] = '0'	ret '0'
	[x] = [val]
	repeat
		[delta] = ([x]*[x] - [val]) / ('2'*[x])
		[x] - [delta]
		[i] + '1'
	until [i]='100' | [delta]='0'
	ret [x]
endfunc

end