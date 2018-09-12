/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_comp.main.c
Author:	Jan Max Meyer
Usage:	Compiler entry / main function
----------------------------------------------------------------------------- */

/*
 * Includes
 */

#include "rb_global.h"

/*
 * Global variables
 */
CINFO	cur;

/*
 * Functions
 */
 
/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_compile()
	
	Author:			Jan Max Meyer
	
	Usage:			Compiles a source string into a virtual machine; This
					function is always bound on an existing compile-time
					environment, so calling it recursively is only possible
					from a given context. At first call, the context is 
					intialized to zero - no context at all!
	
	Parameters:		char*		filename		Filename of source file, only
												for error reporting; This can
												be left (char*)NULL.
					char*		src				Source to be compiled.
					
	Returns:		int			0				No errors, all is fine
								>0				errors where found, number of
												errors
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int rb_comp_compile( char* filename, char* src )
{
	static int	calls		= 0;
	
	/* tmp is virtually stacked - to allow recurisve parsers! */
	CINFO		tmp;
	int			ret;
	vm_addr 	var_reserve;

	PROC( "rb_comp_compile" );
	PARMS( "filename", "%s", filename );
	
	VARS( "calls", "%d", calls );
	if( !calls++ )
	{
		var_reserve = rb_comp_next_addr( &cur.prog, VMC_RESERVE );
		cur.is_global_scope = TRUE;
	}
	else
		memcpy( &tmp, &cur, sizeof( CINFO ) );

	cur.filename = filename;
	cur.line = 1;
	rb_comp_current_srcpos( &cur.stmt_begin );
	
	MSG( "-----------------------------------" );
	VARS( "src", "%s", src );
	cur.src = src;
	ret = rb_parse( (rb_pcb*)NULL );
	MSG( "-----------------------------------" );
	
	if( !--calls )
	{
		vm_addr		global_vars;

		ret += rb_comp_backpatch_proc_calls( (symbol*)NULL );
		ret += rb_comp_backpatch_label_calls( (symbol*)NULL );
		
		global_vars = (vm_addr)rb_comp_get_var_count_of_scope( cur.scope );
		rb_comp_patch( VM_GET_CODE( &cur.prog, 0 ),
			VMC_RESERVE, &global_vars );
			
		fprintf( stderr, "--- Final program ---\n" );
		rb_comp_gen( &cur.prog, VMC_NOP, (void*)NULL );
		rb_vm_dump( stderr, &cur.prog, 0 );
	}
		
	cur.filename = tmp.filename;
	cur.line = tmp.line;
	cur.src = tmp.src;
	rb_comp_current_srcpos( &cur.stmt_begin );

	RETURN( ret );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_reset()
	
	Author:			Jan Max Meyer
	
	Usage:			Resets the compiler (symbol table) and current program.
	
	Parameters:		pboolean		keep_natives	If keep_natives is TRUE, all
												native-related functions,
												variables and constants are
												hold in the symbol table, to
												be used in a subsequent
												compiler run.
					
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_comp_reset( pboolean keep_natives )
{
	PROC( "rb_comp_reset" );
	PARMS( "keep_natives", "%d", keep_natives );
	
	rb_symtab_drop_all( keep_natives );
	
	pfree( cur.prog.code ); /* TODO */
	memset( &cur, 0, sizeof( CINFO ) );
	
	VOIDRET;
}

#if 0
int main( int argc, char** argv )
{
	srcfile		file;
	vm_stack	stack;

	rb_add_native_function( "getlen", getlen,
		TRUE, "v", NATIVEFUNC_IF_POSSIBLE );
	rb_add_native_function( "print", print,
		FALSE, "v+", NATIVEFUNC_AT_RUNTIME );
	rb_add_native_function( "getcharat", charat,
		TRUE, "vv", NATIVEFUNC_IF_POSSIBLE );
	rb_add_native_function( "getchr", getchr,
		TRUE, "v", NATIVEFUNC_IF_POSSIBLE );
	rb_add_native_function( "getasc", getasc,
		TRUE, "v", NATIVEFUNC_IF_POSSIBLE );
	rb_add_native_function( "getpos", getpos,
		TRUE, "vvv", NATIVEFUNC_IF_POSSIBLE );
	rb_add_native_function( "lowvar", lowvar,
		TRUE, "v", NATIVEFUNC_IF_POSSIBLE );
	rb_add_native_function( "upvar", upvar,
		TRUE, "v", NATIVEFUNC_IF_POSSIBLE );
	rb_add_native_function( "trimvar", trimvar,
		TRUE, "v", NATIVEFUNC_IF_POSSIBLE );
	rb_add_native_function( "replacevar", replacevar,
		TRUE, "vvv", NATIVEFUNC_IF_POSSIBLE );
		
	/*
	rb_add_native_function( "include", include,
		FALSE, "v", NATIVEFUNC_AT_COMPTIME );
	*/
	
	rb_add_native_const( "true", "0", FALSE );
	rb_add_native_const( "false", "-1", FALSE );
	
	rb_add_native_var( "test", get_test, set_test );
	
	
	if( argc <= 1 )
	{
		fprintf( stderr, "%s: Missing filename!\n", *argv );
		return EXIT_FAILURE;
	}
	
	switch( rb_comp_read_file( &file, argv[1] ) )
	{
		case 1:
			fprintf( stderr, "%s: File not found: %s\n", *argv, argv[1] );
			return EXIT_FAILURE;
			
		case 0:
			break;
			
		default:
			return EXIT_FAILURE;
			break;
	}
	
	if( !rb_comp_compile( file.filename, file.content ) )
	{
		fprintf( stderr, "--- Running program ---\n" );
		rb_vm_run( &stack, cur.prog.code,
			cur.prog.code_next );
	}
	
	return EXIT_SUCCESS;
}

#endif

void rb_run( void )
{
	vm_stack	stack;

	rb_vm_run( &stack, cur.prog.code,
		cur.prog.code_next );
}
