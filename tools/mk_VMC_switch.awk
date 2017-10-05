BEGIN				{	first = 1
					}

/#define[ \t]+VMC_/		{	if( first )
						{
							print "switch()"
							print "{"

							first = 0
						}

						print "\tcase " $2 ":"

						sub( "VMC_", "", $2 )
						#print "\t\tinstr = \"" $2 "\";"

						print "\t\tbreak;\n"
					}

END					{	print "}"
					}
