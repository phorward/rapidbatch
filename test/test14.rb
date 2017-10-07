[str]  = 'Hello World'
[p] = getlen( [str] )
print 'Die länge von "' # [str] # '" beträgt ' # [p] # ' Zeichen!'
print 'Und das Zeichen an Offset 4 ist "' # getcharat( [str], 4 ) # '"'

[i] = 0
repeat
	[i] = [i] + 1
	print 'Zeichen an Offset ' # [i] # ' ist "' # getcharat( [str], [i] ) # '"'
until [i] = getlen( [str] )


