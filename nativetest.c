/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	nativetest.c
Author:	Jan Max Meyer
		Andreas Harbecke
Usage:	This is a test file for native function development;
		Some functions will be sorted out into other modules, soon.
----------------------------------------------------------------------------- */

/*
 * Includes
 */
 
/* #define __WITH_TRACE */
#include "rb_global.h"

/*
 * Global variables
 */

/*
 * Functions
 */
RB_FCT( getlen )
{
/*RBAUTOIMPORT function getlen v*/

/*RBDOC

	%%function
	getlen( str )
	%%desc:en
	Returns the length of characters in string '''str'''.
	%%desc:ge
	Gibt die Länge (=Anzahl der Zeichen) des Strings '''str''' zurück.

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

	char*		str;

	PROC( "getlen" );
	RB_FCT_DUMP_PARMS();
	
	str = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "str", "%s", str );

	RB_PARM_VAL_SET_LONG( RB_FCT_RET, ( (unsigned long)pstrlen( str ) ) );
		
	VARS( "Length", "%ld", pstrlen( str ) );
	
	RETURN( RB_ERR_OK );
}

RB_FCT( charat )
{
/*RBAUTOIMPORT function getcharat vv*/

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
	char		temp	[ 1 + 1 ];
	char*		str;
	long		cnt;

	PROC( "charat" );
	RB_FCT_DUMP_PARMS();
	
	str = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "str", "%s", str );
	
	cnt = RB_PARM_VAL_GET_LONG( RB_FCT_PARM_ACCESS( 1 ) ) - 1;
	VARS( "cnt", "%ld", cnt );
	
	if( cnt >= 0 && cnt < pstrlen( str ) )
		sprintf( temp, "%c", str[ cnt ] );
	else
		*temp = '\0';
		
	VARS( "temp", "%s", temp );

	RB_PARM_VAL_SET_STR( RB_FCT_RET, pstrdup( temp ) );
	RETURN( RB_ERR_OK );
}

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
	
	RETURN( RB_ERR_OK );
}

RB_FCT( getchr )
{
	long	asciiCode;
	char	asciiSign [ 1 + 1 ];
	
	PROC( "getchr" );
	RB_FCT_DUMP_PARMS();
	
	asciiCode = RB_PARM_VAL_GET_LONG( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "asciiCode", "%ld", asciiCode);
	
	sprintf( asciiSign, "%c", (char) asciiCode );
	
	RB_PARM_VAL_SET_STR( RB_FCT_RET, pstrdup( asciiSign ) );
	RETURN( RB_ERR_OK );
}

RB_FCT( getasc )
{
	char*	asciiSign;
	
	PROC( "getasc" );
	RB_FCT_DUMP_PARMS();
	
	asciiSign = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "asciiSign", "%s", asciiSign);
	
	RB_PARM_VAL_SET_LONG( RB_FCT_RET, (long) *asciiSign );
	RETURN( RB_ERR_OK );
}

RB_FCT( getpos )
{
	char*	sourceString;
	char*	targetString;
	char*	tempString;
	long	position;
	
	PROC( "getpos" );
	RB_FCT_DUMP_PARMS();
	
	sourceString = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "sourceString", "%s", sourceString );
	
	targetString = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 1 ) );
	VARS( "targetString", "%s", targetString );
	
	position = RB_PARM_VAL_GET_LONG( RB_FCT_PARM_ACCESS( 2 ) );
	VARS( "position", "%ld", position);

	for( tempString = sourceString - 1;
		position > 0 && tempString; position-- )
		tempString = strstr( ++tempString, targetString );
	
	if( tempString )
		RB_PARM_VAL_SET_LONG( RB_FCT_RET, tempString - sourceString + 1 );
	else
		RB_PARM_VAL_SET_LONG( RB_FCT_RET, 0 );
	
	RETURN( RB_ERR_OK );
}

RB_FCT( lowvar )
{
	char*	ptr;
	char*	sourceString;
	
	PROC( "lowvar" );
	RB_FCT_DUMP_PARMS();
	
	sourceString = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "sourceString", "%s", sourceString );

	for( ptr = sourceString; *ptr; ptr++ )
	{
		if( *ptr >= 'A' && *ptr <= 'Z' )
			*ptr += 32;
	}
	
	RB_PARM_VAL_SET_STR( RB_FCT_RET, pstrdup( sourceString ) );
	RETURN( RB_ERR_OK );
}

RB_FCT( upvar )
{
	char*	ptr;
	char*	sourceString;
	
	PROC( "upvar" );
	RB_FCT_DUMP_PARMS();
	
	sourceString = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "sourceString", "%s", sourceString );

	for( ptr = sourceString; *ptr; ptr++ )
	{
		if( *ptr >= 'a' && *ptr <= 'z' )
			*ptr -= 32;
	}
	
	RB_PARM_VAL_SET_STR( RB_FCT_RET, pstrdup( sourceString ) );
	RETURN( RB_ERR_OK );
}

RB_FCT( replacevar )
{
/*RBAUTOIMPORT function replacevar vvv*/
	char*	str;
	char*	find;
	char*	replace;
	
	PROC( "lowvar" );
	RB_FCT_DUMP_PARMS();
	
	str = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	find = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 1 ) );
	replace = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 2 ) );
	VARS( "str", "%s", str );
	VARS( "find", "%s", find );
	VARS( "replace", "%s", replace );
	
	str = pstr_replace( str, find, replace );
	
	RB_PARM_VAL_SET_STR( RB_FCT_RET, str );
	RETURN( RB_ERR_OK );
}

RB_FCT( trimvar )
{
/*RBAUTOIMPORT function trimvar v*/

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

	char*	bgnstr;
	char*	endstr;
	char*	sourceString;
	
	PROC( "trimvar" );
	RB_FCT_DUMP_PARMS();
	
	sourceString = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	VARS( "sourceString", "%s", sourceString );
	
	TIMEDUMP;

	bgnstr = sourceString;
	while( *bgnstr == ' ' || *bgnstr == '\t' )
		bgnstr++;
	
	endstr = pstrlen ( bgnstr ) + bgnstr - 1;
	while( *endstr == ' ' || *endstr == '\t' )
		endstr--;

	*(endstr + 1) = '\0';

	TIMEDUMP;
		
	RB_PARM_VAL_SET_STR( RB_FCT_RET, pstrdup( bgnstr ) );
		
	RETURN( RB_ERR_OK );
}


/* Test variable [test] ---------------------------------------------------- */
RB_VARFCT( get_test )
{
	/*RBAUTOIMPORT variable-get test*/
	PROC( "get_test" );
	RB_VARFCT_DUMP_PARMS();
	
	RB_PARM_VAL_SET_STR( RB_VARFCT_VAL,
		pstrdup( "Hello World, das ist ausm System :D" ) );

	RETURN( RB_ERR_OK );
}

RB_VARFCT( set_test )
{
	/*RBAUTOIMPORT variable-set test*/
	PROC( "set_test" );
	RB_VARFCT_DUMP_PARMS();
	
	printf( "My var value >%s<\n", RB_PARM_VAL_GET_STR( RB_VARFCT_VAL ) );

	RETURN( RB_ERR_OK );
}


/*RBDOC

	%%variable
	[test]
	%%desc:en
	Test variable for the development version.
	%%desc:ge
	Dies ist die Testvariable der RB6 Entwicklungsversion.
	
	%%notice:en
	%%notice:ge

RBDOC*/


#if 0
RB_FCT( include )
{
/*RBAUTOIMPORT function include v*/
	char*	filename;
	srcfile	file;
	
	PROC( "compiletime_test" );
	RB_FCT_DUMP_PARMS();
	
	filename = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );

	switch( rb_comp_read_file( &file, filename ) )
	{			
		case RB_ERR_OK:
			break;
			
		default:
		case 1:
			RETURN( 1 );
			break;
	}
	
	
	/* printf( "include >%s< >%s<\n", filename, file.content ); */
		
	rb_comp_compile( file.filename, file.content );
	
	//pfree( file.filename );
	//pfree( file.content );

	RETURN( RB_ERR_OK );
}
#endif


/*
	This is a monumental: The first native %%function
	ever called by the RapidBATCH VM! Keep it, please! ;)

int rb_NATIVE_strlen_TEST( vm_stackitem* ret,
	vm_stackitem* parms, vm_addr parms_cnt )
{
	char*		mystr;
	
	PROC( "rb_NATIVE_strlen_TEST" );
	PARMS( "ret", "%p", ret );
	PARMS( "parms", "%p", parms );
	PARMS( "parms_cnt", "%ld", parms_cnt );
	
	mystr = ITEM_VAL_GET_STR( parms );
	VARS( "mystr", "%s", mystr );
	
	ITEM_VAL_SET_LONG( ret, pstrlen( mystr ) );
		
	RETURN( RB_ERR_OK );
}
*/
