/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	file.c
Author:	Jan Max Meyer
Usage:	File access tools
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "rb_global.h"
#include "rb_native.h"

/*
 * Global variables
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_read_file()

	Author:			Jan Max Meyer

	Usage:			Maps the content of a file into memory.

	Parameters:		char**		cont			The file content, return
												pointer.
					char*		filename		Path to file to be mapped

	Returns:		int			0		On success
								1				If the file could not be found
								RB_ERR...		RB_ERR-define according to its
												meaning

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int rb_read_file( char** cont, char* filename )
{
	FILE*	f;
	char*	c;

	PROC( "rb_read_file" );
	PARMS( "cont", "%p", cont );
	PARMS( "filename", "%s", filename );

	/* Check parameters */
	if( !( cont && filename && *filename ) )
	{
		MSG( "Incomplete parameters!" );
		RETURN( -1 );
	}

	/* Open file */
	if( !( f = fopen( filename, "rb" ) ) )
	{
		MSG( "File could not be opened - wrong path?" );
		RETURN( 1 );
	}

	/* Allocate memory for file */
	fseek( f, 0L, SEEK_END );
	if( !( c = *cont = (char*)pmalloc( ( ftell( f ) + 1 )
			* sizeof( char ) ) ) )
	{
		MSG( "Unable to allocate required memory" );

		fclose( f );
		RETURN( -1 );
	}

	/* Read entire file into buffer */
	fseek( f, 0L, SEEK_SET );

	while( !feof( f ) )
		*(c++) = fgetc( f );

	/* Case: File is empty */
	if( c == *cont )
		c++;

	*( c - 1 ) = '\0';

	fclose( f );

	VARS( "Entire file", "%s", *cont );
	MSG( "All right!" );

	RETURN( 0 );
}

