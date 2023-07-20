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

if [command] ! 'nowarning'
	confirm [cont] = 'WARNING:' # [new_line] # 'This will restore your RapidBATCH Builder registry settings!' # [new_line] # [new_line] # 'Do you REALLY want to continue?'
	if [cont] = [false] halt
endif

infobox 'Preparing RapidBATCH Builder for its first run...', 'show'

rem first of all, set up the registry keys
newregkey 'CURRENT_USER', 'Software'
newregkey 'CURRENT_USER', 'Software\jmksf'
newregkey 'CURRENT_USER', 'Software\jmksf\RapidBATCH'

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
letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'Tool0', '$RBHOME\etc\rbb_synstyles.rb|Syntax Color'
if [errorcode] = [false] goto 'error'
rem letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'Tool0', '$rb_home\etc\rbbtc.rb'
rem if [errorcode] = [false] goto 'error'


wait '500'

infobox 'Setup was successfull!', 'show'
wait '1000'

halt

%error
echo 'ERROR:' # [new_line] # 'Not able to write into the registry!' # [new_line] # 'Please contact your system administrator or J.M.K S.F. support.'
end