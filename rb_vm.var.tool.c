/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_vm.var.tool.c
Author:	Jan Max Meyer
Usage:	Virtual machine variable access tool functions
		(mainly written for variable manipulation beyond the virtual machine)
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
	Function:		rb_vm_var_resolve()
	
	Author:			Jan Max Meyer
	
	Usage:			Resolves a variable pointer according to an array of
					indexes.
					
	Parameters:		vm_var*		var				Base pointer to a variable
												structure.
					vm_val*		idx				Pointer to begin of index
												array. This must be dynamically
												allocated.
					int			idx_depth		Number/depth of elements in idx.
					boolean		allocate		Performs a variable allocation
												if required.

	Returns:		Returns the vm_var-pointer of the desired index, or
					(vm_var*)NULL if the index does not exists and could not
					be allocated.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
/*SDK_EXT:rb_resolve_var*/
vm_var* rb_vm_var_resolve( vm_var* var, vm_val* idx,
	int idx_depth, boolean allocate )
{
	int		i;

	PROC( "rb_vm_var_resolve" );
	PARMS( "var", "%p", var );
	PARMS( "idx", "%p", idx );
	PARMS( "idx_depth", "%d", idx_depth );
	PARMS( "allocate", "%d", allocate );
	
	if( !( var && idx_depth < 0 ) )
	{
		MSG( "Arguments invalid!" );
		RETURN( (vm_var*)NULL );
	}
	
	for( i = 0; i < idx_depth; i++ )
	{
		VARS( "Getting index i", "%d", i );
		if( !( var = rb_vm_var_get_idx( var, &( idx[i] ), allocate ) ) )
		{
			MSG( "Unable to get and/or allocate" );
			break;
		}
	}

	VARS( "var (is result!)", "%p", var );
	RETURN( var );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_var_dim()
	
	Author:			Jan Max Meyer
	
	Usage:			Dimensions a variable to a desired size, within all its
					dimensions.
					
					
	Parameters:		vm_var*		var				Base pointer to a variable
												structure.
					vm_val*		idx				Pointer to begin of index
												array. This must be dynamically
												allocated.
					int			idx_depth		Number/depth of elements in idx.
					boolean		allocate		Performs a variable allocation
												if required.

	Returns:		Returns the vm_var-pointer of the desired index, or
					(vm_var*)NULL if the index does not exists and could not
					be allocated.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
/*SDK_EXT:rb_dim_var*/
vm_var* rb_vm_var_dim( vm_var* var, vm_val* idx,
	int idx_depth, boolean allocate )
{
	int		i;

	PROC( "rb_vm_var_dim" );
	PARMS( "var", "%p", var );
	PARMS( "idx", "%p", idx );
	PARMS( "idx_depth", "%d", idx_depth );
	PARMS( "allocate", "%d", allocate );
	
	if( !( var && idx_depth < 0 ) )
	{
		MSG( "Arguments invalid!" );
		RETURN( (vm_var*)NULL );
	}
	
	for( i = 0; i < idx_depth; i++ )
	{
		VARS( "Getting index i", "%d", i );
		if( !( var = rb_vm_var_get_idx( var, &( idx[i] ), allocate ) ) )
		{
			MSG( "Unable to get and/or allocate" );
			break;
		}
	}

	VARS( "var (is result!)", "%p", var );
	RETURN( var );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_var_copy()
	
	Author:			Jan Max Meyer
	
	Usage:			Copies an entire variable into another one. The other
					variable will be entierely deleted before. Both target
					and source can be sub-dimensions of other variables...
					
					
	Parameters:		vm_var*		dest			Pointer to target variable.
					vm_var*		src				Pointer to source variable.

	Returns:		boolean						TRUE on success,
												FALSE on error.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
/*SDK_EXT:rb_copy_var*/
boolean rb_vm_var_copy( vm_var* dest, vm_var* src )
{
	vm_addr		i;

	PROC( "rb_vm_var_copy" );
	PARMS( "dest", "%p", dest );
	PARMS( "src", "%p", src );
	
	if( !( dest && src ) )
	{
		MSG( "Nothing to do here" );
		RETURN( FALSE );
	}
	
	rb_vm_var_free( dest );
	
	if( !( dest->key = pstrdup( src->key ) ) )
		RB_OUT_OF_MEMORY;
		
	rb_vm_copy_val( VAR_VAL_STRUCT( dest ),
		VAR_VAL_STRUCT( src ), TRUE );  

	if( !( dest->begin = (vm_var*)pmalloc(
			src->begin_cnt * sizeof( vm_var ) ) ) )
		RB_OUT_OF_MEMORY;

	dest->begin_cnt = src->begin_cnt;

	memset( dest->begin, 0, src->begin_cnt * sizeof( vm_var ) );

	if( src->ht )
	{
		if( !( dest->ht = (vm_addr*)pmemdup( src->ht,
				VM_ASSOC_HASH_SIZE * sizeof( vm_addr ) ) ) )
			RB_OUT_OF_MEMORY;
	}
	
	for( i = 0; i < src->begin_cnt; i++ )
		if( !rb_vm_var_copy( &( dest->begin[ i ] ), &( src->begin[ i ] ) ) )
			RETURN( FALSE );
	
	MSG( "All right here!" );
	RETURN( TRUE );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_make_var()
	
	Author:			Jan Max Meyer
	
	Usage:			Validates and creates a variable from a stack item, if this
					is not a variable type. If rb_vm_make_var() gets a value
					type parameter, it converts this value into a variable and
					assigns it to this variable after conversion.

	Parameters:		vm_stackitem*	item		Pointer to item to be converted

	Returns:		vm_var*						A pointer to the variable
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
/*SDK_EXT:rb_value_to_var*/
vm_var* rb_vm_make_var( vm_stackitem* item )
{
	vm_val	tval;

	PROC( "rb_vm_make_var" );
	PARMS( "item", "%p", item );
	
	if( item->type != ITEM_VAL )
	{
		MSG( "Can't convert, or this is already a variable" );
		RETURN( ITEM_GET_VAR( item ) );
	}
	
	memcpy( &tval, ITEM_VAL_STRUCT( item ), sizeof( vm_val ) );
	
	memset( item, 0, sizeof( vm_stackitem ) );
	
	item->type = ITEM_VAR;
	rb_vm_var_set( ITEM_GET_VAR( item ),
		&tval, DONT_COPY_STRING );
	
	RETURN( ITEM_GET_VAR( item ) );
}
