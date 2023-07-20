rem RapidBATCH v5.0 Script-Highlighter for HTML v1.0Beta
rem (C) 2005 by J.M.K S.F., Jan Max Meyer

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
	
	
	if [ok] = '0'
		ret '<b>' # [kw] # '</b>'
	else
		ret [kw]
	endif
endfunc

func convertCode: [text]
	if [text] = '' ret

	infobox 'Please wait a moment...', '0'
	wait '500'
	
	replacevar [text] = [text], '"', '&quot;'
	
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

	infobox 'Done', '0'
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

rem create and set-up the dialog elements
[x] = (([ScreenWidth] / '2') - ('505' / '2'))
[y] = (([ScreenHeight] / '2') - ('400' / '2'))
newdialog 'hl', 'dialog', [x] # '|' # [y] # '|500|420'
letdialog 'hl', 'caption', 'RapidBATCH 5 Script-Highlighter for HTML v1.0Beta'
letdialog 'hl', 'style', 'single'

newdialog 'hl:code', 'edit', '1|1|495|340'
letdialog 'hl:code', 'font', 'Courier New|12|0'
letdialog 'hl:code', 'focus', ''

newdialog 'hl:htmlframe', 'option', '5|340|150|25'
letdialog 'hl:htmlframe', 'caption', 'Create &HTML Frame'
letdialog 'hl:htmlframe', 'checked', [true]
letdialog 'hl:htmlframe', 'event_click', [false]

newdialog 'hl:savetoclipboard', 'button', '5|365|150|30'
letdialog 'hl:savetoclipboard', 'caption', 'Convert to &clipboard'

newdialog 'hl:savetofile', 'button', '170|365|150|30'
letdialog 'hl:savetofile', 'caption', 'Convert to &file'

newdialog 'hl:clear', 'button', '340|365|150|30'
letdialog 'hl:clear', 'caption', 'C&lear'

letdialog 'hl', 'visible', [true]

repeat
	letdialog 'hl:code', 'focus', ''
	rundialog [event] = '0'
	
	if [event] = 'click_hl:savetofile'
		[SaveBox_DefFileExt] = '.html'
		savebox [file] = 'Save HTML-File to...', 'HTML-Files|*.html;*.htm'
		getdialog [text] = 'hl:code', 'text'
		convertCode [text] = [text]
		delfile [file]
		writefile [file], [text]
	elseif [event] = 'click_hl:savetoclipboard'	
		getdialog [text] = 'hl:code', 'text'
		convertCode [clipboard] = [text]
	elseif [event] = 'click_hl:clear'
		letdialog 'hl:code', 'text', ''
	endif
until [event] = 'close_hl'
end