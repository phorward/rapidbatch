rem +-------------------------------------------------------------------+
rem | Image Demo                                                        |
rem +-------------------------------------------------------------------+
rem | This is a demonstration on how to show a bitmap image on a user-  |
rem | defined dialog in RapidBATCH 5.0.                                 |
rem +-------------------------------------------------------------------+

rem create dialog
newdialog 'Demo', 'dialog', '100|100|465|320'
letdialog 'Demo', 'caption', 'Orcinus Orca - Image Demo'
letdialog 'Demo', 'event_size', [true]

rem add image widget
newdialog 'Demo:img', 'image', '1|1|465|320'
letdialog 'Demo:img', 'image', 'orca.bmp'

rem make the dialog visible
letdialog 'Demo', 'visible', [true]

rem update image size once to dialog's size
[event] = 'size_Demo'

repeat
	if [event] = 'size_Demo'
		getdialog [p] = 'Demo', 'position'
		gettok [w] = [p], '|', '3'
		gettok [h] = [p], '|', '4'
		
		letdialog 'Demo:img', 'position', '1|1|' # ([w]-([Metric_Edge_Height] * '2')) # '|' # ([h] - [Metric_Caption_Height]-([Metric_Edge_Height] * '2') - '5')
	endif
	
	rem ok, wait for events
	rundialog [event] = '0'
until [event] = 'close_Demo'
end 