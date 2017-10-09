/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_util.c
Author:	Jan Max Meyer
Usage:	General usage utility functions
----------------------------------------------------------------------------- */

#include "rb_global.h"

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

