rem +-------------------------------------------------------------------+
rem | RapidBATCH v5.1                                                   |
rem +-------------------------------------------------------------------+
rem | Library: Date Functions                                           |
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


rem *********************
rem ** IMPLEMENTATIONS **
rem *********************

rem ===================================================================
rem FUNCTION:
rem isleapyear
rem
rem DESCRIPTION:
rem Checks if the year, assigned as a parameter, is a leap year.
rem
rem PARAMETERS:
rem + [l_year]
rem   4-digit number of a year, e.g. '2005'.
rem
rem RETURN VALUE:
rem '0' if the year is a leap-year, else '-1'
rem ===================================================================
func isleapyear: [l_year]
	if ( [l_year] ~ '400' ) = '0'
		ret [true]
	elseif ( [l_year] ~ '100' ) = '0'
		ret [false]
	elseif ( [l_year] ~ '4' ) = '0'
		ret [true]
	else
		ret [false]
	endif
endfunc

rem ===================================================================
rem FUNCTION:
rem getdayofweek
rem
rem DESCRIPTION:
rem Calculates the weekday-number of a date.
rem Contrubtions for Gaussian weekday calculation by lehmundstroh.
rem
rem PARAMETERS:
rem + [l_day]
rem   The date's day.
rem + [l_month]
rem   The date's month.
rem + [l_year]
rem   The date's year.
rem
rem RETURN VALUE:
rem '0' = Monday
rem '1' = Thuesday
rem '2' = Wednesday
rem '3' = Thursday
rem '4' = Friday
rem '5' = Saturday
rem '6' = Sunday
rem 
rem '-1' = Error in calculation
rem ===================================================================
func getdayofweekneu: [l_day], [l_month], [l_year]
	dec [_n], [_y], [_c], [_yv], [_cv], [_w], [weekday]

rem neue Form gemäß der vollständigen Gaußschen Wochentagsformel, gültig ab 1582, hier aber auch auf die Zeit davor angewendet
rem w=(d+[2,6*m-0,2]+y+[y/4]+[c/4]-2*c) mod 7 (die eckigen Klammern sind Gaußklammern, geben also nur den positiven Ganzzahlteil zurück
rem d ist Tagesdatum (1 bis 31)
rem m ist Gaußsche Monatszahl: März=1...Dez=10, Jan=11, Feb=12
rem y sind die beiden letzten Stellen der Jahreszahl. Für Jan und Feb um 1 vermindert!!!
rem c sind die beiden ersten Stellen der Jahreszahl
rem w ist der Wochentag: Sonntag=0, Montag=1....
rem der Wert aus [2,6*m-0,2] steht fest und wird hier zu [_n] zusammengefasst
rem Bei Modulo-Berechnungen ist zu beachten, dass Gauß immer nur den positiven Wert benutzt. Wird also ein Mod-Wert negativ, so muss der Divisor hinzugezählt werden

	rem [l_month] auf echte Zahl bringen, falls mit führender Null übergeben
	[l_month] + '0'
	if [l_month] = '1'
		[_n] = '28'
	elseif  [l_month] = '2'
		[_n] = '31'
	elseif  [l_month] = '3'
		[_n] = '2'
	elseif  [l_month] = '4'
		[_n] = '5'
	elseif  [l_month] = '5'
		[_n] = '7'
	elseif  [l_month] = '6'
		[_n] = '10'
	elseif  [l_month] = '7'
		[_n] = '12'
	elseif  [l_month] = '8'
		[_n] = '15'
	elseif  [l_month] = '9'
		[_n] = '18'
	elseif  [l_month] = '10'
		[_n] = '20'
	elseif  [l_month] = '11'
		[_n] = '23'
	elseif  [l_month] = '12'
		[_n] = '25'
	endif
	
	rem Jahr vierstellig machen
	copyvar [l_year] = '0000' # [l_year], '-1', '4'
	copyvar [_y] = [l_year], '3', '2'
	copyvar [_c] = [l_year], '1', '2'
	[_y] + '0'
	rem Korrektur für Jan und Feb
	if [l_month] = '1' | [l_month] = '2'
		[_y] - '1'
		if [_y] = '-1'
			[_y] = '99'
			[_c] - '1'
		endif
	endif
	
	rem gazzahlige Teile von [y/4] und [c/4] holen
	[_yv] = [_y] / '4'
	getpos [_pos] = [_yv], '.', '1'
	if [_pos] > '0'
		copyvar [_yv] = [_yv], '1', [_pos] - '1'
	endif
	[_cv] = [_c] / '4'
	getpos [_pos] = [_cv], '.', '1'
	if [_pos] > '0'
		copyvar [_cv] = [_cv], '1', [_pos] - '1'
	endif

	[_w] = [l_day] + [_n] + [_y] + [_yv] + [_cv] - ('2' * [_c])
	[weekday] = ([_w] ~ '7')
	if [weekday] < '0'
		[weekday] + '7'
	endif
	
	if [weekday] = '0'
		ret '7'
	elseif [weekday] = '1'
		ret '0'
	elseif [weekday] = '2'
		ret '1'
	elseif [weekday] = '3'
		ret '2'
	elseif [weekday] = '4'
		ret '3'
	elseif [weekday] = '5'
		ret '4'
	elseif [weekday] = '6'
		ret '5'
	elseif [weekday] = '7'
		ret '6'
	else
		ret '-1'
	endif
endfunc

rem ===================================================================
rem FUNCTION:
rem getdayofweektext
rem
rem DESCRIPTION:
rem Calculates the weekday-number of a date, and returns the english
rem spoken name for it, e.g. 'Monday'. This function is a wrapping
rem function for getdayofweek.
rem
rem PARAMETERS:
rem + [l_day]
rem   The date's day.
rem + [l_month]
rem   The date's month.
rem + [l_year]
rem   The date's year.
rem
rem RETURN VALUE:
rem The weekday's name of the date that is specified.
rem ===================================================================
func getdayofweektext: [l_day], [l_month], [l_year]
	dec [weekday]
	getdayofweek [weekday] = [l_day], [l_month], [l_year]

	if [weekday] = '0' ret 'Monday'
	if [weekday] = '1' ret 'Thuesday'
	if [weekday] = '2' ret 'Wednesday'
	if [weekday] = '3' ret 'Thursday'
	if [weekday] = '4' ret 'Friday'
	if [weekday] = '5' ret 'Saturday'
	if [weekday] = '6' ret 'Sunday'

	ret 'unknown'
endfunc
end