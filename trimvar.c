/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2009 by Phorward Software Technologies
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com

File:		trimvar.c
Author:		Andreas Harbecke
Usage:		Implements the TRIMVAR standard function
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "rb_global.h"
#include "rb_native.h"

/*
 * Global variables
 */

/*
 * Functions
 */


/*RBDOC

	%%function
	trimvar( str )

	%%desc:en
	Removes beginning and trailing whitespace (tabulators, blanks)
	from a string '''str''' and returns the ''trimmed'' string.
	%%desc:ge
	Entfernt führenden und endenden Leerzeichen und Tabulatoren aus
	einem String '''str''', und gibt diesen zurück.

	%%parm:en
	str		The string to be trimmed.
	%%parm:ge
	str		Der zu bearbeitende String.

	%%return:en
	Returns the 'trimmed' string.

	%%return:ge
	Gibt den ''getrimmten'' String zurück.\n
	Enthält der Eingabestring keine führenden oder endenden
	Leerzeichen, so ist der Rückgabestring äquivalent zum
	Eingabestring.

	%%notice:en
	%%notice:ge

RBDOC*/

RB_FCT( trimvar )
{
/*RBAUTOIMPORT function trimvar v*/

	char*	sourceString;
	char*	ret;

	PROC( "trimvar" );
	RB_FCT_DUMP_PARMS();

	sourceString = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "sourceString", "%s", sourceString );

	if( !( ret = pstrdup( pstr_strip( sourceString ) ) ) )
		RB_OUT_OF_MEMORY;

	/*
	TIMEDUMP;

	bgnstr = sourceString;
	while( *bgnstr == ' ' || *bgnstr == '\t' )
		bgnstr++;

	endstr = pstrlen ( bgnstr ) + bgnstr - 1;
	while( *endstr == ' ' || *endstr == '\t' )
		endstr--;

	*(endstr + 1) = '\0';

	TIMEDUMP;
	*/

	VARS( "ret", "%s", ret );
	RB_PARM_VAL_SET_STR( RB_FCT_RET, ret );

	RETURN( 0 );
}
