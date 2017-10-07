/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2009 by Phorward Software Technologies
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com

File:		getpos.c
Author:		Andreas Harbecke
Usage:		Implements the function GETPOS.
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
	getpos( string, find, position )
	%%desc:en
	Description of getpos
	%%desc:ge
	Beschreibung von getpos

	%%parm:en
	string
	find
	position

	%%parm:ge
	string
	find
	position
	%%return:en
	Return value of getpos
	%%return:ge
	Rückgabewert von getpos

	%%notice:en
	%%notice:ge

RBDOC*/

RB_FCT( getpos )
{
/*RBAUTOIMPORT function getpos vvv*/

	char*			sourceString;
	char*			targetString;
	char*			tempString;
	char*			ptr;
	unsigned long	position;
	unsigned long	offset			= 0;
	int				preg_mod		= PREGEX_COMP_NOERRORS;
	int				matches_cnt;
	parray*			matches;
	prange*			pr;

	PROC( "getpos" );
	RB_FCT_DUMP_PARMS();

	sourceString = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "sourceString", "%s", sourceString );

	targetString = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 1 ) );
	VARS( "targetString", "%s", targetString );

	position = RB_PARM_VAL_GET_ULONG( RB_FCT_PARM_ACCESS( 2 ) );
	VARS( "position", "%ld", position);

	VARS( "CASE_INSENSITIVITIY", "%d", CASE_INSENSITIVITY );
	if( CASE_INSENSITIVITY )
		preg_mod |= PREGEX_COMP_INSENSITIVE;

	VARS( "REGEX_ENABLED", "%d", REGEX_ENABLED );
	if( !REGEX_ENABLED )
		preg_mod |= PREGEX_COMP_STATIC;

	/* Perform direct string search if no regex-features are required */
	if( !CASE_INSENSITIVITY && !REGEX_ENABLED )
	{
		for( tempString = sourceString - 1;
			position > 0 && tempString; position-- )
			tempString = strstr( ++tempString, targetString );

		if( tempString )
		{
			for( ptr = sourceString; ptr < tempString;
					ptr += u8_seqlen( ptr ) )
				offset++;
		}
	}
	/* else, use a regular expression */
	else
	{
		VARS( "preg_mod", "%d", preg_mod );
		if( ( matches_cnt = pregex_qmatch( sourceString, targetString,
				preg_mod, &matches ) ) > 0 )
		{
			MSG( "Matches found" );
			VARS( "matches_cnt", "%d", matches_cnt );

			if( position <= matches_cnt )
			{
				pr = (prange*)parray_get( matches, position - 1 );
				offset = pr->begin - targetString;
			}

			parray_free( matches );
		}
		else if( matches_cnt < 0 )
		{
			/* TODO */
			MSG( "Error in regex-handling" );
		}
	}

	VARS( "offset", "%ld", offset );
	if( offset )
		RB_PARM_VAL_SET_LONG( RB_FCT_RET, offset + 1 );
	else
		RB_PARM_VAL_SET_LONG( RB_FCT_RET, 0 );

	RETURN( 0 );
}
