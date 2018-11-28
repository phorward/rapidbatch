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
 * Defines
 */
		
#define RB_VM_STACK_DUMP( stack ) rb_vm_stack_dump( __FILE__, __LINE__, (stack) )

/*
 * Functions
 */
  
/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_stack_push()
	
	Author:			Jan Max Meyer
	
	Usage:			Push a stackitem onto the stack. If required, (re-)allocation
					is done.
					
	Parameters:		vm_stack*		stack			Stack to be used
					vm_stackitem*	item			Item to be stacked			

	Returns:		pboolean							TRUE: Item was successfully
															pushed
													FALSE: If not ;)
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean rb_vm_stack_push( vm_stack* stack, vm_stackitem* item )
{	
	PROC( "rb_vm_stack_push" );
	PARMS( "stack", "%p", stack );
	PARMS( "item", "%p", item );
	
	VARS( "stack->stack_cnt", "%d", stack->stack_cnt );
	VARS( "stack->stack_top", "%d", stack->stack_top );
	
	/* Is memory (re-)allocation required? */
	if( stack->stack_cnt == 0 )
	{
		MSG( "Requiring initial allocation of stack" );
		VARS( "allocating bytes", "%ld",
				MALLOC_STEP * sizeof( vm_stackitem ) );

		stack->stack = (vm_stackitem*)pmalloc(
							MALLOC_STEP * sizeof( vm_stackitem ) );		
		if( !( stack->stack ) )
			RETURN( FALSE );
			
		stack->stack_cnt = MALLOC_STEP;
		stack->stack_top = 0;
	}
	else if( stack->stack_top + 1 == stack->stack_cnt )
	{
		MSG( "Requiring reallocation of stack" );
		VARS( "reallocating bytes", "%ld",
				( stack->stack_cnt + MALLOC_STEP )
					* sizeof( vm_stackitem ) );
					
		stack->stack_cnt += MALLOC_STEP;
		stack->stack = (vm_stackitem*)prealloc( (void*)stack->stack,
							stack->stack_cnt * sizeof( vm_stackitem ) );
		if( !( stack->stack ) )
			RETURN( FALSE );
	}
	
	/* Get the top of stack */
	memcpy( &( stack->stack[ stack->stack_top++ ] ),
				item, sizeof( vm_stackitem ) );
	
	RETURN( TRUE );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_stack_pop()
	
	Author:			Jan Max Meyer
	
	Usage:			Pops an vm_stackitem off the stack.
					The item is not freed, and must be freed manually!
					

	Parameters:		vm_stack*		stack		Stack to be used
					vm_stackitem*	item		Pointer to item, where the
												item is copied to.

	Returns:		pboolean						TRUE: Item was popped
												FALSE: No more items stacked

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean rb_vm_stack_pop( vm_stack* stack, vm_stackitem* item )
{
	PROC( "rb_vm_stack_pop" );
	PARMS( "stack", "%p", stack );
	PARMS( "item", "%p", item );
	
	if( stack->stack_top <= 0 )
		RETURN( FALSE );
	
	memcpy( item, &( stack->stack[ --stack->stack_top ] ),
		sizeof( vm_stackitem ) );
	
	RETURN( TRUE );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_stack_access()
	
	Author:			Jan Max Meyer
	
	Usage:			Access a stack item.					

	Parameters:		vm_stack*		stack		Stack to be used
					vm_addr			offset		Offset of the stack item

	Returns:		vm_stackitem*				Address to the stack item

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
vm_stackitem* rb_vm_stack_access( vm_stack* stack, vm_addr offset )
{
	PROC( "rb_vm_stack_access" );
	PARMS( "stack", "%p", stack );
	PARMS( "offset", "%ld", offset );
	
	if( stack->stack_top <= 0 || offset >= stack->stack_top )
		RETURN( (vm_stackitem*)NULL );
	
	RETURN( &( stack->stack[ offset ] ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_stack_put()
	
	Author:			Jan Max Meyer
	
	Usage:			Puts a stack item into any stack address.

	Parameters:		vm_stack*		stack		Stack to be used
					vm_stackitem*	item		Pointer to item, where the
												item is copied to
					vm_addr			offset		Offset of the stack item

	Returns:		pboolean						TRUE: Item was popped
												FALSE: No more items stacked

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean rb_vm_stack_put( vm_stack* stack, vm_stackitem* item, vm_addr offset )
{
	PROC( "rb_vm_stack_put" );
	PARMS( "stack", "%p", stack );
	PARMS( "item", "%p", item );
	PARMS( "offset", "%ld", offset );
	
	if( stack->stack_top <= 0 || offset >= stack->stack_top )
		RETURN( FALSE );
	
	memcpy( stack->stack + offset, item, sizeof( vm_stackitem ) );
	RETURN( TRUE );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_stack_drop()
	
	Author:			Jan Max Meyer
	
	Usage:			Pops and drops stack elements and (eventually) frees
					their memory.

	Parameters:		vm_stack*		stack		Stack to be used
					vm_addr			count		Number of elements.

	Returns:		pboolean						TRUE: Item was popped
												FALSE: No more items stacked

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_vm_stack_drop( vm_stack* stack, vm_addr count )
{
	vm_stackitem	drop;

	PROC( "rb_vm_stack_drop" );
	PARMS( "stack", "%p", stack );
	PARMS( "count", "%d", count );

	for( ; count > 0; count-- )
	{
		VARS( "count", "%d", count );
		VARS( "stack->stack_top", "%ld", stack->stack_top );
		if( stack->stack_top <= 0 )
			break;

		memcpy( &drop, &( stack->stack[ --stack->stack_top ] ),
			sizeof( vm_stackitem ) );

		rb_vm_item_free( &drop );
	}

	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_item_free()
	
	Author:			Jan Max Meyer
	
	Usage:			Frees a stackitem, according to its type and content.

	Parameters:		vm_stack*		stack		Stack to be used
					int				count		Number of elements. If this
												is <0, all elements are popped
												and dropped.

	Returns:		pboolean						TRUE: Item was popped
												FALSE: No more items stacked

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_vm_item_free( vm_stackitem* item )
{
	PROC( "rb_vm_item_free" );
	PARMS( "item", "%p", item );
	
	VARS( "item->type", "%d", item->type );
	switch( item->type )
	{
		case ITEM_VAL:
			MSG( "Is this of type string?" );
			
			VAL_FREE( ITEM_VAL_STRUCT( item ) );
			break;
			
		case ITEM_VAR:
			MSG( "Freeing variable memory" );
			rb_vm_var_free( &( item->attribute.var ) );
			break;
			
		default:
			MSG( "Do nothing" );
			break;
	}

	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_item_memdup()
	
	Author:			Jan Max Meyer
	
	Usage:			Makes a stack item entirely independent from (possibly
					removed) memory, by copying all dynamically allocated
					memory.

	Parameters:		vm_stack*		stack		Stack to be used
					int				count		Number of elements. If this
												is <0, all elements are popped
												and dropped.

	Returns:		pboolean						TRUE: Item was popped
												FALSE: No more items stacked

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_vm_item_memdup( vm_stackitem* item )
{
	vm_var*		var;

	PROC( "rb_vm_item_free" );
	PARMS( "item", "%p", item );
	
	VARS( "item->type", "%d", item->type );
	switch( item->type )
	{
		case ITEM_VAL:
			MSG( "Is this of type constant string?" );
			
			if( item->attribute.val.type == VAL_CSTR )
			{
				ITEM_VAL_SET_STR( item,
					pstrdup( ITEM_VAL_GET_STR( item ) ) );
			}
			break;
			
		case ITEM_PTR:
			MSG( "Making real variable!" );
			var = item->attribute.var_ptr;
			
			memset( item, 0, sizeof( vm_stackitem ) );
			item->type = ITEM_VAR;
			
			rb_vm_var_copy( &( item->attribute.var ), var );
			break;
			
		default:
			MSG( "Do nothing" );
			break;
	}

	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_stack_dump()
	
	Author:			Jan Max Meyer
	
	Usage:			Dumps the stack, for debug issues.
					

	Parameters:		vm_stack*		stack		Stack to be used

	Returns:		void

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_vm_stack_dump( char* file, int line, vm_stack* stack )
{
	int				i;
	vm_stackitem*	item;
	
 	fprintf( stderr, "--- Dump of Stack %p in %s, line %d ---\n",
 				stack, file, line );

	for( i = 0; i < stack->stack_top; i++ )
	{
		item = &( stack->stack[ i ] );
		fprintf( stderr, "%03d ", i );
		
		if( item->type == ITEM_VAL )
		{
			fprintf( stderr, "Value " );
			rb_vm_dump_val( stderr, ITEM_VAL_STRUCT( item ) );
		}
		else if( item->type == ITEM_VAR )
		{
			fprintf( stderr, "Variable\n" );
			rb_vm_var_dump( &( item->attribute.var ), 1, 0 );
		}
		else if( item->type == ITEM_PTR )
		{
			fprintf( stderr, "Pointer to variable %p",
				item->attribute.var_ptr );
			rb_vm_var_dump( item->attribute.var_ptr, 1, 0 );
		}
		else
		{
			fprintf( stderr, "[UNDEFINED]\n" );
		}
	}

	fprintf( stderr, "\n--- %ld active, %ld left, End of Dump ---\n",
		stack->stack_top, stack->stack_cnt - stack->stack_top );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_nice_type()
	
	Author:			Jan Max Meyer
	
	Usage:			Evaluates the most optimal type for two operands; This is
					required for greater-lower comparisons.
					
	Parameters:		vm_stackitem*				first
					vm_stackitem*				last

	Returns:		byte						Type to convert with
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int rb_vm_nice_type( vm_stackitem* first, vm_stackitem* last )
{
	byte		t[2];
	
	PROC( "rb_vm_nice_type" );
	PARMS( "first", "%p", first );
	PARMS( "last", "%p", last );
	
	if( ( t[0] = ITEM_VAL_TYPE( first ) ) == VAL_UNDEFINED
		|| ( t[1] = ITEM_VAL_TYPE( last ) ) == VAL_UNDEFINED )
	{
		MSG( "Invalid parameters!" );
		RETURN( VAL_UNDEFINED );
	}
	
	VARS( "t[0]", "%d", t[0] );
	VARS( "t[1]", "%d", t[1] );
	
	if( t[0] == t[1] )
	{
		MSG( "Same types, nothing to do here!" );
		RETURN( t[0] );
	}
	
	rb_vm_optimal_value( ITEM_GET_VAL( first ) );
	rb_vm_optimal_value( ITEM_GET_VAL( last ) );
	
	if( ( t[0] = ITEM_VAL_TYPE( first ) ) == 
		( t[1] = ITEM_VAL_TYPE( last ) ) )
	{
		MSG( "Same types now, nothing to do here!" );
		RETURN( t[0] );
	}
	else if( t[0] & VAL_STR || t[1] & VAL_STR )
	{
		rb_vm_convert_value( ITEM_GET_VAL( first ), VAL_STR );
		rb_vm_convert_value( ITEM_GET_VAL( last ), VAL_STR );
		
		RETURN( VAL_STR );
	}

	rb_vm_convert_value( ITEM_GET_VAL( first ), VAL_DBL );
	rb_vm_convert_value( ITEM_GET_VAL( last ), VAL_DBL );
	RETURN( VAL_DBL );
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_run()
	
	Author:			Jan Max Meyer
	
	Usage:			Run a compiled VM program.
					
	Parameters:		vm_prog						Pointer to VM program to
												be executed

	Returns:		int							Return state
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int rb_vm_run( vm_stack* stack, vm_code* code, vm_addr code_cnt )
{
#define STACK			stack
#define BEGIN			code
#define END				( code + code_cnt )
#define FP				fp
#define CP				cp

#define PARM( t )		*( (t*)( CP+1 ) )
#define CP_SKIP_PARM	CP += rb_vm_get_parm_size( *CP )
#define TOS				( stack->stack_top - 1 )

	vm_code*		cp;			/* Code pointer */
	vm_addr			fp;			/* Frame pointer */

	vm_addr			i;
	char			skip;
	vm_stackitem	item;
	vm_stackitem	hold;
	vm_stackitem	op[2];
	
	rb_native_func	native_fct;
	rb_native_var	native_var;

	PROC( "rb_vm_run" );

	/* Initialize stack */
	MSG( "Initializing stack memory" );
	rb_vm_stack_init( STACK );
	rb_vm_stack_push( STACK, &item ); /* Junk push allocates memory! */
	rb_vm_stack_pop( STACK, &item );
	
	MSG( "Begin of execution loop" );
	/* Program execution loop */
	for( CP = BEGIN; CP < END; CP++ )
	{		
		MSG( "--" );
		VARS( "executing CP", "%p", CP - BEGIN );
		/*
		fprintf( stderr, "CP = %p --------------------------------------------\n",
			(void*)( CP - BEGIN ) );
		RB_VM_STACK_DUMP( STACK );
		*/
		
		/* Operational commands taking two arguments */
		if( *CP & (CCLASS_ONE_OPERAND) )
		{
			MSG( "Taking first argument" );
			rb_vm_stack_pop( STACK, &op[0] );
			
			if( *CP & (CCLASS_TWO_OPERAND) )
			{
				MSG( "Getting secondary operand" );
				rb_vm_stack_pop( STACK, &op[1] );
			}

			switch( *CP )
			{
				case VMC_JOIN:
					ITEM_VAL_SET_STR( &item,  pstrcatstr(
								pstrdup( ITEM_VAL_GET_STR( &( op[1] ) ) ),
									ITEM_VAL_GET_STR( &( op[0] ) ),
										FALSE ) );
					break;
				
				case VMC_EQU:
					if( strcmp( pstrget( ITEM_VAL_GET_STR( &op[0] ) ),
					        pstrget( ITEM_VAL_GET_STR( &op[1] ) ) ) == 0 )
						ITEM_VAL_SET_LONG( &item, RB_TRUE );
					else
						ITEM_VAL_SET_LONG( &item, RB_FALSE );

					break;
			
				case VMC_NEQ:
					if( strcmp( ITEM_VAL_GET_STR( &op[0] ),
							ITEM_VAL_GET_STR( &op[1] ) ) == 0 )
						ITEM_VAL_SET_LONG( &item, RB_FALSE );
					else
						ITEM_VAL_SET_LONG( &item, RB_TRUE );

					break;
			
				case VMC_GRT:
				case VMC_LWR:
				case VMC_GRE:
				case VMC_LWE:
					MSG( "Validate the best type for comparison" );
					switch( rb_vm_nice_type( &op[0], &op[1] ) )
					{
						case VAL_UNDEFINED:
							MSG( "Error occured!" );							
							break;

						case VAL_STR:
						case VAL_CSTR:						
							MSG( "Performing string compare" );
							switch( *CP )
							{
								case VMC_GRT:
									if( strcmp( ITEM_VAL_GET_STR( &op[0] ),
										ITEM_VAL_GET_STR( &op[1] ) ) < 0 )
										ITEM_VAL_SET_LONG( &item, RB_TRUE );
									else
										ITEM_VAL_SET_LONG( &item, RB_FALSE );
									break;
								case VMC_LWR:
									if( strcmp( ITEM_VAL_GET_STR( &op[0] ),
										ITEM_VAL_GET_STR( &op[1] ) ) > 0 )
										ITEM_VAL_SET_LONG( &item, RB_TRUE );
									else
										ITEM_VAL_SET_LONG( &item, RB_FALSE );
									break;
								case VMC_GRE:
									if( strcmp( ITEM_VAL_GET_STR( &op[0] ),
										ITEM_VAL_GET_STR( &op[1] ) ) <= 0 )
										ITEM_VAL_SET_LONG( &item, RB_TRUE );
									else
										ITEM_VAL_SET_LONG( &item, RB_FALSE );
									break;
								case VMC_LWE:
									if( strcmp( ITEM_VAL_GET_STR( &op[0] ),
										ITEM_VAL_GET_STR( &op[1] ) ) >= 0 )
										ITEM_VAL_SET_LONG( &item, RB_TRUE );
									else
										ITEM_VAL_SET_LONG( &item, RB_FALSE );
									break;

								default:
									/* This can never be reached! */
									MSG( "Dear programmer, you failed!" );
									break;
							}
							break;
							
						default:
							{
								double d[2];
								MSG( "Performing double compare" );
								
								d[0] = ITEM_VAL_GET_DBL( &op[0] );
								d[1] = ITEM_VAL_GET_DBL( &op[1] );
								
								switch( *CP )
								{
									case VMC_GRT:
										if( d[0] < d[1] )
											d[0] = 1;
										else
											d[0] = 0;
											
										break;
									case VMC_LWR:
										if( d[0] > d[1] )
											d[0] = 1;
										else
											d[0] = 0;
										break;
									case VMC_GRE:
										if( d[0] <= d[1] )
											d[0] = 1;
										else
											d[0] = 0;
										break;
									case VMC_LWE:
										if( d[0] >= d[1] )
											d[0] = 1;
										else
											d[0] = 0;
										break;

									default:
										/* This can never be reached! */
										MSG( "Dear programmer, you failed!" );
										break;
								}
								
								if( d[0] )
									ITEM_VAL_SET_LONG( &item, RB_TRUE );
								else
									ITEM_VAL_SET_LONG( &item, RB_FALSE );								
							}
							break;
					}
					break;
				
				/* Mathematical operations - Strings MUST be converted! */
				case VMC_ADD:
				case VMC_MUL:
				case VMC_SUB:
				case VMC_DIV:
				case VMC_MOD:
					{
						byte	prefer_type		= VAL_DBL;
						
						if( ITEM_VAL_TYPE( &op[0] ) == VAL_LONG 
							|| ITEM_VAL_TYPE( &op[1] ) == VAL_LONG )
							prefer_type = VAL_LONG;
						
						if( ITEM_VAL_TYPE( &op[0] ) == VAL_ADDR 
							|| ITEM_VAL_TYPE( &op[1] ) == VAL_ADDR )
							prefer_type = VAL_ADDR;
						
						/* Special case: Modulus only for integer types! */
						if( prefer_type == VAL_DBL && *CP == VMC_MOD )
							prefer_type = VAL_LONG;
							
						VARS( "prefer_type", "%d", prefer_type );						
						
						switch( prefer_type )
						{
							case VAL_DBL:
								switch( *CP )
								{
									case VMC_ADD:
										ITEM_VAL_SET_DBL( &item,
											ITEM_VAL_GET_DBL( &op[0] )
 											+ ITEM_VAL_GET_DBL( &op[1] ) );
										break;
									case VMC_MUL:
										ITEM_VAL_SET_DBL( &item,
											ITEM_VAL_GET_DBL( &op[0] )
											* ITEM_VAL_GET_DBL( &op[1] ) );
										break;
									case VMC_SUB:
										ITEM_VAL_SET_DBL( &item,
											ITEM_VAL_GET_DBL( &op[1] )
											- ITEM_VAL_GET_DBL( &op[0] ) );
										break;
									case VMC_DIV:
										ITEM_VAL_SET_DBL( &item,
											ITEM_VAL_GET_DBL( &op[1] )
											/ ITEM_VAL_GET_DBL( &op[0] ) );
										break;
										
									default:
										MSG( "Operation not implemented" );
										VARS( "for op", "%d", *CP );
										break;
								}
								break;
								
							case VAL_LONG:
								switch( *CP )
								{
									case VMC_ADD:
										ITEM_VAL_SET_LONG( &item,
											ITEM_VAL_GET_LONG( &op[0] )
 											+ ITEM_VAL_GET_LONG( &op[1] ) );
										break;
									case VMC_MUL:
										ITEM_VAL_SET_LONG( &item,
											ITEM_VAL_GET_LONG( &op[0] )
											* ITEM_VAL_GET_LONG( &op[1] ) );
										break;
									case VMC_SUB:
										ITEM_VAL_SET_LONG( &item,
											ITEM_VAL_GET_LONG( &op[1] )
											- ITEM_VAL_GET_LONG( &op[0] ) );
										break;
									case VMC_DIV:
										ITEM_VAL_SET_LONG( &item,
											ITEM_VAL_GET_LONG( &op[1] )
											/ ITEM_VAL_GET_LONG( &op[0] ) );
										break;
									case VMC_MOD:
										ITEM_VAL_SET_LONG( &item,
											ITEM_VAL_GET_LONG( &op[1] )
							 				% ITEM_VAL_GET_LONG( &op[0] ) );
										break;
										
										
									default:
										MSG( "Operation not implemented" );
										VARS( "for op", "%d", *CP );
										break;
								}
								break;

							case VAL_ADDR:
								switch( *CP )
								{
									case VMC_ADD:
										ITEM_VAL_SET_ADDR( &item,
											ITEM_VAL_GET_ADDR( &op[0] )
 											+ ITEM_VAL_GET_ADDR( &op[1] ) );
										break;
									case VMC_MUL:
										ITEM_VAL_SET_ADDR( &item,
											ITEM_VAL_GET_ADDR( &op[0] )
											* ITEM_VAL_GET_ADDR( &op[1] ) );
										break;
									case VMC_SUB:
										ITEM_VAL_SET_ADDR( &item,
											ITEM_VAL_GET_ADDR( &op[1] )
											- ITEM_VAL_GET_ADDR( &op[0] ) );
										break;
									case VMC_DIV:
										ITEM_VAL_SET_ADDR( &item,
											ITEM_VAL_GET_ADDR( &op[1] )
											/ ITEM_VAL_GET_ADDR( &op[0] ) );
										break;
									case VMC_MOD:
										ITEM_VAL_SET_ADDR( &item,
											ITEM_VAL_GET_ADDR( &op[1] )
							 				% ITEM_VAL_GET_ADDR( &op[0] ) );
										break;
										
										
									default:
										MSG( "Operation not implemented" );
										VARS( "for op", "%d", *CP );
										break;
								}
								break;
								
							default:
								MSG( "FATAL No operation deposited" );
							break;
						}
					}
					break;
			
				case VMC_AND:
					break;
			
				case VMC_OR:
					break;
			
				case VMC_XOR:
					break;
			
				case VMC_LAND:
					if( ITEM_VAL_GET_LONG( &op[0] ) == RB_TRUE
						&& ITEM_VAL_GET_LONG( &op[1] ) == RB_TRUE )
						ITEM_VAL_SET_LONG( &item, RB_TRUE );
					else
						ITEM_VAL_SET_LONG( &item, RB_FALSE );
					break;
			
				case VMC_LOR:
					if( ITEM_VAL_GET_LONG( &op[0] ) == RB_TRUE
						|| ITEM_VAL_GET_LONG( &op[1] ) == RB_TRUE )
						ITEM_VAL_SET_LONG( &item, RB_TRUE );
					else
						ITEM_VAL_SET_LONG( &item, RB_FALSE );
					break;

				case VMC_NOT:
					rb_vm_convert_value( ITEM_VAL_STRUCT( &op[0] ), VAL_DBL );
					ITEM_VAL_SET_DBL( &item, !ITEM_VAL_GET_DBL( &op[0] ) );
					break;
								
				case VMC_NEG:
					rb_vm_convert_value( ITEM_VAL_STRUCT( &op[0] ), VAL_DBL );
					ITEM_VAL_SET_DBL( &item, ITEM_VAL_GET_DBL( &op[0] ) * -1 );					
					break;
				
				default:
					MSG( "Undefined/unknown operation!!" );
					break;
			}
			
			rb_vm_item_free( &op[0] );
			rb_vm_item_free( &op[1] );
			rb_vm_stack_push( STACK, &item );
		}
		else
		{
			switch( *CP )
			{
				case VMC_NOP:
					break;
			
				case VMC_PUSHINT:
				case VMC_PUSHADR:
				case VMC_PUSHCHR:
				case VMC_PUSHDBL:
				case VMC_PUSHSTR:
					
					memset( &item, 0, sizeof( vm_stackitem ) );
					item.type = ITEM_VAL;
					skip = rb_vm_get_parm_size( *CP );
					
					switch( *CP )
					{
						case VMC_PUSHINT:
						case VMC_PUSHCHR:
							item.attribute.val.type = VAL_LONG;
							break;
	
						case VMC_PUSHDBL:
							item.attribute.val.type = VAL_DBL;
							break;
					
						case VMC_PUSHSTR:
						case VMC_PUSHADR:
							item.attribute.val.type = VAL_ADDR;
							break;
					}
					
					memcpy( &( item.attribute.val.value ), CP+1, skip );
					
					/* For constant strings, we push a pointer,
							not the index! */
					if( *CP == VMC_PUSHSTR )
					{
						/* TODO: Is this secure?? */
						ITEM_VAL_SET_CSTR( &item,
							(char*)ITEM_VAL_GET_ADDR( &item ) );
					}
					
					CP += skip;
					
					rb_vm_stack_push( STACK, &item );
					break;
			
				case VMC_RESERVE:
					memset( &item, 0, sizeof( vm_stackitem ) );
					item.type = ITEM_VAR;
					
					for( i = 0; i < PARM( vm_addr ); i++ )
						rb_vm_stack_push( STACK, &item );
						
					CP_SKIP_PARM;
					break;
	
				case VMC_JMP:
					CP = BEGIN + PARM( vm_addr ) - 1;
					VARS( "CP", "%p", CP - BEGIN );
					break;
			
				case VMC_JMPTRUE:
					rb_vm_stack_pop( STACK, &item );
					if( ITEM_VAL_GET_LONG( &item ) == RB_TRUE )
						CP = BEGIN + PARM( vm_addr ) - 1;
					else
						CP_SKIP_PARM;
					break;
			
				case VMC_JMPFALSE:
					rb_vm_stack_pop( STACK, &item );
					if( ITEM_VAL_GET_LONG( &item ) == RB_FALSE )
						CP = BEGIN + PARM( vm_addr ) - 1;
					else
						CP_SKIP_PARM;
					break;
			
				case VMC_CALL:
					/* Saving the current code pointer
						as return address */
					ITEM_VAL_SET_ADDR( &item,
						CP - BEGIN + rb_vm_get_parm_size( *CP ) );
					rb_vm_stack_push( STACK, &item );

					/* Saving the frame pointer */
					ITEM_VAL_SET_ADDR( &item, FP );
					rb_vm_stack_push( STACK, &item );
					
					FP = TOS - 1;
					CP = BEGIN + PARM( vm_addr ) - 1;
					break;
			
				case VMC_RET:
					/* Drop variables */
					rb_vm_stack_drop( STACK, TOS - FP - 1 );
					
					/* Get saved frame pointer */
					rb_vm_stack_pop( STACK, &item );
					FP = ITEM_VAL_GET_ADDR( &item );
					
					/* Get saved code pointer */
					rb_vm_stack_pop( STACK, &item );
					CP = BEGIN + ITEM_VAL_GET_ADDR( &item );
										
					/* Get number of parameters, and drop them */
					rb_vm_stack_pop( STACK, &item );
					rb_vm_stack_drop( STACK, ITEM_VAL_GET_ADDR( &item ) );
					break;

				case VMC_RETV:
					/* RETV: Hold return value! */
					rb_vm_stack_pop( STACK, &hold );
					rb_vm_item_memdup( &hold );
					
					/* Drop variables */
					rb_vm_stack_drop( STACK, TOS - FP - 1 );
					
					/* Get saved frame pointer */
					rb_vm_stack_pop( STACK, &item );
					FP = ITEM_VAL_GET_ADDR( &item );
					
					/* Get saved code pointer */
					rb_vm_stack_pop( STACK, &item );
					CP = BEGIN + ITEM_VAL_GET_ADDR( &item );
										
					/* Get number of parameters, and drop them */
					rb_vm_stack_pop( STACK, &item );
					rb_vm_stack_drop( STACK, ITEM_VAL_GET_ADDR( &item ) );
					
					/* RETV: Re-push return value again */
					rb_vm_stack_push( STACK, &hold );
					break;
			
				case VMC_NCALL:
					memset( &hold, 0, sizeof( vm_stackitem ) );
					hold.type = ITEM_VAL;

					rb_vm_stack_pop( STACK, &item );
					
					native_fct = (rb_native_func)PARM( vm_addr );
					
					(native_fct)( &hold,
						rb_vm_stack_access( STACK,
							TOS - ITEM_VAL_GET_ADDR( &item ) + 1 ),
								(int)ITEM_VAL_GET_ADDR( &item ) );

					rb_vm_stack_drop( STACK, ITEM_VAL_GET_ADDR( &item ) );
					CP_SKIP_PARM;
					
					rb_vm_stack_push( STACK, &hold );
					break;
					
				case VMC_NSTORE:
					rb_vm_stack_pop( STACK, &item );

					native_var = (rb_native_var)PARM( vm_addr );					
					(native_var)( &item );
					CP_SKIP_PARM;
					
					rb_vm_item_free( &item );
					break;
				
				case VMC_NLOAD:
					memset( &item, 0, sizeof( vm_stackitem ) );
					item.type = ITEM_VAL;

					native_var = (rb_native_var)PARM( vm_addr );					
					(native_var)( &item );
					CP_SKIP_PARM;
					
					rb_vm_stack_push( STACK, &item );
					break;
					
				case VMC_DROP:
					rb_vm_stack_drop( STACK, PARM( vm_addr ) );
					break;

				case VMC_STORE:
				{
					vm_var*	var;

					rb_vm_stack_pop( STACK, &item );
					if( !( var = rb_vm_stack_var_resolve( 
							STACK, TRUE, (pboolean*)NULL ) ) )
						OUTOFMEM;

					/* Here, we have to decide between value to variable
						and variable to variable store! */
					switch( item.type )
					{
						case ITEM_VAL:
							rb_vm_var_set( var,
								ITEM_VAL_STRUCT( &item ), TRUE );
							break;
							
						case ITEM_VAR:
							memcpy( var, &( item.attribute.var ),
									sizeof( vm_var ) );
							break;
						case ITEM_PTR:
							rb_vm_var_copy( var, item.attribute.var_ptr );
							break;
							
						default:
							MSG( "So kann ich nich arbeiten, ey!!" );
							break;
					}

					break;
				}
			
				case VMC_LOAD:
				{
					pboolean		get_size = TRUE;
					vm_var*		var;
					
					var = rb_vm_stack_var_resolve( STACK, FALSE, &get_size );
					VARS( "get_size", "%d", get_size );

					if( get_size )
					{
						item.type = ITEM_VAL;
						ITEM_VAL_SET_ADDR( &item, var->begin_cnt );
						rb_vm_stack_push( STACK, &item );
					}
					else
						rb_vm_stack_var_push( STACK, var );

					break;
				}
			
				case VMC_PTRLOAD:
					item.type = ITEM_PTR;
					item.attribute.var_ptr = rb_vm_stack_var_resolve(
							STACK, TRUE, (pboolean*)NULL );
					rb_vm_stack_push( STACK, &item );
					break;
					
				case VMC_CPYLOAD:
				{
					vm_var*	var;

					var = rb_vm_stack_var_resolve(
							STACK, TRUE, (pboolean*)NULL );
				
					memset( &item, 0, sizeof( vm_stackitem ) );
					item.type = ITEM_VAR;
							
					if( !rb_vm_var_copy( &( item.attribute.var ), var ) )
					{
						MSG( "Fatal error on var copying" );
						OUTOFMEM;
					}
					
					rb_vm_stack_push( STACK, &item );
					break;
				}
				case VMC_INIT:
					rb_vm_stack_init_var( STACK, (vm_var*)NULL, -1, 0 );
					break;

				case VMC_FREE:
					rb_vm_var_free( rb_vm_stack_var_resolve(
						STACK, FALSE, (pboolean*)NULL ) );
					break;			

				case VMC_MOV:
					/* Copy only one item, do nothing with memory free */
					rb_vm_stack_pop( STACK, &item );
					rb_vm_stack_pop( STACK, &hold );
					rb_vm_stack_put( STACK, &item,	
						ITEM_VAL_GET_ADDR( &hold ) );
					break;

				case VMC_PUSHFRM:
					ITEM_VAL_SET_ADDR( &item, FP );
					rb_vm_stack_push( STACK, &item );
					break;

				default:
					break;
			}
		}
		
		/* RB_VM_STACK_DUMP( STACK ); */
	}

	RB_VM_STACK_DUMP( STACK );

	RETURN( 0 );
}
