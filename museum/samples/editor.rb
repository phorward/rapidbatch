rem +-------------------------------------------------------------------+
rem | RapidEdit                                                         |
rem +-------------------------------------------------------------------+
rem | This script is a simple demonstration on how to implement a sim-  |
rem | ple plain-text editor using individual dialog programming in Ra-  |
rem | pidBATCH 5.0.                                                     |
rem +-------------------------------------------------------------------+

rem *************
rem ** GLOBALS **
rem *************

rem ********************
rem ** IMPLEMENTATION **
rem ********************

rem *****************
rem ** MAIN SCRIPT **
rem *****************

[OpenBox_DefFileExt] = 'txt'
[SaveBox_DefFileExt] = 'txt'

rem creating the main window
newdialog 'editor', 'dialog', '100|100|640|480'
letdialog 'editor', 'event_size', [true]

rem adding a menu bar
[menu] = '&File:&New|&Open|&Save|Save &as...|-|&Exit;'
[menu] # '&Edit:C&ut' # [tab] # 'Ctrl+X|&Copy' # [tab] # 'Ctrl+C|&Paste' # [tab] # 'Ctrl+V|-|&Delete' # [tab] # 'Del|&Select all;'
[menu] # '&Help:&About...;;'

letdialog 'editor', 'menu', [menu]

rem creating the edit field
newdialog 'editor:edit', 'edit', '1|1|635|438'
letdialog 'editor:edit', 'font', 'Courier New|12|2'

rem open file, if a filename was specified
[file] = [command]
if [file] ! ''
	rem remove quotation marks if available
	replacevar [file] = [file], '"', [null]
	
	rem read the file and put the content into the edit field
	readfile [text] = [file], '0'
	letdialog 'editor:edit', 'text', [text]
endif


letdialog 'editor', 'visible', [true]

rem event handling loop
repeat
	rem set focus to input field
	letdialog 'editor:edit', 'focus', ''

	rem update title bar
	if [file] ! ''
		letdialog 'editor', 'caption', [file] # ' : RapidEdit'
	else
		letdialog 'editor', 'caption', 'RapidEdit'
	endif

	rem wait for events
	rundialog [event] = '0'
	
	rem window size has changed?
	if [event] = 'size_editor'
		getdialog [size] = 'editor', 'position'
		gettok [width] = [size], '|', '3'
		gettok [height] = [size], '|', '4'
		letdialog 'editor:edit', 'position', '1|1|' # ([width]-('2' * [Metric_Edge_Height])) # '|' # ([height]-[Metric_Caption_Height]-[Metric_Menu_Height]-([Metric_Edge_Height] * '2'))
		cont
	rem FILE-MENU ------------------------------------------------------
	rem menu item: File -> New
	elseif [event] = 'click_editor:Menu_&New'
		[file] = ''
		letdialog 'editor:edit', 'text', ''
	rem menu item: File -> Open
	elseif [event] = 'click_editor:Menu_&Open'
		openbox [nfile] = 'Open file', 'Text files|*.txt|All files|*.*'
		if [nfile] ! ''
			readfile [text] = [nfile], '0'
			letdialog 'editor:edit', 'text', [text]
			[file] = [nfile]
		endif
	rem menu item: File -> Save / File -> Save as...
	elseif [event] = 'click_editor:Menu_&Save' | [event] = 'click_editor:Menu_Save &as...'
		if [event] = 'click_editor:Menu_Save &as...' | [file] = ''		
			savebox [nfile] = 'Save file as...', 'Text files|*.txt|All files|*.*'
			if [nfile] ! '' [file] = [nfile]
		else
			[nfile] = [file]
		endif
		
		rem write the file
		if [nfile] ! ''
			[file] = [nfile]
			
			getdialog [text] = 'editor:edit', 'text'
		
			delfile [file]
			writefile [file], [text]
		endif
	rem menu item: File -> Exit
	elseif [event] = 'click_editor:Menu_&Exit'
		halt
	rem EDIT-MENU ------------------------------------------------------
	rem menu item: Edit -> Cut
	elseif [event] = 'click_editor:Menu_C&ut' # [tab] # 'Ctrl+X'
		getdialog [clipboard] = 'editor:edit', 'selection'
		letdialog 'editor:edit', 'selection', [null]
	rem menu item: Edit -> Copy
	elseif [event] = 'click_editor:Menu_&Copy' # [tab] # 'Ctrl+C'
		getdialog [clipboard] = 'editor:edit', 'selection'
	rem menu item: Edit -> Paste
	elseif [event] = 'click_editor:Menu_&Paste' # [tab] # 'Ctrl+V'
		letdialog 'editor:edit', 'selection', [clipboard]
	rem menu item: Edit -> Delete
	elseif [event] = 'click_editor:Menu_&Delete' # [tab] # 'Del'
		letdialog 'editor:edit', 'selection', ''
	rem menu item: Edit -> Select all
	elseif [event] = 'click_editor:Menu_&Select all'
		letdialog 'editor:edit', 'selpos', '1|-1'
	rem HELP-MENU ------------------------------------------------------
	rem menu item: Help -> About...
	elseif [event] = 'click_editor:Menu_&About...'
		echo 'RapidEdit' # [new_line]  # [new_line] # 'Example script for RapidBATCH 5.0'
	endif
until [event] = 'close_editor'
end