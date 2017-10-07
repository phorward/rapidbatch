func fac: [x]
	if [x] = 0
		ret 1
	else
		ret [x] * fac( [x]-1 )
	endif
endfunc
fac [a] = 6
[fak] = 1
[i] = 1 
repeat
	[fak] = [fak] * [i]
	[i] = [i] + 1
until [i] = 9


