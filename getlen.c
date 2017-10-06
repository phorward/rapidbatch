/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2009 by Phorward Software Technologies
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com

File:		getlen.c
Author:		Jan Max Meyer
Usage:		Implements the function GETLEN.
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
	getlen( str )
	%%desc:en
	Returns the length (number of characters) of a string '''str'''.
	%%desc:ge
	Gibt die Länge (Anzahl der Zeichen) des Strings '''str''' zurück.

	%%parm:en
	str		The string, which length should be retrieved.
	%%parm:ge
	str		Der String, dessen Länge bestimmt werden soll.

	%%return:en
	The length (number of characters) of str.
	%%return:ge
	Die Länge (Anzahl der Zeichen) von str.

	%%notice:en
	%%notice:ge

RBDOC*/

RB_FCT( getlen )
{
/*RBAUTOIMPORT function getlen v*/

	char*		str;

	PROC( "getlen" );
	RB_FCT_DUMP_PARMS();

	str = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "str", "%s", str );

#ifdef UTF8
	RB_PARM_VAL_SET_LONG( RB_FCT_RET, ( (unsigned long)u8_strlen( str ) ) );
	VARS( "Length", "%ld", u8_strlen( str ) );
#else
	RB_PARM_VAL_SET_LONG( RB_FCT_RET, ( (unsigned long)pstrlen( str ) ) );
	VARS( "Length", "%ld", pstrlen( str ) );
#endif

	RETURN( 0 );
}

