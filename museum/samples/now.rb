rem +-------------------------------------------------------------------+
rem | now                                                               |
rem +-------------------------------------------------------------------+
rem | Sample script printing current date and time information.         |
rem +-------------------------------------------------------------------+

include 'date.rb'
include 'dialog.rb'

rem --- creating widget "now"
newdialog 'now', 'DIALOG', '1|1|370|80'
letdialog 'now', 'caption', 'Now!'
letdialog 'now', 'style', 'single'

rem --- creating widget "l_current_time"
newdialog 'now:l_current_time', 'LABEL', '5|1|50|25'
letdialog 'now:l_current_time', 'enabled', '0'
letdialog 'now:l_current_time', 'caption', 'Time:'
letdialog 'now:l_current_time', 'font', 'Comic Sans MS|12|0'

rem --- creating widget "l_current_date"
newdialog 'now:l_current_date', 'LABEL', '5|26|50|25'
letdialog 'now:l_current_date', 'enabled', '0'
letdialog 'now:l_current_date', 'caption', 'Date:'
letdialog 'now:l_current_date', 'font', 'Comic Sans MS|12|0'

rem --- creating widget "time"
newdialog 'now:time', 'LABEL', '60|1|400|25'
letdialog 'now:time', 'enabled', '0'
letdialog 'now:time', 'font', 'Comic Sans MS|12|2'

rem --- creating widget "date"
newdialog 'now:date', 'LABEL', '60|26|400|25'
letdialog 'now:date', 'enabled', '0'
letdialog 'now:date', 'font', 'Comic Sans MS|12|2'

rem --- make the dialog window visible and center it!
centerdialog 'now'
letdialog 'now', 'visible', [true]

repeat
	rem --- updating the time's text
	letdialog 'now:time', 'caption', [time]

	rem --- updating the date's text
	getdayofweektext [weekday] = [day], [month], [year]
	
	if [month] = '01' [mon] = 'January'
	if [month] = '02' [mon] = 'February'
	if [month] = '03' [mon] = 'March'
	if [month] = '04' [mon] = 'April'
	if [month] = '05' [mon] = 'May'
	if [month] = '06' [mon] = 'June'
	if [month] = '07' [mon] = 'July'
	if [month] = '08' [mon] = 'August'
	if [month] = '09' [mon] = 'September'
	if [month] = '10' [mon] = 'October'
	if [month] = '11' [mon] = 'November'
	if [month] = '12' [mon] = 'December'
	
	letdialog 'now:date', 'caption', [weekday] # ', ' # [mon] # ' ' # [day] # ', ' # [year]
	
	rem --- same to the dialog's caption bar
	letdialog 'now', 'caption', [time] # ' on ' # [mon] # ' ' # [day] # ', ' # [year]

	rem --- wait for event with timeout (one second)
	rundialog [event] = '1000'

until [event] = 'close_now' | [event] = 'cancel_now'
end