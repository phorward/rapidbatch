rem +-------------------------------------------------------------------+
rem | RapidBATCH v5.1                                                   |
rem +-------------------------------------------------------------------+
rem | Library: Extending String Functions                               |
rem | Version: 1.0                                                      |
rem | Authors: Jan Max Meyer                                            |
rem +-------------------------------------------------------------------+
rem | Copyright (C) 2000-2006 by J.M.K S.F. Software Technologies       |
rem +-------------------------------------------------------------------+
rem | This library is part of the RapidBATCH 5 software package.        |
rem | It may be used within any RapidBATCH script and for free of       |
rem | charge. Changes in this library are allowed for personal purpose. |
rem |                                                                   |
rem | If you find any bugs or wrote own, extending functions, please    |
rem | send us a mail.                                                   |
rem +-------------------------------------------------------------------+

rem *************
rem ** GLOBALS **
rem *************

rem *********************
rem ** IMPLEMENTATIONS **
rem *********************

rem ===================================================================
rem FUNCTION:
rem lcopy
rem
rem DESCRIPTION:
rem Extracts a substring of [chr_cnt]-characters from the left of a
rem string.
rem
rem PARAMETERS:
rem + [val]
rem   The source string.
rem + [chr_cnt]
rem   The number of characters that are returned from the left.
rem
rem RETURN VALUE:
rem The substring copied from the source string.
rem ===================================================================
func lcopy: [val], [chr_cnt]
	if [chr_cnt] < '1' ret [val]
	copyvar [val] = [val], '1', [chr_cnt]
	ret [val]
endfunc

rem ===================================================================
rem FUNCTION:
rem rcopy
rem
rem DESCRIPTION:
rem Extracts a substring of [chr_cnt]-characters from the right of a
rem string.
rem
rem PARAMETERS:
rem + [val]
rem   The source string.
rem + [chr_cnt]
rem   The number of characters that are returned from the right.
rem
rem RETURN VALUE:
rem The substring copied from the source string.
rem ===================================================================
func rcopy: [val], [chr_cnt]
	dec [len]
	getlen [len] = [val]

	if [chr_cnt] > [len] | [chr_cnt] < '1' ret [val]
	
	copyvar [val] = [val], [len] - [chr_cnt] + '1', [chr_cnt]
	ret [val]
endfunc

rem ===================================================================
rem FUNCTION:
rem ltrim
rem
rem DESCRIPTION:
rem Removes leading blank- and tab-characters from a string. Following
rem blanks and tabs are ignored.
rem
rem PARAMETERS:
rem + [val]
rem   The source string.
rem
rem RETURN VALUE:
rem The string without leading blanks.
rem ===================================================================
func ltrim: [val]
	dec [chr], [i], [len]
	getlen [len] = [val]
	
	[i] = '0'
	repeat
		[i] + '1'
		getcharat [chr] = [val], [i]
	until [chr] ! ' ' & [chr] ! [tab] | [i] = [len]
  
	if [i] < [len]
		copyvar [val] = [val], [i], [len]
		ret [val]
	endif
	
	ret ''
endfunc

rem ===================================================================
rem FUNCTION:
rem rtrim
rem
rem DESCRIPTION:
rem Removes following blank- and tab-characters from a string. Leading
rem blanks and tabs are ignored.
rem
rem PARAMETERS:
rem + [val]
rem   The source string.
rem
rem RETURN VALUE:
rem The string without following blanks.
rem ===================================================================
func rtrim: [val]
	dec [chr], [i], [len]
	getlen [len] = [val]

	[i] = [len] + '1'
	repeat
		[i] - '1'
		getcharat [chr] = [val], [i]    
	until [chr] ! ' ' & [chr] ! [tab] | [i] = '0'
	
	if [i] > '0'
		copyvar [val] = [val], '1', [i]
		ret [val]
	endif
	ret ''
endfunc

rem ===================================================================
rem FUNCTION:
rem rgettok
rem
rem DESCRIPTION:
rem Works same like the build-in function GETTOK, but from the right,
rem so the first token returned by RGETTOK is the last token returned
rem by GETTOK.
rem
rem PARAMETERS:
rem + [val]
rem   The string containing the tokens.
rem + [tok]
rem   The token separator substring or character.
rem + [cnt]
rem   The token-number that should be returned from the right.
rem
rem RETURN VALUE:
rem The token that was found from the right.
rem ===================================================================
func rgettok: [val], [tok], [cnt]
	dec [tokcnt]
	
	cntvar [tokcnt] = [val], [tok]
	[tokcnt] + '1'
	
	if [tokcnt] = '1' ret [val]
	
	gettok [val] = [val], [tok], [tokcnt] + '1' - [cnt]
	ret [val]
endfunc

end