func bla
	[a:'1'] = 'Hello'
	[a:2]  = 'World'
	[a]='Testen'
	ret [&a]
endfunc

[x] = bla()
[y] = bla()


