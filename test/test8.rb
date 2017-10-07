[a] = 1
repeat
	if [a] = 5
		break
	elseif [a] = 9
		cont
	endif
	[a] = [a] + 1
until [a] = 10


