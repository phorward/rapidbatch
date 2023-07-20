rem +-------------------------------------------------------------------+
rem | RapidBATCH v5.1                                                   |
rem +-------------------------------------------------------------------+
rem | Library: Extended dialog functions and toolset                    |
rem | Version: 1.1                                                      |
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
dec [btnmenu_X], [btnmenu_Y], [btnmenu_Width], [btnmenu_btnHeight]

dec [xp_height]

if [XPStyleActive] = [true]
	[xp_height] = '10'
else
	[xp_height] = '0'
endif

rem *********************
rem ** IMPLEMENTATIONS **
rem *********************

rem ===================================================================
rem PROCEDURE:
rem centerdialog
rem
rem DESCRIPTION:
rem Centers a dialog window on the screen based on the user's current
rem screen resolution.
rem
rem PARAMETERS:
rem + [dlg_name]
rem   Name of the dialog window to be centered.
rem
rem RETURN VALUE:
rem [errorcode] contains '0' if everything worked fine, else '-1'.
rem ===================================================================
proc centerdialog: [dlg_name]
	dec [position], [tok], [x], [y], [width], [height]
	getdialog [position] = [dlg_name], 'position'
	if [errorcode] = [false] ret
	gettok [width] = [position], [std_sep], '3'
	gettok [height] = [position], [std_sep], '4'
	[x] = ([ScreenWidth] / '2') - ([width] / '2')
	[y] = ([ScreenHeight] / '2') - ([height] / '2')
  
	letdialog [dlg_name], 'position', [x] # [std_sep] # [y] # [std_sep] # [width] # [std_sep] # [height] # [std_sep]
endproc

rem ===================================================================
rem PROCEDURE:
rem showdialog
rem
rem DESCRIPTION:
rem A more quickly possibility to make any widget visible.
rem
rem PARAMETERS:
rem + [dlg_name]
rem   Name of the widget that should be made visible.
rem
rem RETURN VALUE:
rem [errorcode] contains '0' if everything worked fine, else '-1'.
rem ===================================================================
proc showdialog: [dlg_name]
	letdialog [dlg_name], 'visible', [true]
endproc

rem ===================================================================
rem PROCEDURE:
rem hidedialog
rem
rem DESCRIPTION:
rem A more quickly possibility to make any widget invisible.
rem
rem PARAMETERS:
rem + [dlg_name]
rem   Name of the widget that should be made invisible.
rem
rem RETURN VALUE:
rem [errorcode] contains '0' if everything worked fine, else '-1'.
rem ===================================================================
proc hidedialog: [dlg_name]
	letdialog [dlg_name], 'visible', [false]
endproc

rem ===================================================================
rem PROCEDURE:
rem labeldialog
rem
rem DESCRIPTION:
rem Sets the CAPTION-attribute of any widget.
rem
rem PARAMETERS:
rem + [dlg_name]
rem   Name of the widget whose caption should be set.
rem
rem RETURN VALUE:
rem [errorcode] contains '0' if everything worked fine, else '-1'.
rem ===================================================================
proc labeldialog: [dlg_name], [caption]
	letdialog [dlg_name], 'caption', [caption]
endproc


rem ===================================================================
rem PROCEDURE:
rem enabledialog
rem
rem DESCRIPTION:
rem A more quickly possibility to make any widget enabled.
rem
rem PARAMETERS:
rem + [dlg_name]
rem   Name of the widget that should be set enabled.
rem
rem RETURN VALUE:
rem [errorcode] contains '0' if everything worked fine, else '-1'.
rem ===================================================================
proc enableDialog: [dlg_name]
	letdialog [dlg_name], 'enabled', [true]
endproc

rem ===================================================================
rem PROCEDURE:
rem disabledialog
rem
rem DESCRIPTION:
rem A more quickly possibility to make any widget disabled.
rem
rem PARAMETERS:
rem + [dlg_name]
rem   Name of the widget that should be set disabled.
rem
rem RETURN VALUE:
rem [errorcode] contains '0' if everything worked fine, else '-1'.
rem ===================================================================
proc disabledialog: [dlg_name]
	letdialog [dlg_name], 'enabled', [false]
endproc

rem ===================================================================
rem PROCEDURE:
rem focus
rem
rem DESCRIPTION:
rem Focuses any widget.
rem
rem PARAMETERS:
rem + [dlg_name]
rem   Name of the widget the focus should be set to.
rem
rem RETURN VALUE:
rem [errorcode] contains '0' if everything worked fine, else '-1'.
rem ===================================================================
proc focus: [dlg_name]
	letdialog [dlg_name], 'focus', [true]
endproc

rem ===================================================================
rem PROCEDURE:
rem retrievepositions
rem
rem DESCRIPTION:
rem Returns the positions of a widget by storing the X/Y-coordinates
rem as well as the width and height of a widget to variables assigned
rem as pointers.
rem
rem PARAMETERS:
rem + [dlg_name]
rem   Name of the widget whose positions should be retrieved.
rem + [*x]
rem   Pointer to a variable getting the X-coordinate.
rem + [*y]
rem   Pointer to a variable getting the Y-coordinate.
rem + [*width]
rem   Pointer to a variable getting the width.
rem + [*height]
rem   Pointer to a variable getting the height.
rem
rem RETURN VALUE:
rem [errorcode] contains '0' if everything worked fine, else '-1'.
rem ===================================================================
proc retrievepositions: [dlg_name], [*x], [*y], [*width], [*height]
	dec [pos]
	getdialog [pos] = [dlg_name], 'position'
	gettok [x] = [pos], [std_sep], '1'	
	gettok [y] = [pos], [std_sep], '2'
	gettok [width] = [pos], [std_sep], '3'
	gettok [height] = [pos], [std_sep], '4'
endproc

rem ===================================================================
rem FUNCTION:
rem btnmenu
rem
rem DESCRIPTION:
rem Provides a dialog that is completely been written in RapidBATCH
rem code. The dialog is called like LISTBOX or TRAYMENU, where each
rem of the items that is passed as a list is shown as one button.
rem If a button is clicked, its caption text is returned by the func-
rem tion and the dialog is removed from the system again.
rem 
rem btnmenu can be perfectly used to provide a quick program menu
rem within your scripts.
rem
rem Using the variables [btnmenu_X] and [btnmenu_Y], the dialog's 
rem position at start-up can be influenced and set to any desired
rem X-/Y-coordinate. By default, [btnmenu_X] and [btnmenu_Y] are
rem set to '-1', causing that the dialog is automatically centered.
rem By setting [btnmenu_Width], the dialog's with can be changed,
rem and by setting [btnmenu_btnHeight], the height of the generated
rem buttons can be set. The dialog's height is completely dynamic
rem based on the amount of buttons that are created.
rem
rem PARAMETERS:
rem + [title]
rem   Text displayed in the dialog's title bar.
rem + [items]
rem   List of items, where each item is separated by the character
rem   defined in [std_sep].
rem
rem RETURN VALUE:
rem The text of the button that was clicked or, if the user hits the
rem dialog's close-button, an empty-string.
rem ===================================================================
[btnmenu_X] = '-1'
[btnmenu_Y] = '-1'
[btnmenu_Width] = '200'
[btnmenu_btnHeight] = '30'

func btnmenu: [title], [items]
	dec [no], [x], [y], [w], [h], [i], [item], [event]

	[y] = '4'
	
	rem --- first of all, retrieve a unique name for the dialog
	repeat
		randvar [no] = '1024'
		newdialog 'btnmenu_' # [no], 'dialog', '1' # [std_sep] # '1' # [std_sep] # [btnmenu_Width] # [std_sep] # [y]
	until [errorcode] = [true]
	
	rem --- set title and style
	letdialog 'btnmenu_' # [no], 'caption', [title]
 	letdialog 'btnmenu_' # [no], 'style', 'dialog'
	
	rem --- now, build the buttons
	[i] = '0'
	repeat
		[i] + '1'
		gettok [item] = [items], [std_sep], [i]
		if [item] ! ''
			letdialog 'btnmenu_' # [no], 'position', '1' # [std_sep] # '1' # [std_sep] # [btnmenu_Width] # [std_sep] # ([y] + [btnmenu_btnHeight]) + '4'
			
			newdialog 'btnmenu_' # [no] # ':button' # [i], 'button', '5' # [std_sep] # [y] # [std_sep] # ([btnmenu_Width]-'14') # [std_sep] # [btnmenu_btnHeight]
			letdialog 'btnmenu_' # [no] # ':button' # [i], 'caption', [item]
			[y] + [btnmenu_btnHeight] + '2'
		endif
	until [item] = ''
	
	rem --- if the global variales [btnmenu_X] and/or [btnmenu_Y], center the dialog on the
	rem --- corresponding axis
	retrievepositions 'btnmenu_' # [no], [x], [y], [w], [h]
	
	[h] = [h] + [btnmenu_btnHeight] - '8' + [xp_height]
	if [btnmenu_X] < '1'
		[x] = ([ScreenWidth] / '2') - ([btnmenu_Width] / '2')
	else
		[x] = [btnmenu_X]
	endif
	if [btnmenu_Y] < '1'
		[y] = ([ScreenHeight] / '2') - ([h] / '2')
	else
		[y] = [btnmenu_Y]
	endif
	letdialog 'btnmenu_' # [no], 'position', [x] # [std_sep] # [y] # [std_sep] # [w] # [std_sep] # [h]
	
	rem --- make the dialog visible
	showdialog 'btnmenu_' # [no]	
	
	rem --- wait for only one event (but ignore those from other dialogs)
	repeat
		letdialog 'btnmenu_' # [no], 'focus', ''
		rundialog [event] = '0'
		getpos [item] = [event], 'btnmenu_' # [no], '1'
	until [item] > '0'
	
	rem --- the dialog's close button was clicked? Destroy the dialog and return an empty string...
	if [event] = 'close_btnmenu_' # [no]
		deldialog 'btnmenu_' # [no]
		ret ''
	endif
	
	rem --- find the button that was clicked and return it's label
	[i] = '0'
	repeat
		[i] + '1'
		gettok [item] = [items], [std_sep], [i]
		if [item] ! '' & [event] = 'click_btnmenu_' # [no] # ':button' # [i]
			deldialog 'btnmenu_' # [no]
			ret [item]
		endif
	until [item] = ''

	rem --- nothing found? Well, destroy the dialog and also return emptystring
	deldialog 'btnmenu_' # [no]
	ret ''
endfunc

rem ===================================================================
rem FUNCTION:
rem gendlg
rem
rem DESCRIPTION:
rem This function is used to load and generate a dialog from a .RBD
rem file that was created by the RapidBATCH Visual Dialog Designer to
rem create dialog-windows in WYSIWYG (What You See Is What You Get)
rem mode.
rem 
rem The dialog is generated as described in the .RBD formatted file,
rem and can be used like any other RapidBATCH dialog when the function
rem has been called.
rem
rem The advantage of .RBD-files is, that dialog windows can be edited
rem by the RapidBATCH Visual Dialog Designer also after they are used
rem in an executing RapidBATCH script. If a dialog is generated in
rem core RapidBATCH code, it oftenly needs to be changed manually over
rem the dialog creation instructions.
rem
rem PARAMETERS:
rem + [filename]
rem   Relative or absolute path to the .RBD file that should be read
rem   and generated.
rem
rem RETURN VALUE:
rem The widget name of the dialog parent window. If the return value is
rem an empty string, an error occured.
rem ===================================================================
func gendlg: [filename]
	dec [i], [j], [att], [attval], [line], [info], [savenow], [attlist], [mainwin], [eventlist]

	dec [file_sep], [pipe_replace], [prefix], [prefix_len], [len]
	[file_sep] = [tab]
	[pipe_replace] = '^'
	[prefix] = '_'
	
	rem --- getting prefix length
	getlen [prefix_len] = [prefix]

	[i] = '1'
	repeat
		[i] + '1'
		
		readfile [line] = [filename], [i]
		if [line] = '' | [errorcode] = [false] break

		[j] = '0'
		repeat
			[j] + '1'
			gettok [info:[j]] = [line], [file_sep], [j]
		until [j] = '7'


		rem remove prefix
		if [prefix_len] > '0'
			getlen [len] = [info:'2']
			copyvar [info:'2'] = [info:'2'], [prefix_len]+'1', [len]
		endif
		
		if [info:'1'] = 'DIALOG' & [mainwin] = [null]
			[mainwin] = [info:'2']
			[info:'7'] - '10'
		endif

		newdialog [info:'2'], [info:'1'], [info:'4'] # [std_sep] # [info:'5'] # [std_sep] # [info:'6'] # [std_sep] # [info:'7']
		
		[attlist]		= 'ENABLED|VISIBLE|TOOLTIP|'
		
		if [info:'1'] = 'DIALOG'
			[attlist]		# 'CAPTION|ICON|MENU|STATUSBAR|STYLE'
			[eventlist]		= 'click|move|size|mousemove|close|ok|cancel'
		elseif [info:'1'] = 'BUTTON'
			[attlist]		# 'CAPTION|FONT'
			[eventlist]		= 'click'
		elseif [info:'1'] = 'IMAGEBUTTON' | [type] = 'IMAGE'
			[attlist]		# 'IMAGE|IMAGE_TRANSPARENT'
			[eventlist]		= 'click'
		elseif [info:'1'] = 'LABEL'
			[attlist]		# 'CAPTION|FONT'
			[eventlist]		= 'click'
		elseif [info:'1'] = 'INPUT' | [type] = 'EDIT' | [type] = 'EDIT_LINEWRAP' | [type] = 'PWD'
			[attlist]		# 'FONT|READONLY|TEXT'
			[eventlist]		= 'change'
		elseif [info:'1'] = 'OPTION' | [info:'1'] = 'RADIO'
			[attlist]		# 'CAPTION|CHECKED|FONT'
			[eventlist]		= 'click'
		elseif [info:'1'] = 'LIST' | [info:'1'] = 'LIST_SORTED' | [info:'1'] = 'LIST_MULTI' | [info:'1'] = 'LIST_MULTISORTED'
			[attlist]		# 'ITEMS|FONT'
			[eventlist]		= 'dblclick|change'
		elseif [info:'1'] = 'COMBO'
			[attlist]		# 'ITEMS|FONT|TEXT'
			[eventlist]		= 'change'
		elseif [info:'1'] = 'STATIC_COMBO'
			[attlist]		# 'ITEMS|FONT'
			[eventlist]		= 'change'
		elseif [info:'1'] = 'PROGRESS'
			[attlist]		# 'PROGRESS'
			[eventlist]		= ''
		elseif [info:'1'] = 'GROUP'
			[attlist]		# 'CAPTION'
			[eventlist]		= ''
		endif

		gettok [info:'8'] = [line], [file_sep], '8'

		[j] = '0'
		repeat
			[j] + '1'
			gettok [att] = [attlist], '|', [j]
			gettok [attval] = [info:'8'], '|', [j]
			replacevar [attval] = [attval], [pipe_replace], [std_sep]
			
			if [att] = 'VISIBLE' & [info:'1'] = 'DIALOG' cont
			if [att] ! '' & [attval] ! '' letdialog [info:'2'], [att], [attval]
		until [att] = ''
		
		gettok [info:'9'] = [line], [file_sep], '9'
		[j] = '0'
		repeat
			[j] + '1'
			gettok [att] = [eventlist], '|', [j]
			if [att] ! ''			
				gettok [attval] = [info:'9'], '|', [j]
				letdialog [info:'2'], 'event_' # [att], [attval]
			endif
		until [att] = ''		
		
	until [true] = [false]
	
	ret [mainwin]
endfunc

end