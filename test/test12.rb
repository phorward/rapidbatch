func lolptr: [x]
	[y] = [x:1] # [x:2] # [x:3]
	ret [y]
endfunc
[a:1] = 'Hello'
[a:2] = 'World'
[a:3] = 'Test'
[genial] = lolptr( [&a] )
func lolptr2
	[obst:'Apfel'] = 123
	[obst:'Birne'] = 456
	[obst:'Papaya'] = 789
	[i] = 0
	repeat
		[i] = [i] + 1
		[obst] = [obst] + [obst:[i]]
	until [i] = 3
	ret [&obst]
endfunc
[total_genial] = lolptr2()


