rem +-------------------------------------------------------------------+
rem | RapidBATCH v5.1                                                   |
rem +-------------------------------------------------------------------+
rem | Library: Array Help Functions                                     |
rem | Version: 1.1                                                      |
rem | Authors: Jan Max Meyer, Martin Stassig                            |
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
rem PROCEDURE:
rem dirtoarray
rem
rem DESCRIPTION:
rem This procedure uses the FINDFILES function to read a directory's
rem content and export the resulting list of files into an array.
rem
rem PARAMETERS:
rem + [*array]
rem   Pointer to a variable that becomes and receives the file-list
rem + [dir]
rem   Optional path to a directory. Can be left blank.
rem + [filter]
rem   Optional filter; If left blank, the filter '*' is used
rem
rem RETURN VALUE:
rem -
rem ===================================================================
proc dirtoarray: [*array], [dir], [filter]
	dec [tok_pos], [tok],[files]
	
	reset [array]
	
	if [dir] = '' [dir] = [current]
	if [filter] = '' [filter] = '*'
	
	findfiles [files] = [dir] # '\' # [filter], '1'
	[tok_pos] = '1'
	repeat
		gettok [tok] = [files], '|', [tok_pos]
		if [tok] ! ''
			[array[tok_pos]] = [tok]
		endif
		[tok_pos] = [tok_pos] + '1'
	until [tok] = ''
endproc

rem ===================================================================
rem PROCEDURE:
rem viewarray
rem
rem DESCRIPTION:
rem Displays all elements of an array in a messagebox. This procedure
rem should only be used for debug purposes.
rem
rem PARAMETERS:
rem + [*array]
rem   Pointer to an array whose entries will be displayed.
rem
rem RETURN VALUE:
rem -
rem ===================================================================
proc viewarray: [*array]
	if [array:'0'] = '0' ret
	
	dec [i],[text]

	repeat
		[i] = [i] + '1'
		[text] # 'Index ' # [i] # ': ' # [array[i]] # [new_line]
	until [i] = [array'0']
	echo [text]
endproc

rem ===================================================================
rem PROCEDURE:
rem filetoarray
rem
rem DESCRIPTION:
rem Reads a whole file line-by-line and stores each line into a array
rem element.
rem
rem PARAMETERS:
rem + [*array]
rem   Pointer to a variable that receives the file's lines as an array
rem + [file]
rem   The filename of the file that should be read.
rem
rem RETURN VALUE:
rem [errorcode] will be '-1' if the file was not found, else '0'.
rem ===================================================================
proc filetoarray: [*array], [file]
	dec [i], [res], [textLine]
	fileexists [res] = [file]
	
	reset [array]

	if [res] = '-1'
		[errorcode] = '-1'
	else
		[i] = '0'

		repeat
			[i] + '1'
			readfile [textLine] = [file], [i]
			if [errorcode] ! '-1' [array:[i]] = [textLine]
		until [errorcode] = '-1'
		[errorcode] = '0'
	endif
endproc

rem ===================================================================
rem PROCEDURE:
rem bubblesort
rem
rem DESCRIPTION:
rem Implements the bubble sort algorithm in RapidBATCH to sort 
rem array elements alphanumerically.
rem
rem PARAMETERS:
rem + [*array]
rem   Pointer to an array that should be sorted.
rem
rem RETURN VALUE:
rem -
rem ===================================================================
proc bubblesort: [*array]
	if [array:'0'] = '0' ret
	
	dec [i], [j], [k], [x]
	
	repeat
		[i] = [i] + '1'
		[j] = '0'
		repeat
			[j] = [j] + '1'
			[k] = [j] + '1'
			
			if [array[k]] ! '' & [array[j]] > [array[k]]
				[x] = [array[j]]
				[array[j]] = [array[k]]
				[array[k]] = [x]
			endif
		until [j] = [array'0']
	until [i] = [array'0']
endproc

rem ===================================================================
rem PROCEDURE:
rem quicksort
rem
rem DESCRIPTION:
rem Implements the quick sort algorithm in RapidBATCH to sort 
rem array elements alphanumerically. This method is much faster than
rem the bubble sort algorithm.
rem
rem PARAMETERS:
rem + [*array]
rem   Pointer to an array that should be sorted.
rem
rem RETURN VALUE:
rem -
rem ===================================================================
proc quicksort: [*array], [from], [to]
	dec [i], [j], [testValue], [intMiddle], [vntTemp]
	if [from] < [to]
		[intMiddle] = [from] + [to]
		[intMiddle] = [intMiddle]  / '2'

		getpos [pos] = [intMiddle], '.', '1'
		if [pos] ! '0'
			[pos] - '1'
			copyvar [intMiddle] = [intMiddle], '1', [pos]
		endif

		[testValue] = [array[intMiddle]]
		[i] = [from]
		[j] = [to]

		repeat
			upvar [u1] = [array[i]]
			upvar [u2] = [testValue]

			if [u1] < [u2]
				repeat
					[i] + '1'
					upvar [u1] = [array[i]]
				until [u1] > [u2] | [u1] = [u2]
			endif

			upvar [u3] = [array[j]]
			if [u3] > [u2]
				repeat
					[j] - '1'
					upvar [u3] = [array[j]]
				until [u3] < [u2] | [u3] = [u2]
			endif

			if [i] < [j] | [i] = [j]
				[vntTemp] = [array[j]]
				[array[j]] = [array[i]]
				[array[i]] = [vntTemp]
				[i] + '1'
				[j] - '1'
			endif			
		until [i] > [j]

		if [j] < [intMiddle] | [intMiddle] = [j]
			quickSort [array], [from], [j]
			quickSort [array], [i], [to]
		else
			quickSort [array], [i], [to]
			quickSort [array], [from], [j]
		endif
	endif
endproc

rem ===================================================================
rem PROCEDURE:
rem copyarray
rem
rem DESCRIPTION:
rem Copies one array into another one recoursively, including sub-
rem dimensions.
rem
rem PARAMETERS:
rem + [*src]
rem   Pointer to the source array that is copied.
rem + [*dest]
rem   Pointer to the destination array where [src] is copied to.
rem
rem RETURN VALUE:
rem -
rem ===================================================================
proc copyArray: [*src], [*dest]
	if [src:'0'] = '0' ret

	dec [i]
	[i] = '0'

	repeat
		[i] = [i] + '1'
		[dest:[i]] = [src:[i]]
		if [src:[i],'0'] > '0'
			copyarray [src:[i]], [dest:[i]]
		endif
	until [i] = [src:'0']
endproc

rem ===================================================================
rem PROCEDURE:
rem reversearray
rem
rem DESCRIPTION:
rem Flips all entries of an array, so that the last element becomes
rem the first element and the first element the last element.
rem
rem PARAMETERS:
rem + [*array]
rem   Pointer to an array that is flipped.
rem
rem RETURN VALUE:
rem -
rem ===================================================================
proc reversearray: [*array]
	if [array:'0'] = '0' ret
	
	dec [i],[j],[tmparray]
	[i] = [array:'0']
	
	repeat    
		[j] = [j] + '1'
		copyarray [array:[i]], [tmparray:[j]] 
		[i] = [i] - '1'
	until [i] = '0'

	reset [array]
	copyArray [tmparray], [array]
endproc

rem ===================================================================
rem PROCEDURE:
rem deletearrayitem
rem
rem DESCRIPTION:
rem Removes an element from an array. All following elements will
rem automatically moved forward, so that no blank entries result.
rem Sub-dimensions are included when the array is newly arranged.
rem
rem PARAMETERS:
rem + [*array]
rem   Pointer to an array where an element should be removed.
rem + [idx]
rem   The numerical index of the item that should be removed.
rem
rem RETURN VALUE:
rem -
rem ===================================================================
proc deletearrayitem: [*array], [idx]
	if [array:'0'] = '0' ret

	dec [i],[j],[tmparray]
	[i] = '0'
	[j] = '0'
	
	repeat    
		[i] = [i] + '1'
		if [i] ! [idx]
			[j] = [j] + '1'
			[tmparray:[j]] = [array:[i]]
			copyArray [array:[i]], [tmparray:[j]]
		endif
	until [i] = [array:'0']

	reset [array]
	
	if [tmparray:'0'] > '0' copyArray [tmparray], [array]
endproc

rem ===================================================================
rem PROCEDURE:
rem insertarrayitem
rem
rem DESCRIPTION:
rem Inserts an element into an exsting array at the desired position.
rem All existing entries behind the new position are moved backward.
rem Sub-dimensions in the existing elements are automatically
rem included when the array is newly arranged.
rem
rem PARAMETERS:
rem + [*array]
rem   Pointer to an array where an element should be inserted.
rem + [newitem]
rem   An optional value that is written to the new element.
rem   This can be left blank.
rem + [idx]
rem   The numerical index in the array where the item should be
rem   inserted at.
rem
rem RETURN VALUE:
rem -
rem ===================================================================
proc insertarrayitem: [*array], [newitem], [idx]
	dec [i]
	if [idx] < '0' [idx] = '1'
	
	if [idx] < [array:'0']+'1'
		[i] = [array:'0'] + '1'
		repeat
			[i] - '1'
			copyarray [array:[i]], [array:[i]+'1']
		until [i] = [idx]
	endif
	
	[array:[idx]] = [newitem]
endproc

rem ===================================================================
rem PROCEDURE:
rem listtoarray
rem
rem DESCRIPTION:
rem Converts a list (a string, where values are seperated by a special
rem separation character) into an array. Each value from the list will
rem be put each array element. Empty values from the list are ignored!
rem
rem PARAMETERS:
rem + [*array]
rem   Pointer to a variable that receives the entries.
rem + [list]
rem   The string of the sepeated values.
rem + [sep]
rem   The separation character.
rem
rem RETURN VALUE:
rem -
rem ===================================================================
proc listtoarray: [*array], [list], [sep]	
	dec [i],[cnt],[tmpval]
	[i] = '0'

	cntvar [cnt] = [list], [sep]
	repeat
		[i] = [i] + '1'
		gettok [tmpval] = [list], [sep], [i]
		[array[i]] = [tmpval]
	until [i] = [cnt]+'1'
endproc

rem ===================================================================
rem FUNCTION:
rem arraytolist
rem
rem DESCRIPTION:
rem Converts an array to a list (a string, where each element of the
rem array is separated by a separation character or string).
rem
rem PARAMETERS:
rem + [*array]
rem   Pointer to the array whose entries will be converted to the
rem   list.
rem + [sep]
rem   Separation character that separates the values in the list.
rem	If [sep] is left empty, arraytolist automatically uses the 
rem   [std_sep] separation character.
rem
rem RETURN VALUE:
rem The list of items that is generated from the array.
rem ===================================================================
func arraytolist: [*array], [sep]
	if [array:'0'] = '0' ret ''
	if [sep] = '' [sep] = [std_sep]
	
	dec [i],[tmpval]
	[i] = '0'

	repeat
		if [tmpval] ! '' [tmpval] # [sep]
		[i] = [i] + '1'
		[tmpval] # [array[i]]
	until [i] = [array'0']

	ret [tmpval]
endfunc
end