rem ******************************************************************
rem ** Product:    RapidBATCH 5.0                                   **
rem ** Module:     RapidBATCH-to-HTML Script Code Highlighter       **
rem ** Version:    1.0                                              **
rem ******************************************************************
rem ** Copyright (C) 2004, 2005 by Jan Max Meyer, J.M.K.S.F.        **
rem ** http://www.jmksf.com/    mail@jmksf.com                      **
rem ******************************************************************
rem ** This script is part of the RapidBATCH software package and   **
rem ** may be modified for personal use only.                       **
rem ******************************************************************

include 'system.rb'

rem *************
rem ** GLOBALS **
rem *************

dec [lng_info]

rem *******************************
rem ** PROCEDURE IMPLEMENTATIONS **
rem *******************************

rem readlng_info
rem ============
rem This procedure has the purpose to read out the language catalog file by
rem building up a language catalog into the memory. This must only be called
rem once at script startup
proc readlng_info: [lngfile]
	dec [i], [fe], [label], [ch], [pos], [len]

	fileexists [fe] = [lngfile]
	if [fe] = [false]
		msgbox 'Fatal Error', 'Unable to find language definition file: ' # [lngfile], '16'
		halt
	endif

	[i] = '0'	
	repeat
		[i] + '1'
		readfile [label] = [lngfile], [i]
		trimvar [label] = [label]
		
		getcharat [ch] = [label],'1'
		if [label] ! '' & [label] ! 'EOF' & [ch] ! ';'
			getpos [pos] = [label], '=', '1'
			if [pos] > '0'
				copyvar [lng_info:[i],'1'] = [label], '1', [pos]-'1'
				
				getlen [len] = [label]
				copyvar [lng_info:[i],'2'] = [label], [pos]+'1', [len]
			endif
		endif
	until [label] = 'EOF'
	[InfoBox_Enabled] = [false]
endproc

rem getlng_label
rem ============
rem This function returns a text from the language catalog to the
rem corresponding text label entry
func getlng_label: [label]
	dec [i]
	[i] = '0'
	if [lng_info:'0'] = '0' ret [label]

	repeat
		[i] + '1'
		rem echo [lng_info:[i],'1'] # ' ' # [label]
		if [lng_info:[i],'1'] = [label] ret [lng_info:[i],'2']
	until [i] = [lng_info:'0']
	ret [label]
endfunc

rem setlangtodialog
rem ===============
rem sets language specifc labels to dialog elements
proc setlangtodialog: [dialog], [id]
	dec [text]
	getlng_label [text] = [id]	
	letdialog [dialog], 'caption', [text]
endproc

rem verifyKeyword
rem =============
rem checks a determined keyword for an RapidBATCH keyword, to make
rem keywords bold
func verifyKeyword: [kw]
	dec [kw2], [ok]
	lowvar  [kw2] = [kw]
	[ok] = '-1'
	
	if [kw2] = 'if' [ok] = '0'
	if [kw2] = 'else' [ok] = '0'
	if [kw2] = 'elseif' [ok] = '0'
	if [kw2] = 'endif' [ok] = '0'
	if [kw2] = 'end' [ok] = '0'
	if [kw2] = 'gosub' [ok] = '0'
	if [kw2] = 'resume' [ok] = '0'
	if [kw2] = 'goto' [ok] = '0'
	if [kw2] = 'halt' [ok] = '0'
	if [kw2] = 'dec' [ok] = '0'
	if [kw2] = 'repeat' [ok] = '0'
	if [kw2] = 'until' [ok] = '0'
	if [kw2] = 'proc' [ok] = '0'
	if [kw2] = 'endproc' [ok] = '0'
	if [kw2] = 'func' [ok] = '0'
	if [kw2] = 'endfunc' [ok] = '0'
	if [kw2] = 'ret' [ok] = '0'
	if [kw2] = 'reset' [ok] = '0'
	if [kw2] = 'cont' [ok] = '0'
	if [kw2] = 'break' [ok] = '0'
	if [kw2] = 'ext' [ok] = '0'
	
	if [ok] = '0'
		ret '<b>' # [kw] # '</b>'
	else
		ret [kw]
	endif
endfunc

rem convertCode
rem ===========
rem converts RapidBATCH script code to highlighted HTML code
func convertCode: [text]
	if [text] = '' ret

	getlng_label [info_text] = 'message_wait'
	infobox [info_text], '0'
	
	cntvar [max_lines] = [text], [new_line]
	if [max_lines] = '0' [max_lines] = '1'

	[ret_text] = ''
	[line_cnt] = '0'

	repeat
		[line_cnt] + '1'
		gettok [line] = [text], [new_line], [line_cnt]

		trimvar [trimline] = [line]
		if [trimline] ! ''
			[ret] = ''
			getlen [len] = [line]
			[i] = '0'
			[in_str] = '-1'
			[in_var] = '-1'
			[kw] = ''
			getchr [aphostroph] = '39'
			repeat
				[i] + '1'
				getcharat [ch] = [line], [i]
			
				if [ch] = '<' [ch] = '&lt;'
				if [ch] = '>' [ch] = '&gt;'
			
				if [ch] = [aphostroph] & [in_var] = '-1'
					if [kw] ! ''
						[ret] # [kw]
						[kw] = ''
					endif
					if [in_str] = '-1'
						[ret] # '<font color="#0000ff">'
						[ret] # [ch]
						[in_str] = '0'
					else
						[ret] # [ch]
						[ret] # '</font>'
						[in_str] = '-1'
					endif
				elseif [ch] = '[' & [in_str] = '-1'
					if [kw] ! ''
						[ret] # [kw]
						[kw] = ''
					endif
					
					if [in_var] = '-1'
						[ret] # '<font color="#000080">'
					endif
					[ret] # [ch]
					[in_var] + '1'
				elseif [ch] = ']' & [in_str] = '-1' & [in_var] > '-1'
					[ret] # [ch]
					if [in_var] = '0'
						[ret] # '</font>'
					endif
					[in_var] - '1'
				else
					if [in_str] = '-1' & [in_var] = '-1' & [ch] ! ' ' & [ch] ! [tab]
						[kw] # [ch]
					elseif [ch] = ' ' | [ch] = [tab] | [i] = [len]					
						verifyKeyword [kw] = [kw]
					
						lowvar [kw2] = [kw]
						if [kw2] = 'rem'
							[ret] = '<font color="#808080">' # [line] # '</font>'
							[i] = [len]
						else
							[ret] # [kw] # [ch]
						endif
						[kw] = ''					
					else
						[ret] # [ch]
					endif
				endif
			until [i] = [len]
					
			if [kw] ! ''
				verifyKeyword [kw] = [kw]
				[ret] # [kw]
			elseif [in_str] = '0' | [in_var] = '0'
				[ret] # '</font>'
			endif
		
			if [line_cnt] < [max_lines]+'1'
				[ret_text] # [ret] # '<br />' # [crlf]
			else
				[ret_text] # [ret] # [crlf]
			endif
		else
			[ret_text] # '<br />' # [crlf]
		endif
	until [line_cnt] = [max_lines]+'1'
	replacevar [ret_text] = [ret_text], [tab], '&nbsp;&nbsp;&nbsp;&nbsp;'

	getlng_label [info_text] = 'message_done'
	infobox [info_text], '0'
	wait '500'
	[InfoBox_Enabled] = '-1'

	rem Return the Code :)
	dec [frame]
	getdialog [frame] = 'hl:htmlframe', 'checked'
	
	if [frame] = [true]
		ret '<html><div style="font-family: Courier New; font-size: 10pt;">' # [ret_text] # '</div></html>'
	else
		ret [ret_text]
	endif
endfunc


rem *****************
rem ** MAIN SCRIPT **
rem *****************

[last_path] = [current]
chscriptdir
readlng_info 'rb2html.lng'

chdir [last_path]

rem create and set-up the dialog elements
[x] = (([ScreenWidth] / '2') - ('505' / '2'))
[y] = (([ScreenHeight] / '2') - ('400' / '2'))
newdialog 'hl', 'dialog', [x] # '|' # [y] # '|500|420'
setlangtodialog 'hl', 'main_title'
letdialog 'hl', 'style', 'single'

newdialog 'hl:code', 'edit', '1|1|495|340'
letdialog 'hl:code', 'font', 'Courier New|10|0'

rem preload file
if [command] ! ''
	replacevar [command] = [command], '"', [null]
	readfile [file] = [command], '0'
	
	letdialog 'hl:code', 'text', [file]
endif

newdialog 'hl:htmlframe', 'option', '5|340|300|25'
setlangtodialog 'hl:htmlframe', 'option_htmlframe'
letdialog 'hl:htmlframe', 'checked', [true]
letdialog 'hl:htmlframe', 'event_click', [false]

newdialog 'hl:savetoclipboard', 'button', '5|365|150|30'
setlangtodialog 'hl:savetoclipboard', 'button_savetoclipboard'

newdialog 'hl:savetofile', 'button', '170|365|150|30'
setlangtodialog 'hl:savetofile', 'button_savetofile'

newdialog 'hl:clear', 'button', '340|365|150|30'
setlangtodialog 'hl:clear', 'button_clear'

letdialog 'hl', 'visible', [true]

repeat
	letdialog 'hl:code', 'focus', ''
	rundialog [event] = '0'
	
	if [event] = 'click_hl:savetofile'
		[SaveBox_DefFileExt] = '.html'
		getlng_label [lng_text] = 'savefileto'
		savebox [file] = [lng_text], 'HTML-Files|*.html;*.htm'

		getdialog [text] = 'hl:code', 'text'

		convertCode [text] = [text]

		delfile [file]
		writefile [file], [text]
		
		getlng_label [lng_text] = 'openhtmlfile'
		confirm [openfile] = [lng_text]
		if [openfile] = [true]
			open [file]
		endif
	elseif [event] = 'click_hl:savetoclipboard'	
		getdialog [text] = 'hl:code', 'text'
		convertCode [clipboard] = [text]
	elseif [event] = 'click_hl:clear'
		letdialog 'hl:code', 'text', ''
	endif
until [event] = 'close_hl'
end