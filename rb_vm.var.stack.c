/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_vm.stack.c
Author:	Jan Max Meyer
Usage:	Virtual machine variable access and manipulation via the
		virtual machine stack
----------------------------------------------------------------------------- */

/*
 * Includes
 */
/* #define __WITH_TRACE */

#include "rb_global.h"
#include "rb_comp.h"
#include "rb_vm.h"
#include "rb_var.h"
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
	Function:		rb_vm_stack_get_var_base()
	
	Author:			Jan Max Meyer
	
	Usage:			Retrieves a variable base pointer from a stack address.
					If the item is not a variable, then it will be transformed
					into a variable base item!

	Parameters:		vm_stack*		stack		Stack to be used

	Returns:		vm_var*						Pointer to the variable
					

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
vm_var* rb_vm_stack_get_var_base( vm_stack* stack )
{
	vm_stackitem	value;
	vm_stackitem*	var_addr;
	vm_stackitem*	ptr;
	vm_var*			var;
	
	PROC( "rb_vm_stack_get_var_base" );
	PARMS( "stack", "%p", stack );

	var_addr = rb_vm_stack_access( stack, stack->stack_top - 1 );
	ptr = rb_vm_stack_access( stack, ITEM_VAL_GET_ADDR( var_addr ) );
	
	VARS( "ptr->type", "%d", ptr->type );
	switch( ptr->type )
	{
		case ITEM_VAR:
			var = &( ptr->attribute.var );
			break;
			
		case ITEM_PTR:
			var = ptr->attribute.var_ptr;
			break;
			
		default:
			MSG( "Transforming stack item into a variable!" );

			memcpy( &value, ptr, sizeof( vm_stackitem ) );
			memset( ptr, 0, sizeof( vm_stackitem ) );
			ptr->type = ITEM_VAR;
			
			var = &( ptr->attribute.var );
			rb_vm_var_set( var, ITEM_VAL_STRUCT( &value ),
							DONT_COPY_STRING );
	}
	
	RETURN( var );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_stack_var_resolve()
	
	Author:			Jan Max Meyer
	
	Usage:			Retrieves the vm_var-pointer to a variable, according
					to the stacked index and index depth information.

	Parameters:		vm_stack*		stack		Stack to be used
					boolean			allocate	TRUE: Memory will be (re-)allo-
												cated to the desired index, if
												required
												FALSE: Memory will not be re-
												allocated, if memory does not
												exists, the function immedia-
												telly returns (vm_var*)NULL.
					boolean			drop_index	TRUE: Drops all index variables
												FALSE: Doesn't modify the stack

	Returns:		vm_var*						Pointer to the variable
					

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	21.06.2009	Jan Max Meyer	Added feature of getting array sizes by
								accessing element zero.
----------------------------------------------------------------------------- */
vm_var* rb_vm_stack_var_resolve( vm_stack* stack,
	boolean allocate, boolean* get_size )
{
	int				i;
	vm_var*			var;
	vm_var*			prev_var;
	vm_stackitem*	ptr;
	vm_stackitem*	count;
	boolean			set_size	= FALSE;
	
	PROC( "rb_vm_stack_var_resolve" );
	PARMS( "stack", "%p", stack );
	PARMS( "allocate", "%d", allocate );
	PARMS( "get_size", "%p", get_size );
		
	/* Get variable base pointer */
	prev_var = var = rb_vm_stack_get_var_base( stack );
	
	/* Get depth counter */
	count = rb_vm_stack_access( stack, stack->stack_top - 2 );
	
	/* Get pointer to variable by resolving all indexes */
	for( i = ITEM_VAL_GET_LONG( count ); i > 0; i-- )
	{
		ptr = rb_vm_stack_access( stack, stack->stack_top - 2 - i );
 		if( !( var = rb_vm_var_get_idx( prev_var,
 			ITEM_VAL_STRUCT( ptr ), allocate ) ) )
 		{
 			MSG( "Can't get variable" );
 			break;
 		}
 		
 		/*
 		VARS( "get_size", "%p", get_size );
 		VARS( "*get_size", "%d", get_size ? *get_size : 0 );
 		VARS( "prev_var", "%p", prev_var );
 		VARS( "var", "%p", var );
 		*/
 		
 		/* If the function returned the same variable as the
 			previous, a dimension size request was made */
 		if( prev_var == var )
 		{
			if( get_size && *get_size )
			{
				MSG( "Size request - stopping variable resolvement loop" );
				set_size = TRUE;
				break;
				/*
				if( rb_vm_optimal_value( ITEM_VAL_STRUCT( ptr ) )
				{
					if( ITEM_VAL_GET_ADDR( ptr ) == 0 )
					{
						set_size = TRUE;
						break;
					}
				}
				*/
			}
			else
			{
				rb_vm_error( "array_index_of_null" );
			}
 		}
 		
 		prev_var = var;
	}
	
	/* Drop used items off the stack and free memory */
	MSG( "Dropping stack elements" );
	rb_vm_stack_drop( stack, ITEM_VAL_GET_LONG( count ) + 2 );
	
	/* Set size flag? */
	if( get_size )
		*get_size = set_size;
	
	RETURN( var );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_stack_init_var()
	
	Author:			Jan Max Meyer
	
	Usage:			Initializes a variable structure to a fixed dimension depth.

	Parameters:		vm_stack*		stack		Stack to be used.
					vm_var*			var			Pointer to variable; Only used
												by rb_vm_stack_init_var() it-
												self in recursive call.
												Should be (vm_var*)NULL for
												the initial call.
					int				max			Maximum depth of the index
												values that resist on the
												stack. This must be -1 for the
												initial call.
					int				depth		The current recursion depth;
												only for the recursion.

	Returns:		boolean						TRUE: On success,
												FALSE: else
					

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
boolean rb_vm_stack_init_var( vm_stack* stack, vm_var* var, int max, int depth )
{
	vm_addr			i		= TRUE;
	vm_stackitem*	index;

	PROC( "rb_vm_stack_init_var" );
	PARMS( "stack", "%p", stack );
	PARMS( "var", "%p", var );
	PARMS( "max", "%d", max );
	PARMS( "depth", "%d", depth );
	
	if( max < 0 )
	{
		MSG( "INIT" );
		/* Get variable base pointer */
		var = rb_vm_stack_get_var_base( stack );
	
		/* Get depth counter */
		index = rb_vm_stack_access( stack, stack->stack_top - 2 );
		
		/* Begin of recursion! */
		i = rb_vm_stack_init_var( stack, var,
			(int)ITEM_VAL_GET_LONG( index ), 0 );
	
		/* Drop used items off the stack and free memory */
		rb_vm_stack_drop( stack, ITEM_VAL_GET_LONG( index ) + 2 );
	}
	else if( depth < max )
	{
		index = rb_vm_stack_access( stack,
			stack->stack_top - 2 - max + depth );

		if( !rb_vm_var_get_idx( var, &( index->attribute.val ), TRUE ) )
		{
			MSG( "Allocation error?" );
			RETURN( FALSE );
		}
		
		VARS( "depth", "%d", depth );
		VARS( "max", "%d", max );

		MSG( "Allocate yet another dimension!" );
		VARS( "var->begin_cnt", "%ld", var->begin_cnt );
		for( i = 0; i < var->begin_cnt; i++ )
		{
			VARS( "i", "%ld", i );
			if( !rb_vm_stack_init_var( stack,
					var->begin + i, max, depth + 1 ) )
			{
				MSG( "Abort, has an error occured?" );
				RETURN( FALSE );
			}
		}
	}
	
	RETURN( i );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_stack_var_push()
	
	Author:			Jan Max Meyer
	
	Usage:			Pushes a variable.					

	Parameters:		vm_stack*		stack		Stack to be used
					vm_var*			var			Variable value to be pushed

	Returns:		status						ERR_OK on success,
												else one of the error defines.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
status rb_vm_stack_var_push( vm_stack* stack, vm_var* var )
{
	vm_stackitem	val;

	PROC( "rb_vm_stack_var_push" );
	PARMS( "stack", "%p", stack );
	PARMS( "var", "%p", var );
	
	if( !( stack ) )
		RETURN( RB_ERR_PARMS );
		
	memset( &val, 0, sizeof( vm_stackitem ) );
	val.type = ITEM_VAL;

	if( var ) 
	{
		MSG( "Var exists, reading content" );
		if( rb_vm_var_get( ITEM_VAL_STRUCT( &val ),
				var, FALSE ) == RB_ERR_OK )
		{
			if( rb_vm_stack_push( stack, &val ) )
				RETURN( RB_ERR_OK );
		}
	}
	else
	{
		MSG( "Variable does not exist, pushing empty string" );
		ITEM_VAL_SET_CSTR( &val, (char*)NULL );
		
		if( rb_vm_stack_push( stack, &val ) )
			RETURN( RB_ERR_OK );
	}
	
	RETURN( RB_ERR_FAILURE );
}
