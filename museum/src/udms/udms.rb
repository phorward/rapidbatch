rem J.M.K S.F. UDMS - User Documentation Management System v1.6.2
rem Copyright (C) 2004, 2005 by J.M.K S.F., Jan Max Meyer
[udms_version] = 'v1.6.2'

rem Global declarations and script startup ----------------------------------------------------------------------------------
rem include 'rb5_extdialog.rb'
rem include 'rb5_dlgtools.rb'

include 'dialog.rb'

dec [contentfile],[content_info], [text_blocks], [template_file]

[template_dir] = 'templates'
[content_dir] = 'content'
[output_dir] = 'output'
[fct_dir] = 'functions'
[menu_mode] = '1'

rem Copyright message
[oldheight] = [InfoBox_Height]
[oldwidth] = [InfoBox_Width]
[InfoBox_Height] = '40'
[InfoBox_Width] = '360'
infobox 'J.M.K S.F. User Documentation Management System ' # [udms_version] # [new_line] # 'Copyright (C) 2004, 2005 by J.M.K S.F. Software Technologies', 'show'
wait '2000'
[InfoBox_Enabled] = '-1'
[InfoBox_Height] = [oldheight]
[InfoBox_Width] = [oldwidth]

[std_sep] = '§'

[ListBox_Width] = '300'
[ListBox_Height] = '500'

[EditBox_Width] = '800'
[EditBox_Height] = '600'
[app_icon] = 'udms_small.ico'

rem Procedure: createNewContent ---------------------------------------------------------------------------------------------
proc createnewcontent
	dec [cont_header], [c_exists], [templates], [block], [type]
	confirm [c_exists] = 'Do you want to create a new content file?'
	if [c_exists] = [false] ret

	inputbox [contentfile] = 'Create new content file', 'Please enter filename (without extension!):', 'default'
	if [contentfile] = '' ret
	[contentfile] # '.cont'
	newfile [content_dir] # '\' # [contentfile]

	findfiles [templates] = [template_dir] # '\*.html', '1'

	repeat
		listbox [cont_header] = 'Select template file', [templates]
		if [errorcode] = '-2'
			error 'No templates available. Please define one first.'
			halt
		endif
	until [cont_header] ! ''
	
	[cont_header] # [std_sep]
	repeat
		inputbox [block] = 'Define text block', 'Enter a name for new text-block:', ''
		if [block] ! ''
			listbox [type] = 'Select type:', 'Single-line§Multi-line'
			if [type] = 'Single-line'
				[type] = '0'
			elseif [type] = 'Multi-line'
				[type] = '1'
			endif
			if [type] = ''
				echo 'You have to specify a type.'
			else
				upvar [block] = [block]
				[cont_header] # [type] # [block] # [std_sep]
			endif
		endif
	until [block] = ''
	writefile [content_dir] # '\' # [contentfile], [cont_header]
endproc

rem Procedure: getContentHeader ---------------------------------------------------------------------------------------------
proc getContentHeader: [filename], [*header], [*template]
	dec [head], [i], [len], [tmp]
	reset [header]
	readfile [head] = [content_dir] # '\' # [filename], '1'
	gettok [template] = [head], [std_sep], '1'
	[i] = '1'
	repeat
		[i] + '1'
		gettok [tmp] = [head], [std_sep], [i]
		if [tmp] ! ''
			getcharat [header:[i]-'1','2'] = [tmp], '1'
			getlen [len] = [tmp]
			copyvar [header:[i]-'1','1'] = [tmp], '2', [len]
		endif
	until [tmp] = ''	
endproc

rem Procedure: buildChapterList ---------------------------------------------------------------------------------------------
proc buildChapterList: [filename], [*info]
	infobox 'Building chapter list...', 'show'
	dec [item], [pos], [len], [i], [tmp], [j]
	reset [info]
	[i] = '0'
	repeat
		[i] + '1'
		readfile [item] = [content_dir] # '\' # [filename], [i]+'1'
		if [item] ! '' & [item] ! 'EOF'
			gettok [info:[i],'1'] = [item], [std_sep], '1'
			[j] = '1'
			repeat
				[j] + '1'
				gettok [tmp] = [item], [std_sep], [j]
				if [tmp] ! ''
					[info:[i],[j]] = [tmp]
				endif
			until [tmp] = ''
		endif
	until [item] = '' | [item] = 'EOF'
	infobox 'Building chapter list...Ready', 'show'
rem 	wait '1000'
	[InfoBox_Enabled] = 'hide'
endproc

rem Procedure: writeContentFile ---------------------------------------------------------------------------------------------
proc writeContentFile: [filename], [*info], [*blocks], [tfilename]
	dec [i],[j],[line],[cont_header]
	delfile [content_dir] # '\' # [filename] # '.bak'
	renamefile [content_dir] # '\' # [filename], [content_dir] # '\' # [filename] # '.bak'
	if [errorcode] = [false] echo 'Warning: Unable to create backup-file!'

	[cont_header] = [tfilename]
	if [blocks,'0'] > '0'
		[i] = '0'
		repeat
			[i] + '1'
			[cont_header] # [std_sep] # [blocks,[i],'2'] # [blocks,[i],'1']
		until [i] = [blocks,'0']
	endif
	writefile [content_dir] # '\' # [filename], [cont_header]

	[i] = '0'
	if [info:'0'] > '0'
		repeat
			[i] + '1'
			if [info:[i],'1'] ! ''
				[j] = '0'
				[line] = ''
				repeat
					[j] + '1'
					if [line] ! '' [line] # [std_sep]
					[line] # [info:[i],[j]]
				until [j] = [info:[i],'0']
				writefile [content_dir] # '\' # [filename], [line]
			endif
		until [i] = [info:'0']
	endif
	infobox [filename] # ' has been updated.', 'show'
	wait '1000'
	[InfoBox_Enabled] = 'hide'
endproc

rem Function: convertHTML ---------------------------------------------------------------------------------------------------
func convertHTML: [text]
	rem replacevar [text] = [text], [new_line], '<br>'
	replacevar [text] = [text], '€', '&euro;'
	replacevar [text] = [text], 'ä', '&auml;'
	replacevar [text] = [text], 'Ä', '&Auml;'
	replacevar [text] = [text], 'ö', '&ouml;'
	replacevar [text] = [text], 'Ö', '&Ouml;'
	replacevar [text] = [text], 'ü', '&uuml;'
	replacevar [text] = [text], 'Ü', '&Uuml;'

	replacevar [text] = [text], '<!-- br -->', ''

	rem CCS Style commands
	replacevar [text] = [text], '[picture]', '<!-- picture BEGIN --><div class="picture">'
	replacevar [text] = [text], '[/picture]', '</div><!-- picture END -->'
	replacevar [text] = [text], '\indent\', ''

	replacevar [text] = [text], '[code]', '<!-- code BEGIN --><div class="code">'
	replacevar [text] = [text], '[/code]', '</div><!-- code END-->'

	rem replacevar [text] = [text], '	', '&nbsp;&nbsp;'

	ret [text]
endfunc

rem Procedure: generateHTMLContent ------------------------------------------------------------------------------------------
proc generateHTMLContent: [filename], [*info], [*blocks], [use_template]
	dec [i], [j], [template], [tmp_text], [outfile]

	readfile [template] = [template_dir] # '\' # [use_template], '0'
	
	replacevar [template] = [template], [crlf], [new_line]
	
	[i] = '0'
	repeat
		[i] + '1'
		if [info[i],'1'] ! ''
			[outfile] = [template]
			delfile 'output\' # [info[i],'1']
			infobox 'Generating ' # [info[i],'1'] # '...', 'show'			
			[j] = '1'
			repeat
				[j] + '1'				
				convertHTML [tmp_text] = [info[i],[j]]
				replacevar [outfile] = [outfile], '#' # [blocks:[j]-'1','1'] # '#', [tmp_text]
			until [j] = [info[i],'0']

			rem some Optimization...
			replacevar [outfile] = [outfile], '<!-- code END--><br><br>', '<!-- code END-->'
			replacevar [outfile] = [outfile], '<!-- code END--><br>', '<!-- code END-->'
			replacevar [outfile] = [outfile], '<br>', '<br />'

			rem Build-in UDMS blocks
			if [i] < [info:'0']
				 replacevar [outfile] = [outfile], '#UDMS_NEXT#', '<a href="' # [info[i]+'1','1'] # '">&gt;</a>'
			else
				 replacevar [outfile] = [outfile], '#UDMS_NEXT#', '&nbsp;'
			endif

			if [i] > '1'
				 replacevar [outfile] = [outfile], '#UDMS_PREV#', '<a href="' # [info[i]-'1','1'] # '">&lt;</a>'
			else
				 replacevar [outfile] = [outfile], '#UDMS_PREV#', '&nbsp;'
			endif


			writefile 'output\' # [info[i],'1'], [outfile]
		endif
	until [i] = [info:'0']

	[InfoBox_Enabled] = 'hide'
endproc

rem Procedure: chapterEditor ------------------------------------------------------------------------------------------------
proc chapterEditor: [title], [*chapter]
	dec [j], [y], [event], [text]
	
	newdialog 'ed', 'dialog', '1§1§480§0'
	letdialog 'ed', 'style', 'single'
	letdialog 'ed', 'caption', [title] # ' :: UDMS Chapter Editor'
	
	[j] = '1'
	[y] = '1'
	repeat
		[y] + '20'
		[j] + '1'
		
		[text] = [chapter:[j]]
		
		rem replacevar [text] = [text], '>><!--lb-->', '>>' # [new_line]
		rem replacevar [text] = [text], '<<br>>', '~~~~~~~~~br~~~~~~~~~'
		replacevar [text] = [text], '<!-- br -->', [new_line]
		rem replacevar [text] = [text], '~~~~~~~~~br~~~~~~~~~', '<<br>>'

		if [text_blocks:[j]-'1','2'] = '0'
			rem inputbox [content_info:[i],[j]] = [text_blocks:[j]-'1','1'] # ' - Edit', 'Please enter a value for ' # [text_blocks:[j]-'1','1'], [content_info:[i],[j]]
			newdialog 'ed:group' # [j], 'group', '1§' # [y] - '12' # '§812§40'

			newdialog 'ed:edit' # [j], 'input', '10§' # [y] # '§800§24'
			[y] + '22'
		else
			rem editbox [content_info:[i],[j]] = [text_blocks:[j]-'1','1'] # ' - Edit', [content_info:[i],[j]], 'writeable'
			newdialog 'ed:group' # [j], 'group', '1§' # [y] - '12' # '§812§170'

			newdialog 'ed:edit' # [j], 'edit_linewrap', '10§' # [y] # '§800§150'
			
			if [j] < '4' & [j] = ([text_blocks:'0']+'1')
				letdialog 'ed:edit' # [j], 'position', '10§' # [y] # '§800§600'
				letdialog 'ed:group' # [j], 'position', '1§' # [y] - '12' # '§812§620'

				[y] + '590'
			else			
				[y] + '140'
			endif
		endif
		letdialog 'ed:edit' # [j], 'text', [text]
		letdialog 'ed:edit' # [j], 'font', 'Courier New§10§0'
		letdialog 'ed:edit', 'event_change', [false]
		letdialog 'ed:group' # [j], 'caption', [text_blocks:[j]-'1','1']
	until [j] = [text_blocks:'0']+'1'
			
	rem create and set OK button
	newdialog 'ed:ok', 'button', '1§' # [y] + '20' # '§812§25'
	letdialog 'ed:ok', 'caption', '&Save && Close'
	
	rem XP Themes?
	if [XPStyleActive] = [true]
		letdialog 'ed', 'position', '1§1§820§' # ([y]+'80')
	else
		letdialog 'ed', 'position', '1§1§820§' # ([y]+'70')
	endif
		
	centerdialog 'ed'
	focus 'ed:edit2'

	letdialog 'ed','visible', [true]
	repeat
		rundialog [event] = '0'
		rem echo [event]
		if [event] = 'close_ed'
			deldialog 'ed'
			ret
		endif
	until [event] = 'click_ed:ok'
	
	rem debugmode step
	
	[j] = '1'
	repeat
		[j] + '1'
		getdialog [chapter:[j]] = 'ed:edit' # [j], 'text'

		replacevar [chapter:[j]] = [chapter:[j]], [new_line], '<!-- br -->'
	until [j] = [text_blocks:'0']+'1'
	
	deldialog 'ed'
endproc


rem Main routines -----------------------------------------------------------------------------------------------------------
%getcont
findfiles [content_files] = 'content\*.cont', '1'
if [content_files] = '' goto create_contents
listbox [contentfile] = 'Open content:', [content_files]
if [contentfile] = '' goto create_contents

fileexists [c_exists] = [content_dir] # '\' # [contentfile]
if [c_exists] = [false]
	%create_contents
	createNewContent
	if [contentfile] = '' halt
endif

%reload_content
getcontentheader [contentfile], [text_blocks], [template_file]
buildChapterList [contentfile], [content_info]
%menu

repeat
	if [menu_mode] = '0'
		traymenu [cmd] = [contentfile] # ' :: UDMS', 'Create chapter§Edit chapter§Delete chapter§-§Save content file§Manually edit content§-§Generate HTML§Generate HTML (Batch Mode)§-§Copy list of filenames§-§Create new content file§Change content file§-§About...§Switch menu§Quit'
	else
		btnmenu [cmd] = [contentfile] # ' :: UDMS', 'Create chapter§Edit chapter§Delete chapter§Save content file§Manually edit content§Generate HTML§Generate HTML (Batch Mode)§Copy list of filenames§Create new content file§Change content file§About...§Switch menu§Quit'
	endif
until [cmd] ! ''

if [cmd] = 'Create chapter'
	replacevar [pretext] = [contentfile], '.cont', ''

	inputbox [create] = 'Create new chapter', 'Please enter a final filename for new chapter:', [pretext] # '_'
	if [create] = '' goto menu
	[content_info:[content_info:'0']+'1','1'] = [create] # '.html'

	chapterEditor [content_info:[content_info:'0'], '1'], [content_info:[content_info:'0']]

	goto menu
elseif [cmd] = 'Edit chapter'
	[i] = '0'
	[chapters] = ''

	if [content_info:'1','1'] = ''
		echo 'No content available.'
		goto menu
	endif
	repeat
		if [i] > '0' [chapters] # '§'
		[i] + '1'
		[chapters] # [content_info:[i],'1']
	until [i] = [content_info:'0']

	listbox [title] = 'Edit chapter', [chapters]
	if [title] = '' goto menu

	[i] = '0'
	repeat
		[i] + '1'

		if [title] = [content_info:[i],'1']
			chapterEditor [title], [content_info:[i]]
			goto menu
		endif
	until [i] = [content_info:'0']
	echo 'Error: Chapter not found. Try to reload the file.'
	goto menu
elseif [cmd] = 'Delete chapter'
	[i] = '0'
	[chapters] = ''

	if [content_info:'1','1'] = ''
		echo 'No content available.'
		goto menu
	endif
	repeat
		if [i] > '0' [chapters] # '§'
		[i] + '1'
		[chapters] # [content_info:[i],'1']
	until [i] = [content_info:'0']

	listbox [title] = 'Delete chapter', [chapters]
	if [title] = '' goto menu

	[i] = '0'
	repeat
		[i] + '1'
		if [title] = [content_info:[i],'1']
			[content_info:[i],'1'] = ''
		
			writeContentFile [contentfile], [content_info], [text_blocks], [template_file]
			buildChapterList [contentfile], [content_info]

			goto menu
		endif
	until [i] = [content_info:'0']
	goto menu
elseif [cmd] = 'Copy list of filenames'
	[i] = '0'
	[chapters] = ''

	if [content_info:'1','1'] = ''
		echo 'No content available.'
		goto menu
	endif
	repeat
	if [i] > '0' [chapters] # [crlf]
		[i] + '1'
		[chapters] # [content_info:[i],'1']
	until [i] = [content_info:'0']
	
	[clipboard] = [chapters]
	echo 'List of filenames was copied to the clipboard.'
elseif [cmd] = 'Generate HTML'
	generateHTMLContent [contentfile], [content_info], [text_blocks], [template_file]
	goto menu
elseif [cmd] = 'Generate HTML (Batch Mode)'
	findfiles [content_files] = 'content\*.cont', '1'
	if [content_files] = '' goto menu
	[ListBox_MultiSel] = '0'
	listbox [contentfiles] = 'Generate HTML Batch Mode:', [content_files]
	[ListBox_MultiSel] = '-1'
	if [contentfiles] = '' goto menu
	
	
	[i] = '0'
	[tok] = ''
	repeat
		[i] + '1'
		gettok [bcontentfile] = [contentfiles], '§', [i]

		if [bcontentfile] ! ''
		getcontentheader [bcontentfile], [btext_blocks], [btemplate_file]
		buildChapterList [bcontentfile], [bcontent_info]
			generateHTMLContent [bcontentfile], [bcontent_info], [btext_blocks], [btemplate_file]
		endif
	until [bcontentfile] = ''
	
	echo 'HTML Batch Generation done'

	goto menu
elseif [cmd] = 'Save content file'
	writeContentFile [contentfile], [content_info], [text_blocks], [template_file]
	goto menu
elseif [cmd] = 'Manually edit content'
	writeContentFile [contentfile], [content_info], [text_blocks], [template_file]
	readfile [current_content] = [content_dir] # '\' # [contentfile], '0'
	editbox [current_content] = [contentfile], [current_content], 'writeable'
	if [current_content] = '' goto menu
	delfile [content_dir] # '\' # [contentfile]
	writefile [content_dir] # '\' # [contentfile], [current_content]
	echo [contentfile] # ' changes had been saved.'
	getcontentheader [contentfile], [text_blocks], [template_file]
	buildChapterList [contentfile], [content_info]
	goto menu
elseif [cmd] = 'Create new content file'
	writeContentFile [contentfile], [content_info], [text_blocks], [template_file]
	createnewcontent
	goto reload_content
elseif [cmd] = 'Change content file'
	writeContentFile [contentfile], [content_info], [text_blocks], [template_file]
	goto getcont
elseif [cmd] = 'About...'
	echo 'J.M.K S.F. User Documentation Management System ' # [udms_version] # [new_line] # 'Copyright (C) 2004, 2005 by J.M.K S.F., Jan Max Meyer' # [new_line] # [new_line] # 'www.jmksf.com	 mail@jmksf.com'
elseif [cmd] = 'Switch menu'
	if [menu_mode] = '0'
	[menu_mode] = '1'
	else
	[menu_mode] = '0'
	endif
elseif [cmd] = 'Quit'
	writeContentFile [contentfile], [content_info], [text_blocks], [template_file]
	halt
endif

goto menu

end