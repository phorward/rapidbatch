rem J.M.K S.F. HTML Content Tree Creator v1.0
rem Copyright (C) 2005 by J.M.K S.F., Jan Max Meyer
[ctc_version] = 'v1.0'

rem Global declarations and script startup ----------------------------------------------------------------------------------
rem include 'rb5_extdialog.rb'
rem include 'rb5_dlgtools.rb'

include 'system.rb'
include 'dialog.rb'
include 'array.rb'

dec [current_file], [filechanged]

dec [items]

[item_level]		= '1'
[item_label] 		= '2'
[item_page] 		= '3'
[item_alias]		= '4'

[content_dir] = 'mcontent'
[template_dir] = 'mtemplates'
[output_dir] = 'moutput'

[resdir] = 'resources'
[cont_dir] = 'contents'

rem Copyright message
[oldheight] = [InfoBox_Height]
[oldwidth] = [InfoBox_Width]
[InfoBox_Height] = '40'
[InfoBox_Width] = '360'
infobox 'J.M.K S.F. HTML Content Tree Creator ' # [ctc_version] # [new_line] # 'Copyright (C) 2005 by J.M.K S.F. Software Technologies', 'show'
rem wait '2000'
[InfoBox_Enabled] = '-1'
[InfoBox_Height] = [oldheight]
[InfoBox_Width] = [oldwidth]


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


rem Procedure: saveFile -----------------------------------------------------------------------------------------------------
proc saveFile: [filename]
	dec [i]	
	delfile [filename]
	
	[i] = '0'
	repeat
		[i] + '1'
		writefile [filename], [items:[i], [item_level]] # '|' # [items:[i], [item_label]] # '|' # [items:[i], [item_page]] # '|' # [items:[i], [item_alias]]
	until [i] = [items:'0']
	
	echo 'Menu description was saved to ' # [filename]	
endproc

rem Procedure: openFile -----------------------------------------------------------------------------------------------------
proc openFile: [filename]
	dec [i], [line]
	
	reset [items]
	
	[i] = '0'
	repeat
		[i] + '1'
		readfile [line] = [filename], [i]
		if [line] = '' | [line] = 'EOF' break

		listtoarray [items:[i]], [line], '|'
	until [line] = 'EOF'
endproc

rem Procedure: genMenu ------------------------------------------------------------------------------------------------------
func genMenu
	dec [level], [maxlevel], [i], [j], [k], [hasothernode], [code]
	
	rem first of all, get the highest level
	[i] = '0'
	[maxlevel] = '0'
	repeat
		[i] + '1'
		if [maxlevel] < [items:[i], [item_level]]
			[maxlevel] = [items:[i], [item_level]]
		endif
	until [i] = [items:'0']
	[maxlevel] + '1'

		
	rem ok, go through the items, and generate the HTML code
	[i] = '0'
	repeat
		[i] + '1'
		
		[code] # [tab] # '<tr>' # [new_line] # [tab] # [tab]
		
		[level] = [items:[i], [item_level]]
		
		rem draw previous parent tree lines (|)
		[j] = '0'
		if [level] > '1'
			repeat
				[j] + '1'
				if [lastparents:[j]+'1'] = [true]
					[code] # '<td class="icon"><img src="l.gif" /></td>'
				else
					[code] # '<td class="icon"><img src="e.gif" /></td>'
				endif
			until ([j]+'1') = [level]
		endif

		[hasothernode] = [false]
		[k] = [i]
		repeat
			[k] + '1'
			if [items:[k],[item_level]] = [level]
				[hasothernode] = [true]
				break
			endif
			if [items:[k],[item_level]] < [level] break
		until [k] = [items:'0']

		rem draw the knot in front of the item
		if [level] > '0'			
			if [hasothernode] = [true]
				[code] # '<td class="icon"><img src="b.gif" /></td>'
			else
				[code] # '<td class="icon"><img src="nb.gif" /></td>'
			endif
			
			[j] + '1'
		endif

		rem ok, now draw the item's icon

		if [items:[i], [item_page]] = [null]
			if [i] < [items:'0'] & [level] > '0' & [items:[i]+'1', [item_level]] > [level]
				[code] # '<td class="icon"><img src="fp.gif" /></td>'
			elseif [level] > '0'
				[code] # '<td class="icon"><img src="f.gif" /></td>'
			else
				[code] # '<td class="icon"><img src="fr.gif" /></td>'
			endif
		else
			if [level] = '0'
				[code] # '<td class="icon"><img src="dr.gif" /></td>'
			else
				[code] # '<td class="icon"><img src="d.gif" /></td>'
			endif
		endif
		[j] + '1'

		
		rem update lastparent-info
		[lastparents:[level]+'1'] = [hasothernode]

		rem display the label
		rem echo [items:[i], [item_label]] # ' ' # [j]
		
		[code] # '<td '
		
		if [items:[i], [item_page]] = [null] [code] # ' class="folder" '
		
		if ([maxlevel]-[j])+'1' > '1'
			
			[code] # 'colspan="' # ([maxlevel]-[j])+'1' # '">'
		else
			[code] # '>'
		endif
		
		if [items:[i], [item_alias]] ! [null]
			[code] # '<a name="' # [items:[i], [item_alias]] # '"></a>'
		endif
				
		if [items:[i], [item_page]] ! [null]
			[code] # '<a href="' # [cont_dir] # '/' # [items:[i], [item_page]] # '" target="content">' # [items:[i], [item_label]] # '</a>'
		else
			[code] # [items:[i], [item_label]]
		endif
		
		[code] # '</td>'
		
		[code] # [new_line] # [tab] # '</tr>' # [new_line]
	until [i] = [items:'0']
	
	ret [code]
endfunc

rem Procedure: SetCaption ---------------------------------------------------------------------------------------------------
proc SetCaption
	dec [title]
	if [current_file] = [null]
		[title] = 'untitled'
	else
		[title] = [current_file]
	endif
	letdialog 'ctc', 'caption', 'HTML Content Tree Generator ' # [ctc_version] # ' - ' # [title]
endproc


rem Procedure: copyresource
proc copyresource: [resfile], [tardir]
	dec [ex]
	
	fileexists [ex] = [tardir] # '\' # [resfile]
	if [ex] = [false]
		copyfile [resdir] # '\' # [resfile], [tardir] # '\' # [resfile]
	endif
endproc

rem Main routines -----------------------------------------------------------------------------------------------------------
%getcont
rem findfiles [content_files] = 'mcontent\*.men', '1'
rem if [content_files] = '' goto create_contents
rem listbox [contentfile] = 'Open menu definition:', [content_files]
rem if [contentfile] = '' goto create_contents

rem fileexists [c_exists] = [content_dir] # '\' # [contentfile]
rem if [c_exists] = [false]
rem 	%create_contents
rem 	createNewContent
rem 	if [contentfile] = '' halt
rem endif


rem --- first of all, evaluate a possible menu descriptor
rem getmenu_froctcef [def], [items], [item_levels]

rem --- creating code for widget ctc
newdialog 'ctc', 'DIALOG', '220|100|481|445'

if [errorcode] = [true]
	letdialog 'ctc', 'style', 'single'
	letdialog 'ctc', 'menu', '&File:&New|&Open|&Save|Save &as...|-|&Exit;&Menu:&Generate;&Help:&About...;;'
	letdialog 'ctc', 'event_ok', [true]
	
	rem --- creating code for widget list
	newdialog 'ctc:list', 'LIST', '1|1|430|300'
	letdialog 'ctc:list', 'enabled', '0'
	letdialog 'ctc:list', 'event_change', [true]
	
	rem --- creating code for widget m_up
	newdialog 'ctc:m_up', 'IMAGEBUTTON', '432|1|40|40'
	letdialog 'ctc:m_up', 'enabled', '0'
	letdialog 'ctc:m_up', 'image_transparent', 'img\b_up.bmp'
	
	rem --- creating code for widget m_down
	newdialog 'ctc:m_down', 'IMAGEBUTTON', '432|40|40|40'
	letdialog 'ctc:m_down', 'enabled', '0'
	letdialog 'ctc:m_down', 'image_transparent', 'img\b_down.bmp'
	
	rem --- creating code for widget m_left
	newdialog 'ctc:m_left', 'IMAGEBUTTON', '432|80|40|40'
	letdialog 'ctc:m_left', 'enabled', '0'
	letdialog 'ctc:m_left', 'image_transparent', 'img\b_left.bmp'
	
	rem --- creating code for widget m_right
	newdialog 'ctc:m_right', 'IMAGEBUTTON', '432|120|40|40'
	letdialog 'ctc:m_right', 'enabled', '0'
	letdialog 'ctc:m_right', 'image_transparent', 'img\b_right.bmp'

	rem --- creating code for widget l_citem
	newdialog 'ctc:l_citem', 'LABEL', '5|305|95|20'
	letdialog 'ctc:l_citem', 'caption', 'Item title:'
	
	rem --- creating code for widget citem
	newdialog 'ctc:citem', 'INPUT', '100|300|330|25'
	letdialog 'ctc:citem', 'enabled', '0'
	letdialog 'ctc:citem', 'readonly', '-1'

	rem --- creating code for widget l_citem
	newdialog 'ctc:l_citem_page', 'LABEL', '5|330|95|20'
	letdialog 'ctc:l_citem_page', 'caption', 'HTML-Page:'
		
	rem --- creating code for widget citem_page
	newdialog 'ctc:citem_page', 'INPUT', '100|325|330|25'
	letdialog 'ctc:citem_page', 'enabled', '0'
	letdialog 'ctc:citem_page', 'readonly', '-1'
	
	rem --- creating code for widget l_citem_alias
	newdialog 'ctc:l_citem_alias', 'LABEL', '5|355|95|20'
	letdialog 'ctc:l_citem_alias', 'caption', 'Aliases:'
		
	rem --- creating code for widget citem_alias
	newdialog 'ctc:citem_alias', 'INPUT', '100|350|330|25'
	letdialog 'ctc:citem_alias', 'enabled', '0'
	letdialog 'ctc:citem_alias', 'readonly', '-1'
	
	rem --- creating code for widget item_add
	newdialog 'ctc:item_add', 'BUTTON', '1|375|142|25'
	letdialog 'ctc:item_add', 'enabled', '0'

	letdialog 'ctc:item_add', 'caption', '&Insert'
		
	rem --- creating code for widget item_update
	newdialog 'ctc:item_update', 'BUTTON', '144|375|142|25'
	letdialog 'ctc:item_update', 'enabled', '-1'
	letdialog 'ctc:item_update', 'caption', '&Update'
		
	rem --- creating code for widget item_delete
	newdialog 'ctc:item_delete', 'BUTTON', '288|375|142|25'
	letdialog 'ctc:item_delete', 'enabled', '-1'
	letdialog 'ctc:item_delete', 'caption', '&Delete'
endif

chscriptdir
[scriptdir] = [current]

	
rem --- make the dialog window visible
centerdialog 'ctc'
SetCaption
letdialog 'ctc', 'visible', [true]
letdialog 'ctc:citem', 'focus', [true]


[filechanged] = [false]
if [command] ! [null]
	openFile [command]
	[current_file] = [command]
	gosub 'updatemenu'
	SetCaption
endif

%correct
repeat
	rundialog [event] = '0'

	rem --- event handling for click on m_up
	if [event] = 'click_ctc:m_up'
		getdialog [pos] = 'ctc:list', 'selindex'
		if [pos] < '2' cont
		
		copyarray [items:[pos]-'1'], [item]
		copyarray [items:[pos]], [items:[pos]-'1']
		copyarray [item], [items:[pos]]
		
		[pos] - '1'
		
		[filechanged] = [true]
		
		gosub 'updatemenu'
		
	rem --- event handling for click on m_down
	elseif [event] = 'click_ctc:m_down'
		getdialog [pos] = 'ctc:list', 'selindex'
		if [pos] = [items:'0'] cont
		
		copyarray [items:[pos]+'1'], [item]
		copyarray [items:[pos]], [items:[pos]+'1']
		copyarray [item], [items:[pos]]

		[pos] + '1'

		[filechanged] = [true]
		
		gosub 'updatemenu'
		
	rem --- event handling for click on b_left
	elseif [event] = 'click_ctc:m_left'
		if [pos] > '0' & [items:[pos], [item_level]] > '0'
			[items:[pos], [item_level]] - '1'
			gosub 'updatemenu'

			[filechanged] = [true]
		endif			
	rem --- event handling for click on b_right
	elseif [event] = 'click_ctc:m_right'
		if [pos] > '0'
			[items:[pos], [item_level]] + '1'
			gosub 'updatemenu'
	
			[filechanged] = [true]
		endif		
	rem --- event handling for click on b_right
	elseif [event] = 'change_ctc:list'
		getdialog [pos] = 'ctc:list', 'selindex'
		letdialog 'ctc:citem', 'text', [items:[pos],[item_label]]
		letdialog 'ctc:citem_page', 'text', [items:[pos],[item_page]]
		letdialog 'ctc:citem_alias', 'text', [items:[pos],[item_alias]]
		letdialog 'ctc:citem', 'selpos', '1|-1'
		letdialog 'ctc:citem', 'focus', [true]
	rem --- event handling for click on item_add
	elseif [event] = 'click_ctc:item_add'
		getdialog [label] = 'ctc:citem', 'text'
		getdialog [page] = 'ctc:citem_page', 'text'
		getdialog [alias] = 'ctc:citem_alias', 'text'		
				
		if [label] ! ''
			getdialog [pos] = 'ctc:list', 'selindex'
			if [pos] < '1'
				[pos] = '1'
			else
				[pos] + '1'
			endif

			if [pos] > '1'
				[item:[item_level]] = [items:[pos]-'1', [item_level]]
			else
				[item:[item_level]] = '0'
			endif
			
			[item:[item_label]] = [label]
			[item:[item_page]] = [page]
			[item:[item_alias]] = [alias]

			insertarrayitem [items], [null], [pos]
			copyarray [item], [items:[pos]]
			
			gosub 'updatemenu'
			
			letdialog 'ctc:citem', 'selpos', '1|-1'
			letdialog 'ctc:citem', 'focus', [true]

			[filechanged] = [true]
		endif

	rem --- event handling for click on item_update
	elseif [event] = 'click_ctc:item_update' | [event] = 'ok_ctc'
		getdialog [items:[pos],[item_label]] = 'ctc:citem', 'text'
		getdialog [items:[pos],[item_page]] = 'ctc:citem_page', 'text'
		getdialog [items:[pos],[item_alias]] = 'ctc:citem_alias', 'text'

		[filechanged] = [true]			
		gosub 'updatemenu'
		
		[pos]+'1'
		letdialog 'ctc:list', 'selindex', [pos]
		letdialog 'ctc:citem', 'text', [items:[pos],[item_label]]
		letdialog 'ctc:citem_page', 'text', [items:[pos],[item_page]]
		letdialog 'ctc:citem_alias', 'text', [items:[pos],[item_alias]]
		letdialog 'ctc:citem', 'selpos', '1|-1'
		letdialog 'ctc:citem', 'focus', [true]
	rem --- event handling for click on item_delete
	elseif [event] = 'click_ctc:item_delete'
		if [pos] < '1' cont

		deletearrayitem [items], [pos]
		
		if [pos] > '1' [pos]-'1'

		[filechanged] = [true]
		gosub 'updatemenu'
	elseif [event] = 'click_ctc:Menu_&Save' | [event] = 'click_ctc:Menu_Save &as...'
		if [current_file] = [null] | [event] = 'click_ctc:Menu_Save &as...'
			[SaveBox_DefFileExt] = 'men'
			savebox [current_file] = 'Save menu description as...', 'Menu Descriptors (*.men)|*.men'
			if [current_file] = [null] cont
		endif
		
		saveFile [current_file]

		[filechanged] = [false]
		SetCaption
	elseif [event] = 'click_ctc:Menu_&Open'
		[OpenBox_DefFileExt] = 'men'
		openbox [ncurrent_file] = 'Open menu descriptor', 'Menu Descriptors (*.men)|*.men'
		if [ncurrent_file] = [null] cont
		
		openFile [ncurrent_file]
		[current_file] = [ncurrent_file]
		[filechanged] = [false]
		
		gosub 'updatemenu'
		
		SetCaption
	elseif [event] = 'click_ctc:Menu_&New'
		if [filechanged] = [true]
			confirm [errorcode] = 'Do you really want to create a new file?'
			if [errorcode] = [false] cont
		endif
		
		reset [items]
		[current_file] = [null]
				
		gosub 'updatemenu'
		
		SetCaption
	elseif [event] = 'click_ctc:Menu_&Generate'
		genMenu [html]
		
		folderbox [output] = 'Select output folder:', [scriptdir] # '\output'
		if [output] ! [null]
			copyresource 'b.gif', [output]
			copyresource 'd.gif', [output]
			copyresource 'dr.gif', [output]
			copyresource 'e.gif', [output]
			copyresource 'f.gif', [output]
			copyresource 'fp.gif', [output]
			copyresource 'fr.gif', [output]
			copyresource 'l.gif', [output]
			copyresource 'nb.gif', [output]

			rem copyresource 'rb5_small.gif', [output]
		
			copyresource 'menu.css', [output]

			delfile [output] # '\' # 'frameset.html'
			readfile [menu] = [resdir] # '\' # 'frameset.html', '0'
			replacevar [menu] = [menu], [crlf], [new_line]
			
			inputbox [title] = 'Frameset Title', 'Enter an optional frameset title:', ''
			
			replacevar [menu] = [menu], '#TITLE#', [title]
			writefile [output] # '\' # 'frameset.html', [menu]
			
			delfile [output] # '\' # 'menu.html'
			readfile [menu] = [resdir] # '\' # 'menu.html', '0'			
			replacevar [menu] = [menu], [crlf], [new_line]
			replacevar [menu] = [menu], '#TREE_CODE#', [html]
			replacevar [menu] = [menu], '#TITLE#', [title]
			writefile [output] # '\' # 'menu.html', [menu]
			
			mkdir [output] # '\' # [cont_dir]
		endif
	endif

until [event] = 'close_ctc'

if [filechanged] = [true]
	confirm [errorcode] = 'Save before quit?'
	if [errorcode] = [false] halt

	if [current_file] = [null]
		[SaveBox_DefFileExt] = 'men'
		savebox [current_file] = 'Save menu description as...', 'Menu Descriptors (*.men)|*.men'
		if [current_file] = [null] goto 'correct'
	endif
	
	saveFile [current_file]		
endif

halt

rem --- sub routine for updating the item list
%updatemenu
[list] = [null]
letdialog 'ctc:list', 'enabled', [false]

if [items:'0'] > '0'
	[i] = '0'
	repeat
		[i] + '1'
		if [items:[i][item_level]] > '0'
			[j] = '0'
			repeat
				[j] + '1'
				[list] # ' -> '
			until [j] = [items:[i][item_level]]
		endif
		[list] # [items:[i][item_label]] # ' (' # [items:[i][item_page]] # '/' # [items:[i][item_alias]] # ')' # [std_sep]
	until [i] = [items:'0']
endif

letdialog 'ctc:list', 'items', [list]
letdialog 'ctc:list', 'selindex', [pos]

if [list] ! [null]
	letdialog 'ctc:item_update', 'enabled', '0'
	letdialog 'ctc:item_delete', 'enabled', '0'
else
	letdialog 'ctc:item_update', 'enabled', '-1'
	letdialog 'ctc:item_delete', 'enabled', '-1'
endif

letdialog 'ctc:list', 'enabled', [true]
letdialog 'ctc:list', 'focus', [true]

resume

end