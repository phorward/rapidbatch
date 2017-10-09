/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_run.c
Author:	Jan Max Meyer
Usage:	Virtual machine kernel
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "rb_global.h"

/*
 * Global variables
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_get_dbl_val()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the double value of a vm_val-structure, and
					converts it, if desired.
					
	Parameters:		vm_val*		val				Pointer to the value structure
					pboolean		convert			TRUE: Convert if required
												FALSE: Don't convert

	Returns:		Returns the double value. If no conversation is done,
					and the value is not of type double, 0.0 is returned.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
double rb_vm_get_dbl_val( vm_val* val, pboolean convert ) /*SDK_EXT*/
{
	if( val->type != VAL_DBL )
	{
		if( convert )
			rb_vm_convert_value( val, VAL_DBL );
		else
			return 0.0;
	}
	
	return val->value.vdbl;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_get_long_val()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the long value of a vm_val-structure, and converts
					it, if desired.
					
	Parameters:		vm_val*		val				Pointer to the value structure
					pboolean		convert			TRUE: Convert if required
												FALSE: Don't convert

	Returns:		Returns the long value. If no conversation is done,
					and the value is not of type long, 0L is returned.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
long rb_vm_get_long_val( vm_val* val, pboolean convert ) /*SDK_EXT*/
{
	if( val->type != VAL_LONG )
	{
		if( convert )		
			rb_vm_convert_value( val, VAL_LONG );
		else
			return 0L;
	}
	
	return val->value.vlong;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_get_addr_val()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the long value of a vm_val-structure, and converts
					it, if desired.
					
	Parameters:		vm_val*		val				Pointer to the value structure
					pboolean		convert			TRUE: Convert if required
												FALSE: Don't convert

	Returns:		Returns the vm_addr value. If no conversation is done,
					and the value is not of type vm_addr, 0L is returned.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
vm_addr rb_vm_get_addr_val( vm_val* val, pboolean convert ) /*SDK_EXT*/
{
	if( val->type != VAL_ADDR )
	{
		if( convert )		
			rb_vm_convert_value( val, VAL_ADDR );
		else
			return 0L;
	}
	
	return val->value.vaddr;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_get_str_val()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the string value of a vm_val-structure, and converts
					it, if desired.
					
	Parameters:		vm_val*		val				Pointer to the value structure
					pboolean		convert			TRUE: Convert if required
												FALSE: Don't convert

	Returns:		Returns the string value. If no conversation is done,
					and the value is not of type string, (char*)NULL is
					returned.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char* rb_vm_get_str_val( vm_val* val, pboolean convert ) /*SDK_EXT*/
{
	if( val->type != VAL_CSTR
		&& val->type != VAL_STR )
	{
		if( convert )
			rb_vm_convert_value( val, VAL_STR );
		else
			return (char*)NULL;
	}
	
	if( val->type == VAL_CSTR )
		return val->value.vcstr;
	
	return val->value.vstr.str;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_free_val()
	
	Author:			Jan Max Meyer
	
	Usage:			Frees the possibly used memory of a value.
					
	Parameters:		vm_val*		val				Pointer to the value structure

	Returns:		void
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_vm_free_val( vm_val* val ) /*SDK_EXT*/
{
	if( VAL_TYPE( val ) == VAL_STR )
		pfree( val->value.vstr.str );
	
	VAL_TYPE( val ) = VAL_UNDEFINED;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_copy_val()
	
	Author:			Jan Max Meyer
	
	Usage:			Copies the possibly used memory of a value.
					
	Parameters:		vm_val*		dst				Pointer to the destination
													value structure
					vm_val*		src				Pointer to the source value
													structure
	Returns:		pboolean						TRUE on success,
												FALSE on error
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean rb_vm_copy_val( vm_val* dst, vm_val* src, pboolean withdup ) /*SDK_EXT*/
{
	PROC( "rb_vm_copy_val" );
	PARMS( "dst", "%p", dst );
	PARMS( "src", "%p", src );
	PARMS( "withdup", "%d", withdup );
	
	if( VAL_TYPE( dst ) == VAL_STR )
		pfree( dst->value.vstr.str );
	
	memcpy( dst, src, sizeof( vm_val ) );
	if( withdup && VAL_TYPE( src ) == VAL_STR )
	{
		VAL_SET_STR( dst, pstrdup( VAL_GET_STR( src ) ) );
		if( VAL_GET_STR( src ) && !dst )
			RETURN( FALSE );
	}
	
	RETURN( TRUE );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_dump_val()
	
	Author:			Jan Max Meyer
	
	Usage:			Dumps a value to stderr.
					
	Parameters:		vm_val*		val				The value to be dumped
	Returns:		void
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_vm_dump_val( FILE* stream, vm_val* val ) /*SDK_EXT*/
{
	if( !stream )
		stream = stderr;
		
	if( !val )
	{
		fprintf( stream, "(val is NULL)\n" );
		return;
	}

	switch( val->type )
	{
		case VAL_LONG:
			fprintf( stream, "(long) %ld",
				val->value.vlong );
			break;
		case VAL_ADDR:
			fprintf( stream, "(address) %lx",
				val->value.vaddr );
			break;
		case VAL_DBL:
			fprintf( stream, "(double) %lf",
				val->value.vdbl );
			break;
		case VAL_STR:
			fprintf( stream, "(dynamic string) >%s<",
				val->value.vstr.str );
			break;
		case VAL_CSTR:
			fprintf( stream, "(constant string) >%s<",
				val->value.vcstr );
			break;
	
		case VAL_UNDEFINED:
		default:
			fprintf( stream, "(undefined) %d",
				val->type );
	}
	
	fprintf( stream, "\n" );
}
