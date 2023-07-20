rem +-------------------------------------------------------------------+
rem | RapidBATCH v5.1                                                   |
rem +-------------------------------------------------------------------+
rem | Library: Extending System Functions                               |
rem | Version: 1.2                                                      |
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
rem PROCEDURE:
rem copydir
rem
rem DESCRIPTION:
rem Copies a whole directory, including sub-directories.
rem
rem PARAMETERS:
rem + [source]
rem   Path to the source directory.
rem + [target]
rem   Path to the target directory.
rem
rem RETURN VALUE:
rem [errorcode] gets '0' if no error occured, else '-1'.
rem ===================================================================
proc copydir: [source], [target]
	dec [tok], [i], [files], [isDir]

	findfiles [files] = [source] # '\*', '1'
	if [files] = ''
		[errorcode] = '-1'
		ret
	endif
	mkdir [target]
	repeat
		calcvar [i] = [i] + '1'
		gettok [tok] = [files], '|', [i]
		if [tok] ! ''
			getfileatt [isDir] = [source] # '\' # [tok], 'DIRECTORY'
			if [isDir] = '0'
				copydir [source] # '\' # [tok], [target] # '\' # [tok]
			else
				copyfile [source] # '\' # [tok], [target] # '\' # [tok]
			endif
		endif
	until [tok] = ''
	[errorcode] = '0'
endproc

rem ===================================================================
rem PROCEDURE:
rem copydir
rem
rem DESCRIPTION:
rem Moves a whole directory, including sub-directories.
rem
rem PARAMETERS:
rem + [source]
rem   Path to the source directory.
rem + [target]
rem   Path to the target directory.
rem
rem RETURN VALUE:
rem [errorcode] gets '0' if no error occured, else '-1'.
rem ===================================================================
proc moveDir: [source], [target]
	dec [tok], [i], [files], [isDir]

	findfiles [files] = [source] # '\*', '1'
	if [files] = ''
		[errorcode] = '-1'
		ret
	endif
	mkdir [target]
	repeat
		calcvar [i] = [i] + '1'
		gettok [tok] = [files], '|', [i]
		if [tok] ! ''
			getfileatt [isDir] = [source] # '\' # [tok], 'DIRECTORY'
			if [isDir] = '0'
				movedir [source] # '\' # [tok], [target] # '\' # [tok]
			else
				renamefile [source] # '\' # [tok], [target] # '\' # [tok]
			endif
		endif
	until [tok] = ''
	rmdir [source]
	[errorcode] = '0'
endproc


rem ===================================================================
rem PROCEDURE:
rem deldir
rem
rem DESCRIPTION:
rem Deletes a whole directory, including sub-directories.
rem
rem PARAMETERS:
rem + [dir]
rem   Path to the directory to be deleted.
rem
rem RETURN VALUE:
rem [errorcode] gets '0' if no error occured, else '-1'.
rem ===================================================================
proc deldir: [dir]
	dec [tok], [i], [files], [isDir]

	findfiles [files] = [dir] # '\*', '1'
	if [files] ! ''
		[i] = '0'
		repeat
			[i] + '1'
			gettok [tok] = [files], '|', [i]
			if [tok] ! ''
				getfileatt [isDir] = [dir] # '\' # [tok], 'DIRECTORY'
				if [isDir] = '0'
					deldir [dir] # '\' # [tok]
					if [errorcode] = [false] ret
				else
					delfile [dir] # '\' # [tok]
					if [errorcode] = [false] ret
				endif
			endif
		until [tok] = ''
	endif

	rmdir [dir]
endproc


rem ===================================================================
rem PROCEDURE:
rem getcmdlineparameters
rem
rem DESCRIPTION:
rem Retrieves command-line parameters and extracts values from it to
rem an array.
rem
rem PARAMETERS:
rem + [line]
rem   String of command-line parameters (e.g. from [command])
rem + [*values]
rem   Pointer to an array that receives the command-line parameteres.
rem
rem RETURN VALUE:
rem none
rem ===================================================================
proc getcmdlineparms: [line], [*values]
	dec [i], [j], [pos], [ch], [len]
	trimvar [line] = [line]
	getlen [len] = [line]

	if [line] = '' ret

	[i] = '0'
	[pos] = '1'

	repeat
		[i] + '1'
		getcharat [ch] = [line], [i]
		
		if [ch] = ' ' cont
		if [ch] ! '"'
			[j] = [i]-'1'
			
			repeat
				[j] + '1'
				getcharat [ch] = [line], [j]
				if [ch] ! ' '
					[values:[pos]] # [ch]
				endif
			until [j] = [len] | [ch] = ' '
			[i] = [j]
		else
			[j] = [i]
			
			repeat
				[j] + '1'
				getcharat [ch] = [line], [j]
				if [ch] ! '"'
					[values:[pos]] # [ch]
				endif
			until [j] = [len] | [ch] = '"'
			[i] = [j]
		endif
		
		[pos] + '1'
	until [i] = [len]
endproc

rem ===================================================================
rem PROCEDURE:
rem chscriptdir
rem
rem DESCRIPTION:
rem Useful function that is used by scripts that need to read files
rem from their own directory where the script is saved to (e.g. lan-
rem guage definition files or images).
rem This procedure switches to the script's save directory by extract-
rem ing the path over the [ownname]-variable.
rem
rem PARAMETERS:
rem
rem RETURN VALUE:
rem [errorcode] gets '0' if no error occured, else '-1'.
rem ===================================================================
proc chscriptdir
	dec [i], [ch], [dir]
	
	getlen [i] = [ownname]
	repeat
		[i] - '1'
		getcharat [ch] = [ownname], [i]
		if [ch] = '\'
			copyvar [dir] = [ownname], '1', [i]-'1'
			chdir [dir]
			break
		endif
	until [i] = '1'
endproc

end