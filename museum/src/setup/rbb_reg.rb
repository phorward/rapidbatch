rem ******************************************************************
rem ** Product:    RapidBATCH 5.0                                   **
rem ** Module:     RapidBATCH Builder User-depended Registry Setup  **
rem ** Version:    1.0                                              **
rem ******************************************************************
rem ** Copyright (C) 2004, 2005 by Jan Max Meyer, J.M.K.S.F.        **
rem ** http://www.jmksf.com/    mail@jmksf.com                      **
rem ******************************************************************
rem ** This script is part of the RapidBATCH software package and   **
rem ** may be modified for personal use only.                       **
rem ******************************************************************

getlen [len] = [command]

if [len] > '0'
	[i] = '0'
	repeat
		[i] + '1'
		getcharat [ch] = [command], [i]
		if [ch] = ' '
			copyvar [setup_path] = [command], [i]+'1', [len]
			replacevar [setup_path] = [setup_path], '"', [null]
			trimvar [setup_path] = [setup_path]
			
			copyvar [command] = [command], '1', [i]
			lowvar [command] = [command]
			trimvar [command] = [command]			
			break
		endif
	until [i] = [len]
endif

if [command] ! 'reg' & [command] ! 'unreg'
	echo 'This script may only be executed by RapidBATCH setup.'
	halt
endif

[reg_cat] = 'Software'
[regpath] = 'jmksf\RapidBATCH'
[toolsdef] = 'etc\tools.def'

if [command] = 'reg'

	infobox 'Preparing RapidBATCH Builder for its first run...', 'show'

	rem first of all, set up the registry keys
	newregkey 'CURRENT_USER', [reg_cat] # '\jmksf\RapidBATCH'

	rem setting up the syntax highlighing classes default colors
	letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'CLASS_KEYWORD', '000000'
	if [errorcode] = [false] goto 'error'
	letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'CLASS_VALUE', '0000ff'
	if [errorcode] = [false] goto 'error'
	letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'CLASS_VARIABLE', '000080'
	if [errorcode] = [false] goto 'error'
	letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'CLASS_COMMENT', '808080'
	if [errorcode] = [false] goto 'error'
	letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'CLASS_SPECIAL_COMMENT', '008000'
	if [errorcode] = [false] goto 'error'
	letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'CLASS_LABEL', 'ff0000'
	if [errorcode] = [false] goto 'error'
	letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'CLASS_OPERATOR', '008000'
	if [errorcode] = [false] goto 'error'
	letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'CLASS_TEXT', '000000'
	if [errorcode] = [false] goto 'error'
	letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'CLASS_BKGND', 'eeeeee'
	if [errorcode] = [false] goto 'error'

	rem setting up the editor's font type
	letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'FontFace', 'Courier New'
	if [errorcode] = [false] goto 'error'

	rem RapidBATCH Builder's install path
rem 	cntvar [i] = [ownname], '\'
rem 	if [i] ! ''
rem 		[path] = ''
rem 		repeat
rem 			[i] - '1'
rem 			gettok [p] = [ownname], '\', [i]
rem 			if [path] ! ''
rem 				[path] = [p] # '\' # [path]
rem 			else
rem 				[path] = [p]
rem 			endif
rem 		until [i] = '1'
rem 		echo [path]
rem 	endif
	
	letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'rb_home', [setup_path]
	
	if [errorcode] = [false] goto 'error'

	[toolsdef] = [setup_path] # '\' # [toolsdef]
	fileexists [f_ex] = [toolsdef]
	if [f_ex] = [true]
		[i] = '0'
		repeat
			[i] + '1'
			readfile [line] = [toolsdef], [i]
			if [errorcode] = [true]
				gettok [name] = [line], ':', '1'
				gettok [path] = [line], ':', '2'
				letregval 'String', 'CURRENT_USER', [reg_cat] # '\' # [regpath], 'Tool' # [i]-'1', [path] # '|' # [name]
				if [errorcode] = [false] goto 'error'
			endif
		until [errorcode] = [false]
	endif
elseif [command] = 'unreg'
	infobox 'Preparing uninstall of RapidBATCH...', 'show'
	
	delregkey 'CURRENT_USER', [reg_cat] # '\jmksf\RapidBATCH'
	delregkey 'CURRENT_USER', [reg_cat] # '\jmksf'
endif

wait '500'	
infobox 'Done!', 'show'
wait '1000'

halt

%error
echo 'ERROR: Unable to write into the registry!' # [new_line] # 'Please contact your system administrator or J.M.K S.F. support.'
end