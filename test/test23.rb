[i] = 6
[a:'Hello'] = 'Hello'

func bla: [d]
	print [d]
	ret [d] # [d]
endfunc

[a:bla( 'Hello' )] = bla( 'yeaah' )
print [a:'HelloHello']

