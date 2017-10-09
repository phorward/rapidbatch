/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_string.c
Author:	Jan Max Meyer
Usage:	Provides functions for rbstring-datatype management and manipulation
----------------------------------------------------------------------------- */

/*
 * Includes
 */
/* #define __WITH_TRACE */
#include "rb_global.h"

/*
 * Global variables
 */

/*
 * Functions
 */
 
/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_str_pool_append()
	
	Author:			Jan Max Meyer
	
	Usage:			Inserts a string into an array (pool) of strings, and (re)-
					allocates memory, if required. If the string is already
					within the pool, it is not inserted, and a pointer to the
					existing string will be returned.
					
	Parameters:		char***	pool				Pointer to the string pool
					char*		insert				The string to be inserted
					pboolean		do_copy				Copy 'insert' if it is
													engaged into the pool
	Returns:		char*							Returns the pointer to the
													string. (char*)NULL will
													be returned on error case.
													
													If do_copy is FALSE, and
													the return value is equal
													to insert, than, in dynami-
													cally allocated strings,
													the pointer must not be
													free'd.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char* rb_str_pool_append( char*** pool, char* insert, pboolean do_copy )
{
	char**			p;
	unsigned int	cnt;
	
	PROC( "rb_str_pool_append" );
	PARMS( "pool", "%p", pool );
	PARMS( "insert", "%s", insert );
	PARMS( "do_copy", "%d", do_copy );
	
	if( !( pool && insert ) )
	{
		MSG( "pool is null!" );
		RETURN( (char*)NULL );
	}
	
	for( p = *pool, cnt = 0; p && *p; p++, cnt++ )
	{
		VARS( "*p", "%s", *p );
		if( !strcmp( *p, insert ) )
			break;
	}
	
	if( !( p && *p ) )
	{
		VARS( "cnt", "%d", cnt );
		if( !( cnt % MALLOC_STEP ) )
		{
			MSG( "reallocation required" );
			VARS( "( cnt + MALLOC_STEP + 1 )", "%d",
					( cnt + MALLOC_STEP + 1 ) );

			if( !( *pool = (char**)prealloc( (char**)*pool,
					( cnt + MALLOC_STEP + 1 ) * sizeof( char* ) ) ) )
			{
				MSG( "Can't reserve storage - out of memory?" );
				RETURN( (char*)NULL );
			}
			
			p = *pool + cnt;
			
			VARS( "p", "%p", p );
			VARS( "*p", "%p", *p );
			memset( p, 0, ( MALLOC_STEP + 1 ) * sizeof( char* ) );
		}
		
		if( do_copy )
			insert = pstrdup( insert );

		*p = insert;
	}
	
	RETURN( *p );
}

