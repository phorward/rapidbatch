/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2009 by Phorward Software Technologies
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com

File:		gettok.c
Author:
Usage:
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "rb_global.h"

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
	gettok( string, separator, count )
	%%desc:en
	Description of gettok
	%%desc:ge
	Beschreibung von gettok

	%%parm:en
	string
	separator
	count

	%%parm:ge
	string
	separator
	count
	%%return:en
	Return value of gettok
	%%return:ge
	Rückgabewert von gettok

	%%notice:en
	%%notice:ge

RBDOC*/

RB_FCT( gettok )
{
	char*	string;
	char*	separator;
	int		count;
	int		flags		= PREGEX_COMP_NOREF
						| PREGEX_COMP_NOERRORS
						;
	int		res;
	char**	result;
	parray*	parts;


/*RBAUTOIMPORT function gettok vvv*/

	PROC( "gettok" );
	RB_FCT_DUMP_PARMS();

	string = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "string", "%s", string );

	separator = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 1 ) );
	VARS( "separator", "%s", separator );

	count = (int)RB_PARM_VAL_GET_LONG( RB_FCT_PARM_ACCESS( 2 ) );
	VARS( "count", "%d", count );

	VARS( "CASE_INSENSITIVITY", "%d", CASE_INSENSITIVITY );
	if( CASE_INSENSITIVITY )
		flags |= PREGEX_COMP_INSENSITIVE;

	VARS( "REGEX_ENABLED", "%d", REGEX_ENABLED );
	if( !REGEX_ENABLED )
		flags |= PREGEX_COMP_STATIC;

	if( !( CASE_INSENSITIVITY && REGEX_ENABLED ) )
	{
		MSG( "Calling Turbo-mode string tokenizer" );
		if( ( res = pstrsplit( &result, string, separator, count ) ) < 0 )
			OUTOFMEM;

		if( res == count )
			RB_PARM_VAL_SET_STR( RB_FCT_RET, pstrdup( result[ res - 1 ] ) );

		pfree( result );
	}
	else
	{
		/* TODO 06.10.2017 NOT FOR PORTING NOW!! */

		/*
		MSG( "Calling Power-mode regular expression string splitter" );
		if( ( res = pregex_split( separator, string, flags, &parts ) ) < 0 )
			OUTOFMEM;

		if( res >= count )
		{
			if( !( string = (char*)pmalloc(
					( parts[ count - 1 ].end - parts[ count - 1 ].begin + 1 )
						* sizeof( char ) ) ) )
				OUTOFMEM;

			pstrncpy( string, parts[ count - 1 ].begin,
					parts[ count - 1 ].end - parts[ count - 1 ].begin );
			string[ parts[ count - 1 ].end - parts[ count - 1 ].begin ] = '\0';

			VARS( "string", "%s", string );
			RB_PARM_VAL_SET_STR( RB_FCT_RET, string );
		}


		pfree( parts );

		*/
	}

	RETURN( 0 );
}

