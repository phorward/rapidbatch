rem ******************************************************************
rem ** Product:    RapidBATCH 5.0                                   **
rem ** Module:     RapidBATCH Menu Designer                         **
rem ** Version:    1.0                                              **
rem ******************************************************************
rem ** Copyright (C) 2004, 2005 by Jan Max Meyer, J.M.K.S.F.        **
rem ** http://www.jmksf.com/    mail@jmksf.com                      **
rem ******************************************************************
rem ** This script is part of the RapidBATCH software package and   **
rem ** may be modified for personal use only.                       **
rem ******************************************************************

include 'dialog.rb'
include 'array.rb'
include 'system.rb'
chscriptdir

rem *************
rem ** GLOBALS **
rem *************

dec [items], [lng_info]
[btnmenu_Width] = '300'

rem *******************************
rem ** PROCEDURE IMPLEMENTATIONS **
rem *******************************

rem --- readlng_info
rem reads customized labels and texts from a language definition file
proc readlng_info: [lngfile]
	dec [i], [fe], [label], [ch]

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
			gettok [lng_info:[i],'1'] = [label], '=', '1'
			gettok [lng_info:[i],'2'] = [label], '=', '2'
		endif
	until [label] = 'EOF'
	[InfoBox_Enabled] = [false]
endproc

rem --- getlng_label
rem returns the text of an corresponding text-id from the loaded language definition
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

rem --- setlangtodialog
rem sets language specifc labels and tooltips to dialog elements
proc setlangtodialog: [dialog], [id]
	dec [text]
	getlng_label [text] = [id]
	if [text] ! [id]
		letdialog [dialog], 'caption', [text]
	endif
	
	getlng_label [text] = 'tip_' # [id]
	if [text] ! 'tip_' # [id]
		letdialog [dialog], 'tooltip', [text]
	endif
endproc

rem --- showmenu
rem interfacing function for the btnmenu-dialog
func showmenu: [title], [menu]
	dec [mitems], [ret], [i]
	getlng_label [title] = [title]
	getlng_label [menu] = [menu]
	
	listtoarray [mitems], [menu], '|'
	btnmenu [ret] = [title], [menu]
	if [ret] = '' ret ''
	
	[i] = '0'
	repeat
		[i] + '1'
		if [ret] = [mitems:[i]]
			ret [i]
		endif
	until [i] = [mitems:'0']

	ret [ret]
endfunc


rem --- update_itemlist
rem updates the list of menu items on the dialog
proc update_itemlist: [*items], [selidx]
	dec [list], [i], [j]
	
	if [items:'0'] > '0'
		[i] = '0'
		repeat
			[i] + '1'
			if [items:[i],'1'] > '0'
				[j] = '0'
				repeat
					[j] + '1'
					[list] # '----'
				until [j] = [items:[i],'1']
			endif
			[list] # [items:[i],'2'] # '|'
		until [i] = [items:'0']
	endif
		
	letdialog 'md:list', 'items', [list]
	if [list] ! ''
		letdialog 'md:list', 'selindex', [selidx]
		
		enabledialog 'md:list'
		enabledialog 'md:b_del'
		enabledialog 'md:b_edit'
		enabledialog 'md:b_copy'
		enabledialog 'md:b_gen'
		enabledialog 'md:b_preview'
		enabledialog 'md:b_new'		
	else
		disabledialog 'md:list'
		disabledialog 'md:b_del'
		disabledialog 'md:b_edit'
		disabledialog 'md:b_copy'
		disabledialog 'md:b_gen'
		disabledialog 'md:b_preview'
		disabledialog 'md:b_new'
	endif	
endproc

rem --- edit_item
rem updates the list of menu items on the dialog
func edit_item: [oldname]
	dec [title], [prompt], [error], [newname]
	
	if [oldname] = ''
		getlng_label [title] = 'editlabel_title_new'
	else
		getlng_label [title] = 'editlabel_title_edit'
	endif
	getlng_label [prompt] = 'editlabel_prompt'
	
	inputbox [newname] = [title], [prompt], [oldname]
	if [newname] = '' [newname] = [oldname]
	
	ret [newname]
endfunc

rem --- insert_item
rem inserts an item behind the specified index
proc insert_item: [*items], [pos], [label]
	insertarrayitem [items], [null], [pos]

	if [items:'0'] > '0' & [pos] > '1'
		[items:[pos],'1'] = [items:[pos]-'1','1']
	else
		[items:[pos],'1'] = '0'
	endif
	[items:[pos],'2'] = [label]
endproc

rem --- generate_code
rem generates menu sourcecode from item description
func generate_menu: [*items], [withif], [win]
	dec [code], [menu], [i], [j], [pos], [cnt], [quot], [firstif], [temp]
	
	getchr [quot] = '39'
	if [withif] = [true]
		[code] # 'newdialog ' # [quot] # [win] # [quot] # ', ' # [quot] # 'dialog' # [quot] # ', ' # [quot] # '1|1|640|480' # [quot]
		[code] # [new_line]
	endif
	
	[pos] = '0'
	[cnt] = '0'
	
	[i] = '0'

	repeat
		[i] + '1'
		if [items:[i],'1'] = [pos]
			if [cnt] > '0' [menu] # '|'
			[menu] # [items:[i],'2']

			[cnt] + '1'
		elseif [items:[i],'1'] = [pos] + '1' & [i] > '1'
			[menu] # ':' # [items:[i],'2']
			[pos] + '1'
			[cnt] = '1'
		elseif [items:[i],'1'] = [pos] - '1'
			[menu] # ';' # [items:[i],'2']
			[pos] - '1'
			[cnt] = '1'
		elseif [items:[i],'1'] < [pos] - '1'
			[j] = '-1'
			repeat
				[j] + '1'
				[menu] # ';'
			until [j] = [pos]-'1'
			[menu] # [items:[i],'2']
			[pos] = [items:[i],'1']
			[cnt] = '1'
		elseif [items:[i],'1'] > [pos] + '1' | [i] = '1' & [items:[i],'1'] > '0'
			getlng_label [temp] = 'error_menutree'
			replacevar [temp] = [temp], '%item', [items:[i],'2']
			msgbox 'Code Generator Error', [temp], '16'
			ret
		endif
	until [i] = [items:'0']
	
	[i] = '-1'
	repeat
		[i] + '1'
		[menu] # ';'
	until [i] = [pos]

	if [withif] = [true]
		rem write: letdialog '[win]', 'menu', '[code]'
		[code] # 'letdialog ' # [quot] # [win] # [quot] # ', ' # [quot] # 'menu' # [quot] # ', ' # [quot] # [menu] # [quot] # [new_line]

		rem write: letdialog '[win]', 'visible', [true]
		[code] # 'letdialog ' # [quot] # [win] # [quot] # ', ' # [quot] # 'visible' # [quot] # ', [true]' # [new_line]
		
		rem write: repeat
		[code] # [new_line] # 'repeat' # [new_line]

		rem write: rundialog
		[code] # [tab] # 'rundialog [event] = ' # [quot] # '0' # [quot]
				
		[whichif] = 'if'
		[i] = '0'
		repeat
			[i] + '1'
						
			if [i] < [items:'0'] & [items:[i],'1'] = [items:[i]+'1','1'] | [items:[i],'1'] > [items:[i]+'1','1']
				rem write: if/elseif [event] = 'click_[win]:menu[items[i],'2']'
				[code] # [new_line] # [tab] # [whichif]
				[code] # ' [event] = ' # [quot] # 'click_' # [win] # ':menu_' # [items:[i],'2'] # [quot]

				rem write: comment
				getlng_label [text] = 'codegen_insertcodehere'
				replacevar [text] = [text], '%item', [items:[i],'2']				
				
				[code] # [new_line] # [tab] # [tab] # 'rem ' # [text]
				rem was: *** insert operational code here ***
				
				
				if [whichif] = 'if' [whichif] = 'elseif'
			endif
		until [i] = [items:'0']
		
		if [whichif] ! 'if'
			[code] # [new_line] # [tab] # 'endif'
		endif
		
		rem write: until
		[code] # [new_line] # 'until [event] = ' # [quot] # 'close_' # [win] # [quot] # [new_line]
		
		replacevar [code] = [code], [new_line], [crlf]
	else
		[code] = [menu]
	endif
	ret [code]
endfunc

rem **************************
rem ** MAIN SCRIPT ROUTINES **
rem **************************

rem --- first of all, read the language definition file
readlng_info 'md.lng'

rem --- create the base window
newdialog 'md', 'dialog', '1|1|365|250'
centerdialog 'md'
letdialog 'md', 'style', 'single'
setlangtodialog 'md', 'main_title'

letdialog 'md', 'event_ok', [false]
letdialog 'md', 'event_cancel', [false]

rem --- create the listbox
newdialog 'md:list', 'list', '1|1|310|150'
letdialog 'md:list', 'event_dblclick', [true]

rem --- create the add-button
newdialog 'md:b_add', 'button', '1|150|100|25'
setlangtodialog 'md:b_add', 'button_add'

rem --- create the edit-button
newdialog 'md:b_edit', 'button', '105|150|100|25'
setlangtodialog 'md:b_edit', 'button_edit'

rem --- create the del-button
newdialog 'md:b_del', 'button', '210|150|100|25'
setlangtodialog 'md:b_del', 'button_del'

rem --- create the preview-button
newdialog 'md:b_preview', 'button', '1|175|310|25'
setlangtodialog 'md:b_preview', 'button_preview'

rem --- create the gen-button
newdialog 'md:b_gen', 'button', '1|200|310|25'
setlangtodialog 'md:b_gen', 'button_generate'

newdialog 'md:b_new', 'button', '310|150|45|75'
setlangtodialog 'md:b_new', 'button_new'

rem --- create the copy-button
rem newdialog 'md:b_copy', 'button', '315|1|100|25'
rem setlangtodialog 'md:b_copy', 'button_copy'

rem --- create the move up-button
newdialog 'md:b_mup', 'imagebutton', '315|1|36|36'
letdialog 'md:b_mup', 'image_transparent', 'md_b_mup.bmp'
setlangtodialog 'md:b_mup', 'button_mup'
rem setlangtodialog 'md:b_mup', 'button_mup'

rem --- create the move down-button
newdialog 'md:b_mdown', 'imagebutton', '315|38|36|36'
letdialog 'md:b_mdown', 'image_transparent', 'md_b_mdown.bmp'
setlangtodialog 'md:b_mdown', 'button_mdown'

rem setlangtodialog 'md:b_mdown', 'button_mdown'

rem --- create the move left-button
newdialog 'md:b_mleft', 'imagebutton', '315|76|36|36'
letdialog 'md:b_mleft', 'image_transparent', 'md_b_mleft.bmp'
setlangtodialog 'md:b_mleft', 'button_mleft'

rem --- create the move right-button
newdialog 'md:b_mright', 'imagebutton', '315|114|36|36'
letdialog 'md:b_mright', 'image_transparent', 'md_b_mright.bmp'
setlangtodialog 'md:b_mright', 'button_mright'

rem finally show the dialog
showdialog 'md'


[items:'1','1'] = '0'
[items:'1','2'] = 'Datei'
[items:'2','1'] = '1'
[items:'2','2'] = 'Neu'
[items:'3','1'] = '1'
[items:'3','2'] = 'Öffnen'
[items:'4','1'] = '1'
[items:'4','2'] = 'Speichern'
[items:'5','1'] = '1'
[items:'5','2'] = '-'
[items:'6','1'] = '1'
[items:'6','2'] = 'Beenden'







update_itemlist [items], '1'

rem begin event loop
%next
	focus 'md'
	rundialog [event] = '0'

	gosub [event]
goto 'next'
	
	%click_md:b_add
		edit_item [label] = ''
		if [label] ! ''
			getdialog [idx] = 'md:list', 'selindex'
			if [idx] = '-1' [idx] = '0'
			insert_item [items], [idx]+'1', [label]
			update_itemlist [items], [idx]+'1'
		endif
	resume
		
	%dblclick_md:list
	%click_md:b_edit
		getdialog [idx] = 'md:list', 'selindex'
		edit_item [items:[idx],'2'] = [items:[idx],'2']
		update_itemlist [items], [idx]
	resume
		
	%click_md:b_del
		getdialog [idx] = 'md:list', 'selindex'
		deletearrayitem [items], [idx]
		update_itemlist [items], [idx]-'1'
	resume
	%click_md:b_copy
		getdialog [idx] = 'md:list', 'selindex'
		
		insert_item [items], [idx]+'1', [items:[idx],'2']
		update_itemlist [items], [idx]+'1'
	resume
	
	%click_md:b_mright
		getdialog [idx] = 'md:list', 'selindex'
		if [idx] = '-1' resume

		[items:[idx],'1'] + '1'
		update_itemlist [items], [idx]
	resume

	%click_md:b_mleft
		getdialog [idx] = 'md:list', 'selindex'
		if [idx] = '-1' resume

		if [items:[idx],'1'] > '0'
			[items:[idx],'1'] - '1'
			update_itemlist [items], [idx]
		endif
	resume

	%click_md:b_mup
		getdialog [idx] = 'md:list', 'selindex'
		if [idx] = '-1' resume
		
		[i] = '0'
		repeat
			[i] + '1'
			if [i] = [idx] & [i] > '1'
				[tmp] = [items:[i],'1']
				[items:[i],'1'] = [items:[i]-'1','1']
				[items:[i]-'1','1'] = [tmp]

				[tmp] = [items:[i],'2']
				[items:[i],'2'] = [items:[i]-'1','2']
				[items:[i]-'1','2'] = [tmp]

				
				update_itemlist [items], [idx]-'1'
				break
			endif
		until [i] = [items:'0']
	resume
	
	%click_md:b_mdown
		getdialog [idx] = 'md:list', 'selindex'
		if [idx] = '-1' resume
		
		[i] = '0'
		repeat
			[i] + '1'
			if [i] = [idx] & [i] < [items:'0']
				[tmp] = [items:[i],'1']
				[items:[i],'1'] = [items:[i]+'1','1']
				[items:[i]+'1','1'] = [tmp]

				[tmp] = [items:[i],'2']
				[items:[i],'2'] = [items:[i]+'1','2']
				[items:[i]+'1','2'] = [tmp]
								
				update_itemlist [items], [idx]+'1'
				break
			endif
		until [i] = [items:'0']
	resume
	
	%click_md:b_preview
		generate_menu [code] = [items], [false], [null]
		if [code] = '' resume
		
		hidedialog 'md'
		
		rem create preview window
		newdialog 'preview', 'dialog', '1|1|640|480'
		
		getlng_label [text] = 'preview_title'
		letdialog 'preview', 'caption', [text]
		letdialog 'preview', 'menu', [code]
	
		rem create label
		newdialog 'preview:info', 'label', '1|1|500|50'
		letdialog 'preview:info', 'font', 'Arial Black|16|0'

		centerdialog 'preview'
		showdialog 'preview'
		
		rem run testing event loop
		repeat
			rundialog [event] = '0'
			if [event] ! 'close_preview'
				labeldialog 'preview:info', [event]
			endif
		until [event] = 'close_preview'
		
		deldialog 'preview'
		
		showdialog 'md'
	resume


	%click_md:b_gen
		showmenu [option] = 'menu_genwhat_title', 'menu_genwhat_def'
		if [option] = '' resume
		
		if [option] = '1'
			getlng_label [prompt] = 'codegen_windowname'
			inputbox [name] = 'Code Generator', [prompt], 'myDialog'
			
			generate_menu [code] = [items], [true], [name]
		else
			generate_menu [code] = [items], [false], [null]
		endif
		
		if [code] = '' resume

		showmenu [option] = 'menu_gennext_title', 'menu_gennext_def'
		if [option] = '' resume
		
		if [option] = '1'
			[clipboard] = [code]
			getlng_label [text] = 'msg_copiedtoclipboard'
			echo [text]
		elseif [option] = '2'
			[EditBox_Ok] = ''
			getlng_label [EditBox_Cancel] = 'btn_next'
			getlng_label [text] = 'codegen_codeview'
			editbox [null] = [text], [code], 'readonly'
		else
			getlng_label [text] = 'codegen_selectfilename'
			[SaveBox_DefFileExt] = 'rb'
			savebox [file] = [text], 'RapidBATCH Scripts|*.rb|All files|*.*'
			
			if [file] ! ''
				writefile [file], [code]
				if [errorcode] = [true]
					getlng_label [text] = 'codegen_filesaved'
					msgbox 'Code Generator', [text], '64'
					
					shell '..\rbb4.exe ' # [file], 'show'
				else
					getlng_label [text] = 'codegen_filenotsaved'
					msgbox 'Code Generator', [text], '16'
				endif
			endif
		endif
	resume
	
	%click_md:b_new
		getlng_label [text] = 'ask_newmenu'
		confirm [answer] = [text]
		if [answer] = [true]
			reset [items]
			update_itemlist [items], '-1'
		endif
	resume

	%close_md
		if [items:'0'] > '0'
			getlng_label [text] = 'ask_newmenu'
			confirm [answer] = [text]
			if [answer] = [false] resume
		endif
	halt		
end