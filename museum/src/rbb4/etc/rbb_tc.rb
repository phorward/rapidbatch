rem ******************************************************************
rem ** Product:    RapidBATCH 5.0                                   **
rem ** Module:     RapidBATCH Builder Tool Menu Editor              **
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

rem *************
rem ** GLOBALS **
rem *************
[rb_regpath] = 'Software\jmksf\RapidBATCH'
[toollist] = [main] # ':toollist'

[entry_sep] = '|'

[language] = 'english'

rem *******************************
rem ** PROCEDURE IMPLEMENTATIONS **
rem *******************************

rem --- getlangtext
rem provides and returns language specific texts
func getlangtext: [id]
	if [language] = 'english'
		if [id] = 'main_title'
			ret 'RapidBATCH Builder Tool Configuration'
		elseif [id] = 'new_title'
			ret 'Add new tool'
		elseif [id] = 'edit_title'
			ret 'Edit tool information'
		elseif [id] = 'button_add'
			ret '&Add entry'
		elseif [id] = 'button_edit'
			ret '&Edit entry'
		elseif [id] = 'button_del'
			ret '&Remove entry'
		elseif [id] = 'button_copy'
			ret '&Copy entry'
		elseif [id] = 'button_mup'
			ret 'Move &up'
		elseif [id] = 'button_mdown'
			ret 'Move &down'
		elseif [id] = 'group_proginfo'		
			ret 'Tool information'
		elseif [id] = 'label_path'
			ret 'Path:'
		elseif [id] = 'label_label'
			ret 'Description:'
		elseif [id] = 'button_selectprog'
			ret '&Select'
		elseif [id] = 'button_ok'
			ret '&OK'
		elseif [id] = 'button_cancel'
			ret '&Cancel'
		elseif [id] = 'filter_programs'
			ret 'Program files|*.exe;*.com;*.bat'
		elseif [id] = 'title_selectpath'
			ret 'Please select a program path...'
		elseif [id] = 'message_filloutallfields'
			ret 'Please fill out both fields!'
		elseif [id] = 'message_savechanges'
			ret 'Do you want to save any changes?'
		else
			ret 'undefined'
		endif
	else
		if [id] = 'main_title'
			ret 'RapidBATCH Werkzeugkonfiguration'
		elseif [id] = 'new_title'
			ret 'Neues Programm hinzufügen'
		elseif [id] = 'edit_title'
			ret 'Programminformationen bearbeiten'
		elseif [id] = 'button_add'
			ret '&Hinzufügen'
		elseif [id] = 'button_edit'
			ret '&Bearbeiten'
		elseif [id] = 'button_del'
			ret '&Entfernen'
		elseif [id] = 'button_copy'
			ret '&Kopieren'
		elseif [id] = 'button_mup'
			ret 'Nach &oben'
		elseif [id] = 'button_mdown'
			ret 'Nach &unten'
		elseif [id] = 'group_proginfo'		
			ret 'Programminformationen'
		elseif [id] = 'label_path'
			ret 'Programmpfad:'
		elseif [id] = 'label_label'
			ret 'Bezeichnung:'
		elseif [id] = 'button_selectprog'
			ret '&Wählen'
		elseif [id] = 'button_ok'
			ret '&OK'
		elseif [id] = 'button_cancel'
			ret '&Abbrechen'
		elseif [id] = 'filter_programs'
			ret 'Programmdateien|*.exe;*.com;*.bat'
		elseif [id] = 'title_selectpath'
			ret 'Bitte Programmpfad wählen...'
		elseif [id] = 'message_filloutallfields'
			ret 'Bitte füllen Sie beide Felder aus!'
		elseif [id] = 'message_savechanges'
			ret 'Möchten Sie die Konfiguration speichern?'
		else
			ret 'undefiniert'
		endif
	endif
endfunc

rem --- setlangtodialog
rem sets language specifc labels to dialog elements
proc setlangtodialog: [dialog], [id]
	dec [text]
	getlangtext [text] = [id]	
	labeldialog [dialog], [text]
endproc

rem --- get_toolinfo
rem procedure retriving tool information from the registry
proc get_toolinfo: [*tool_labels], [*tool_files]
	dec [cnt], [entry]
	if [tool_labels:'0'] > '0'
		reset [tool_labels], [tool_files]
	endif

	[cnt] = '0'
	repeat
		[cnt] + '1'
		
		getregval [entry] = 'String', 'CURRENT_USER', [rb_regpath], 'Tool' # ([cnt]-'1')
		if [errorcode] = [true]
			if [entry] ! '-'
				gettok [tool_files[cnt]] = [entry], [entry_sep], '1'
				gettok [tool_labels[cnt]] = [entry], [entry_sep], '2'
			else
				[tool_labels[cnt]] = '-'
			endif
		endif
	until [errorcode] = [false]
endproc

rem --- put_toolinfo
rem procedure saving tool information to the registry
proc put_toolinfo: [*tool_labels], [*tool_files]
	dec [cnt], [entry], [item]

	rem first of all, delete all existing Tool-entries from the registry that exist
	[cnt] = '0'
	repeat
		[cnt] + '1'		
		delregval 'CURRENT_USER', [rb_regpath], 'Tool' # ([cnt]-'1')
	until [errorcode] = [false]
	
	rem next, re-create the tool entries based on the edited information
	if [tool_labels'0'] = '0' ret
	
	[cnt] = '0'
	repeat
		[cnt] + '1'	
		if [tool_labels[cnt]] ! '-'
			[item] = [tool_files[cnt]] # [entry_sep] # [tool_labels[cnt]]
		else
			[item] = '-'
		endif
		letregval 'String', 'CURRENT_USER', [rb_regpath], 'Tool' # ([cnt]-'1'), [item]
	until [cnt] = [tool_labels'0']
endproc

rem --- update_itemlist
rem updates the list of menu items on the dialog
proc update_itemlist: [*tool_labels], [*tool_files], [selidx]
	dec [list]
	arraytolist [list] = [labels], [std_sep]
	
	letdialog 'tc:list', 'items', [list]
	if [list] ! ''
		letdialog 'tc:list', 'selindex', [selidx]
		
		enabledialog 'tc:list'
		enabledialog 'tc:b_del'
		enabledialog 'tc:b_edit'
		enabledialog 'tc:b_copy'
		enabledialog 'tc:b_mup'
		enabledialog 'tc:b_mdown'
	else
		disabledialog 'tc:list'
		disabledialog 'tc:b_del'
		disabledialog 'tc:b_edit'
		disabledialog 'tc:b_copy'
		disabledialog 'tc:b_mup'
		disabledialog 'tc:b_mdown'
	endif	
endproc

rem --- edit_tool
rem procedure processing a dialog for editing a tool
proc edit_tool: [*label], [*path]
	dec [event], [tmp_path], [tmp_label], [filter], [text]
		
	rem --- create the dialog window
	newdialog 'edit', 'dialog', '1|1|298|128'
	letdialog 'edit', 'style', 'dialog'
	
	letdialog 'edit', 'event_ok', [true]
	letdialog 'edit', 'event_cancel', [true]

	centerdialog 'edit'
	
	rem if both label and path are empty
	if [label] = '' & [path] = ''
		setlangtodialog 'edit', 'new_title'
	else
		setlangtodialog 'edit', 'edit_title'
	endif
	

	rem --- group box: dialog window type
	newdialog 'edit:group_proginfo', 'group', '2|1|290|70'
	setlangtodialog 'edit:group_proginfo', 'group_proginfo'
	
	rem --- labels for the input fields
	newdialog 'edit:l_path', 'label', '10|20|80|21'
	setlangtodialog 'edit:l_path', 'label_path'
	
	newdialog 'edit:l_label', 'label', '10|44|80|21'
	setlangtodialog 'edit:l_label', 'label_label'

	rem --- input field for tool path
	newdialog 'edit:path', 'input', '90|16|130|25'
	
	rem --- button to open a file open dialog
	newdialog 'edit:b_select', 'button', '220|16|60|23'
	setlangtodialog 'edit:b_select', 'button_selectprog'	
	
	rem --- input field for tool label
	newdialog 'edit:label', 'input', '90|38|190|25'

	rem --- finally, add some buttons, "OK" and "Cancel"
	newdialog 'edit:ok', 'button', '10|75|130|25'
	setlangtodialog 'edit:ok', 'button_ok'

	newdialog 'edit:cancel', 'button', '150|75|130|25'
	setlangtodialog 'edit:cancel', 'button_cancel'

	showdialog 'edit'
	
	rem --- fill-up the fields
	letdialog 'edit:path', 'text', [path]
	letdialog 'edit:label', 'text', [label]
	
	focus 'edit:path'		
	repeat
	 	rundialog [event] = '0'
	 	
	 	if [event] = 'click_edit:b_select'
	 		getlangtext [filter] = 'filter_programs'
	 		openbox [tmp_path] = 'Select tool path:', [filter]
	 		if [tmp_path] ! ''
	 			letdialog 'edit:path', 'text', [tmp_path]
	 		endif
	 	elseif [event] = 'click_edit:ok' | [event] = 'ok_edit'
 			rem get the data from the input fields to test them
	 		getdialog [tmp_label] = 'edit:label', 'text'
		 		
	 		rem is the label not empty?
		 	if [tmp_label] ! ''
		 		if [tmp_label] = '-'
			 		[path] = ''
			 		[label] = [tmp_label]
		 			break		 		
		 		endif
		 		
		 		getdialog [tmp_path] = 'edit:path', 'text'
		 		if [tmp_path] ! ''		 		
			 		[path] = [tmp_path]
			 		[label] = [tmp_label]
			 		break
			 	else
			 		rem output information message
			 		getlangtext [text] = 'message_filloutallfields'
			 		echo [text]
			 	endif
		 	else
		 		rem output information message
		 		getlangtext [text] = 'message_filloutallfields'
		 		echo [text]
		 	endif
	 	endif
	 	
	until [event] = 'close_edit' | [event] = 'click_edit:cancel' | [event] = 'cancel_edit'
 	
 	rem VERY IMPORTANT: Destroy the dialog when it is created within a procedure or a loop!
 	deldialog 'edit'
endproc

rem **************************
rem ** MAIN SCRIPT ROUTINES **
rem **************************

rem --- create the base window
newdialog 'tc', 'dialog', '1|1|320|235'
centerdialog 'tc'
letdialog 'tc', 'style', 'single'
setlangtodialog 'tc', 'main_title'
if [language] = 'english'
	[menu] = '&Configuration:&Save|Save && &Exit|Exit &only;'
	rem [menu] # '&Edit:&Add|&Edit|De&lete|-|&Copy|Move &up|Move &down;'
else
	[menu] = '&Konfiguration:&Speichern|Speichern && &Beenden|&Nur beenden;'
	rem [menu] # '&Bearbeiten:&Hinzufügen|&Editieren|&Entfernen|-|&Kopieren|Nach &oben verschieben|Nach &unten verschieben;'
endif
letdialog 'tc', 'menu', [menu]

rem --- create the listbox
newdialog 'tc:list', 'list', '1|1|200|185'
letdialog 'tc:list', 'event_dblclick', [true]

rem --- create the add-button
newdialog 'tc:b_add', 'button', '205|1|100|25'
setlangtodialog 'tc:b_add', 'button_add'

rem --- create the edit-button
newdialog 'tc:b_edit', 'button', '205|30|100|25'
setlangtodialog 'tc:b_edit', 'button_edit'

rem --- create the del-button
newdialog 'tc:b_del', 'button', '205|60|100|25'
setlangtodialog 'tc:b_del', 'button_del'

rem --- create the copy-button
newdialog 'tc:b_copy', 'button', '205|100|100|25'
setlangtodialog 'tc:b_copy', 'button_copy'

rem --- create the move up-button
newdialog 'tc:b_mup', 'button', '205|130|100|25'
setlangtodialog 'tc:b_mup', 'button_mup'

rem --- create the move down-button
newdialog 'tc:b_mdown', 'button', '205|160|100|25'
setlangtodialog 'tc:b_mdown', 'button_mdown'

rem read out the current registy and display each entry
rem in the listbox of the configurator's main dialog
get_toolinfo [labels], [files]
update_itemlist [labels], [files], '1'

rem finally show the dialog
showdialog 'tc'

rem begin event loop
%next
	focus 'tc'
	rundialog [event] = '0'
	gosub [event]
goto next
	
	%click_tc:b_add
		reset [nlabel], [nfile]
		edit_tool [nlabel], [nfile]
		if [nlabel] ! ''
			[labels:[labels:'0']+'1'] = [nlabel]
			[files:[files:'0'] + '1'] = [nfile]
			update_itemlist [labels], [files], [labels:'0']
		endif
	resume
		
	%dblclick_tc:list
	%click_tc:b_edit
		getdialog [idx] = 'tc:list', 'selindex'
		edit_tool [labels[idx]], [files[idx]]
		update_itemlist [labels], [files], [idx]
	resume
		
	%click_tc:b_del
		getdialog [idx] = 'tc:list', 'selindex'
		
		rem first ask if the entry should really be deleted
		if [language] = 'english'
			confirm [confdelete] = 'Do you really want to delete "' # [labels[idx]] # '" from the tools menu?'
		elseif [language] = 'german'
			confirm [confdelete] = 'Möchten Sie "' # [labels[idx]] # '" wirklich von Werkzeug-Menü entfernen?'
		endif
				
		if [confdelete] = [true]
			rem delete item from array
			deletearrayitem [labels], [idx]
			deletearrayitem [files], [idx]
		endif
		update_itemlist [labels], [files], [idx]-'1'
	resume
	%click_tc:b_copy
		getdialog [idx] = 'tc:list', 'selindex'
		[labels[labels'0']+'1'] = [labels[idx]]
		[files[files'0']+'1'] = [files[idx]]
		update_itemlist [labels], [files], [labels:'0']
	resume
	
	
	%click_tc:b_mup
		getdialog [idx] = 'tc:list', 'selindex'
		[i] = '0'
		repeat
			[i] + '1'
			if [i] = [idx] & [i] > '1'
				[tmp] = [labels[i]]
				[labels[i]] = [labels[i]-'1']
				[labels[i]-'1'] = [tmp]

				[tmp] = [files[i]]
				[files[i]] = [files[i]-'1']
				[files[i]-'1'] = [tmp]

				update_itemlist [labels], [files], [idx]-'1'
				break
			endif
		until [i] = [labels'0']
	resume
	
	%click_tc:b_mdown
		getdialog [idx] = 'tc:list', 'selindex'
		[i] = '0'
		repeat
			[i] + '1'
			if [i] = [idx] & [i] < [labels'0']
				[tmp] = [labels[i]]
				[labels[i]] = [labels[i]+'1']
				[labels[i]+'1'] = [tmp]

				[tmp] = [files[i]]
				[files[i]] = [files[i]+'1']
				[files[i]+'1'] = [tmp]

				update_itemlist [labels], [files], [idx]+'1'
				break
			endif
		until [i] = [labels'0']
	resume

	%close_tc
	rem ask if the user wants to save the entries
	getlangtext [text] = 'message_savechanges'
	confirm [confnosave] = [text]
	
	if [confnosave] = [true]
		put_toolinfo [labels], [files]
	endif
	
	rem finally halt the program
	halt
	
	%click_tc:Menu_&Save
	put_toolinfo [labels], [files]
	resume
	
	%click_tc:Menu_Save && &Exit
	put_toolinfo [labels], [files]
	
	rem halt the program
	halt
	
	%click_tc:Menu_Exit &only
	halt
end