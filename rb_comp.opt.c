/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_comp.codegen.c
Author:	Jan Max Meyer
Usage:	Virtual machine code optimizer and compile-time code execution
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
	Function:		rb_comp_opt_expr()
	
	Author:			Jan Max Meyer
	
	Usage:			Optimizes an expression by running it in the virtual
					machine. The function will immediatelly patch the virtual
					machine program if the executed code results in a
					correct result-value.
					
					WARNING: This function must be used very carefully from
							 within the compiler; Only code-parts with
							 entirely constant operators can be executed.
							 If wrong code is passed, it may occur that the
							 virtual machine crashes.
					
	Parameters:		vm_prog*	prog			Program in which optimization
												should be performed
					vm_code		instr			The command which should be
												allocated (including a possible
												parameter)
					boolean		with_return		TRUE: Expression returns a value
												FALSE: If not!

	Returns:		int			RB_ERR_OK		on success
								RB_ERR...		on failure
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int rb_comp_opt_expr( vm_prog* prog, vm_addr begin_opt, boolean with_return )
{
	vm_stack		stack;
	vm_stackitem	item;
	vm_code*		exec;
	vm_addr			size;
	
	PROC( "rb_comp_opt_expr" );
	PARMS( "prog", "%p", prog );
	PARMS( "begin_opt", "%ld", begin_opt );
	PARMS( "with_return", "%d", with_return );
	
	size = prog->code_next - begin_opt;
	VARS( "size", "%ld", size );
	
	/* Copy code into temporary virtual machine */
	MSG( "Creating temporary virtual machine for execution" );
	if( !( exec = (vm_code*)pmalloc( size * sizeof( vm_code ) ) ) )
	{
		MSG( "Failed to reserve temporary virtual machine" );
		RB_OUT_OF_MEMORY;
	}
	
	memcpy( exec, prog->code + begin_opt,
		( prog->code_next - begin_opt ) * sizeof( vm_code ) );
		
	MSG( "Resizing virtual machine" );
	memset( prog->code + begin_opt, 0,
			( prog->code_next - begin_opt ) * sizeof( vm_code ) );
	prog->code_next = begin_opt;
	
	/* Run this machine! */
	if( rb_vm_run( &stack, exec, size )
			!= RB_ERR_OK )
	{
		MSG( "Fatal error on virtual machine execution!" );
		
		pfree( exec );
		RETURN( RB_ERR_FAILURE );
	}
	
	/* Use return value as optimization result? */
	if( with_return )
	{
		MSG( "Last stack element value is reused" );
		rb_vm_stack_pop( &stack, &item );
		if( stack.stack_top > 0 )
		{
			MSG( "Fatal error, stack contains too many values!" );
			
			pfree( exec );
			RETURN( RB_ERR_FAILURE );
		}

		rb_comp_push_constant( ITEM_VAL_GET_STR( &item ) );
		rb_vm_item_free( &item );
	}

	pfree( exec );

	RETURN( RB_ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_opt_test()
	
	Author:			Jan Max Meyer
	
	Usage:			Tests a code segment if it can be optimized; Optimization
					(and therefore compile-time execution of code!) is not
					possible if variable access is performed.

	Parameters:		vm_code*	code			Pointer to code where to
												test from
					vm_addr		code_cnt		End of code

	Returns:		boolean						TRUE: If optimization can be
														performed
												FALSE: If there are instructions
														used which can't be
														handled at compile-time
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
boolean rb_comp_opt_test( vm_code* code, vm_addr code_cnt )
{
	vm_code*		cp;
	
	PROC( "rb_comp_opt_test" );
	PARMS( "code", "%p", code );
	PARMS( "code_cnt", "%ld", code_cnt );
	
	for( cp = code; cp < code + code_cnt; cp++ )
	{
		switch( *cp )
		{
			/*
				When this code is build-up
				only of these instructions,
				than it can be ran at compile-time
			*/
			case VMC_NOP:
			case VMC_PUSHINT:
			case VMC_PUSHADR:
			case VMC_PUSHCHR:
			case VMC_PUSHDBL:
			case VMC_PUSHSTR:
			case VMC_NCALL:
			case VMC_DROP:
			case VMC_JOIN:
			case VMC_ADD:
			case VMC_SUB:
			case VMC_DIV:
			case VMC_MUL:
			case VMC_MOD:
			case VMC_NOT:
			case VMC_EQU:
			case VMC_NEQ:
			case VMC_GRT:
			case VMC_LWR:
			case VMC_GRE:
			case VMC_LWE:
			case VMC_NEG:
			case VMC_AND:
			case VMC_OR:
			case VMC_XOR:
			case VMC_LAND:
			case VMC_LOR:
				break;

			default:
				MSG( "This instruction can't be used at compile-time" );
				RETURN( FALSE );
		}

		cp += VM_GET_PARAM_SIZE( *cp );
	}

	RETURN( TRUE );
}
