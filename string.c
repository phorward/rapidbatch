/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	string.c
Author:	Jan Max Meyer
		Andreas Harbecke
Usage:	String modification functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */

/* #define __WITH_TRACE */
#include "rb_global.h"
#include "rb_native.h"

/*
 * Global variables
 */

/*
 * Functions
 */

RB_FCT( print )
{
/*RBAUTOIMPORT procedure print v+*/

/*RBDOC

	%%procedure:en
	print( output^1^ [, output^n^ ] )
	%%procedure:ge
	print( ausgabe^1^ [, ausgabe^n^] )

	%%desc:en
	Outputs expressions to stdout.\n
	Each expression '''output''' will be in a single new line each.
	%%desc:ge
	Jeder Ausdruck '''ausgabe''' wird in jeweils in einer einzelnen Zeile ausgegeben.

	%%parm:en
	output		An arbitrary expression to be printed.
	%%parm:ge
	output		Ein beliebiger Ausdruck, der ausgegeben werden soll.

	%%notice:en
	%%notice:ge

RBDOC*/
	char*	str;
	int		i;

	PROC( "print" );
	RB_FCT_DUMP_PARMS();

	for( i = 0; i < RB_FCT_PARM_COUNT(); i++ )
	{
		str = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( i ) );
		VARS( "str", "%s", str );

		printf( "%s\n", str );
	}

	RETURN( 0 );
}

