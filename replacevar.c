/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2009 by Phorward Software Technologies
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com

File:		replacevar.c
Author:		Jan Max Meyer
Usage:		Implements the function REPLACEVAR.
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "rb_global.h"
#include "rb_native.h"

/*
 * Global variables
 */

extern BOOLEAN REGEX_ENABLED;
extern BOOLEAN CASE_INSENSITIVITY;

/*
 * Functions
 */


/*RBDOC

	%%function
	replacevar( string, find, replace )
	%%desc:en
	Description of replacevar
	%%desc:ge
	Beschreibung von replacevar

	%%parm:en
	string
	find
	replace

	%%parm:ge
	string
	find
	replace
	%%return:en
	Return value of replacevar
	%%return:ge
	Rückgabewert von replacevar

	%%notice:en
	%%notice:ge

RBDOC*/

RB_FCT( replacevar )
{
/*RBAUTOIMPORT function replacevar vvv*/
	char*	str;
	char*	find;
	char*	replace;
	int		flags		= PREGEX_COMP_NOREF
						| PREGEX_COMP_NOERRORS
						;

	PROC( "replacevar" );
	RB_FCT_DUMP_PARMS();

	str = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	find = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 1 ) );
	replace = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 2 ) );
	VARS( "str", "%s", str );
	VARS( "find", "%s", find );
	VARS( "replace", "%s", replace );

	VARS( "CASE_INSENSITIVITY", "%d", CASE_INSENSITIVITY );
	if( CASE_INSENSITIVITY )
		flags |= PREGEX_COMP_INSENSITIVE;

	VARS( "REGEX_ENABLED", "%d", REGEX_ENABLED );
	if( !REGEX_ENABLED )
		flags |= PREGEX_COMP_STATIC;

	if( !( CASE_INSENSITIVITY && REGEX_ENABLED ) )
	{
		MSG( "Calling Turbo-mode string replacer" );
		if( !( str = pstrreplace( str, find, replace ) ) )
			RB_OUT_OF_MEMORY;
	}
	else
	{
		MSG( "Calling Power-mode regular expression string replacer" );
		str = pregex_qreplace( find, str, replace, flags );
	}

	RB_PARM_VAL_SET_STR( RB_FCT_RET, str );
	RETURN( 0 );
}
