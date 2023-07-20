rem ******************************************************************
rem ** Product:    RapidBATCH 5.0                                   **
rem ** Module:     Visual Dialog Designer                           **
rem ** Version:    1.2.1                                            **
rem ******************************************************************
rem ** Copyright (C) 2005, 2006 by Jan Max Meyer, J.M.K.S.F.        **
rem ** http://www.jmksf.com/    mail@jmksf.com                      **
rem **                                                              **
rem ** Thanks to: Klaus Fischer (for the pattern extension)         **
rem ******************************************************************
rem ** This script is part of the RapidBATCH software package and   **
rem ** may be modified for personal use only.                       **
rem **                                                              **
rem ** It is not allowed to use or sell this software tool under an **
rem ** other name or with any other product than RapidBATCH.        **
rem ******************************************************************

include 'dialog.rb'
include 'system.rb'
include 'array.rb'

rem *************
rem ** GLOBALS **
rem *************

dec [s_width], [s_height]

[s_width] = [ScreenWidth]
[s_height] = [ScreenHeight]
rem [s_width] = '850'
rem [s_height] = '810'


rem Well okay, here we define some global variables with different purposes
dec [x], [mx], [y], [my], [current_widget], [mode], [current_widget_enabled], [pos], [raster_toggle], [raster_x], [raster_y]

rem Raster einschalten!
[raster_toggle] = [true]
[raster_x] = '5'
[raster_y] = '5'

rem Dialog Designer display name
dec [dd_name], [dd_version], [dd_affix], [dd_dir]
[dd_name] = 'RapidBATCH Visual Dialog Designer'

rem to set the title also to all ECHO- and CONFIRM-calls, let's update the
rem title bar texts over the particular pre-declared variables
[Echo_Title] = [dd_name]
[Confirm_Title] = [dd_name]

rem Dialog Designer version number
[dd_version] = '1.2.1'

rem Dialog Designer version affix (e.g. for beta versions)
[dd_affix] = ''

rem enabled state of the current widget that is selected
[current_widget_enabled] = [true]

rem position-counter for toolbar buttons on the dd-widget
dec [tb_pos]
[tb_pos] = '1'

rem designer window, where widgets are placed on
dec [designwin]

rem designer window prefix (to avoid problems with own dialogs)
dec [prefix]
[prefix] = '_'

rem variables for file specific operations
dec [current_file], [file_changed]

rem devitation on systems with / without XP style and different window widths
dec [xp_pix_y], [xp_pix_x], [menu_height]

[xp_pix_y] = [Metric_Caption_Height] + ([Metric_Edge_Height] * '2')
[xp_pix_x] = [Metric_Edge_Height]
[menu_height] = '0'

rem RBD file formating characters, that are used and replaced in files
dec [file_sep], [pipe_replace]
[file_sep] = [tab]
[pipe_replace] = '^'

rem widget info system and defines
dec [w_type], [w_name], [w_x], [w_h], [w_width], [w_height], [w_code], [w_xname], [w_attrib], [w_attrib_types], [w_events], [w_event_states]

rem widget attribute ID's
[w_type]		= '1'
[w_name] 		= '2'
[w_x] 		= '3'
[w_y] 		= '4'
[w_width] 		= '5'
[w_height] 		= '6'
[w_code]		= '7'
[w_xname]		= '8'
[w_attrib]		= '9'
[w_attrib_types]	= '10'
[w_events]		= '11'
[w_event_states]	= '12'

rem declaring the global widget array that contains all widget information
dec [widgets]

rem declaring the global language catalog array, that is used to make
rem RapidBATCH Visual Dialog Designer completely language independent
dec [lng_info], [lng_title], [new_name]

rem keep the current path, to be switched to again later
dec [start_dir]
[start_dir] = [current]

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
		
		if [errorcode] ! [false]
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
		endif
	until [label] = 'EOF' | [errorcode] = [false]
	[InfoBox_Enabled] = [false]
endproc

rem getlng_label
rem ============
rem This function returns a text from the language catalog to the corres-
rem ponding text label entry
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

rem enableIDE
rem =========
rem Enables or disables all dialog windows from the IDE, to display a
rem modal-mode window, e.g. the Code Generator's options dialog
proc enableIDE: [state]
	letdialog 'dd', 'enabled', [state]
	letdialog 'dw', 'enabled', [state]
	letdialog [designwin], 'enabled', [state]
	
	if [state] = [true]
		letdialog 'dd', 'focus', [true]
		letdialog 'dw', 'focus', [true]
		letdialog [designwin], 'focus', [true]
	endif
endproc

rem item_editor
rem ===========
rem The item-editor is used to edit and manage the items specified as the
rem "ITEMS" attributes of LIST- and COMBO-widgets
func item_editor: [list]
	dec [event], [items], [item], [lng_text], [pos]

	rem --- make this window modal
	enableIDE [false]

	rem --- creating code for widget listedit
	newdialog 'listedit', 'DIALOG', '260|132|244|302'
	letdialog 'listedit', 'style', 'static_tool'
	letdialog 'listedit', 'event_ok', [true]
	letdialog 'listedit', 'event_cancel', [true]
	
	getlng_label [lng_text] = 'itemedit_title'
	letdialog 'listedit', 'caption', [lng_text]	
	
	rem --- creating code for widget item_list
	newdialog 'listedit:item_list', 'LIST', '2|2|190|220'
	letdialog 'listedit:item_list', 'enabled', '0'
	letdialog 'listedit:item_list', 'visible', '0'
	
	if [list] ! ''
		letdialog 'listedit:item_list', 'items', [list]
		letdialog 'listedit:item_list', 'selindex', '1'

		letdialog 'listedit:change_item', 'enabled', '0'
		letdialog 'listedit:del_item', 'enabled', '0'
		
		listtoarray [items], [list], [std_sep]
	endif
	
	letdialog 'listedit:item_list', 'event_change', [true]
	
	rem --- creating code for widget item_up
	newdialog 'listedit:item_up', 'IMAGEBUTTON', '195|2|40|40'
	letdialog 'listedit:item_up', 'enabled', '0'
	letdialog 'listedit:item_up', 'visible', '0'
	letdialog 'listedit:item_up', 'image_transparent', [dd_dir] # '\img\b_up.bmp'

	getlng_label [lng_text] = 'itemedit_tt_mup'
	letdialog 'listedit:item_up', 'tooltip', [lng_text]
	
	rem --- creating code for widget item_down
	newdialog 'listedit:item_down', 'IMAGEBUTTON', '195|42|40|40'
	letdialog 'listedit:item_down', 'enabled', '0'
	letdialog 'listedit:item_down', 'visible', '0'	
	letdialog 'listedit:item_down', 'image_transparent', [dd_dir] # '\img\b_down.bmp'
	
	getlng_label [lng_text] = 'itemedit_tt_mdown'
	letdialog 'listedit:item_down', 'tooltip', [lng_text]
	
	rem --- creating code for widget current_item
	newdialog 'listedit:current_item', 'INPUT', '3|222|190|25'
	letdialog 'listedit:current_item', 'enabled', '0'
	letdialog 'listedit:current_item', 'visible', '0'
	letdialog 'listedit:current_item', 'readonly', '-1'
	
	rem --- creating code for widget add_item
	newdialog 'listedit:add_item', 'BUTTON', '3|248|60|25'
	letdialog 'listedit:add_item', 'enabled', '0'
	letdialog 'listedit:add_item', 'visible', '0'
	
	getlng_label [lng_text] = 'itemedit_add'
	letdialog 'listedit:add_item', 'caption', [lng_text]
	
	rem --- creating code for widget change_item
	newdialog 'listedit:change_item', 'BUTTON', '68|248|60|25'
	letdialog 'listedit:change_item', 'enabled', '-1'
	letdialog 'listedit:change_item', 'visible', '0'

	getlng_label [lng_text] = 'itemedit_change'
	letdialog 'listedit:change_item', 'caption', [lng_text]
	
	rem --- creating code for widget del_item
	newdialog 'listedit:del_item', 'BUTTON', '132|248|60|25'
	letdialog 'listedit:del_item', 'enabled', '-1'
	letdialog 'listedit:del_item', 'visible', '0'

	getlng_label [lng_text] = 'itemedit_delete'
	letdialog 'listedit:del_item', 'caption', [lng_text]

	rem --- make the listedit dialog visible!
	centerdialog 'listedit'
	letdialog 'listedit', 'visible', '0'
	
	if [list] ! ''
		letdialog 'listedit:item_list', 'focus', [true]
	else
		letdialog 'listedit:current_item', 'focus', [true]
	endif
	
	repeat
		getdialog [list] = 'listedit:item_list', 'items'
		if [list] ! [null]
			letdialog 'listedit:change_item', 'enabled', '0'
			letdialog 'listedit:del_item', 'enabled', '0'
		else
			letdialog 'listedit:change_item', 'enabled', '-1'
			letdialog 'listedit:del_item', 'enabled', '-1'
		endif
		
		rundialog [event] = '0'

		rem --- event handling for click on item_up
		if [event] = 'click_listedit:item_up'
			getdialog [pos] = 'listedit:item_list', 'selindex'
			if [pos] < '2' cont
			
			[item] = [items:[pos]-'1']
			[items:[pos]-'1'] = [items:[pos]]
			[items:[pos]] = [item]
			
			[pos] - '1'
			
			gosub 'updatelist'
		rem --- event handling for click on item_down
		elseif [event] = 'click_listedit:item_down'
			getdialog [pos] = 'listedit:item_list', 'selindex'
			if [pos] = [items:'0'] cont
			
			[item] = [items:[pos]+'1']
			[items:[pos]+'1'] = [items:[pos]]
			[items:[pos]] = [item]
			
			[pos] + '1'
			
			gosub 'updatelist'
		rem --- event handling for click on add_item
		elseif [event] = 'click_listedit:add_item' | [event] = 'ok_listedit'
			getdialog [item] = 'listedit:current_item', 'text'
			if [item] ! ''
				getdialog [pos] = 'listedit:item_list', 'selindex'
				if [pos] < '1'
					[pos] = '1'
				else
					[pos] + '1'
				endif

				insertarrayitem [items], [item], [pos]
				gosub 'updatelist'
				
				letdialog 'listedit:current_item', 'selpos', '1|-1'
				letdialog 'listedit:current_item', 'focus', [true]
			endif
		rem --- event handling for click on change_item
		elseif [event] = 'click_listedit:change_item'
			getdialog [item] = 'listedit:current_item', 'text'
			[items:[pos]] = [item]
						
			gosub 'updatelist'
			rem letdialog 'listedit:item_list', 'selindex', [pos]
		rem --- event handling for click on del_item
		elseif [event] = 'click_listedit:del_item'
			getdialog [pos] = 'listedit:item_list', 'selindex'
			if [pos] < '1' cont

			deletearrayitem [items], [pos]
			
			if [pos] > '1' [pos]-'1'

			gosub 'updatelist'

		rem --- event handling for changing the list
		elseif [event] = 'change_listedit:item_list'
			getdialog [item] = 'listedit:item_list', 'selection'
			letdialog 'listedit:current_item', 'text', [item]
		endif
	until [event] = 'close_listedit' | [event] = 'esc_listedit'
	
	deldialog 'listedit'
	enableIDE [true]
	
	ret [list]
	
	rem --- sub routine for updating the item list
	%updatelist
	arraytolist [list] = [items], [std_sep]
	letdialog 'listedit:item_list', 'items', [list]
	letdialog 'listedit:item_list', 'selindex', [pos]
	resume
endfunc

rem getmenu_fromdef
rem ===============
rem This procedure maps the items of a menu-definition into an array
rem structure
proc getmenu_fromdef: [def], [*items], [*item_levels]
	dec [len], [i], [j], [pos], [item], [ch]

	if [def] ! ''
		[pos] = '0'
		[i] = '0'
		[j] = '0'
		
		getlen [len] = [def]
		repeat
			[j] + '1'
			getcharat [ch] = [def], [j]
			if [ch] = '|'
				[i] + '1'
				
				if [item] ! ''
					[items:[i]] = [item]
					[item_levels:[i]] = [pos]
				endif
				
				[item] = ''
			elseif [ch] = ':'
				[i] + '1'
				
				if [item] ! ''
					[items:[i]] = [item]
					[item_levels:[i]] = [pos]
				endif
				
				[pos] + '1'
				[item] = ''
			elseif [ch] = ';'
				[i] + '1'
				
				if [item] ! ''
					[items:[i]] = [item]
					[item_levels:[i]] = [pos]
				endif
				
				[pos] - '1'
				[item] = ''
			else	
				[item] # [ch]
			endif
		until [j] = [len]
	endif
endproc

rem menu_editor
rem ===========
rem The menu-editor is used to edit and manage the menu bar of a dialog
rem window
func menu_editor: [def]
	dec [item], [pos], [items], [item_levels], [lng_info], [list], [cnt], [i], [j], [ch], [len], [lng_text]

	enableIDE [false]

	rem --- first of all, evaluate a possible menu descriptor
	getmenu_fromdef [def], [items], [item_levels]

	rem --- creating code for widget md
	newdialog 'md', 'DIALOG', '220|100|481|370'
	if [errorcode] = [true]
		getlng_label [lng_text] = 'md_title'
		letdialog 'md', 'caption', [lng_text]

		letdialog 'md', 'style', 'static_tool'
		letdialog 'md', 'event_ok', [true]
		letdialog 'md', 'event_cancel', [true]
		
		rem --- creating code for widget list
		newdialog 'md:list', 'LIST', '1|1|430|300'
		letdialog 'md:list', 'enabled', '0'
		letdialog 'md:list', 'event_change', [true]
		
		rem --- creating code for widget m_up
		newdialog 'md:m_up', 'IMAGEBUTTON', '432|1|40|40'
		letdialog 'md:m_up', 'enabled', '0'
		letdialog 'md:m_up', 'image_transparent', [dd_dir] # '\img\b_up.bmp'
		
		getlng_label [lng_text] = 'md_item_m_up'
		letdialog 'md:m_up', 'tooltip', [lng_text]
		
		rem --- creating code for widget m_down
		newdialog 'md:m_down', 'IMAGEBUTTON', '432|40|40|40'
		letdialog 'md:m_down', 'enabled', '0'
		letdialog 'md:m_down', 'image_transparent', [dd_dir] # '\img\b_down.bmp'

		getlng_label [lng_text] = 'md_item_m_down'
		letdialog 'md:m_down', 'tooltip', [lng_text]

		rem --- creating code for widget m_left
		newdialog 'md:m_left', 'IMAGEBUTTON', '432|80|40|40'
		letdialog 'md:m_left', 'enabled', '0'
		letdialog 'md:m_left', 'image_transparent', [dd_dir] # '\img\b_left.bmp'

		getlng_label [lng_text] = 'md_item_m_left'
		letdialog 'md:m_left', 'tooltip', [lng_text]
		
		rem --- creating code for widget m_right
		newdialog 'md:m_right', 'IMAGEBUTTON', '432|120|40|40'
		letdialog 'md:m_right', 'enabled', '0'
		letdialog 'md:m_right', 'image_transparent', [dd_dir] # '\img\b_right.bmp'

		getlng_label [lng_text] = 'md_item_m_right'
		letdialog 'md:m_right', 'tooltip', [lng_text]

		rem --- creating code for widget citem
		newdialog 'md:citem', 'INPUT', '1|300|430|25'
		letdialog 'md:citem', 'enabled', '0'
		letdialog 'md:citem', 'readonly', '-1'
		
		rem --- creating code for widget item_add
		newdialog 'md:item_add', 'BUTTON', '1|324|142|25'
		letdialog 'md:item_add', 'enabled', '0'

		getlng_label [lng_text] = 'md_item_add'
		letdialog 'md:item_add', 'caption', [lng_text]
		
		rem --- creating code for widget item_update
		newdialog 'md:item_update', 'BUTTON', '144|324|142|25'
		letdialog 'md:item_update', 'enabled', '-1'

		getlng_label [lng_text] = 'md_item_update'
		letdialog 'md:item_update', 'caption', [lng_text]
		
		rem --- creating code for widget item_delete
		newdialog 'md:item_delete', 'BUTTON', '288|324|142|25'
		letdialog 'md:item_delete', 'enabled', '-1'

		getlng_label [lng_text] = 'md_item_delete'
		letdialog 'md:item_delete', 'caption', [lng_text]
	endif
		
	rem --- make the dialog window visible
	centerdialog 'md'
	letdialog 'md', 'visible', [true]
	letdialog 'md:citem', 'focus', [true]
	
	rem --- update the item list if required
	if [def] ! ''
		gosub 'updatemenu'
	endif
	
	%correct
	repeat
		rundialog [event] = '0'
	
		rem --- event handling for click on m_up
		if [event] = 'click_md:m_up'
			if [pos] < '2' cont
			
			[item] = [items:[pos]-'1']
			[items:[pos]-'1'] = [items:[pos]]
			[items:[pos]] = [item]

			[item] = [item_levels:[pos]-'1']
			[item_levels:[pos]-'1'] = [item_levels:[pos]]
			[item_levels:[pos]] = [item]

			[pos] - '1'
			
			gosub 'updatemenu'
			
		rem --- event handling for click on m_down
		elseif [event] = 'click_md:m_down'
			if [pos] = [items:'0'] cont
			
			[item] = [items:[pos]+'1']
			[items:[pos]+'1'] = [items:[pos]]
			[items:[pos]] = [item]

			[item] = [item_levels:[pos]+'1']
			[item_levels:[pos]+'1'] = [item_levels:[pos]]
			[item_levels:[pos]] = [item]

			[pos] + '1'
			
			gosub 'updatemenu'
			
		rem --- event handling for click on b_left
		elseif [event] = 'click_md:m_left'
			if [pos] > '0' & [item_levels[pos]] > '0'
				[item_levels[pos]] - '1'
				gosub 'updatemenu'
			endif			
		rem --- event handling for click on b_right
		elseif [event] = 'click_md:m_right'
			if [pos] > '0'
				[item_levels[pos]] + '1'
				gosub 'updatemenu'
			endif
			
		rem --- event handling for click on b_right
		elseif [event] = 'change_md:list'
			getdialog [pos] = 'md:list', 'selindex'
			letdialog 'md:citem', 'text', [items:[pos]]
			
		rem --- event handling for click on item_add
		elseif [event] = 'click_md:item_add' | [event] = 'ok_md'
			getdialog [item] = 'md:citem', 'text'
			if [item] ! ''
				getdialog [pos] = 'md:list', 'selindex'
				if [pos] < '1'
					[pos] = '1'
				else
					[pos] + '1'
				endif
				
				insertarrayitem [items], [item], [pos]
				
				if [pos] > '1'
					insertarrayitem [item_levels], [item_levels[pos]-'1'], [pos]
				else
					insertarrayitem [item_levels], '0', [pos]
				endif
				
				gosub 'updatemenu'
				
				letdialog 'md:citem', 'selpos', '1|-1'
				letdialog 'md:citem', 'focus', [true]
			endif

		rem --- event handling for click on item_update
		elseif [event] = 'click_md:item_update'
			getdialog [item] = 'md:citem', 'text'
			if [item] ! [null]
				[items:[pos]] = [item]
				gosub 'updatemenu'
			endif			
		rem --- event handling for click on item_delete
		elseif [event] = 'click_md:item_delete'
			if [pos] < '1' cont

			deletearrayitem [items], [pos]
			deletearrayitem [item_levels], [pos]
			
			if [pos] > '1' [pos]-'1'

			gosub 'updatemenu'
		endif
	
	until [event] = 'close_md' | [event] = 'cancel_md'
	
	rem --- creating the menu descriptor
	if [item_levels:'0'] = '0'
		[menu_height] = '0'
		letdialog 'md', 'visible', [false]
		enableIDE [true]
		ret ''
	endif
	
	[i] = '0'
	[cnt]= '0'
	[pos] = '0'
	[def] = ''
	repeat
		[i] + '1'
		if [item_levels:[i]] = [pos]
			if [cnt] > '0' [def] # [std_sep]
			[def] # [items:[i]]

			[cnt] + '1'
		elseif [item_levels:[i]] = [pos] + '1' & [i] > '1'
			[def] # ':' # [items:[i]]
			[pos] + '1'
			[cnt] = '1'
		elseif [item_levels:[i]] = [pos] - '1'
			[def] # ';' # [items:[i]]
			[pos] - '1'
			[cnt] = '1'
		elseif [item_levels:[i]] < [pos] - '1'
			[j] = '-1'
			repeat
				[j] + '1'
				[def] # ';'
			until [j] = [pos]-'1'
			[def] # [items:[i]]
			[pos] = [item_levels:[i]]
			[cnt] = '1'
		elseif [item_levels:[i]] > [pos] + '1' | [i] = '1' & [item_levels:[i]] > '0'
		
			getlng_label [lng_text] = 'md_error'
			replacevar [lng_text] = [lng_text], '%ITEM', [items:[i]]
			msgbox 'Error', [lng_text], '16'
			goto 'correct'
		endif
	until [i] = [items:'0']	
	
	letdialog 'md', 'visible', [false]
	enableIDE [true]


	[menu_height] = [Metric_Menu_Height]
	ret [def] # ';'
	
	rem --- sub routine for updating the item list
	%updatemenu
	[list] = [null]
	
	if [items:'0'] > '0' & [item_levels:'0'] > '0'
		[i] = '0'
		repeat
			[i] + '1'
			if [item_levels:[i]] > '0'
				[j] = '0'
				repeat
					[j] + '1'
					[list] # ' -> '
				until [j] = [item_levels:[i]]
			endif
			[list] # [items:[i]] # [std_sep]
		until [i] = [items:'0']
	endif

	letdialog 'md:list', 'items', [list]
	letdialog 'md:list', 'selindex', [pos]

	if [list] ! [null]
		letdialog 'md:item_update', 'enabled', '0'
		letdialog 'md:item_delete', 'enabled', '0'
	else
		letdialog 'md:item_update', 'enabled', '-1'
		letdialog 'md:item_delete', 'enabled', '-1'
	endif
	
	resume
endfunc

rem event_editor
rem ============
rem The event-editor is used to enable and disable events that can
rem happen to a widget.
proc event_editor: [pos]
	dec [event], [lng_text], [eid], [estate], [i]

	enableIDE [false]
	
	rem --- creating code for widget ee
	newdialog 'ee', 'DIALOG', '220|100|235|250'

	rem --- only create the rest of the dialog if this is the first
	rem     call of event_editor
	if [errorcode] = [true]
		getlng_label [lng_text] = 'ee_title'
		replacevar [lng_text] = [lng_text], '#WIDGET', [widgets:[pos], [w_xname]]
		letdialog 'ee', 'caption', [lng_text]
		
		letdialog 'ee', 'style', 'DIALOG'
		
		letdialog 'ee', 'event_ok', [true]
		letdialog 'ee', 'event_cancel', [true]
		
		rem --- creating code for widget e_group
		newdialog 'ee:e_group', 'GROUP', '3|1|120|210'
		letdialog 'ee:e_group', 'enabled', '0'
		
		getlng_label [lng_text] = 'ee_group'
		letdialog 'ee:group', 'caption', [lng_text]
		
		rem --- creating code for widget e_click
		newdialog 'ee:e_click', 'OPTION', '10|15|100|21'
		letdialog 'ee:e_click', 'caption', 'CLICK'
		
		rem --- creating code for widget e_dblclick
		newdialog 'ee:e_dblclick', 'OPTION', '10|36|100|21'
		letdialog 'ee:e_dblclick', 'caption', 'DBLCLICK'
		
		rem --- creating code for widget e_change
		newdialog 'ee:e_change', 'OPTION', '10|57|100|21'
		letdialog 'ee:e_change', 'caption', 'CHANGE'
		
		rem --- creating code for widget e_close
		newdialog 'ee:e_close', 'OPTION', '10|78|100|21'
		letdialog 'ee:e_close', 'caption', 'CLOSE'
		
		rem --- creating code for widget e_move
		newdialog 'ee:e_move', 'OPTION', '10|99|100|21'
		letdialog 'ee:e_move', 'caption', 'MOVE'

		rem --- creating code for widget e_size
		newdialog 'ee:e_size', 'OPTION', '10|120|100|21'
		letdialog 'ee:e_size', 'caption', 'SIZE'
		
		rem --- creating code for widget e_mousemove
		newdialog 'ee:e_mousemove', 'OPTION', '10|141|100|21'
		letdialog 'ee:e_mousemove', 'caption', 'MOUSEMOVE'
		
		rem --- creating code for widget e_ok
		newdialog 'ee:e_ok', 'OPTION', '10|162|100|21'
		letdialog 'ee:e_ok', 'caption', 'OK'

		rem --- creating code for widget e_cancel
		newdialog 'ee:e_cancel', 'OPTION', '10|183|100|21'
		letdialog 'ee:e_cancel', 'caption', 'CANCEL'
		
		rem --- creating code for widget ok
		newdialog 'ee:ok', 'BUTTON', '125|5|100|25'
		letdialog 'ee:ok', 'enabled', '0'
		getlng_label [lng_text] = 'ok'
		letdialog 'ee:ok', 'caption', [lng_text]
		
		rem --- creating code for widget cancel
		newdialog 'ee:cancel', 'BUTTON', '125|30|100|25'
		letdialog 'ee:cancel', 'enabled', '0'
		getlng_label [lng_text] = 'cancel'
		letdialog 'ee:cancel', 'caption', [lng_text]
	endif

	rem --- first, disable all checkboxes and uncheck them
	letdialog 'ee:e_click', 'enabled', [false]
	letdialog 'ee:e_click', 'checked', [false]
	
	letdialog 'ee:e_dblclick', 'enabled', [false]
	letdialog 'ee:e_dblclick', 'checked', [false]
	
	letdialog 'ee:e_change', 'enabled', [false]
	letdialog 'ee:e_change', 'checked', [false]
	
	letdialog 'ee:e_close', 'enabled', [false]
	letdialog 'ee:e_close', 'checked', [false]
	
	letdialog 'ee:e_move', 'enabled', [false]
	letdialog 'ee:e_move', 'checked', [false]
	
	letdialog 'ee:e_size', 'enabled', [false]
	letdialog 'ee:e_size', 'checked', [false]
	
	letdialog 'ee:e_mousemove', 'enabled', [false]
	letdialog 'ee:e_mousemove', 'checked', [false]
	
	letdialog 'ee:e_ok', 'enabled', [false]
	letdialog 'ee:e_ok', 'checked', [false]
	
	letdialog 'ee:e_cancel', 'enabled', [false]
	letdialog 'ee:e_cancel', 'checked', [false]	
	
	
	rem --- enable events that are possible
	if [widgets:[pos],[w_events]] ! ''
		[i] = '0'
		repeat
			[i] + '1'
			gettok [eid] = [widgets:[pos],[w_events]], '|', [i]

			if [eid] ! ''
				letdialog 'ee:e_' # [eid], 'enabled', [true]
				gettok [estate] = [widgets:[pos],[w_event_states]], '|', [i]
				if [estate] = [true]
					letdialog 'ee:e_' # [eid], 'checked', [true]
				endif			
			endif
		until [eid] = ''
	endif
	
	rem --- make the dialog window visible
	centerdialog 'ee'
	letdialog 'ee', 'visible', [true]
	
	repeat
		rundialog [event] = '0'
	
		rem --- event handling for click on ok
		if [event] = 'click_ee:ok' | [event] = 'ok_ee'
			if [widgets:[pos],[w_events]] ! ''
			
				[widgets:[pos],[w_event_states]] = ''
				
				[i] = '0'
				repeat
					[i] + '1'
					gettok [eid] = [widgets:[pos],[w_events]], '|', [i]
		
					if [eid] ! ''
						letdialog 'ee:e_' # [eid], 'enabled', [true]
						getdialog [estate] = 'ee:e_' # [eid], 'checked'
						
						if [widgets:[pos],[w_event_states]] ! ''
							[widgets:[pos],[w_event_states]] # '|'
						endif
						
						[widgets:[pos],[w_event_states]] # [estate]
					endif
				until [eid] = ''
			endif

			[file_changed] = [true]
			break			
		endif
	
	until [event] = 'close_ee' | [event] = 'cancel_ee' | [event] = 'click_ee:cancel'
	
	letdialog 'ee', 'visible', [false]
	enableIDE [true]

endproc

rem changeWidgetOrder
rem =================
rem Lets the user change the widget creation order
proc changeWidgetOrder
	dec [l], [i], [lng_text], [tmp_widget], [sel]
	
	rem --- creating code for widget "cwo"
	newdialog 'cwo', 'DIALOG', '223|101|234|308'
	if [errorcode] = [true]	
		letdialog 'cwo', 'style', 'DIALOG'
		
		getlng_label [lng_text] = 'cwo_title'
		letdialog 'cwo', 'caption', [lng_text]
	
		rem --- creating code for widget "wlist"
		newdialog 'cwo:wlist', 'LIST', '1|1|180|272'
		letdialog 'cwo:wlist', 'enabled', '0'
	
		rem --- creating code for widget "m_up"
		newdialog 'cwo:m_up', 'IMAGEBUTTON', '183|2|40|40'
		letdialog 'cwo:m_up', 'enabled', '0'
		letdialog 'cwo:m_up', 'image_transparent', [dd_dir] # '\img\b_up.bmp'
	
		rem --- creating code for widget "m_down"
		newdialog 'cwo:m_down', 'IMAGEBUTTON', '183|42|40|40'
		letdialog 'cwo:m_down', 'enabled', '0'
		letdialog 'cwo:m_down', 'image_transparent', [dd_dir] # '\img\b_down.bmp'
	endif
	
	if [widgets:'0'] = '1'
		getlng_label [lng_text] = 'msg_no_child_widgets'
		echo [lng_text]
		ret
	endif
	
	rem --- make the dialog window visible
	enableIDE [false]
	centerdialog 'cwo'
	letdialog 'cwo', 'visible', [true]
	
	[sel] = '1'
	gosub 'update_widget_list'
	
	repeat
		rundialog [event] = '0'
	
		rem --- event handling for click on "m_up"
		if [event] = 'click_cwo:m_up'
			getdialog [sel] = 'cwo:wlist', 'selindex'
			
			if [sel] > '1'
				copyarray [widgets:[sel]+'1'], [tmp_widget]				
				copyarray [widgets:[sel]], [widgets:[sel]+'1']
				copyarray [tmp_widget], [widgets:[sel]]
				
				[sel] - '1'
				gosub 'update_widget_list'
				
				[file_changed] = [true]
			endif			
		rem --- event handling for click on "m_down"
		elseif [event] = 'click_cwo:m_down'
			getdialog [sel] = 'cwo:wlist', 'selindex'
			if [sel] < [widgets:'0']-'1'
				copyarray [widgets:[sel]+'1'], [tmp_widget]
				copyarray [widgets:[sel]+'1'+'1'], [widgets:[sel]+'1']
				copyarray [tmp_widget], [widgets:[sel]+'1'+'1']
				
				[sel] + '1'
				gosub 'update_widget_list'
				
				[file_changed] = [true]				
			endif			
		endif
	
	until [event] = 'close_cwo'

	enableIDE [true]
	letdialog 'cwo', 'visible', [false]
	ret
	
	%update_widget_list
	[i] = '1'
	[l] = ''
	
	if [widgets:'0'] > '1'
		repeat
			[i] + '1'
			if [l] ! '' [l] # [std_sep]
			
			[l] # [widgets:[i],[w_xname]] # ' (' # [widgets:[i],[w_type]] # ')'
		until [i] = [widgets:'0']
	endif
	
	letdialog 'cwo:wlist', 'items', [l]
	letdialog 'cwo:wlist', 'selindex', [sel]

	resume
	
endproc

rem updateWidgetList
rem ================
rem Reloads the list of widgets on the widget attributes window on the left
proc updateWidgetList
	dec [l], [i]
	
	[i] = '0'

	if [widgets:'0'] > '0'
		repeat
			[i] + '1'
			if [l] ! '' [l] # [std_sep]
			
			[l] # [widgets:[i],[w_xname]] # ' (' # [widgets:[i],[w_type]] # ')'
		until [i] = [widgets:'0']
	endif
	
	letdialog 'dw:wlist', 'items', [l]
	letdialog 'dw:wlist', 'selindex', [i]+'1'
endproc

rem updateAttribInfo
rem ================
rem Reloads the list of supported and possible attributes to the currently
rem selected widget type.
proc updateAttribInfo
	dec [att], [val], [i], [type]
	getdialog [att] = 'dw:a_list', 'selection'
	getdialog [val] = [widgets:[current_widget][w_name]], [att]
	
	[i] = '0'
	repeat
		[i] + '1'
		gettok [type] = [widgets:[current_widget],[w_attrib]], '|', [i]
	until [type] = [att]
	
	gettok [type] = [widgets:[current_widget],[w_attrib],'1'], '|', [i]
	
	letdialog 'dw:a_string', 'visible', [false]
	letdialog 'dw:a_bool_true', 'visible', [false]
	letdialog 'dw:a_bool_false', 'visible', [false]

	letdialog 'dw:a_file_name', 'visible', [false]
	letdialog 'dw:a_file_select', 'visible', [false]

	letdialog 'dw:a_itemstring', 'visible', [false]
	letdialog 'dw:a_items_edit', 'visible', [false]
	
	letdialog 'dw:a_menustring', 'visible', [false]
	letdialog 'dw:a_menu_edit', 'visible', [false]

	letdialog 'dw:a_style', 'visible', [false]
	
	if [type] = 'string'	
		letdialog 'dw:a_string', 'text', [val]
		letdialog 'dw:a_string', 'visible', [true]
		
		letdialog 'dw:a_string', 'selpos', '1|-1'
		letdialog 'dw:a_string', 'focus', [true]
	elseif [type] = 'bool'
		letdialog 'dw:a_bool_true', 'visible', [true]
		letdialog 'dw:a_bool_false', 'visible', [true]
		letdialog 'dw:a_bool_true', 'checked', [false]
		letdialog 'dw:a_bool_false', 'checked', [false]
		
		if [val] = [true]
			letdialog 'dw:a_bool_true', 'checked', [true]
		else
			letdialog 'dw:a_bool_false', 'checked', [true]		
		endif
	elseif [type] = 'file'
		letdialog 'dw:a_file_name', 'visible', [true]
		letdialog 'dw:a_file_select', 'visible', [true]

		if [val] = ''
			openbox [val] = 'Select a file:', 'Image-Files|*.ico;*.bmp|All files|*.*'
			letdialog [widgets:[current_widget][w_name]], [att], [val]
		endif

		letdialog 'dw:a_file_name', 'text', [val]
		letdialog 'dw:a_file_name', 'focus', [true]
		letdialog 'dw:a_file_name', 'selpos', '1|-1'
	elseif [type] = 'itemlist'
		letdialog 'dw:a_itemstring', 'visible', [true]
		letdialog 'dw:a_items_edit', 'visible', [true]

		if [val] = [null]
			item_editor [val] = [null]
			letdialog [widgets:[current_widget][w_name]], [att], [val]
		endif
		letdialog 'dw:a_itemstring', 'text', [val]
		
		letdialog 'dw:a_itemstring', 'focus', [true]
		letdialog 'dw:a_itemstring', 'selpos', '1|-1'
	elseif [type] = 'menu'
		letdialog 'dw:a_menustring', 'visible', [true]
		letdialog 'dw:a_menu_edit', 'visible', [true]
		letdialog 'dw:a_menustring', 'text', [val]

		letdialog 'dw:a_menustring', 'focus', [true]
		letdialog 'dw:a_menustring', 'selpos', '1|-1'
	elseif [type] = 'style'	
		letdialog 'dw:a_style', 'text', [val]
		letdialog 'dw:a_style', 'visible', [true]
		
		letdialog 'dw:a_style', 'focus', [true]

	endif
endproc

rem updateWidgetInfo
rem ================
rem Updates the positions of the widget when it is moved or scaled
proc updateWidgetInfo
	retrievepositions [widgets:[current_widget][w_name]], [widgets:[current_widget][w_x]], [widgets:[current_widget][w_y]], [widgets:[current_widget][w_width]], [widgets:[current_widget][w_height]]
	letdialog 'dw:p_x', 'text', [widgets:[current_widget][w_x]]
	letdialog 'dw:p_y', 'text', [widgets:[current_widget][w_y]]
	letdialog 'dw:p_width', 'text', [widgets:[current_widget][w_width]]
	letdialog 'dw:p_height', 'text', [widgets:[current_widget][w_height]]
endproc

rem updateModeState
rem ===============
rem Updates the state of the movement modes
proc updateModeState
	letdialog 'dw:m_none', 'checked', [false]
	letdialog 'dw:m_move', 'checked', [false]
	letdialog 'dw:m_scale', 'checked', [false]

	if [mode] = 'none'
		letdialog 'dw:m_none', 'checked', [true]
	elseif [mode] = 'move'
		letdialog 'dw:m_move', 'checked', [true]
	else
		letdialog 'dw:m_scale', 'checked', [true]
	endif
		
endproc

rem setCurrentWidget
rem ================
rem Sets a new widget as current widget and updates all its display
rem informations
proc setCurrentWidget
	rem --- setting the item
	letdialog 'dw:wlist', 'selindex', [current_widget]
	letdialog 'dw:a_list', 'items', [widgets:[current_widget],[w_attrib]]

	rem --- updating the information
	updateWidgetInfo
	updateAttribInfo
	
	rem --- do not enable movement functions if [current_widget] is the
	rem --- base dialog window
	if [current_widget] > '1'
		if [mode] ! 'none'
			getdialog [current_widget_enabled] = [widgets:[current_widget][w_name]], 'enabled'
			letdialog [widgets:[current_widget][w_name]], 'enabled', [false]
		endif

		letdialog 'dw:m_none', 'enabled', [true]
		letdialog 'dw:m_move', 'enabled', [true]
		letdialog 'dw:m_scale', 'enabled', [true]
		letdialog 'dw:m_group', 'enabled', [true]
		
		letdialog 'dd:Menu_menu_edit_remove_widget', 'enabled', [true]
		letdialog 'dd:Menu_menu_edit_rename_widget', 'enabled', [true]
		letdialog 'dd:Menu_menu_edit_duplicate_widget', 'enabled', [true]
	else
		letdialog 'dw:m_none', 'enabled', [false]
		letdialog 'dw:m_move', 'enabled', [false]
		letdialog 'dw:m_scale', 'enabled', [false]
		letdialog 'dw:m_group', 'enabled', [false]
		
		letdialog 'dd:Menu_menu_edit_remove_widget', 'enabled', [false]
		letdialog 'dd:Menu_menu_edit_rename_widget', 'enabled', [false]
		letdialog 'dd:Menu_menu_edit_duplicate_widget', 'enabled', [false]
	endif

endproc


rem createNewWidget
rem ================
rem Creates a new widget; This procedure is called both when interactively
rem creating new widgets or when the widgets are read from a file.
proc createNewWidget: [type], [dx], [dy], [dw], [dh], [name], [internal], [update_list], [on_file_load]
	dec [pos], [lng_text], [lng_title]
	
	upvar [type] = [type]

	[pos] = '0'
	if [widgets:'0'] > '0'
		repeat
			[pos] + '1'
			if [widgets:[pos],[w_type]] = ''
				break
			endif
		until [pos] = [widgets:'0']
	endif
	[pos] + '1'
	
	if [name] = [null] & [internal] = [null]
	
		getlng_label [lng_title] = 'msg_new_widget_title'
		getlng_label [lng_text] = 'msg_new_widget_name'
		
		replacevar [lng_text] = [lng_text], '%TYPE', [type]

		inputbox [name] = [lng_title], [lng_text], 'myWidget' # ([pos]-'1')
		if [name] = '' ret

		if [designwin] = [null]
			[internal] = [name]
		else
			[internal] = [designwin] # ':' # [name]
		endif
	endif

	rem work-around:
	[errorcode] = '0'
	
	newdialog [internal], [type], [dx] # '|' # [dy] # '|' # [dw] # '|' # [dh]
	if [errorcode] = [true]	
		[widgets:[pos],[w_type]] 		= [type]
		
		[widgets:[pos],[w_name]] 		= [internal]
				
		[widgets:[pos],[w_xname]] 		= [name]
		[widgets:[pos],[w_x]] 			= [dx]
		[widgets:[pos],[w_y]] 			= [dy]
		[widgets:[pos],[w_width]] 		= [dw]
		[widgets:[pos],[w_height]] 		= [dh]

		[widgets:[pos],[w_attrib]]		= 'ENABLED|VISIBLE|TOOLTIP|'
		[widgets:[pos],[w_attrib],'1']	= 'bool|bool|string|'
		
		[widgets:[pos],[w_events]]		= [null]
		[widgets:[pos],[w_event_states]] 	= [null]
		
		if [type] = 'DIALOG'
			[widgets:[pos],[w_attrib]]		# 'CAPTION|ICON|MENU|STATUSBAR|STYLE'
			[widgets:[pos],[w_attrib],'1']	# 'string|file|menu|string|style'
			
			[widgets:[pos],[w_events]]		= 'click|move|size|mousemove|close|ok|cancel'
			[widgets:[pos],[w_event_states]]	= '-1|-1|-1|-1|0|-1|-1'
		elseif [type] = 'BUTTON'
			[widgets:[pos],[w_attrib]]		# 'CAPTION|FONT'
			[widgets:[pos],[w_attrib],'1']	# 'string|string'

			[widgets:[pos],[w_events]]		= 'click'
			[widgets:[pos],[w_event_states]]	= '0'
		elseif [type] = 'IMAGEBUTTON' | [type] = 'IMAGE'
			[widgets:[pos],[w_attrib]]		# 'IMAGE|IMAGE_TRANSPARENT'
			[widgets:[pos],[w_attrib],'1']	# 'file|file'

			[widgets:[pos],[w_events]]		= 'click'
			[widgets:[pos],[w_event_states]]	= '0'
		elseif [type] = 'LABEL'
			[widgets:[pos],[w_attrib]]		# 'CAPTION|FONT'
			[widgets:[pos],[w_attrib],'1']	# 'string|string'

			[widgets:[pos],[w_events]]		= 'click'
			[widgets:[pos],[w_event_states]]	= '-1'
		elseif [type] = 'INPUT' | [type] = 'EDIT' | [type] = 'EDIT_LINEWRAP' | [type] = 'PWD'
			[widgets:[pos],[w_attrib]]		# 'FONT|READONLY|TEXT'
			[widgets:[pos],[w_attrib],'1']	# 'string|bool|string'

			[widgets:[pos],[w_events]]		= 'change'
			[widgets:[pos],[w_event_states]]	= '-1'
		elseif [type] = 'OPTION' | [type] = 'RADIO'
			[widgets:[pos],[w_attrib]]		# 'CAPTION|CHECKED|FONT'
			[widgets:[pos],[w_attrib],'1']	# 'string|bool|string'

			[widgets:[pos],[w_events]]		= 'click'
			[widgets:[pos],[w_event_states]]	= '0'
		elseif [type] = 'LIST' | [type] = 'LIST_SORTED' | [type] = 'LIST_MULTI' | [type] = 'LIST_MULTISORTED'
			[widgets:[pos],[w_attrib]]		# 'ITEMS|FONT'
			[widgets:[pos],[w_attrib],'1']	# 'itemlist|string'

			[widgets:[pos],[w_events]]		= 'dblclick|change'
			[widgets:[pos],[w_event_states]]	= '-1|-1'
		elseif [type] = 'COMBO'
			[widgets:[pos],[w_attrib]]		# 'ITEMS|FONT|TEXT'
			[widgets:[pos],[w_attrib],'1']	# 'itemlist|string|string'
			
			[widgets:[pos],[w_events]]		= 'change'
			[widgets:[pos],[w_event_states]]	= '-1'
		elseif [type] = 'STATIC_COMBO'
			[widgets:[pos],[w_attrib]]		# 'ITEMS|FONT'
			[widgets:[pos],[w_attrib],'1']	# 'itemlist|string'
			
			[widgets:[pos],[w_events]]		= 'change'
			[widgets:[pos],[w_event_states]]	= '-1'
		elseif [type] = 'PROGRESS'
			[widgets:[pos],[w_attrib]]		# 'PROGRESS'
			[widgets:[pos],[w_attrib],'1']	# 'string'
			
			[widgets:[pos],[w_events]]		= ''
			[widgets:[pos],[w_event_states]]	= ''
		elseif [type] = 'GROUP'
			[widgets:[pos],[w_attrib]]		# 'CAPTION'
			[widgets:[pos],[w_attrib],'1']	# 'string'
		endif		
		
		if [designwin] = [null]
			[designwin] = [internal]
			letdialog [designwin], 'event_click', '0'
			letdialog [designwin], 'event_move', '0'
			letdialog [designwin], 'event_size', '0'
			letdialog [designwin], 'event_mousemove', '0'
			rem letdialog [designwin], 'event_enter', '0'
		endif

		if [update_list] = [true]
			[current_widget] = [pos]
		
			updateWidgetList
		
			[mode] = 'none'
			setCurrentWidget
		
			[file_changed] = [true]
		endif
		
		if [on_file_load] = [false]
			rem setting some default values
			if [type] = 'IMAGE' | [type] = 'IMAGEBUTTON'
				letdialog [internal], 'image_transparent', [dd_dir] # '\img\default_image.bmp'
			elseif [type] = 'LABEL' | [type] = 'BUTTON' | [type] = 'OPTION' | [type] = 'RADIO' | [type] = 'DIALOG'
				letdialog [internal], 'caption', [name]
			endif
		endif
	else
		[current_widget] = '0'
		echo 'Error: Unable to create widget!'
	endif
endproc


rem gencode
rem =======
rem Generates RapidBATCH script source-code based on the current dialog
rem definition that is loaded into the Dialog Designer
func gencode: [method], [with_attribs], [event_model]
	dec [code], [ccode], [pcode], [i], [j], [q], [att], [attval], [events], [is_active], [first_if], [widget_name], [prefix_len], [len]
	dec [designwin_without_prefix], [menu], [ch], [pos], [menu], [menu_items], [menu_levels], [eid], [estate], [widget_height]
	
	[first_if] = [true]
	
	rem --- build events array
	[events:'1'] = 'click'
	[events:'2'] = 'dblclick'
	[events:'3'] = 'move'
	[events:'4'] = 'size'
	[events:'5'] = 'mousemove'
	[events:'6'] = 'esc'
	[events:'7'] = 'enter'
	[events:'8'] = 'change'

	rem --- retrieve single quotation character (')
	getchr [q] = '39'
	
	rem --- getting prefix length
	getlen [prefix_len] = [prefix]
	
	rem --- only at rbd_file method
	if [method] = 'rbd_file'
		[code] # 'rem --- importing dialog.rb for gendlg-function' # [crlf]
		[code] # 'include ' # [q] # 'dialog.rb' # [q] # [crlf]
		[code] # [crlf]
		[code] # 'rem --- building the dialog' # [crlf]
		[code] # 'gendlg [main] = ' # [q] # [current_file] # [q] # [crlf]
		[code] # 'if [main] = [null] halt' # [crlf] # [crlf]
	endif
	
	rem --- for dialog windows: Create menu event handler, if a menu exists	
	getdialog [menu] = [widgets:'1',[w_name]], 'MENU'
	if [menu] ! ''
		getmenu_fromdef [menu], [menu_items], [menu_levels]
		
		if [menu_items:'0'] > '0'
			[i] = '0'
			repeat
				[i] + '1'
						
				if [i] < [menu_items:'0'] & [menu_levels:[i]] = [menu_levels:[i]+'1'] | [menu_levels:[i]] > [menu_levels:[i]+'1'] & [menu_items:[i]] ! '-'
					if [event_model] = 'if_elseif'
						rem --- add comment for new control part
						[ccode] # [tab] # 'rem --- event handling for menu item "' # [menu_items:[i]] # '"' # [crlf]
			
						[ccode] # [tab]
						if [first_if] = [true]
							[ccode] # 'if'
							[first_if] = [false]
						else
							[ccode] # 'elseif'
						endif
			
						[ccode] # ' [event] = ' # [q] # 'click_' # [widgets:'1',[w_xname]] # ':Menu_' # [menu_items[i]] # [q] # [crlf]
						[ccode] # [tab] # [tab] # 'rem *** insert event code here ***' # [crlf] # [tab] # [tab] # [crlf]

					elseif [event_model] = 'gosub_resume'						
						rem --- add comment for new control part
						[pcode] # 'rem --- event handling for menu item "' # [menu_items:[i]] # '"' # [crlf]
			
						[pcode] # '%' # 'click_' # [widgets:'1',[w_xname]] # ':Menu_' # [menu_items[i]] # [crlf]
						
						[pcode] # [tab] # 'rem *** insert event code here ***' # [crlf] # [tab] # [crlf]

						[pcode] # 'resume' # [crlf]
						
					else
						rem --- add comment for new control part
						[ccode] # [tab]
						if [first_if] = [true]
							[ccode] # 'if'
							[first_if] = [false]
						else
							[ccode] # 'elseif'
						endif
						
						replacevar [proc_name] = [widgets:'1',[w_xname]] # '_Menu_' # [menu_items[i]] # '_click', ' ', '_'
						replacevar [proc_name] = [proc_name], ':', '_'
						replacevar [proc_name] = [proc_name], '.', ''
						replacevar [proc_name] = [proc_name], '&', ''
						
						[ccode] # ' [event] = ' # [q] # 'click_' # [widgets:'1',[w_xname]] # ':Menu_' # [menu_items[i]] # [q] # [crlf]
						[ccode] # [tab] # [tab] # [proc_name] # [crlf]
						
						[pcode] # 'rem --- event handling procedure for menu item "' # [menu_items:[i]] # '"' # [crlf]
						[pcode] # 'proc ' # [proc_name] # [crlf]
						[pcode] # [tab] # 'rem *** insert event code here ***' # [crlf]
						[pcode] # 'endproc' # [crlf] # [crlf]
					endif
				endif
			until [i] = [menu_items:'0']
		endif
	endif
	
	rem --- loop through the widget list
	[i] = '0'
	repeat
		[i] + '1'
		
		if [prefix_len] > '0'
			getlen [len] = [widgets:[i],[w_name]]
			copyvar [widget_name] = [widgets:[i],[w_name]], [prefix_len]+'1', [len]
		else
			[widget_name] = [widgets:[i],[w_name]]
		endif
		
		if [designwin_without_prefix] = [null]
			[designwin_without_prefix] = [widget_name]
		endif
	
		rem only at core method
		if [method] = 'core'
			rem --- add comment for new widget
			[code] # 'rem --- creating code for widget "' # [widgets:[i],[w_xname]] # '"'
			[code] # [crlf]

			rem --- statement to create new widget
			rem --- gen: newdialog '[widgets:[i],[w_name]]', '[widgets:[i],[w_type]]', '[widgets:[i],[w_x]]|[widgets:[i],[w_y]]|[widgets:[i],[w_width]]|[widgets:[i],[w_height]]'
			
			rem solving the problem with the combo-box height
			if [widgets:[i],[w_type]] = 'COMBO' & [widgets:[i],[w_height]] < '30'
				[widget_height] = '200'
			else
				[widget_height] = [widgets:[i],[w_height]]
			endif
			
			[code] # 'newdialog ' # [q] # [widget_name] # [q] # ', '
			[code] # [q] # [widgets:[i],[w_type]] # [q] # ', '
			[code] # [q] # [widgets:[i],[w_x]] # [std_sep] # [widgets:[i],[w_y]] # [std_sep]
			[code] # [widgets:[i],[w_width]] # [std_sep] # [widget_height] # [q]
			[code] # [crlf]
			
		endif
		
		rem --- build statements for attributes (if used!)
		if [method] = 'core' & [with_attribs] = [true]
			[j] = '0'
			repeat
				[j] + '1'
				gettok [att] = [widgets:[i],[w_attrib]], '|', [j]
				if [att] ! ''
					getdialog [attval] = [widgets:[i],[w_name]], [att]
					if [attval] ! ''
						lowvar [att] = [att]
						
						rem --- let's do some optimizations: Is this an attribute
						rem     that REALLY needs to be generated?
						if [i] = '1' & [att] = 'visible' cont
						if [i] = '1' & [att] = 'enabled' cont
						if [att] = 'enabled' & [attval] = [true] cont
						if [att] = 'readonly' & [attval] = [false] cont
						if [i] > '1' & [att] = 'visible' & [attval] = [true] cont
						
						rem --- generate the code!
						[code] # 'letdialog ' # [q] # [widget_name] # [q] # ', '
						[code] # [q] # [att] # [q] # ', '
						
						if [attval] = [true]
							[code] # '[true]' # [crlf]
						elseif [attval] = [false]
							[code] # '[false]' # [crlf]
						else
							[code] # [q] # [attval] # [q] # [crlf]
						endif
					endif
				endif
			until [att] = ''
		endif
				
		rem --- build event handling for widgets
		if [event_model] ! ''
			if [i] > '0'
				[j] = '0'
				repeat
					[j] + '1'
					gettok [eid] = [widgets:[i],[w_events]], '|', [j]
					if [eid] = '' break
					gettok [is_active] = [widgets:[i],[w_event_states]], '|', [j]
					
					rem --- build statements for setting events (if used!)
					if [method] = 'core' & [with_attribs] = [true]
					
						if [eid] ! 'close'
							dec [e_used]
						
							rem --- for all child-widgets: check the default widget's event mode, and enable only
							rem     those events which aren't set automatically!
							if [i] > '1'
								getdialog [e_used] = [widgets:[i],[w_name]], 'event_' # [eid]
							else
								[e_used] = [false]
							endif

							if [e_used] ! [is_active]
								rem gen: letdialog '[widgets:[i],[w_name]]' , '[att]', '[attval]'
								[code] # 'letdialog ' # [q] # [widget_name] # [q] # ', '
								[code] # [q] # 'event_' # [eid] # [q] # ', '
								
								if [is_active] = [true]
									[code] # '[true]' # [crlf]
								else
									[code] # '[false]' # [crlf]
								endif								
							endif
						endif

					endif
					
					rem getdialog [is_active] = [widgets:[i],[w_name]], 'event_' # [events[j]]					
			
					rem Note: Do not make event for dialog's close event: It always is created!
					if [is_active] = [true]	& [eid] ! 'close'
						if [event_model] = 'if_elseif'
							rem --- add comment for new control part
							[ccode] # [tab] # 'rem --- event handling for ' # [eid] # ' on "' # [widgets:[i],[w_xname]] # '"' # [crlf]
				
							[ccode] # [tab]
							if [first_if] = [true]
								[ccode] # 'if'
								[first_if] = [false]
							else
								[ccode] # 'elseif'
							endif
				
							[ccode] # ' [event] = ' # [q] # [eid] # '_' # [widget_name] # [q] # [crlf]
							[ccode] # [tab] # [tab] # 'rem *** insert event code here ***' # [crlf] # [tab] # [tab] # [crlf]

						elseif [event_model] = 'gosub_resume'						
							rem --- add comment for new control part
							[pcode] # 'rem --- event handling for ' # [eid] # ' on "' # [widgets:[i],[w_xname]] # '"' # [crlf]
				
							[pcode] # '%' # [eid] # '_' # [widget_name] # [crlf]
							
							[pcode] # [tab] # 'rem *** insert event code here ***' # [crlf] # [tab] # [crlf]

							[pcode] # 'resume' # [crlf]
							
						else
							rem --- add comment for new control part
							[ccode] # [tab]
							if [first_if] = [true]
								[ccode] # 'if'
								[first_if] = [false]
							else
								[ccode] # 'elseif'
							endif
							
							replacevar [proc_name] = [widget_name] # '_' # [eid], ' ', '_'
							replacevar [proc_name] = [proc_name], ':', '_'
							replacevar [proc_name] = [proc_name], '.', ''
							replacevar [proc_name] = [proc_name], '&', ''
							
							[ccode] # ' [event] = ' # [q] # [eid] # '_' # [widget_name] # [q] # [crlf]
							[ccode] # [tab] # [tab] # [proc_name] # [crlf]
							
							[pcode] # 'rem --- event handling procedure for ' # [eid] # ' on "' # [widgets:[i],[w_xname]] # '"' # [crlf]
							[pcode] # 'proc ' # [proc_name] # [crlf]
							[pcode] # [tab] # 'rem *** insert event code here ***' # [crlf]
							[pcode] # 'endproc' # [crlf] # [crlf]
						endif
					endif
				until [eid] = ''
			endif
		endif
		
		if [method] = 'core' [code] # [crlf]
	until [i] = [widgets:'0']
	
	if [event_model] = 'if_elseif' | [event_model] = 'procedures' & [first_if] = [false]
		[ccode] # [tab] # 'endif' # [crlf]
	endif
	
	if [event_model] = 'procedures'
		[code] # [pcode]
	endif
	
	rem --- gen: letdialog '[widgets:[i],[w_name]]' , '[att]', '[attval]'
	[code] # 'rem --- make the dialog window visible' # [crlf]
	[code] # 'letdialog ' # [q] # [designwin_without_prefix] # [q] # ', '

	[code] # [q] # 'visible' # [q] # ', ' # '[true]' # [crlf]
	
	if [event_model] ! ''
		[code] # [crlf]
		[code] # 'repeat' # [crlf]
		[code] # [tab] # 'rundialog [event] = ' # [q] # '0' # [q] # [crlf]
	
		if [event_model] = 'gosub_resume'
			[code] # [crlf]
			[code] # [tab] # 'if [event] ! ' # [q] # 'close_' # [designwin_without_prefix] # [q] # [crlf]
			[code] # [tab] # [tab] # 'gosub [event]' # [crlf]
			[code] # [tab] # 'endif' # [crlf]
		elseif [ccode] ! ''
			[code] # [crlf] # [ccode] # [crlf]
		endif
	
		[code] # 'until [event] = ' # [q] # 'close_' # [designwin_without_prefix] # [q] # [crlf]
	
		if [event_model] = 'gosub_resume'
			[code] # [crlf] # [crlf] # 'halt' # [crlf] # [crlf] # [pcode] # [crlf]
		endif
	endif
	
	[code] # 'end'
			
	ret [code]
endfunc

rem gencode_options
rem ===============
rem Displays the code generator's option dialog window; This dialog has also
rem been generated by the Visual Dialog Designer itself ;) ...
proc gencode_options
	dec [event], [checked], [lng_text], [method], [model], [withatt], [gc], [filename], [len], [prefix_len], [rb_path]

	enableIDE [false]

	rem --- creating code for widget cg
	newdialog 'cg', 'DIALOG', '318|162|383|387'
	
	rem if the dialog already exists, do nothing ;)
	if [errorcode] = [true]
		letdialog 'cg', 'style', 'dialog'

		getlng_label [lng_text] = 'cg_title'
		letdialog 'cg', 'caption', [lng_text]
	
		rem --- creating code for widget output_group
		newdialog 'cg:output_group', 'GROUP', '1|1|376|106'
		letdialog 'cg:output_group', 'enabled', '0'
		letdialog 'cg:output_group', 'visible', '0'
		
		getlng_label [lng_text] = 'cg_output_group'
		letdialog 'cg:output_group', 'caption', [lng_text]
	
		rem --- creating code for widget output_clipboard
		newdialog 'cg:output_clipboard', 'RADIO', '10|15|160|21'
		letdialog 'cg:output_clipboard', 'enabled', '0'
		letdialog 'cg:output_clipboard', 'visible', '0'
		letdialog 'cg:output_clipboard', 'checked', '0'
	
		getlng_label [lng_text] = 'cg_output_clipboard'
		letdialog 'cg:output_clipboard', 'caption', [lng_text]

		rem --- creating code for widget output_file
		newdialog 'cg:output_file', 'RADIO', '10|34|110|20'
		letdialog 'cg:output_file', 'enabled', '0'
		letdialog 'cg:output_file', 'visible', '0'
		letdialog 'cg:output_file', 'checked', '-1'

		getlng_label [lng_text] = 'cg_output_file'
		letdialog 'cg:output_file', 'caption', [lng_text]

		rem --- creating code for widget l_output_file_name
		newdialog 'cg:l_output_file_name', 'LABEL', '28|57|80|20'
		letdialog 'cg:l_output_file_name', 'enabled', '-1'
		letdialog 'cg:l_output_file_name', 'visible', '0'

		getlng_label [lng_text] = 'cg_output_file_name'
		letdialog 'cg:l_output_file_name', 'caption', [lng_text]
	
		rem --- creating code for widget output_file_name
		newdialog 'cg:output_file_name', 'INPUT', '120|52|230|25'
		letdialog 'cg:output_file_name', 'enabled', '-1'
		letdialog 'cg:output_file_name', 'visible', '0'
		letdialog 'cg:output_file_name', 'readonly', '-1'
		
		getlen [prefix_len] = [prefix]
		getlen [len] = [designwin]
		
		copyvar [filename] = [designwin], [prefix_len]+'1', [len]
		letdialog 'cg:output_file_name', 'text', [filename] # '.rb'
		
		rem --- creating code for widget output_file_select
		newdialog 'cg:output_file_select', 'BUTTON', '350|52|20|25'
		letdialog 'cg:output_file_select', 'enabled', '-1'
		letdialog 'cg:output_file_select', 'visible', '0'
		letdialog 'cg:output_file_select', 'caption', '...'
		
		rem --- creating code for widget output_file_rbb
		newdialog 'cg:output_file_rbb', 'OPTION', '122|80|250|21'
		letdialog 'cg:output_file_rbb', 'enabled', '-1'
		letdialog 'cg:output_file_rbb', 'visible', '0'
		
		getlng_label [lng_text] = 'cg_output_file_rbb'
		letdialog 'cg:output_file_rbb', 'caption', [lng_text]
		letdialog 'cg:output_file_rbb', 'checked', '0'
		
		rem --- creating code for widget method_group
		newdialog 'cg:method_group', 'GROUP', '1|107|376|60'
		letdialog 'cg:method_group', 'enabled', '0'
		letdialog 'cg:method_group', 'visible', '0'
	
		getlng_label [lng_text] = 'cg_method_group'
		letdialog 'cg:method_group', 'caption', [lng_text]
		
		rem --- creating code for widget method_core
		newdialog 'cg:method_core', 'RADIO', '10|122|300|21'
		letdialog 'cg:method_core', 'enabled', '0'
		letdialog 'cg:method_core', 'visible', '0'
	
		getlng_label [lng_text] = 'cg_method_core'
		letdialog 'cg:method_core', 'caption', [lng_text]
		
		letdialog 'cg:method_core', 'checked', '0'
		
		rem --- creating code for widget method_rbd_file
		newdialog 'cg:method_rbd_file', 'RADIO', '10|140|300|21'
		letdialog 'cg:method_rbd_file', 'enabled', '0'
		letdialog 'cg:method_rbd_file', 'visible', '0'
	
		getlng_label [lng_text] = 'cg_method_rbd_file'
		letdialog 'cg:method_rbd_file', 'caption', [lng_text]
		
		letdialog 'cg:method_rbd_file', 'checked', '-1'
		
		rem --- creating code for widget genopt_group
		newdialog 'cg:genopt_group', 'GROUP', '1|168|376|142'
		letdialog 'cg:genopt_group', 'enabled', '0'
		letdialog 'cg:genopt_group', 'visible', '0'
		
		getlng_label [lng_text] = 'cg_genopt_group'
		letdialog 'cg:genopt_group', 'caption', [lng_text]
		
		rem --- creating code for widget genopt_with_attributes
		newdialog 'cg:genopt_with_attributes', 'OPTION', '10|184|300|21'
		letdialog 'cg:genopt_with_attributes', 'enabled', '0'
		letdialog 'cg:genopt_with_attributes', 'visible', '0'
		
		getlng_label [lng_text] = 'cg_genopt_with_attributes'
		letdialog 'cg:genopt_with_attributes', 'caption', [lng_text]
	
		letdialog 'cg:genopt_with_attributes', 'checked', '0'
		
		rem --- creating code for widget genopt_event_handler
		newdialog 'cg:genopt_event_handler', 'OPTION', '10|204|300|21'
		letdialog 'cg:genopt_event_handler', 'enabled', '0'
		letdialog 'cg:genopt_event_handler', 'visible', '0'
	
		getlng_label [lng_text] = 'cg_genopt_event_handler'
		letdialog 'cg:genopt_event_handler', 'caption', [lng_text]
		
		letdialog 'cg:genopt_event_handler', 'checked', '-1'
		
		rem --- creating code for widget event_model
		newdialog 'cg:event_model', 'GROUP', '27|224|341|80'
		letdialog 'cg:event_model', 'enabled', '-1'
		letdialog 'cg:event_model', 'visible', '0'
		getlng_label [lng_text] = 'cg_genopt_event_model'
		letdialog 'cg:event_model', 'caption', [lng_text]
		
		rem --- creating code for widget event_model_ifelseif
		newdialog 'cg:event_model_ifelseif', 'RADIO', '36|240|300|21'
		letdialog 'cg:event_model_ifelseif', 'enabled', '-1'
		letdialog 'cg:event_model_ifelseif', 'visible', '0'
	
		getlng_label [lng_text] = 'cg_event_model_ifelseif'
		letdialog 'cg:event_model_ifelseif', 'caption', [lng_text]
		
		letdialog 'cg:event_model_ifelseif', 'checked', '0'
		
		rem --- creating code for widget event_model_gosub_resume
		newdialog 'cg:event_model_gosub_resume', 'RADIO', '36|260|300|21'
		letdialog 'cg:event_model_gosub_resume', 'enabled', '-1'
		letdialog 'cg:event_model_gosub_resume', 'visible', '0'
			
		getlng_label [lng_text] = 'cg_event_model_gosub_resume'
		letdialog 'cg:event_model_gosub_resume', 'caption', [lng_text]
	
		letdialog 'cg:event_model_gosub_resume', 'checked', '-1'
		
		rem --- creating code for widget event_model_procedures
		newdialog 'cg:event_model_procedures', 'RADIO', '36|280|300|21'
		letdialog 'cg:event_model_procedures', 'enabled', '-1'
		letdialog 'cg:event_model_procedures', 'visible', '0'
	
		getlng_label [lng_text] = 'cg_event_model_procedures'
		letdialog 'cg:event_model_procedures', 'caption', [lng_text]
	
		letdialog 'cg:event_model_procedures', 'checked', '-1'
		
		rem --- creating code for widget generate
		newdialog 'cg:generate', 'BUTTON', '116|320|140|35'
		letdialog 'cg:generate', 'enabled', '0'
		letdialog 'cg:generate', 'visible', '0'
		letdialog 'cg:generate', 'font', 'Arial|12|2'
	
		getlng_label [lng_text] = 'cg_generate'
		letdialog 'cg:generate', 'caption', [lng_text]
	endif
	
	
	if [current_file] = [null]
		letdialog 'cg:method_rbd_file', 'checked', '-1'
		letdialog 'cg:method_rbd_file', 'enabled', '-1'
		letdialog 'cg:method_core', 'checked', '0'
	else
		letdialog 'cg:method_rbd_file', 'enabled', '0'
	endif
	
	getregval [rb_path] = 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', 'rb_home'
	if [rb_path] = [null]
		letdialog 'cg:output_file_rbb', 'checked', '-1'
		letdialog 'cg:output_file_rbb', 'enabled', '-1'
	endif
	
	rem --- make the dialog visible
	letdialog 'cg', 'visible', '0'
	
	repeat
		rundialog [event] = '0'
	
		rem --- event handling for click on output_clipboard
		if [event] = 'click_cg:output_clipboard'
			letdialog 'cg:l_output_file_name', 'enabled', '-1'
			letdialog 'cg:output_file_name', 'enabled', '-1'
			letdialog 'cg:output_file_select', 'enabled', '-1'
			letdialog 'cg:output_file_rbb', 'enabled', '-1'
		rem --- event handling for click on output_file
		elseif [event] = 'click_cg:output_file'
			letdialog 'cg:l_output_file_name', 'enabled', '0'
			letdialog 'cg:output_file_name', 'enabled', '0'
			letdialog 'cg:output_file_select', 'enabled', '0'
			
			if [rb_path] = [null]
				letdialog 'cg:output_file_rbb', 'checked', '-1'
				letdialog 'cg:output_file_rbb', 'enabled', '-1'
			else
				letdialog 'cg:output_file_rbb', 'enabled', '0'
			endif			
		rem --- event handling for click on output_file_select
		elseif [event] = 'click_cg:output_file_select'
			[SaveBox_DefFileExt] = 'rb'
			getlng_label [lng_text] = 'cg_script_filename'
			savebox [filename] = [lng_text], 'RapidBATCH Scripts|*.rb'
			if [filename] ! ''
				letdialog 'cg:output_file_name', 'text', [filename]
			endif
			
		rem --- event handling for click on genopt_event_handler
		elseif [event] = 'click_cg:genopt_event_handler'
			getdialog [checked] = 'cg:genopt_event_handler', 'checked'
			if [checked] = [true]
				letdialog 'cg:event_model', 'enabled', [true]
				letdialog 'cg:event_model_ifelseif', 'enabled', [true]
				letdialog 'cg:event_model_gosub_resume', 'enabled', [true]
				letdialog 'cg:event_model_procedures', 'enabled', [true]
			else
				letdialog 'cg:event_model', 'enabled', [false]
				letdialog 'cg:event_model_ifelseif', 'enabled', [false]
				letdialog 'cg:event_model_gosub_resume', 'enabled', [false]
				letdialog 'cg:event_model_procedures', 'enabled', [false]
			endif
		rem --- event handling for click on generate
		elseif [event] = 'click_cg:generate'
			
			getdialog [checked] = 'cg:method_rbd_file', 'checked'
			
			if [checked] = [true]
				[method] = 'rbd_file'
			else
				[method] = 'core'
			endif
			
			getdialog [checked] = 'cg:genopt_event_handler', 'checked'
			if [checked] = [true]
				getdialog [checked] = 'cg:event_model_ifelseif', 'checked'
				if [checked] = [true] [model] = 'if_elseif'
				getdialog [checked] = 'cg:event_model_gosub_resume', 'checked'
				if [checked] = [true] [model] = 'gosub_resume'
				getdialog [checked] = 'cg:event_model_procedures', 'checked'
				if [checked] = [true] [model] = 'procedures'				
			endif
			
			getdialog [withatt] = 'cg:genopt_with_attributes', 'checked'
			
			gencode [gc] = [method], [withatt], [model]
			
			getdialog [checked] = 'cg:output_clipboard', 'checked'
			if [checked] = [true]
				[clipboard] = [gc]
				
				rem display info message :)
				getlng_label [lng_text] = 'cg_msg_clipboard_succeeded'

				echo [lng_text]
			else
				getdialog [filename] = 'cg:output_file_name', 'text'
				if [filename] ! ''
					fileexists [checked] = [filename]
					if [checked] = [true]
						getlng_label [lng_text] = 'cg_msg_file_exists'
						confirm [checked] = [lng_text]
						
						if [checked] = [false] break
						
						delfile [filename]
					endif
					
					replacevar [gc] = [gc], [crlf], [new_line]
					writefile [filename], [gc]
					
					getdialog [checked] = 'cg:output_file_rbb', 'checked'
					if [checked] = [true]						
						shell '"' # [rb_path] # '\rbb4.exe" "' # [filename] # '"', 'show'
					else
						fileexists [checked] = [filename]
						
						if [checked] = [true]
							getlng_label [lng_text] = 'cg_msg_file_written'
							echo [lng_text]
						endif
					endif
				endif
			endif
			
			break
			
		rem --- event handling for click on method_core
		elseif [event] = 'click_cg:method_core'
			letdialog 'cg:genopt_with_attributes', 'enabled', '0'
		rem --- event handling for click on method_rbd_file
		elseif [event] = 'click_cg:method_rbd_file'
			letdialog 'cg:genopt_with_attributes', 'enabled', '-1'
		endif
	until [event] = 'close_cg'
	
	rem deldialog 'cg'
	letdialog 'cg', 'visible', '-1'
	
	enableIDE [true]
endproc


rem set_raster
rem ==========
rem Shows the raster settings dialog
proc set_raster
	dec [lng_text], [tmpval]
	
	enableIDE [false]
	
	rem --- creating code for widget "rs"
	newdialog 'rs', 'DIALOG', '492|271|187|112'
	if [errorcode] = [true]

		getlng_label [lng_text] = 'rs_title'
		letdialog 'rs', 'caption', [lng_text]
		letdialog 'rs', 'style', 'DIALOG'
		letdialog 'rs', 'event_ok', [true]
		letdialog 'rs', 'event_cancel', [true]		
	
		rem --- creating code for widget "raster_x"
		newdialog 'rs:raster_x', 'INPUT', '80|0|50|25'
		letdialog 'rs:raster_x', 'enabled', '0'
		letdialog 'rs:raster_x', 'readonly', '-1'
		letdialog 'rs:raster_x', 'event_change', [true]
	
		rem --- creating code for widget "l_raster_x"
		newdialog 'rs:l_raster_x', 'LABEL', '1|5|80|16'
		letdialog 'rs:l_raster_x', 'enabled', '0'
		getlng_label [lng_text] = 'rs_raster_x'
		letdialog 'rs:l_raster_x', 'caption', [lng_text]
	
		rem --- creating code for widget "raster_y"
		newdialog 'rs:raster_y', 'INPUT', '80|25|50|25'
		letdialog 'rs:raster_y', 'enabled', '0'
		letdialog 'rs:raster_y', 'readonly', '-1'
	
		rem --- creating code for widget "l_raster_y"
		newdialog 'rs:l_raster_y', 'LABEL', '1|29|80|16'
		letdialog 'rs:l_raster_y', 'enabled', '0'
		getlng_label [lng_text] = 'rs_raster_y'
		letdialog 'rs:l_raster_y', 'caption', [lng_text]
	
		rem --- creating code for widget "l_pixel_x"
		newdialog 'rs:l_pixel_x', 'LABEL', '130|5|50|16'
		letdialog 'rs:l_pixel_x', 'enabled', '0'
		getlng_label [lng_text] = 'rs_pixel'
		letdialog 'rs:l_pixel_x', 'caption', [lng_text]
	
		rem --- creating code for widget "l_pixel_y"
		newdialog 'rs:l_pixel_y', 'LABEL', '130|29|50|16'
		letdialog 'rs:l_pixel_y', 'enabled', '0'
		letdialog 'rs:l_pixel_y', 'caption', [lng_text]
	
		rem --- creating code for widget "ok"
		newdialog 'rs:ok', 'BUTTON', '1|60|90|25'
		letdialog 'rs:ok', 'enabled', '0'
	
		getlng_label [lng_text] = 'ok'
		letdialog 'rs:ok', 'caption', [lng_text]
		letdialog 'rs:ok', 'caption', [lng_text]
	
		rem --- creating code for widget "cancel"
		newdialog 'rs:cancel', 'BUTTON', '90|60|90|25'
		letdialog 'rs:cancel', 'enabled', '0'

		getlng_label [lng_text] = 'cancel'
		letdialog 'rs:cancel', 'caption', [lng_text]
	endif
	
	letdialog 'rs:raster_x', 'text', [raster_x]
	letdialog 'rs:raster_y', 'text', [raster_y]	
	
	centerdialog 'rs'

	rem --- make the dialog window visible
	letdialog 'rs', 'visible', [true]
	
	letdialog 'rs:raster_x', 'selpos', '1|-1'
	letdialog 'rs:raster_y', 'selpos', '1|-1'	
	letdialog 'rs:raster_x', 'focus', [true]
	
	repeat
		rundialog [event] = '0'
	
		rem --- event handling for click on "ok"
		if [event] = 'click_rs:ok' | [event] = 'ok_rs'
			getdialog [raster_x] = 'rs:raster_x', 'text'
			getdialog [raster_y] = 'rs:raster_y', 'text'			

			break
		rem --- event handling for change on "cancel"
		elseif [event] = 'change_rs:raster_x'
			getdialog [tmpval] = 'rs:raster_x', 'text'
			letdialog 'rs:raster_y', 'text', [tmpval]

		rem --- event handling for click on "cancel"
		elseif [event] = 'click_rs:cancel'
			break
			
		endif	
	until [event] = 'close_rs' | [event] = 'cancel_rs'
	
	letdialog 'rs', 'visible', [false]
	enableIDE [true]	
endproc

rem dd_settitle
rem ===========
rem Set the Dialog Designer's title bar caption
proc dd_settitle
	dec [cf]
	
	if [current_file] = [null]
		[cf] = 'noname'
	else
		[cf] = [current_file]
	endif
	
	labeldialog 'dd', [dd_name] # ' v' # [dd_version] # [dd_affix] # ' - ' # [cf]
endproc


rem save_file
rem =========
rem Saves the current dialog definition into a RBD-file
proc save_file: [*filename], [saveas]
	dec [i], [j], [att], [attval], [line], [lng_text], [estate]
	if [filename] = '' | [saveas] = [true]
		[SaveBox_DefFileExt] = 'rbd'
		getlng_label [lng_text] = 'msg_save_rbd'
		savebox [filename] = [lng_text], 'Dialog Designer Files|*.rbd'
		if [filename] = '' ret
	endif
	
	delfile [filename]
	
	writefile [filename], 'RBVDD:' # [dd_version]
	
	[i] = '0'
	repeat
		[i] + '1'
		
		[line] = ''
		
		[line] # [widgets:[i],[w_type]]
		[line] # [file_sep]
		[line] # [widgets:[i],[w_name]]
		[line] # [file_sep]
		[line] # [widgets:[i],[w_xname]]
		[line] # [file_sep]
		[line] # [widgets:[i],[w_x]]
		[line] # [file_sep]
		[line] # [widgets:[i],[w_y]]
		[line] # [file_sep]
		[line] # [widgets:[i],[w_width]]
		[line] # [file_sep]
		[line] # [widgets:[i],[w_height]]
		[line] # [file_sep]
		
		[j] = '0'
		repeat
			[j] + '1'
			gettok [att] = [widgets:[i],[w_attrib]], '|', [j]
			if [att] ! ''
				getdialog [attval] = [widgets:[i],[w_name]], [att]
				replacevar [attval] = [attval], '|', [pipe_replace]
				if [j] > '1' [line] # '|'
				[line] # [attval]
			endif
		until [att] = ''
		
		[line] # [file_sep]
		[j] = '0'
		repeat
			[j] + '1'
			gettok [estate] = [widgets:[i],[w_event_states]], '|', [j]
			if [estate] ! ''
				if [j] > '1' [line] # '|'
				[line] # [estate]
			endif
		until [estate] = ''
		
		writefile [filename], [line]
	until [i] = [widgets:'0']

	getlng_label [lng_text] = 'msg_save_succeeded'
	echo [lng_text]
	
	[file_changed] = [false]
endproc

rem open_file
rem =========
rem Loads the current dialog definition from a RBD-file into the
rem dialog designer
proc open_file: [*filename], [open_this_file]
	dec [i], [j], [att], [attval], [line], [info], [savenow], [lng_text]

	if [file_changed] = [true]
		getlng_label [lng_text] = 'msg_saveanychanges'
		msgbox [savenow] = [dd_name], [lng_text], '3' + '32'
		if [savenow] = '6'
			save_file [filename], [false]
		elseif [savenow] = '2'
			ret
		endif
	endif
	
	if [open_this_file] ! [true]
		getlng_label [lng_text] = 'msg_open_rbd'
	
		[OpenBox_DefFileExt] = 'rbd'
		openbox [filename] = [lng_text], 'Dialog Designer Files|*.rbd'
		if [filename] = '' ret
	endif
	
	if [designwin] ! [null]
		deldialog [designwin]
		[designwin] = [null]
		reset [widgets]
	endif

	[i] = '1'
	repeat
		[i] + '1'
		
		readfile [line] = [filename], [i]
		if [errorcode] = [false] break

		[j] = '0'
		repeat
			[j] + '1'
			gettok [info:[j]] = [line], [file_sep], [j]
		until [j] = '7'
		
		if [info:'1'] = 'DIALOG' & [XPStyleActive] = [true] [info:'7'] - '10'
		
		createNewWidget [info:'1'], [info:'4'], [info:'5'], [info:'6'], [info:'7'], [info:'3'], [info:'2'], [false], [true]
		
		gettok [info:'8'] = [line], [file_sep], '8'

		[j] = '0'
		repeat
			[j] + '1'
			gettok [att] = [widgets:[i]-'1',[w_attrib]], '|', [j]
			gettok [attval] = [info:'8'], '|', [j]
			replacevar [attval] = [attval], [pipe_replace], '|'

			if [att] ! '' & [attval] ! '' letdialog [widgets:[i]-'1',[w_name]], [att], [attval]
		until [att] = ''
		
		gettok [widgets:[i]-'1',[w_event_states]] = [line], [file_sep], '9'				
	until [true] = [false]

	[current_widget] = '1'
	updateWidgetList
	[mode] = 'none'
	setCurrentWidget

	letdialog [designwin], 'visible', [true]
	[file_changed] = [false]
endproc

rem copyWidget
rem ==========
rem Dublicates a widget
proc copyWidget: [*info], [newname]
	dec [i], [att], [attval]
	createNewWidget [info:[w_type]], [info:[w_x]], [info:[w_y]], [info:[w_width]], [info:[w_height]], [newname], [designwin] # ':' # [newname], [false], [false]

	[i] = '0'
	repeat
		[i] + '1'
		gettok [att] = [info:[w_attrib]], '|', [i]
		getdialog [attval] = [info:[w_name]], [att]
		if [att] ! '' & [attval] ! '' letdialog [widgets:[widgets:'0'],[w_name]], [att], [attval]
	until [att] = ''
	
	[widgets:[widgets:'0'], [w_events]] = [info:[w_events]]
	[widgets:[widgets:'0'], [w_event_states]] = [info:[w_event_states]]
endproc

rem new_file
rem ========
rem Creates a new, empty dialog
proc new_file: [*filename]
	dec [win_name], [lng_text], [lng_title]
	
	if [file_changed] = [true]
		getlng_label [lng_text] = 'msg_saveanychanges'
		msgbox [savenow] = [dd_name], [lng_text], '3' + '32'

		if [savenow] = '6'
			save_file [filename], [false]
		elseif [savenow] = '2'
			ret
		endif
	endif
	
	getlng_label [lng_title] = 'msg_new_widget_title'
	getlng_label [lng_text] = 'msg_new_dialog_name'	
	inputbox [win_name] = [lng_title], [lng_text], 'myDialog'
	
	if [win_name] = [null] ret
	
	if [designwin] ! [null]
		deldialog [designwin]

		[designwin] = [null]
		reset [widgets]
	endif

	createNewWidget 'dialog', '220', '100', [s_width] - '220' - '50', [s_height] - '100' - '200', [win_name], [prefix] # [win_name], [true], [false]
	
	letdialog [designwin], 'visible', [true]
	[file_changed] = [false]

	rem start-up mode: none
	[mode] = 'none'
	updateModeState
	
	[current_file] = [null]
	[file_changed] = [false]
endproc

rem createToolbarButton
rem ===================
rem Help procedure, adding a button to the toolbar
proc createToolbarButton: [name], [img]
	dec [text]
	newdialog 'dd:' # [name], 'imagebutton', [tb_pos] # '|2|30|30'
	letdialog 'dd:' # [name], 'image_transparent', 'img\' # [img] # '.bmp'
	
	getlng_label [text] = 'tb_' # [name]
	letdialog 'dd:' # [name], 'tooltip', [text]
	
	[tb_pos] + '30'
endproc

rem getMenuItemCaptions
rem ===================
rem Replaces the language text definitions in a sub menu trough texts from the 
rem language catalog
proc getMenuItemCaptions: [menu]
	dec [i], [item], [lng_text]

	[i] = '0'
	repeat
		[i] + '1'
		gettok [item] = [menu], [std_sep], [i]
		if [item] ! '' & [item] ! '-'
			getlng_label [lng_text] = [item]
			letdialog 'dd:Menu_' # [item], 'caption', [lng_text]
		endif
	until [item] = ''
endproc

rem selectWidget
rem ============
rem Select a widget from the widget list
func selectWidget
	dec [i], [list], [lng_text]
	
	[i] = '0'
	enableIDE [false]
	
	if [widgets:'0'] < '2'
		getlng_label [lng_text] = 'msg_no_child_widgets'
		msgbox 'Error', [lng_text], '16'
	else
		[i] = '1'
		repeat
			[i] + '1'
			if [list] ! '' [list] # [std_sep]
			[list] # [widgets:[i],[w_xname]]
		until [i] = [widgets:'0']
		
		[ListBox_Width] = '300'
		getlng_label [lng_text] = 'msg_select_widget'
		listbox [item] = [lng_text], [list]


		if [item] ! ''
			[i] = '1'
			repeat
				[i] + '1'
				if [widgets:[i],[w_xname]] = [item] break
			until [i] = [widgets:'0']
		else
			[i] = '0'
		endif
	endif
	
	enableIDE [true]
	
	ret [i]
endfunc

proc aboutbox: [timeout]
	enableIDE [false]
	
	rem --- creating code for widget about
	newdialog 'about', 'DIALOG', '432|177|310|390'
	if [errorcode] = [true]
		letdialog 'about', 'style', 'DIALOG'
		letdialog 'about', 'caption', [dd_name]
		letdialog 'about', 'event_click', [true]		
	
		rem --- creating code for widget logo
		newdialog 'about:logo', 'IMAGE', '1|1|306|266'
		letdialog 'about:logo', 'enabled', '0'
		letdialog 'about:logo', 'image_transparent', 'img\dd_logo.bmp'
	
		rem --- creating code for widget copyright_group
		newdialog 'about:copyright_group', 'GROUP', '1|260|303|103'
		letdialog 'about:copyright_group', 'enabled', '0'
	
		rem --- creating code for widget label_progname
		newdialog 'about:label_progname', 'LABEL', '5|270|290|16'
		letdialog 'about:label_progname', 'enabled', '0'
		letdialog 'about:label_progname', 'font', 'Sans Serif|8|2'
		letdialog 'about:label_progname', 'caption', [dd_name] # ', Version ' # [dd_version] # [dd_affix]

		rem --- creating code for widget label_copyright
		newdialog 'about:label_copyright', 'LABEL', '4|290|290|16'
		letdialog 'about:label_copyright', 'enabled', '0'
		letdialog 'about:label_copyright', 'font', 'Sans Serif|8|0'
		letdialog 'about:label_copyright', 'caption', 'Copyright 2005, 2006 by J.M.K S.F. Software Technologies'
	
		rem --- creating code for widget label_website
		newdialog 'about:label_website', 'LABEL', '5|306|290|16'
		letdialog 'about:label_website', 'enabled', '0'
		letdialog 'about:label_website', 'font', 'Sans Serif|8|0'
		letdialog 'about:label_website', 'caption', 'http://www.jmksf.de/    mail@jmksf.com'
	
		rem --- creating code for widget label_writteninrb
		newdialog 'about:label_writteninrb', 'LABEL', '5|330|290|30'
		letdialog 'about:label_writteninrb', 'enabled', '0'
		letdialog 'about:label_writteninrb', 'font', 'Sans Serif|7|0'
		letdialog 'about:label_writteninrb', 'caption', 'This powerful piece of software was 100% implemented in core RapidBATCH 5 source code.'
	endif
	
	rem --- center the dialog
	centerdialog 'about'
	
	rem --- make the dialog window visible
	letdialog 'about', 'visible', [true]

	rundialog [null] = [timeout]
	
	letdialog 'about', 'visible', [false]
	enableIDE [true]
endproc

rem **************************
rem ** MAIN SCRIPT ROUTINES **
rem **************************

rem first of all, change to the script home directory
chscriptdir
[dd_dir] = [current]

rem show splash screen for 3 seconds
aboutbox '2000'

rem reading the language catalog
readlng_info 'dd.lng'

rem set some build-in dialog captions
getlng_label [InputBox_Ok] = 'ok'
getlng_label [ListBox_Ok] = 'ok'
getlng_label [InputBox_Cancel] = 'cancel'

rem Dialog Designer Main Window -----------------------------------------------
newdialog 'dd', 'dialog', '1|1|' # [s_width] - '5' # '|75'
letdialog 'dd', 'style', 'single'
letdialog 'dd', 'event_size', [true]

dd_settitle

[submenu_file] = 'menu_file_new|menu_file_open|menu_file_save|menu_file_save_as|-|menu_file_quit'
[submenu_edit] = 'menu_edit_remove_widget|menu_edit_rename_widget|menu_edit_duplicate_widget|-|menu_edit_events|menu_edit_widgetorder|-|menu_edit_raster_toggle|menu_edit_raster_settings'
[submenu_widget] = 'menu_widget_button|menu_widget_input|menu_widget_pwd|menu_widget_edit|menu_widget_edit_linewrap|menu_widget_label|menu_widget_radio|menu_widget_option|menu_widget_image|menu_widget_imagebutton|menu_widget_list|menu_widget_list_multi|menu_widget_list_sorted|menu_widget_list_multisorted|menu_widget_combo|menu_widget_static_combo|menu_widget_progress|menu_widget_group'
[submenu_help] = 'menu_help_manual|-|menu_help_license|menu_help_www|-|menu_help_about'

rem create File-menu
getlng_label [mitem] = 'menu_file'
[menu] = [mitem] # ':' # [submenu_file] # ';'

rem create Edit-menu
getlng_label [mitem] = 'menu_edit'
[menu] # [mitem] # ':' # [submenu_edit] # ';'

rem create Create-menu
getlng_label [mitem] = 'menu_widget'
[menu] # [mitem] # ':' # [submenu_widget] # ';'

rem create Code Generator-menu
[menu] # 'menu_gen' # [std_sep]

rem create Help-menu
getlng_label [mitem] = 'menu_help'
[menu] # [mitem] # ':' # [submenu_help] # ';'

rem set the menu to the main window
letdialog 'dd', 'menu', [menu] # ';'

rem rename captions of the menu entries
getMenuItemCaptions [submenu_file]
getMenuItemCaptions [submenu_edit]
getMenuItemCaptions [submenu_widget]
getMenuItemCaptions 'menu_gen'
getMenuItemCaptions [submenu_help]


createToolbarButton 'file_new', 'fnew'
createToolbarButton 'file_open', 'fopen'
createToolbarButton 'file_save', 'fsave'

[tb_pos] + '5'

createToolbarButton 'button_new', 'wbutton'
createToolbarButton 'input_new', 'winput'
createToolbarButton 'pwd_new', 'wpwd'
createToolbarButton 'edit_new', 'wedit'
createToolbarButton 'edit_linewrap_new', 'wedit_linewrap'
createToolbarButton 'label_new', 'wlabel'
createToolbarButton 'radio_new', 'wradio'
createToolbarButton 'option_new', 'woption'
createToolbarButton 'image_new', 'wimage'
createToolbarButton 'imagebutton_new', 'wimagebutton'
createToolbarButton 'list_new', 'wlist'
createToolbarButton 'list_multi_new', 'wlist_multi'
createToolbarButton 'list_sorted_new', 'wlist_sorted'
createToolbarButton 'list_multisorted_new', 'wlist_multisorted'
createToolbarButton 'combo_new', 'wcombo'
createToolbarButton 'static_combo_new', 'wstatic_combo'
createToolbarButton 'progress_new', 'wprogress'
createToolbarButton 'group_new', 'wgroup'
[tb_pos] + '5'
createToolbarButton 'ggen', 'ggen'
[tb_pos] + '5'
createToolbarButton 'hman', 'hman'

rem TESTING ONLY!
rem letdialog 'dd', 'visible', '0'
rem rundialog [null] = '0'
rem halt

rem turn raster on
letdialog 'dd:Menu_menu_edit_raster_toggle', 'checked', [raster_toggle]

rem Dialog Designer Object Window ---------------------------------------------
newdialog 'dw', 'dialog', '1|100|200|510'
letdialog 'dw', 'style', 'dialog'

getlng_label [lngtext] = 'dw_title'
letdialog 'dw', 'caption', [lngtext]

rem Group: Mode
newdialog 'dw:mode_group', 'group', '2|2|190|80'
getlng_label [lngtext] = 'dw_mode_title'
letdialog 'dw:mode_group', 'caption', [lngtext]

newdialog 'dw:m_none', 'radio', '8|18|180|21'
getlng_label [lngtext] = 'dw_mode_none'
letdialog 'dw:m_none', 'caption', [lngtext]

newdialog 'dw:m_move', 'radio', '8|36|180|21'
getlng_label [lngtext] = 'dw_mode_move'
letdialog 'dw:m_move', 'caption', [lngtext]

newdialog 'dw:m_scale', 'radio', '8|54|180|21'
getlng_label [lngtext] = 'dw_mode_scale'
letdialog 'dw:m_scale', 'caption', [lngtext]

rem Group: Widget-List
newdialog 'dw:wlist_group', 'group', '2|82|190|200'
getlng_label [lngtext] = 'dw_widgets_title'
letdialog 'dw:wlist_group', 'caption', [lngtext]

newdialog 'dw:wlist', 'list', '8|98|180|180'
letdialog 'dw:wlist', 'event_change', [true]

rem Group: Positions
newdialog 'dw:p_group', 'group', '2|285|190|70'
getlng_label [lngtext] = 'dw_positions_title'
letdialog 'dw:p_group', 'caption', [lngtext]

rem X-Position
newdialog 'dw:p_l_x', 'label', '10|304|35|25'
getlng_label [lngtext] = 'dw_positions_x'
letdialog 'dw:p_l_x', 'caption', [lngtext]

newdialog 'dw:p_x', 'input', '50|298|40|25'
letdialog 'dw:p_x', 'event_change', [true]

rem Y-Position
newdialog 'dw:p_l_y', 'label', '92|304|35|25'
getlng_label [lngtext] = 'dw_positions_y'
letdialog 'dw:p_l_y', 'caption', [lngtext]

newdialog 'dw:p_y', 'input', '130|298|40|25'
letdialog 'dw:p_y', 'event_change', [true]

rem Width
newdialog 'dw:p_l_width', 'label', '10|326|35|25'
getlng_label [lngtext] = 'dw_positions_width'
letdialog 'dw:p_l_width', 'caption', [lngtext]

newdialog 'dw:p_width', 'input', '50|322|40|25'
letdialog 'dw:p_width', 'event_change', [true]

rem Height
newdialog 'dw:p_l_height', 'label', '92|326|35|25'
getlng_label [lngtext] = 'dw_positions_height'
letdialog 'dw:p_l_height', 'caption', [lngtext]

newdialog 'dw:p_height', 'input', '130|322|40|25'
letdialog 'dw:p_height', 'event_change', [true]

rem Group: Attributes
newdialog 'dw:a_group', 'group', '2|355|190|130'
getlng_label [lngtext] = 'dw_attributes_title'
letdialog 'dw:a_group', 'caption', [lngtext]

newdialog 'dw:a_list', 'list_sorted', '8|370|180|85'
letdialog 'dw:a_list', 'event_change', [true]

rem Attribute string field
newdialog 'dw:a_string', 'input', '8|455|180|25'
letdialog 'dw:a_string', 'event_change', [true]
letdialog 'dw:a_string', 'visible', [false]

rem Attribute bool field
newdialog 'dw:a_bool_true', 'radio', '8|455|90|25'
getlng_label [lngtext] = 'dw_attributes_true'
letdialog 'dw:a_bool_true', 'caption', [lngtext]
letdialog 'dw:a_bool_true', 'visible', [false]

newdialog 'dw:a_bool_false', 'radio', '100|455|90|25'
getlng_label [lngtext] = 'dw_attributes_false'
letdialog 'dw:a_bool_false', 'caption', [lngtext]
letdialog 'dw:a_bool_false', 'visible', [false]

rem Attribute file field
newdialog 'dw:a_file_name', 'input', '8|455|120|25'
letdialog 'dw:a_file_name', 'event_change', [true]
letdialog 'dw:a_file_name', 'visible', [false]

newdialog 'dw:a_file_select', 'button', '130|455|60|25'
getlng_label [lngtext] = 'dw_attributes_file_select'
letdialog 'dw:a_file_select', 'caption', [lngtext]
letdialog 'dw:a_file_select', 'visible', [false]

rem Attribute list editor field
newdialog 'dw:a_itemstring', 'input', '8|455|120|25'
letdialog 'dw:a_itemstring', 'event_change', [true]
letdialog 'dw:a_itemstring', 'visible', [false]

newdialog 'dw:a_items_edit', 'button', '130|455|60|25'
getlng_label [lngtext] = 'dw_attributes_items_edit'
letdialog 'dw:a_items_edit', 'caption', [lngtext]
letdialog 'dw:a_items_edit', 'visible', [false]

rem Attribute menu editor field
newdialog 'dw:a_menustring', 'input', '8|455|120|25'
letdialog 'dw:a_menustring', 'event_change', [true]
letdialog 'dw:a_menustring', 'visible', [false]

newdialog 'dw:a_menu_edit', 'button', '130|455|60|25'
getlng_label [lngtext] = 'dw_attributes_menu_edit'
letdialog 'dw:a_menu_edit', 'caption', [lngtext]
letdialog 'dw:a_menu_edit', 'visible', [false]

rem Attribute style field
newdialog 'dw:a_style', 'combo', '8|455|180|200'
letdialog 'dw:a_style', 'event_change', [true]
letdialog 'dw:a_style', 'visible', [false]
letdialog 'dw:a_style', 'items', '|DIALOG|SINGLE|TOOL|STATIC_TOOL'

rem Attribute font field
rem newdialog 'dw:a_font_name', 'combo', '8|455|90|200'
rem findfiles [fonts] = 'C:\windows\fonts\*', '1'
rem letdialog 'dw:a_font_name', 'items', [fonts]
rem letdialog 'dw:a_font_name', 'visible', [false]

rem newdialog 'dw:a_font_size', 'combo', '96|455|45|200'


rem letdialog 'dw:a_font_size', 'visible', [false]
rem newdialog 'dw:a_font_style', 'combo', '140|455|48|200'
rem letdialog 'dw:a_font_style', 'items', 'none|bold|italic|underline|b+i|b+u|b+i+u|i+u|b+u'

rem Attribute font field

rem show the dialogs
letdialog 'dd', 'visible', [true]
letdialog 'dw', 'visible', [true]

rem create the designer dialog, activate some events
[file_changed] = [false]

if [command] = ''
	dec [op]
	
	rem show simple menu here!
	getlng_label [lng_text] = 'msg_whattodo'
	getlng_label [lng_text:'1'] = 'msg_createnew'
	getlng_label [lng_text:'2'] = 'msg_openexisting'

	enableIDE [false]
	btnmenu [op] = [lng_text], [lng_text:'1'] # [std_sep] # [lng_text:'2']
	enableIDE [true]

	if [op] = [lng_text:'1']	
		new_file [null]
	elseif [op] = [lng_text:'2']
		open_file [current_file], [false]
		dd_settitle
	endif
else
	[current_file] = [command]
	replacevar [current_file] = [current_file], '"', ''
	open_file [current_file], [true]
	dd_settitle
endif

if [designwin] = [null] halt

rem go back to the starting directory
chdir [start_dir]

%dd_event_loop
repeat
	rundialog [dd_event] = '0'
	
	if [dd_event] = 'change_dw:wlist'
		getdialog [current_widget] = 'dw:wlist', 'selindex'
		setCurrentWidget
	endif

	rem toolbar-buttons for file-operations
	if [dd_event] = 'click_dd:file_new' | [dd_event] = 'click_dd:Menu_menu_file_new'
		new_file [current_file]
		dd_settitle
	elseif [dd_event] = 'click_dd:file_open' | [dd_event] = 'click_dd:Menu_menu_file_open'
		open_file [current_file], [false]
		dd_settitle
	elseif [dd_event] = 'click_dd:file_save' | [dd_event] = 'click_dd:Menu_menu_file_save'
		save_file [current_file], [false]
		dd_settitle
	elseif [dd_event] = 'click_dd:Menu_menu_file_save_as'
		save_file [current_file], [true]
		dd_settitle
	endif
	
	rem toolbar-buttons for edit-operations
	if [dd_event] = 'click_dd:Menu_menu_edit_remove_widget'
		rem selectWidget [pos]
		if [current_widget] = '1' cont
		
		getlng_label [lng_text] = 'msg_reallyremovewidget'
		replacevar [lng_text] = [lng_text], '%WIDGET', [widgets:[current_widget],[w_xname]]
		confirm [errorcode] = [lng_text]
		
		if [errorcode] = [true]		
			deldialog [widgets:[current_widget],[w_name]]
			if [errorcode] = [true]
				deletearrayitem [widgets], [current_widget]
				
				[current_widget] = [current_widget] - '1'
				updateWidgetList
				setCurrentWidget
			endif
		endif
	elseif [dd_event] = 'click_dd:Menu_menu_edit_rename_widget'
		rem selectWidget [pos]
		if [current_widget] = '1' cont

		getlng_label [lng_title] = 'msg_renamewidget_title'
		getlng_label [lng_text] = 'msg_renamewidget'
		replacevar [lng_text] = [lng_text], '%WIDGET', [widgets:[current_widget],[w_xname]]

		inputbox [new_name] = [lng_title], [lng_text], [widgets:[current_widget],[w_xname]]
		if [new_name] = [widgets:[current_widget],[w_xname]] | [new_name] = '' cont
		
		copyWidget [widgets:[current_widget]], [new_name]
		
		deldialog [widgets:[current_widget], [w_name]]
		copyarray [widgets:[widgets:'0']], [widgets:[current_widget]]
		deletearrayitem [widgets], [widgets:'0']

		updateWidgetList
		setCurrentWidget
		
		[file_changed] = [true]
	elseif [dd_event] = 'click_dd:Menu_menu_edit_duplicate_widget'
		rem selectWidget [pos]
		rem if [pos] = '0' cont
		if [current_widget] = '1' cont

		getlng_label [lng_title] = 'msg_new_widget_title'
		getlng_label [lng_text] = 'msg_dupwidgetname'
		replacevar [lng_text] = [lng_text], '%WIDGET', [widgets:[current_widget],[w_xname]]

		inputbox [new_name] = [lng_title], [lng_text], [widgets:[current_widget],[w_xname]]
		if [new_name] = [widgets:[current_widget],[w_xname]] | [new_name] = '' cont
		
		copyWidget [widgets:[current_widget]], [new_name]

		letdialog [widgets:[widgets:'0'],[w_name]], 'position', [widgets:[widgets:'0'],[w_x]] # '|' # [widgets:[widgets:'0'],[w_y]]+[widgets:[current_widget],[w_height]] # '|' # [widgets:[widgets:'0'],[w_width]] # '|' # [widgets:[widgets:'0'],[w_height]]
		
		[current_widget] = [widgets:'0']
		updateWidgetList
		setCurrentWidget

		[file_changed] = [true]
	elseif [dd_event] = 'click_dd:Menu_menu_edit_events'
		event_editor [current_widget]
	elseif [dd_event] = 'click_dd:Menu_menu_edit_widgetorder'
		getdialog [current_widget] = 'dw:wlist', 'selection'
		changeWidgetOrder

		updateWidgetList
		letdialog 'dw:wlist', 'selection', [current_widget]

		getdialog [current_widget] = 'dw:wlist', 'selindex'
		setCurrentWidget
	elseif [dd_event] = 'click_dd:Menu_menu_edit_raster_toggle'
		getdialog [raster_toggle] = 'dd:Menu_menu_edit_raster_toggle', 'checked'

		if [raster_toggle] = [true]
			[raster_toggle] = [false]
		else
			[raster_toggle] = [true]
		endif
		letdialog 'dd:Menu_menu_edit_raster_toggle', 'checked', [raster_toggle]
	elseif [dd_event] = 'click_dd:Menu_menu_edit_raster_settings'
		set_raster
	endif

	rem toolbar-buttons for dialog widget creation 
	if [dd_event] = 'click_dd:button_new' | [dd_event] = 'click_dd:Menu_menu_widget_button'
		createNewWidget 'button', '1', '1', '100', '25', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:input_new' | [dd_event] = 'click_dd:Menu_menu_widget_input'
		createNewWidget 'input', '1', '1', '100', '25', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:pwd_new' | [dd_event] = 'click_dd:Menu_menu_widget_pwd'
		createNewWidget 'pwd', '1', '1', '100', '25', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:edit_new' | [dd_event] = 'click_dd:Menu_menu_widget_edit'
		createNewWidget 'edit', '1', '1', '200', '150', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:edit_linewrap_new' | [dd_event] = 'click_dd:Menu_menu_widget_edit_linewrap'
		createNewWidget 'edit_linewrap', '1', '1', '200', '150', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:label_new' | [dd_event] = 'click_dd:Menu_menu_widget_label'
		createNewWidget 'label', '1', '1', '100', '16', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:option_new' | [dd_event] = 'click_dd:Menu_menu_widget_option'
		createNewWidget 'option', '1', '1', '100', '20', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:radio_new' | [dd_event] = 'click_dd:Menu_menu_widget_radio'
		createNewWidget 'radio', '1', '1', '100', '20', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:image_new' | [dd_event] = 'click_dd:Menu_menu_widget_image'
		createNewWidget 'image', '1', '1', '100', '100', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:imagebutton_new' | [dd_event] = 'click_dd:Menu_menu_widget_imagebutton'
		createNewWidget 'imagebutton', '1', '1', '50', '50', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:list_new' | [dd_event] = 'click_dd:Menu_menu_widget_list'
		createNewWidget 'list', '1', '1', '200', '150', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:list_sorted_new' | [dd_event] = 'click_dd:Menu_menu_widget_list_sorted'
		createNewWidget 'list_sorted', '1', '1', '200', '150', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:list_multi_new' | [dd_event] = 'click_dd:Menu_menu_widget_list_multi'
		createNewWidget 'list_multi', '1', '1', '200', '150', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:list_multisorted_new' | [dd_event] = 'click_dd:Menu_menu_widget_list_multisorted'
		createNewWidget 'list_multisorted', '1', '1', '200', '150', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:combo_new' | [dd_event] = 'click_dd:Menu_menu_widget_combo'
		createNewWidget 'combo', '1', '1', '100', '200', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:static_combo_new' | [dd_event] = 'click_dd:Menu_menu_widget_static_combo'
		createNewWidget 'static_combo', '1', '1', '100', '200', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:progress_new' | [dd_event] = 'click_dd:Menu_menu_widget_progress'
		createNewWidget 'progress', '1', '1', '100', '25', [null], [null], [true], [false]
	elseif [dd_event] = 'click_dd:group_new' | [dd_event] = 'click_dd:Menu_menu_widget_group'
		createNewWidget 'group', '1', '1', '300', '300', [null], [null], [true], [false]
	endif
	
	rem toolbar-buttons to generate code
	if [dd_event] = 'click_dd:ggen' | [dd_event] = 'click_dd:Menu_menu_gen'
		gencode_options
	endif

	rem help menu and toolbar
	if [dd_event] = 'click_dd:Menu_menu_help_manual' | [dd_event] = 'click_dd:hman'
		open [dd_dir] # '\doc\manual.html'
	elseif [dd_event] = 'click_dd:Menu_menu_help_license'
		open [dd_dir] # '\dd_license.txt'
	elseif [dd_event] = 'click_dd:Menu_menu_help_www'
		open 'http://www.rapidbatch.com/'
	elseif [dd_event] = 'click_dd:Menu_menu_help_about'
		aboutbox '0'
	endif
	
	
	rem events on the widget options dialog ---
	if [dd_event] = 'click_dw:m_none'
		[mode] = 'none'
		letdialog [widgets:[current_widget][w_name]], 'enabled', [current_widget_enabled]
	elseif [dd_event] = 'click_dw:m_move'
		if [mode] ! 'none'
			letdialog [widgets:[current_widget][w_name]], 'enabled', [current_widget_enabled]
		endif
		
		[mode] = 'move'
		if [current_widget] > '1'
			getdialog [current_widget_enabled] = [widgets:[current_widget][w_name]], 'enabled'
			letdialog [widgets:[current_widget][w_name]], 'enabled', [false]
			retrievepositions [designwin], [x], [y], [null], [null]
			
			rem mouseevent 'move', [widgets:[current_widget][w_x]] + [x] - [xp_pix_x] # '|' # [widgets:[current_widget][w_y]] + [y] + [xp_pix_y]
			rem mouseevent 'move', [widgets:[current_widget][w_width]] + [widgets:[current_widget][w_x]] + [x] - [xp_pix_x] # '|' # [widgets:[current_widget][w_height]] + [widgets:[current_widget][w_y]] + [y] + [xp_pix_y] + [menu_height]
			rem [x] = [x] - ( [x] ~ '10' )
			mouseevent 'move', [widgets:[current_widget][w_width]] + [widgets:[current_widget][w_x]] + [x] - [xp_pix_x] # '|' # [widgets:[current_widget][w_height]] + [widgets:[current_widget][w_y]] + [y] + [xp_pix_y] + [menu_height]

		endif
	elseif [dd_event] = 'click_dw:m_scale'
		if [mode] ! 'none'
			letdialog [widgets:[current_widget][w_name]], 'enabled', [current_widget_enabled]
		endif
		
		[mode] = 'scale'
		if [current_widget] > '1'
			getdialog [current_widget_enabled] = [widgets:[current_widget][w_name]], 'enabled'
			letdialog [widgets:[current_widget][w_name]], 'enabled', [false]
			retrievepositions [designwin], [x], [y], [null], [null]

			mouseevent 'move', [widgets:[current_widget][w_width]] + [widgets:[current_widget][w_x]] + [x] - [xp_pix_x] # '|' # [widgets:[current_widget][w_height]] + [widgets:[current_widget][w_y]] + [y] + [xp_pix_y] + [menu_height]
		endif
	endif
	
	if [current_widget] > '0'
		if [dd_event] = 'change_dw:a_list'
			updateAttribInfo
		elseif [dd_event] = 'change_dw:a_string' | [dd_event] = 'change_dw:a_file_name' | [dd_event] = 'change_dw:a_itemstring' | [dd_event] = 'change_dw:a_menustring' | [dd_event] = 'change_dw:a_style'
			if [dd_event] = 'change_dw:a_string'
				getdialog [val] = 'dw:a_string', 'text'
			elseif [dd_event] = 'change_dw:a_file_name'
				getdialog [val] = 'dw:a_file_name', 'text'
			elseif [dd_event] = 'change_dw:a_itemstring'
				getdialog [val] = 'dw:a_itemstring', 'text'
			elseif [dd_event] = 'change_dw:a_menustring'
				getdialog [val] = 'dw:a_menustring', 'text'
			elseif [dd_event] = 'change_dw:a_style'
				getdialog [val] = 'dw:a_style', 'text'
			endif

			getdialog [att] = 'dw:a_list', 'selection'

			letdialog [widgets:[current_widget][w_name]], [att], [val]

			if [dd_event] = 'change_dw:a_style'
				rem getdialog [att] = [widgets:[current_widget][w_name]], 'visible'
				rem echo [att]
				
				rem letdialog [widgets:[current_widget][w_name]], 'visible', [false]
				letdialog [widgets:[current_widget][w_name]], 'visible', [true]
			endif
			
			[file_changed] = [true]
		elseif [dd_event] = 'click_dw:a_bool_true'
			getdialog [att] = 'dw:a_list', 'selection'
			letdialog [widgets:[current_widget][w_name]], [att], [true]

			[file_changed] = [true]
		elseif [dd_event] = 'click_dw:a_bool_false'
			getdialog [att] = 'dw:a_list', 'selection'
			letdialog [widgets:[current_widget][w_name]], [att], [false]

			[file_changed] = [true]
		elseif [dd_event] = 'click_dw:a_file_select'
			getdialog [att] = 'dw:a_list', 'selection'
					
			openbox [val] = 'Select a file:', 'Image-Files|*.ico;*.bmp|All files|*.*'

			if [val] ! ''
				letdialog [widgets:[current_widget][w_name]], [att], [val]
				updateAttribInfo
			endif
		elseif [dd_event] = 'click_dw:a_items_edit'
			getdialog [att] = 'dw:a_list', 'selection'
			getdialog [val] = [widgets:[current_widget][w_name]], [att]
			
			item_editor [val] = [val]
			
			if [val] ! ''
				letdialog [widgets:[current_widget][w_name]], [att], [val]
				updateAttribInfo
			endif
		elseif [dd_event] = 'click_dw:a_menu_edit'
			getdialog [att] = 'dw:a_list', 'selection'
			getdialog [val] = [widgets:[current_widget][w_name]], [att]
			
			menu_editor [val] = [val]
			
			letdialog [widgets:[current_widget][w_name]], [att], [val]
			updateAttribInfo
		endif
		
		if [dd_event] = 'change_dw:p_x' | [dd_event] = 'change_dw:p_y' | [dd_event] = 'change_dw:p_width' | [dd_event] = 'change_dw:p_height'
			getdialog [widgets:[current_widget][w_x]] 	= 'dw:p_x', 'text'
			getdialog [widgets:[current_widget][w_y]] 	= 'dw:p_y', 'text'
			getdialog [widgets:[current_widget][w_width]] 	= 'dw:p_width', 'text'
			getdialog [widgets:[current_widget][w_height]] 	= 'dw:p_height', 'text'
			letdialog [widgets:[current_widget][w_name]], 'position', [widgets:[current_widget][w_x]] # '|' # [widgets:[current_widget][w_y]] # '|' # [widgets:[current_widget][w_width]] # '|' # [widgets:[current_widget][w_height]]

			[file_changed] = [true]
		endif
		
		rem minimize all if dd window is sized!
		if [dd_event] = 'size_dd'
			getdialog [val] = 'dw', 'visible'
			if [val] = [false]
				letdialog 'dw', 'visible', [true]
				letdialog [designwin], 'visible', [true]
			else
				letdialog 'dw', 'visible', [false]
				letdialog [designwin], 'visible', [false]
			endif
		endif
		
		if [dd_event] = 'move_' # [designwin] | [dd_event] = 'size_' # [designwin]
			[current_widget] = '1'
			
			[mode] = 'none'
			setCurrentWidget

			[file_changed] = [true]
		elseif [current_widget] > '1'
			if [mode] ! 'none'
				if [dd_event] = 'mousemove_' # [designwin]
					retrievepositions [designwin], [x], [y], [null], [null]

					if [mode] = 'scale'
						[mx] = ([mouse_x] - [x] - [widgets:[current_widget][w_x]])
						[my] = ([mouse_y] - [y] - [widgets:[current_widget][w_y]] - [xp_pix_y] - [menu_height])
						if [raster_toggle] = [true]
							[mx] = [mx] - ([mx] ~ [raster_x])
							[my] = [my] - ([my] ~ [raster_y])
						endif
						letdialog [widgets:[current_widget][w_name]], 'position', [widgets:[current_widget][w_x]] # '|' # [widgets:[current_widget][w_y]] # '|' # [mx] # '|' # [my]
					else
						[mx] = ([mouse_x] - [x] - [xp_pix_x] - [widgets:[current_widget][w_width]])
						[my] = ([mouse_y] - [y] - [xp_pix_y] - [menu_height] - [widgets:[current_widget][w_height]])
						if [raster_toggle] = [true]
							[mx] = [mx] - ([mx] ~ [raster_x])
							[my] = [my] - ([my] ~ [raster_y])
						endif
						letdialog [widgets:[current_widget][w_name]], 'position', [mx] # '|' # [my] # '|' # [widgets:[current_widget][w_width]] # '|' # [widgets:[current_widget][w_height]]
					endif
					updateWidgetInfo
				elseif [dd_event] = 'click_' # [designwin]
					letdialog [widgets:[current_widget][w_name]], 'enabled', [current_widget_enabled]

					[mode] = 'none'
					updateModeState
				endif
	
				[file_changed] = [true]
			endif
		endif
	endif	
until [dd_event] = 'close_dd' | [dd_event] = 'click_dd:Menu_menu_file_quit'

if [file_changed] = [true]
	getlng_label [lng_text] = 'msg_saveanychanges'
	msgbox [savenow] = [dd_name], [lng_text], '3' + '32'
	if [savenow] = '6'
		save_file [current_file], [false]
	elseif [savenow] = '2'
		goto 'dd_event_loop'
	endif
endif
end
