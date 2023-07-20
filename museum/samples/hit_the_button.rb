rem +-------------------------------------------------------------------+
rem | hit the button                                                    |
rem +-------------------------------------------------------------------+
rem | Try to hit the button appearing on random positions in this tiny  |
rem | game written in RapidBATCH 5.0.                                   |
rem +-------------------------------------------------------------------+

[Echo_Title] = '(C) 2005 by coolzero0001'
newdialog 'myDialog', 'DIALOG', '190|190|800|600'
letdialog 'myDialog', 'CAPTION', 'hit the button :: by coolzero0001'

newdialog 'myDialog:Go', 'button', '10|10|300|200'

letdialog 'myDialog:Go', 'caption', 'click me!!!'
letdialog 'myDialog:Go', 'font', 'Arial|12|2'

letdialog 'myDialog', 'VISIBLE', [true]

[points] = '0'

repeat
	rem read out the dialog's size
	getdialog [size] = 'myDialog', 'position'
	
	gettok [pos_x] = [size], [std_sep], '3'
	gettok [pos_y] = [size], [std_sep], '4'
	
	rem randomize the new position
	randvar [pos_x] = [pos_x] - '100'
	randvar [pos_y] = [pos_y] - '20'

	rem set the button's new position
	letdialog 'myDialog:Go', 'position', [pos_x] #'|' #[pos_y] #'|100|50'
	
	rem wait for events
	rundialog [x] = '400'
	
	rem button clicked?
	if [x] = 'click_myDialog:Go'
		echo 'YOU GOT IT! Continue...'
		[points] + '1000'
	endif
until [x] = 'close_myDialog'

if [points] > '1000'
	echo 'Excellent, you took ' # [points] # ' points!'
elseif [points] = '1000'
	echo 'Fine, you hit it for one time!'
else
	echo 'Nice try!'
endif
end