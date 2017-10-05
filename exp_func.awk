BEGIN			{
					#Structs
					structs[ "vm_stackitem" ] = "rb_param"
					structs[ "vm_val" ] = "rb_val"
					structs[ "vm_var" ] = "rb_var"
					structs[ "vm_stack" ] = "rb_stack"
					structs[ "vm_prog" ] = "rb_prog"
					structs[ "vm_addr" ] = "unsigned long"
					structs[ "srcpos" ] = "void"
					structs[ "srcline" ] = "unsigned long"
					structs[ "char" ] = "char"

					#Functions
					while( ( getline line < "EXPORT.FCT" ) > 0 )
					{
						split( line, parts, ":" )
						fct[ parts[1] ] = parts[2]
					}

					if( 0 && mode != "internal" )
					{
						print "/* Structs */"
						for( s in structs )
						{
							if( s != "vm_addr" &&
								s != "srcline" && 
								s != "srcpos" &&
								s != "char" )
							{
								if( mode == "internal" )
									print "typedef " s " " structs[ s ] ";"
								else
									print "typedef void* " structs[ s ] ";"
							}
						}
					}

					print
					print "#ifndef MAKE_PROTOTYPES"
					print "/* Prototypes */"
				}

END				{
					print "#endif /* MAKE_PROTOTYPES */"
				}

/\/\*.*\*\//	{	#ignore comments
				}

				{
					for( f in fct )
					{
						if( match( $0, f ) > 0 )
						{
							line = $0

							if( mode != "internal" )
							{
								for( s in structs )
								{
									find = "\\<" s 
									gsub( find, structs[ s ], line )
								}
							}

							print line
							if( fct[ f ] != "" )
								print "#define " fct[ f ] " " f
						}
					}
				}
