rem RapidBATCH Builder v4.0.0
rem Copyright (C) 2005 by J.M.K S.F. Software Technologies
rem
rem Help cript: 	rbbsetup.rb
rem Usage:		rbbsetup.rb [nowarning]
rem Description:	Setting up RapidBATCH Builder for first run
rem
rem DO ONLY EDIT THIS FILE WHEN YOU'RE SURE WHAT YOU'RE DOING!

if [command] ! 'nowarning'
	confirm [cont] = 'WARNING:' # [new_line] # 'This will restore your RapidBATCH Builder registry settings!' # [new_line] # [new_line] # 'Do you REALLY want to continue?'
	if [cont] = [false] halt
endif

infobox 'Preparing RapidBATCH Builder for first run...', 'show'

rem setting up the syntax highlighing classes default colors
letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'CLASS_KEYWORD', '000000'
if [errorcode] = [false] goto 'error'
letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'CLASS_VALUE', '0000ff'
if [errorcode] = [false] goto 'error'
letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'CLASS_VARIABLE', '000080'
if [errorcode] = [false] goto 'error'
letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'CLASS_COMMENT', '808080'
if [errorcode] = [false] goto 'error'
letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'CLASS_SPECIAL_COMMENT', '008000'
if [errorcode] = [false] goto 'error'
letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'CLASS_LABEL', 'ff0000'
if [errorcode] = [false] goto 'error'
letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'CLASS_OPERATOR', '008000'
if [errorcode] = [false] goto 'error'
letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'CLASS_TEXT', '000000'
if [errorcode] = [false] goto 'error'
letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'CLASS_BKGND', 'ffffff'
if [errorcode] = [false] goto 'error'

rem setting up the editor's font type
letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'FontFace', 'Courier New'
if [errorcode] = [false] goto 'error'

rem setting up a default number of tools displayed in the tools menu

wait '500'

infobox 'Setup was successfull!', 'show'
wait '1000'

halt

%error
echo 'ERROR:' # [new_line] # 'Not able to write into the registry!' # [new_line] # 'Please contact your system administrator or J.M.K S.F. support.'
end