/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_vm.error.c
Author:	Jan Max Meyer
Usage:	Virtual machine error messaging
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
 * Defines
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_error()
	
	Author:			Jan Max Meyer
	
	Usage:			Virtual machine runtime error reporting and abort.
					
	Parameters:		char*		msg			Error message identifier

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_vm_error( char* msg )
{
	fprintf( stderr, "\n*** RUNTIME ERROR ***\n%s\n\naborting...", msg );
	exit( 1 );
}
