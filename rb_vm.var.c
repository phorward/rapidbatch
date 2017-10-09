/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_vm.var.c
Author:	Jan Max Meyer
Usage:	Virtual machine variable access
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
	Function:		rb_vm_var_get_idx()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns a variable index, and re-allocates memory, if
					desired.
					
	Parameters:		vm_var*		var				Pointer to variable structure
												in current dimension
					vm_val*		idx				Pointer to vm_val structure,
												that contains the value
					pboolean		alloc			Allocates memory that fits to
												the desired index, if true.					

	Returns:		Returns the vm_var-pointer of the desired index, or
					(vm_var*)NULL if the index does not exists and could not
					be allocated.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
vm_var* rb_vm_var_get_idx( vm_var* var, vm_val* idx, pboolean alloc )
{
	vm_addr		prev	= 0;
	vm_addr		off		= 0;
	char*		key		= (char*)NULL;
	int			hash;

	PROC( "rb_vm_var_get_idx" );
	PARMS( "var", "%p", var );
	PARMS( "idx", "%p", idx );
	PARMS( "alloc", "%d", alloc );

	/* This will make an optimal value */
	rb_vm_optimal_value( idx );
	
	/* Get the index according to the value type */
	if( idx->type == VAL_STR || idx->type == VAL_CSTR )
	{
		MSG( "processing hashed offset of associative value" );

		if( idx->type == VAL_STR )
			key = idx->value.vstr.str;
		else
			key = idx->value.vcstr;
		
		VARS( "key", "%s", key );
		hash = hash_from_str( key, VM_ASSOC_HASH_SIZE );

		if( !( var->ht ) )
		{
			if( !( var->ht = (vm_addr*)pmalloc( VM_ASSOC_HASH_SIZE
						* sizeof( vm_addr ) ) ) )
			{
				MSG( "Out of memory?" );
				RETURN( (vm_var*)NULL );
			}
				
			memset( var->ht, 0, VM_ASSOC_HASH_SIZE * sizeof( vm_addr ) );
		}
		else
		{
			for( off = var->ht[ hash ]; off;
					prev = off, off = var->begin[ off ].next )
			{
				if( strcmp( var->begin[ off - 1 ].key, key ) == 0 )
				{
					MSG( "Found index by key, fine" );
					RETURN( var->begin + off - 1 );
				}
			}
		}
		
		MSG( "Re-allocation required!" );
		off = var->begin_cnt + 1;
	}
	else
	{
		rb_vm_convert_value( idx, VAL_ADDR );
		if( !( off = (vm_addr)idx->value.vaddr ) )
		{
			MSG( "Offset is 0 - return base variable" );
			RETURN( var );
		}
	}

	VARS( "off", "%ld", off );
	VARS( "var->begin_cnt", "%ld", var->begin_cnt );
	
	/* Is memory allocation allowed and required? */
	if( var->begin_cnt < off )
	{
		if( alloc )
		{
			MSG( "Memory allocation required" );
			if( !( var->begin ) )		
				var->begin = (vm_var*)pmalloc( off * sizeof( vm_var ) );
			else
				var->begin = (vm_var*)prealloc( (vm_var*)( var->begin ),
								off * sizeof( vm_var ) );
				
			VARS( "var->begin", "%p", var->begin );
			if( !( var->begin ) )
			{
				MSG( "Ran out of memory?" );
				RETURN( (vm_var*)NULL );
			}
			
			/* Initialize all (new) elements to zero! */
			for( ; var->begin_cnt < off; var->begin_cnt++ )
				memset( &( var->begin[ var->begin_cnt ] ), 0, sizeof( vm_var ) );
	
			/* If key is nonzero, use this as key value */
			if( key )
			{
				if( !( var->begin[ off - 1 ].key = pstrdup( key ) ) )
				{
					MSG( "Out of memory" );
					RETURN( (vm_var*)NULL );
				}
	
				if( prev )
					var->begin[ prev - 1 ].next = off;
				else
					var->ht[ hash ] = off;
			}
		}
		else
			off = 0;
	}
	
	MSG( "Successful return :)" );
	RETURN( off == 0 ? (vm_var*)NULL : var->begin + off - 1 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_var_set()
	
	Author:			Jan Max Meyer
	
	Usage:			Stores a value into an existing variable pointer.
					
	Parameters:		vm_var*			var				Pointer to vm_var structure
					vm_val			value			The value to be used
					pboolean			no_dup			TRUE: No duplication if
														string value
													FALSE: Duplicate string
														values

	Returns:		status							ERR_OK on success, on error
													a status define.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
status rb_vm_var_set( vm_var* var, vm_val* value, pboolean no_dup )
{
	PROC( "rb_vm_var_set" );
	PARMS( "var", "%p", var );
	PARMS( "value", "%p", value );
	PARMS( "no_dup", "%d", no_dup );
	
	if( !var )
	{
		MSG( "Arguments invalid!" );
		RETURN( RB_ERR_PARMS );
	}
	
	rb_vm_copy_val( VAR_VAL_STRUCT( var ), value, !no_dup );

	RETURN( RB_ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_var_get()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns a value from an existing variable pointer.
					
	Parameters:		vm_val*			value			Return pointer for the value
					vm_var*			var				Pointer to vm_var structure
					pboolean			no_dup			TRUE: No duplication if
														string value
													FALSE: Duplicate string
														values

	Returns:		status							ERR_OK on success, on error
													a status define.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
status rb_vm_var_get( vm_val* value, vm_var* var, pboolean no_dup )
{
	PROC( "rb_vm_var_get" );
	PARMS( "var", "%p", var );
	PARMS( "value", "%p", value );
	PARMS( "no_dup", "%d", no_dup );
	
	if( !( var && value ) )
	{
		MSG( "Arguments invalid!" );
		RETURN( RB_ERR_PARMS );
	}
	
	/* rb_vm_var_dump( var, 0, 0 ); */
	rb_vm_copy_val( value, VAR_VAL_STRUCT( var ), !no_dup );
	/* rb_vm_dump_val( VAR_VAL_STRUCT( var ) ); */

	VARS( "Returning pointer", "%p", value );
	RETURN( RB_ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_var_free()
	
	Author:			Jan Max Meyer
	
	Usage:			Frees a variable's memory recursively.					

	Parameters:		vm_var*		var			Variable to be freed

	Returns:		void

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
/*SDK_EXT:pfree_var*/
void rb_vm_var_free( vm_var* var )
{
	vm_addr		i;
	
	PROC( "rb_vm_var_free" );
	PARMS( "var", "%p", var );
	
	if( !var )
	{
		MSG( "Empty parameter, nothing to do" );
		VOIDRET;
	}

	pfree( var->key );
	VAL_FREE( VAR_VAL_STRUCT( var ) );
	pfree( var->ht );
	
	MSG( "Freeing variable sub system" );
	for( i = 0; i < var->begin_cnt; i++ )
		rb_vm_var_free( &( var->begin[ i ] ) );	
	
	pfree( var->begin );
	
	/* Reset the variable */
	memset( var, 0, sizeof( vm_var ) );

	VOIDRET;
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_var_dump()
	
	Author:			Jan Max Meyer
	
	Usage:			Dumps a variable recursively to stderr.
					

	Parameters:		vm_var*		var			Variable to be dumped

	Returns:		void

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
/*SDK_EXT:rb_var_dump*/
void rb_vm_var_dump( vm_var* var, int tabs, vm_addr cnt )
{
	vm_addr		i;
	
	for( i = 0; i < tabs; i++ )
		fprintf( stderr, "  " );
		
	if( tabs > 1 )
		fprintf( stderr, "(%ld) ", cnt );

	fprintf( stderr, "Key: >%s< Value: ", var->key );
	rb_vm_dump_val( stderr, VAR_VAL_STRUCT( var ) );
	
	tabs++;
	for( i = 0; i < var->begin_cnt; i++ )
		rb_vm_var_dump( &( var->begin[i] ), tabs + 1, i );
	tabs--;
}
