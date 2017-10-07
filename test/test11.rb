func lolptr: [x]
	[y] = [x:1] # [x:2] # [x:3]
	ret [y]
endfunc
[a:1] = 'Hello'
[a:2] = 'World'
[a:3] = 'Test'
[genial] = lolptr( [&a] )



