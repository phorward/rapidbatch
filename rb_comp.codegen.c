/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_comp.codegen.c
Author:	Jan Max Meyer
Usage:	Virtual machine code generator
----------------------------------------------------------------------------- */

/*
 * Includes
 */

#include "rb_global.h"

/*
 * Global variables
 */
static vm_prog*		patch_prog;
static vm_code		patch_instr;
static vm_addr		patch_addr;
static pboolean		patch_required	= FALSE;

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_next_addr()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets up a new code address and returns its pointer.
					
	Parameters:		vm_prog*	prog			Program to be extended
					vm_code		instr			The command which should be
												allocated (including a possible
												parameter)

	Returns:		vm_addr						The new (next) address
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
vm_addr rb_comp_next_addr( vm_prog* prog, vm_code instr )
{
	char		allocsize;
	
	PROC( "rb_comp_next_addr" );
	PARMS( "prog", "%p", prog );
	PARMS( "instr", "%d", instr );
	
	allocsize = 1 + VM_GET_PARAM_SIZE( instr );
	VARS( "required size for this command is", "%d", allocsize );
	
	prog->cur_addr = prog->code_next;
	VARS( "start", "%ld", prog->cur_addr );

	for( ; allocsize > 0; allocsize-- )
	{
		if( prog->code_cnt == 0 )
		{
			prog->code = (vm_code*)pmalloc(
							MALLOC_STEP * sizeof( vm_code ) );
			prog->code_cnt = MALLOC_STEP;
		}
		else if( prog->code_next == prog->code_cnt )
		{
			prog->code = (vm_code*)prealloc( (vm_code*)( prog->code ),
							( prog->code_cnt + MALLOC_STEP )
								* sizeof( vm_code ) );
			prog->code_cnt += MALLOC_STEP;
		}
		
		if( !( prog->code ) )
			RB_OUT_OF_MEMORY;
		
		prog->code[ prog->code_next++ ] = 0;
	}
	
	prog->code[ prog->cur_addr ] = instr;
	
	/* Patch a possible Jump! */
	if( patch_required && patch_prog == prog )
	{
		rb_comp_patch( VM_GET_CODE( prog, patch_addr ), patch_instr,
					&( prog->cur_addr ) );
		patch_required = FALSE;
	}
	
	RETURN( prog->cur_addr );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_patch_jmp()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets the code generator's patch variables to patch a program
					address with the next code memory request. This causes lesser
					generation of NOP-instructions.
					
					USE CAREFULLY!
					
	Parameters:		vm_prog						Program to be patched
					vm_addr						Address of the instruction, where
												a program address must be
												patched.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_comp_patch_jmp( vm_prog* prog, vm_code instr, vm_addr call )
{
	patch_required = TRUE;
	patch_prog = prog;
	patch_instr = instr;
	patch_addr = call;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_cur_addr()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets up a new code address and returns a pointer.
					
	Parameters:		vm_prog*	prog			Program to be extended

	Returns:		vm_addr						Current address
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
vm_addr rb_comp_cur_addr( vm_prog* prog )
{
	PROC( "rb_comp_cur_addr" );
	PARMS( "prog", "%p", prog );

	RETURN( prog->cur_addr );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_patch()
	
	Author:			Jan Max Meyer
	
	Usage:			Patches an existing vm_code item with a cmd and a
					possible operand.
					
	Parameters:		vm_code*	code			Code item to be patched
					int			instr			Virtual machine command to be
												set up. If < 0, then the
												instruction is not set, only
												the parameter.
					void*		parm			Parameter pointer; This must be
												of a type instr requires.

	Returns:		pboolean						TRUE, if all worked fine,
												FALSE, in case param contained
												invalid data for the instr.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
/* pboolean rb_comp_patch_d( char* file, int line, vm_code* code, int instr, void* parm ) */
pboolean rb_comp_patch( vm_code* code, int instr, void* parm )
{
	char	size;

	PROC( "rb_comp_patch" );
	PARMS( "code", "%p", code );
	PARMS( "instr", "%d", instr );
	PARMS( "parm", "%p", parm );
	
	/*
	fprintf( stderr, "BACKPATCHING from %s %d: %x as %d\n", file, line, code - ( cur_prog.code ), instr );
	rb_vm_dump( stderr, &cur_prog );
	fprintf( stderr, "\n" );
	*/

	if( instr >= 0 )
		*code = (vm_code)instr;

	VARS( "instr", "%d", instr );
	if( ( size = VM_GET_PARAM_SIZE( instr ) ) )
	{
		MSG( "Instruction has parameter!" );
		memcpy( code+1, parm, size );
	}
	
	/*
	rb_vm_dump( stderr, &cur_prog );
	fprintf( stderr, "---------\n\n" );
	*/

	RETURN( TRUE );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_gen()
	
	Author:			Jan Max Meyer
	
	Usage:			Main code generation function, that creates a new code
					within a virtual machine program.
					
	Parameters:		vm_prog*	prog			Program to be extended
					vm_cmd		instr			Virtual machine command
					void*		parm			Parameter pointer; This must be
												of a type instr requires.

	Returns:		vm_addr						Address of new instruction
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
vm_addr rb_comp_gen( vm_prog* prog, vm_code instr, void* parm )
{
	vm_addr		next;	
	
	PROC( "rb_comp_gen" );
	PARMS( "prog", "%p", prog );
	PARMS( "instr", "%d", instr );
	PARMS( "parm", "%p", parm );
	
	next = rb_comp_next_addr( prog, instr );
	rb_comp_patch( VM_GET_CODE( prog, next ), instr, parm );

	RETURN( next );
}
