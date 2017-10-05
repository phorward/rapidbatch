/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_vm.native.c
Author:	Jan Max Meyer
Usage:	Virtual machine utility functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
/* #define __WITH_TRACE */
#include "rb_global.h"
#include "rb_comp.h"
#include "rb_vm.h"

#ifndef MAKE_PROTOTYPES
#include "rb_proto.h"
#endif

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
	Function:		rb_vmwrap_GET_PARAM()
	
	Author:			Jan Max Meyer
	
	Usage:			Gets a stack item relatively from another stackitem;
					Wrapper for native function SDK.
					
	Parameters:		vm_stackitem*	base		Pointer to base item
					int				offset		Offset relatively to base to get
					int				max			Maximum offset; If < 0, offset
												can be negative.

	Returns:		vm_stackitem*				The stack item
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
vm_stackitem* rb_vmwrap_GET_PARAM( vm_stackitem* base, int offset, int max ) /*SDK_EXT*/
{
	PROC( "rb_vmwrap_GET_PARAM" );
	PARMS( "base", "%p", base );
	PARMS( "offset", "%d", offset );
	PARMS( "max", "%d", max );
	
	if( !( base ) )
	{
		MSG( "Base is NULL!" );
		RETURN( (vm_stackitem*)NULL );
	}
	
	if( offset >= max )
	{
		MSG( "Offset out of bounds" );
		RETURN( (vm_stackitem*)NULL );
	}
	
	MSG( "Valid values, returning pointer" );
	RETURN( base + offset );
}

/* Macro replacement wrappers */
/* vm_stackitem-related */
vm_var* rb_vmwrap_ITEM_GET_VAR( vm_stackitem* parm ) /*SDK_EXT*/
{
	PROC( "rb_vmwrap_ITEM_GET_VAR" );
	PARMS( "parm", "%p", parm );
	
	RETURN( ITEM_GET_VAR( parm ) );
}

vm_val* rb_vmwrap_ITEM_GET_VAL( vm_stackitem* parm ) /*SDK_EXT*/
{
	PROC( "rb_vmwrap_ITEM_GET_VAL" );
	PARMS( "parm", "%p", parm );
	
	RETURN( ITEM_GET_VAL( parm ) );
}

/* vm_val-related */
void rb_vmwrap_VAL_SET_ADDR( vm_val* val, vm_addr v ) /*SDK_EXT*/
{
	RB_VAL_SET_ADDR( val, v );
}

void rb_vmwrap_VAL_SET_LONG( vm_val* val, long v ) /*SDK_EXT*/
{
	RB_VAL_SET_LONG( val, v );
}

void rb_vmwrap_VAL_SET_DBL( vm_val* val, double v ) /*SDK_EXT*/
{
	RB_VAL_SET_DBL( val, v );
}

void rb_vmwrap_VAL_SET_CSTR( vm_val* val, char* v ) /*SDK_EXT*/
{
	RB_VAL_SET_CSTR( val, v );
}

void rb_vmwrap_VAL_SET_STR( vm_val* val, char* v ) /*SDK_EXT*/
{
	RB_VAL_SET_STR( val, v );
}

uchar rb_vmwrap_VAL_TYPE( vm_val* val ) /*SDK_EXT*/
{
	return RB_VAL_TYPE( val );
}

/* vm_var-related */
vm_val* rb_vmwrap_VAR_VAL_STRUCT( vm_var* var ) /*SDK_EXT*/
{
	return RB_VAR_VAL_STRUCT( var );
}

uchar rb_vmwrap_VAR_VAL_TYPE( vm_var* var ) /*SDK_EXT*/
{
	return RB_VAR_VAL_TYPE( var );
}
