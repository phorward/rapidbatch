/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_util.c
Author:	Jan Max Meyer
Usage:	General usage utility functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
/* #define __WITH_TRACE 1 */
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
	Function:		memdup()
	
	Author:			Jan Max Meyer
	
	Usage:			Duplicates a memory entry onto the heap.
					
	Parameters:		void*		ptr				Pointer to memory
					size_t		size			Size of pointer
	
	Returns:		void*						Pointer to memory copy.
												Cast this with your type!
												Returns (void*)NULL on error!
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void* rb_memdup( void* ptr, size_t size )
{
	void*	ret;
	
	if( !( ptr || size ) )
		return (void*)NULL;
		
	ret = pmalloc( size );
	memcpy( ret, ptr, size );
		
	return ret;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		hash_from_str()
	
	Author:			Jan Max Meyer
	
	Usage:			Duplicates a memory entry onto the heap.
					
	Parameters:		char*		str				String value to be hashed
					int			size			Hash table size
	
	Returns:		int							The hashed index value
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int hash_from_str( char* str, int size )
{
	int		v = 0;
	char*	p;

	for( p = str; *str; str++ )
		v += (int)*str;

	return v % size;
}

