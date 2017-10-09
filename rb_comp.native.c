/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_comp.native.c
Author:	Jan Max Meyer
Usage:	Native scripting function management
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
	Function:		rb_add_native_function()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets up a new native function to be called from within
					RapidBATCH scripts.
					
	Parameters:		char*				name	The name of the native
													build-in function
					rb_native_func		fct		Pointer to the build-in
													function
					pboolean				isfunc		Defines if the user-function
													is threaded as a function
													or a procedure.
					char*				parmdef		Parameter definition; This
														defines the number
														and type of the 
														parameters to be
														checked.
														
														"v" Parameter by value
														"p" Parameter by ref
														"+" Variable list of
															parameters following
					pboolean				compiletime	TRUE: Function is a compile-
															time function and is
															executed at compile
															time
													FALSE: Function is a runtime
															function and is exe-
															cuted at run-time

	Returns:		long							Returns 0 on
													success.
													
													Else, -1 is
													returned in case if the
													function already exists,
													and -1 when
													parameters are missing.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int rb_add_native_function( char* name, rb_native_func fct,
	pboolean isfunc, char* parmdef, int run_at )
{
	symbol*			sym;

	PROC( "rb_add_native_function" );
	PARMS( "name", "%s", name );
	PARMS( "fct", "%p", fct );
	PARMS( "isfunc", "%s", BOOLEAN_STR( isfunc ) );
	PARMS( "parmdef", "%s", parmdef );
	PARMS( "run_at", "%d", run_at );
	
	if( !( name && *name && fct && parmdef ) )
	{
		MSG( "There is something wrong with your parameters" );
		RETURN( -1 );
	}
	
	MSG( "Adding symbol table entry" );
	if( !rb_symtab_find( name, SYM_PROC ) )
	{
		MSG( "Symbol doesn't exist, fine!" );
		
		if( !( sym = rb_symtab_new( name, SYM_PROC ) ) )
		{
			MSG( "Failed to create new symbol" );
			RETURN( -1 );
		}

		sym->native = TRUE;
		sym->global = TRUE;
		
		sym->defined = TRUE;
		sym->used = TRUE;
		sym->patched = TRUE;
		
		sym->constant = run_at;

		sym->function = isfunc;
		sym->offset.function = fct;

		if( !( sym->parmdef = (char*)pstrdup( parmdef ) ) )
		{
			MSG( "Failed to allocate memory for parmdef" );
			RETURN( -1 );
		}

		RETURN( 0 );
	}

	MSG( "The function already exists!" );
	RETURN( -1 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_add_native_var()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets up a new native variable to be called from within
					RapidBATCH scripts.
					
	Parameters:		char*			name			The name of the native
													build-in variable.
					rb_native_var	get				Pointer to the variable
													get function
					rb_native_var	set				Pointer to the variable
													set function

	Returns:		int								Returns 0 on
													success.
													
													Else, -1 is
													returned in case if the
													function already exists,
													and -1 when
													parameters are missing.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int rb_add_native_var( char* name, rb_native_var get, rb_native_var set )
{
	symbol*			sym;

	PROC( "rb_add_native_var" );
	PARMS( "name", "%s", name );
	PARMS( "get", "%p", get );
	PARMS( "set", "%p", set );
	
	if( !( name && *name ) )
	{
		MSG( "There is something wrong with your parameters" );
		RETURN( -1 );
	}
	
	MSG( "Adding symbol table entry" );
	if( !rb_symtab_find( name, SYM_VAR ) )
	{
		MSG( "Symbol doesn't exist, fine!" );

		if( !( sym = rb_symtab_new( name, SYM_VAR ) ) )
		{
			MSG( "Failed to create new symbol" );
			RETURN( -1 );
		}

		sym->native = TRUE;
		sym->global = TRUE;
		
		sym->defined = TRUE;
		sym->used = TRUE;

		sym->offset.variable.get = get;
		sym->offset.variable.set = set;
		
		RETURN( 0 );
	}

	RETURN( -1 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_add_native_const()
	
	Author:			Jan Max Meyer
	
	Usage:			Sets up a new native constant variable for its usage within
					scripts.
					
	Parameters:		char*			name			The name of the native
													build-in variable.
					char*			value			The constant value; This
													is always string, and will
													be converted to a better
													suiting type (if possible!)
													during compile time.
					pboolean			dup_val			Specifies if 'value' shall
													be duplicated into a newly 
													allocated address.

	Returns:		int								Returns 0 on
													success.
													
													Else, -1 is
													returned in case if the
													function already exists,
													and -1 when
													parameters are missing.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int rb_add_native_const( char* name, char* value, pboolean dup_val )
{
	symbol*			sym;

	PROC( "rb_add_native_const" );
	PARMS( "name", "%s", name );
	PARMS( "value", "%s", value );
	
	if( !( name && *name && value ) )
	{
		MSG( "There is something wrong with your parameters" );
		RETURN( -1 );
	}
	
	MSG( "Adding symbol table entry" );
	if( !rb_symtab_find( name, SYM_VAR ) )
	{
		MSG( "Symbol doesn't exist, fine!" );

		if( !( sym = rb_symtab_new( name, SYM_VAR ) ) )
		{
			MSG( "Failed to create new symbol" );
			RETURN( -1 );
		}

		sym->native = TRUE;
		sym->global = TRUE;
		sym->constant = TRUE;
		
		sym->defined = TRUE;
		sym->used = TRUE;
		
		if( dup_val )
			sym->offset.constant = pstrdup( value );
		else
			sym->offset.constant = value;
		
		RETURN( 0 );
	}

	RETURN( -1 );
}
