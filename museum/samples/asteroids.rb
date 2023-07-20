rem +-------------------------------------------------------------------+
rem | RapidAsteroids                                                    |
rem +-------------------------------------------------------------------+
rem | A clone of the classic PC game asteroids, written in 100% Rapid-  |
rem | BATCH core source. 
rem +-------------------------------------------------------------------+

include 'array.rb'
include 'dialog.rb'

rem *************
rem ** GLOBALS **
rem *************

dec [ship_x], [ship_y], [ship_width], [ship_height], [step_x], [step_y], [move_x], [move_y], [x], [y], [cnt], [i], [j]
dec [s_step_x], [s_step_y], [a_step_y], [ast_x], [laser_y], [space_y], [gfx_metric], [win_h], [win_w], [lifes]
dec [asteroids], [lasers]

rem --- setting some default values and metrics
[s_step_x] = '2'
[s_step_y] = '2'

[a_step_y] = '5'
[space_y] = '22'

[win_w] = '400'
[win_h] = '300'

[pos_x] = '1'
[pos_y] = '2'

[gfx_metric] = '22'

[lifes] = '3'

rem *********************
rem ** IMPLEMENTATIONS **
rem *********************

proc createAsteroid
	dec [x], [i], [j], [k], [useval]
	
	if [asteroids:'0'] > '0'
		[i] = '0'
		repeat
			[i] + '1'
			if [asteroids:[i]] = [null] break
		until [i] = [asteroids:'0']
		if [i] = [asteroids:'0'] [i] + '1'
	else
		[i] = '1'
	endif
	
	[useval] = [false]
	
	repeat
		randvar [x] = [win_w] + [gfx_metric]
		if [asteroids:'0'] = '0' break
		
		[useval] = [true]
		[j] = '0'
		repeat
			[j] + '1'			
			if [asteroids:[j]] = [x] [useval] = [false]
		until [j] = [asteroids:'0']
	until [useval] = [true]
	
	newdialog 'sa:asteroid_' # [i], 'image', [x] # '|' # [space_y] # '|' # [gfx_metric] # '|' # [gfx_metric]
	letdialog 'sa:asteroid_' # [i], 'image_transparent', 'asteroid.bmp'

	[asteroids:[i]] = 'sa:asteroid_' # [i]
	[asteroids:[i],[pos_x]] = [x]
	[asteroids:[i],[pos_y]] = [space_y]
endproc

proc createLaser
	dec [i]
	
	if [ship_y]-[gfx_metric] < [space_y] ret

	if [lasers:'0'] > '0'
		[i] = '0'
		repeat
			[i] + '1'
			if [lasers:[i]] = [null] break
		until [i] = [lasers:'0']
		if [i] = [lasers:'0'] [i] + '1'
	else
		[i] = '1'
	endif
	
	newdialog 'sa:laser_' # [i], 'image', [ship_x] # '|' # [ship_y]-[gfx_metric] # '|' # [gfx_metric] # '|' # [gfx_metric]
	letdialog 'sa:laser_' # [i], 'image_transparent', 'laser.bmp'

	[lasers:[i]] = 'sa:laser_' # [i]
	[lasers:[i],[pos_x]] = [ship_x]
	[lasers:[i],[pos_y]] = [ship_y]-[gfx_metric]
endproc

proc crash
	dec [i]

	[i] = '0'
	repeat
		[i] + '1'
		deldialog [asteroids:[i]]
	until [i] = [asteroids:'0']
	reset [asteroids]

	if [lasers:'0'] > '0'
		[i] = '0'
		repeat
			[i] + '1'
			deldialog [lasers:[i]]
		until [i] = [lasers:'0']
		reset [lasers]
	endif

	letdialog 'sa:ship', 'image_transparent', 'explosion.bmp'

	letdialog 'sa:life_' # [lifes], 'image_transparent', 'ship_death.bmp'
	[lifes] - '1'
	
	if [lifes] = '0'
		echo 'Game Over!'
		halt
	endif

	wait '500'
	
	letdialog 'sa:ship', 'image_transparent', 'ship.bmp'	
endproc

rem *****************
rem ** MAIN SCRIPT **
rem *****************

rem --- main dialog
newdialog 'sa', 'dialog', '1|1|400|300'
letdialog 'sa', 'event_mousemove', [true]
letdialog 'sa', 'event_click', [true]
letdialog 'sa', 'style', 'single'
letdialog 'sa', 'caption', 'Rapid Asteroid'

rem --- lifes status
[cnt] = '0'
repeat
	[cnt] + '1'
	
	newdialog 'sa:life_' # [cnt], 'image', ([win_w] - ([cnt] * [gfx_metric]) - '5' # '|1|' # [gfx_metric] # '|' # [gfx_metric])
	letdialog 'sa:life_' # [cnt], 'image_transparent', 'ship.bmp'
until [cnt] = [lifes]

rem --- space ship
newdialog 'sa:ship', 'image', ([win_w] / '2' - ([gfx_metric] / '2')) # '|' # ([win_h] / '2' - ([gfx_metric] / '2')) # '|' # [gfx_metric] # '|' # [gfx_metric]
letdialog 'sa:ship', 'image_transparent', 'ship.bmp'
letdialog 'sa:ship', 'event_click', [true]

rem --- center and show the dialog
centerdialog 'sa'
letdialog 'sa', 'visible', [true]

repeat
	rem --- wait for events
	rundialog [event] = '20'
	
	rem --- at mousemove, update the position the ship should move to
	if [event] = 'mousemove_sa'
		retrievepositions 'sa', [x], [y], [null], [null]
		[move_x] = [mouse_x] - [x]
		[move_y] = [mouse_y] - [y] - [Metric_Caption_Height] + [Metric_Border_Height]
		
		rem letdialog 'sa', 'caption', [move_x] # ' ' # [move_y] # ' ' # [ship_x] # ' ' # [ship_y]
		
		wait '15'
	endif
	
	rem --- update the ship's position
	[step_x] = '0'
	[step_y] = '0'
	retrievepositions 'sa:ship', [ship_x], [ship_y], [ship_width], [ship_height]
	if [ship_x] < [move_x] - ([ship_width]/'2') & ([ship_x] + [s_step_x]) < [move_x] [step_x] + [s_step_x]
	if [ship_y] < [move_y] - ([ship_height]/'2') & ([ship_y] + [s_step_y]) < [move_y] [step_y] + [s_step_y]
	if [ship_x] > [move_x] - ([ship_width]/'2') & ([ship_x] - [s_step_x]) > [move_x] [step_x] - [s_step_x]
	if [ship_y] > [move_y] - ([ship_height]/'2') & ([ship_y] - [s_step_y]) > [move_y] [step_y] - [s_step_y]
	
	[ship_x] + [step_x]
	[ship_y] + [step_y]
	
	if [ship_y] < [space_y] [ship_y] = [space_y]	
	letdialog 'sa:ship', 'position', [ship_x] # '|' # [ship_y] # '|' # [gfx_metric] # '|' # [gfx_metric]
	
	rem --- randomize if a new asteroid should be created
	randvar [newast] = '30'
	if ([newast] ~ '5') = '0' createAsteroid

	rem --- if the user clicks with the mouse, create a new laser
	if [event] = 'click_sa' | [event] = 'click_sa:ship' createLaser

	rem --- update the asteroid positions
	if [asteroids:'0'] > '0'
		[i] = '0'
		repeat
			[i] + '1'
			
			if [asteroids:[i]] ! [null]			
				[asteroids:[i],[pos_y]] + [a_step_y]
				letdialog [asteroids:[i]], 'position', [asteroids:[i],[pos_x]] # '|' # [asteroids:[i],[pos_y]] # '|' # [gfx_metric] # '|' # [gfx_metric]
		
				if [asteroids:[i],[pos_y]] > [win_h]
					deldialog [asteroids:[i]]
					[asteroids:[i]] = [null]
				elseif [asteroids:[i],[pos_x]] > [ship_x] & [asteroids:[i],[pos_x]] < [ship_x] + [gfx_metric]
	 				if [asteroids:[i],[pos_y]] > [ship_y] & [asteroids:[i],[pos_y]] < [ship_y] + [gfx_metric]
	 					crash
	 					break
	 				endif
				elseif [asteroids:[i],[pos_x]] < [ship_x] & [asteroids:[i],[pos_x]] + [gfx_metric] > [ship_x]
	 				if [asteroids:[i],[pos_y]] > [ship_y] & [asteroids:[i],[pos_y]] < [ship_y] + [gfx_metric]
	 					crash
	 					break
	 				endif
				endif
			endif
		until [i] = [asteroids:'0']
	endif
	
	
	rem --- update the laser positions
	if [lasers:'0'] > '0'
		[i] = '0'
		repeat
			[i] + '1'
			
			if [lasers:[i]] ! [null]

				[lasers:[i],[pos_y]] - [a_step_y]
	
				if [lasers:[i],[pos_y]] < [space_y]
					deldialog [lasers:[i]]
					[lasers:[i]] = [null]
				else
					if [asteroids:'0'] > '0'
						[j] = '0'
						repeat
							[j] + '1'
							
							if [asteroids:[j]] ! [null]
								if [asteroids:[j],[pos_x]] > [lasers:[i],[pos_x]] & [asteroids:[j],[pos_x]] < [lasers:[i],[pos_x]] + [gfx_metric]
									if [asteroids:[j],[pos_y]] > [lasers:[i],[pos_y]] & [asteroids:[j],[pos_y]] < [lasers:[i],[pos_y]] + [gfx_metric]
										deldialog [asteroids:[j]]
										deldialog [lasers:[i]]
										[asteroids:[j]] = [null]
										[lasers:[i]] = [null]
										break
									endif
								elseif [asteroids:[j],[pos_x]] < [lasers:[i],[pos_x]] & [asteroids:[j],[pos_x]] + [gfx_metric] > [lasers:[i],[pos_x]]
									if [asteroids:[j],[pos_y]] > [lasers:[i],[pos_y]] & [asteroids:[j],[pos_y]] < [lasers:[i],[pos_y]] + [gfx_metric]
										deldialog [asteroids:[j]]
										deldialog [lasers:[i]]
										[asteroids:[j]] = [null]
										[lasers:[i]] = [null]
										break
									endif
								endif
							endif
						until [j] = [asteroids:'0']
					endif
					
					if [lasers:[i]] ! [null]
						letdialog [lasers:[i]], 'position', [lasers:[i],[pos_x]] # '|' # [lasers:[i],[pos_y]] # '|' # [gfx_metric] # '|' # [gfx_metric]
					endif
				endif
			endif
		until [i] = [lasers:'0']
	endif


until [event] = 'close_sa'
end
