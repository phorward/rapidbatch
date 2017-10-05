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
#include "rb_comp.h"
#ifndef MAKE_PROTOTYPES
#include "rb_proto.h"
#endif

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
					
	Parameters:		uchar***	pool				Pointer to the string pool
					uchar*		insert				The string to be inserted
					boolean		do_copy				Copy 'insert' if it is
													engaged into the pool
	Returns:		uchar*							Returns the pointer to the
													string. (uchar*)NULL will
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
uchar* rb_str_pool_append( uchar*** pool, uchar* insert, boolean do_copy )
{
	uchar**			p;
	unsigned int	cnt;
	
	PROC( "rb_str_pool_append" );
	PARMS( "pool", "%p", pool );
	PARMS( "insert", "%s", insert );
	PARMS( "do_copy", "%d", do_copy );
	
	if( !( pool && insert ) )
	{
		MSG( "pool is null!" );
		RETURN( (uchar*)NULL );
	}
	
	for( p = *pool, cnt = 0; p && *p; p++, cnt++ )
	{
		VARS( "*p", "%s", *p );
		if( !pstrcmp( *p, insert ) )
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

			if( !( *pool = (uchar**)prealloc( (uchar**)*pool,
					( cnt + MALLOC_STEP + 1 ) * sizeof( uchar* ) ) ) )
			{
				MSG( "Can't reserve storage - out of memory?" );
				RETURN( (uchar*)NULL );
			}
			
			p = *pool + cnt;
			
			VARS( "p", "%p", p );
			VARS( "*p", "%p", *p );
			memset( p, 0, ( MALLOC_STEP + 1 ) * sizeof( uchar* ) );
		}
		
		if( do_copy )
			insert = pstrdup( insert );

		*p = insert;
	}
	
	RETURN( *p );
}

