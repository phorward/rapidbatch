BEGIN				{
						on_export = 0
					}

/\/\*ENDEXPORT\*\//	{
						on_export = 0
					}

					{
						if( on_export )
							print $0
					}

/\/\*EXPORT/		{
						line = $0
						gsub( /\/\*/, "", line );
						gsub( /\*\//, "", line );
						split( line, ex, ":" )

						if( ex[2] == mode || ex[2] == "" )
						{
							print ""
							if( ex[3] != "" )
								print "/* " ex[3] " */"

							on_export = 1
						}
					}

