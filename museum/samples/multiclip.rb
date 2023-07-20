rem +-------------------------------------------------------------------+
rem | MultiClip                                                         |
rem +-------------------------------------------------------------------+
rem | This nice utility provides a "clipboard" for more than one value. |
rem | Each time a value is copied into the clipboard, MultiClip automa- |
rem | tically adds this value to its list. One click on the desired va- |
rem | lue does copy it into the clipboard again. To delete a value from |
rem | the list, use the X-button behind each item.                      |
rem +-------------------------------------------------------------------+

include 'array.rb'

rem *************
rem ** GLOBALS **
rem *************

dec [clips], [i], [last_deleted]
dec [multiclip_Width], [multiclip_btnHeight], [updateclip]

[updateclip] = [false]

[multiclip_Width] = '200'
[multiclip_btnHeight] = '25'

rem *********************
rem ** IMPLEMENTATIONS **
rem *********************

rem build_clip_menu
rem ===============
rem Generates buttons for the clipboard-items
proc build_clip_menu: [*items]
	dec [h], [i], [item], [ilen]
	[h] = '4'

	if [items:'0'] > '0'
		[i] = '0'
		repeat
			[i] + '1'
			if [items:[i]] ! ''
				[item] = [items:[i]]

				rem cut the text displayed on the button and make it looking well ;)
				trimvar [item] = [item]
				replacevar [item] = [item],[crlf], ' '
				replacevar [item] = [item],[new_line], ' '
				replacevar [item] = [item],[tab], ' '

				rem if the text's length is more than 60 characters, replace the rest
				rem with '...' - this is only for a better view of the item
				getlen [ilen] = [item]
				if [ilen] > '20'
					copyvar [item] = [item], '1', '20'
					[item] # ' ...'
				endif
				
				rem create the button to select the desired item
				deldialog 'multiclip' # ':clip_' # [i]
				newdialog 'multiclip' # ':clip_' # [i], 'button', '5' # [std_sep] # [h] # [std_sep] # ([multiclip_Width]-'34') # [std_sep] # [multiclip_btnHeight]
				letdialog 'multiclip' # ':clip_' # [i], 'caption', [item]
				
				rem create the delete-button
				deldialog 'multiclip' # ':del_' # [i]
				newdialog 'multiclip' # ':del_' # [i], 'button', [multiclip_Width] - '30' # [std_sep] # [h] # [std_sep] # '25' # [std_sep] # [multiclip_btnHeight]
				letdialog 'multiclip' # ':del_' # [i], 'caption', 'X'
				letdialog 'multiclip' # ':del_' # [i], 'font', 'Arial|10|2'

				[h] + [multiclip_btnHeight] + '2'
			endif
		until [i] = [items:'0']
	endif
	
	rem update the dialog's height
	letdialog 'multiclip', 'height', ([h] + [Metric_Caption_Height] + ('3' * [Metric_Edge_Width]))
endproc

rem **************************
rem ** MAIN SCRIPT ROUTINES **
rem **************************

rem creating the base window
newdialog 'multiclip', 'dialog', '1' # [std_sep] # '1' # [std_sep] # [multiclip_Width] # [std_sep] # [Metric_Caption_Height]
letdialog 'multiclip', 'style', 'single'
letdialog 'multiclip', 'visible', [true]
letdialog 'multiclip', 'caption', 'MultiClip :: ' # [clips:'0'] # ' item(s) available'

rem event handing
repeat
	rem if there is a new clipboard entry, add it!
	if [clipboard] ! [null] & [clipboard] ! [last_deleted] & [updateclip] = [false]
		[updateclip] = [true]
		if [clips:'0'] > '0'
			[i] = '0'
			repeat
				[i] + '1'
				if [clipboard] = [clips:[i]]
					[updateclip] = [false]
					break
				endif
			until [i] = [clips:'0']
		endif
		
		if [updateclip] = [true]
			insertarrayitem [clips], [clipboard], [clips:'0']+'1'
		endif
	endif

	rem update the items if required
	if [updateclip] = [true]
		build_clip_menu [clips]
		letdialog 'multiclip', 'caption', 'MultiClip :: ' # [clips:'0'] # ' item(s) available'
		
		[updateclip] = [false]
	endif

	rem wait for events, timeout 3 seconds
	rem (each timeout, MultiClip checks the clipboard for new content!)
	rundialog [event] = '3000'
	
	rem perform user-initated operation
	if [event] ! [null] & [event] ! 'close_multiclip'
		if [clips:'0'] > '0'
			[i] = '0'
			repeat
				[i] + '1'

				if [event] = 'click_multiclip' # ':del_' # [i]
					[last_deleted] = [clips:[i]]
					deletearrayitem [clips], [i]
					[updateclip] = [true]
					break
				elseif [event] = 'click_multiclip' # ':clip_' # [i]
					[clipboard] = [clips:[i]]
					break
				endif
			until [i] = [clips:'0']
		endif
	endif
until [event] = 'close_multiclip'
end