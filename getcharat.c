/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2009 by Phorward Software Technologies
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com

File:		getcharat.c
Author:		Andreas Harbecke
Usage:		Implements the function GETCHARAT.
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

/*RBDOC

	%%function:en
	getcharat( str, offset )
	%%function:ge
	getcharat( str, position )

	%%desc:en
	Returns the character at '''offset''' in '''str'''.
	If '''offset''' is greater than the string's length, or
	even negative, an empty string will be returned.

	%%desc:ge
	Gibt das Zeichen an '''position''' im String '''str'''
	zurück. Ist '''position''' größer als die Stringlänge
	oder gar negativ wird ein Leerstring zurückgegeben.

	%%parm:en
	str		The string, from which the character should be retrieved.
	%%parm:en
	offset	The offset where the character should be retuned
							from.

	%%parm:ge
	str		Der String, aus welchem ein Zeichen zurückgegeben werden soll.
	%%parm:ge
	position Die Position des gewünschten Zeichens aus '''str'''.

	%%return:en
	The character on the given offset.
	%%return:ge
	Das Zeichen an der gewünschten Position.

	%%notice:en
	If an invalid offset is given, an emptystring will be the result.
	%%notice:ge
	Wird ein fehlerhafter Positionswert übergeben ist der Rückgabewert
	der Funktion ein Leerstring.

RBDOC*/

RB_FCT( getcharat )
{
/*RBAUTOIMPORT function getcharat vv*/

	char		temp	[ 4 + 1 ];
	char*		str;
	u_long		cnt;

	PROC( "getcharat" );
	RB_FCT_DUMP_PARMS();

	str = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "str", "%s", str );

	cnt = RB_PARM_VAL_GET_ULONG( RB_FCT_PARM_ACCESS( 1 ) );
	VARS( "cnt", "%ld", cnt );

#ifdef UTF8
	for( cnt--; *str && cnt; str += u8_seqlen( str ), cnt-- )
		;
#else
	for( cnt--; *str && cnt; str++, cnt-- )
		;
#endif

	if( cnt >= 0 && cnt < pstrlen( str ) )
		sprintf( temp, "%c", str[ cnt ] );
	else
		*temp = '\0';

	VARS( "temp", "%s", temp );

	RB_PARM_VAL_SET_STR( RB_FCT_RET, pstrdup( temp ) );
	RETURN( 0 );
}

