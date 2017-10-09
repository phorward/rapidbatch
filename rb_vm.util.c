/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_vm.util.c
Author:	Jan Max Meyer
Usage:	Virtual machine utility functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
/* #define __WITH_TRACE */
#include "rb_global.h"

/*
 * Global variables
 */
static byte parm_size[]	=
	{
		(char)sizeof( long ),			/* PUSHINT */
		(char)sizeof( vm_addr ),		/* PUSHADR */
		(char)sizeof( char ),			/* PUSHCHR */
		(char)sizeof( double ),			/* PUSHDBL */
		(char)sizeof( char* ),			/* PUSHSTR */
		(char)sizeof( vm_addr ),		/* RESERVE */
		(char)sizeof( vm_addr ),		/* JMP */
		(char)sizeof( vm_addr ),		/* JMPTRUE */
		(char)sizeof( vm_addr ),		/* JMPFALSE */
		(char)sizeof( vm_addr ),		/* CALL */
		(char)sizeof( rb_native_func ),	/* NCALL */
		(char)sizeof( vm_addr ),		/* DROP */
		(char)sizeof( rb_native_var ),	/* NSTORE */
		(char)sizeof( rb_native_var )	/* NLOAD */
	};

/*
 * Defines
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_convert_value()
	
	Author:			Jan Max Meyer
	
	Usage:			Converts a value of any type to another, specified type,
					if required.
					
	Parameters:		vm_val*		val				Pointer to value structure
					char		to_type			Type of conversion

	Returns:		pboolean						TRUE if the value has been
													converted
												FALSE if not
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean rb_vm_convert_value( vm_val* val, char to_type )
{
	char*		ptr;
	char		tmp_s	[ 32 ];

	PROC( "rb_vm_convert_value" );
	PARMS( "val", "%p", val );
	PARMS( "to_type", "%d", to_type );
	
	VARS( "val->type", "%d", val->type );
	switch( to_type )
	{
		case VAL_STR:
			switch( val->type )
			{
				case VAL_LONG:
					sprintf( tmp_s, "%ld", val->value.vlong );
					ptr = tmp_s;
					break;
	
				case VAL_ADDR:
					sprintf( tmp_s, "%ld", val->value.vaddr );
					ptr = tmp_s;
					break;
					
				case VAL_DBL:
					sprintf( tmp_s, "%lf", val->value.vdbl );
					VARS( "tmp_s", "%s", tmp_s );
					
					for( ptr = tmp_s + pstrlen( tmp_s ) - 1;
							ptr > tmp_s; ptr-- )
					{
						if( *ptr == '0' )
							*ptr = '\0';
						else if( *ptr == '.' )
						{
							*ptr = '\0';
							break;
						}
					}
					
					ptr = tmp_s;
					break;
					
				case VAL_CSTR:
					ptr = val->value.vcstr;
					break;
					
				default:
					MSG( "No conversion required" );
					RETURN( FALSE );
			}

			VARS( "ptr", "%s", ptr );

			val->type = VAL_STR;
			if( !( val->value.vstr.str = pstrdup( ptr ) ) )
				RB_OUT_OF_MEMORY;
			val->value.vstr.len = pstrlen( ptr );

			break;
		
		case VAL_LONG:
			switch( val->type )
			{
				case VAL_ADDR:
					val->value.vlong = (long)val->value.vaddr;
					break;
					
				case VAL_DBL:
					val->value.vlong = (long)val->value.vdbl;
					break;
				
				case VAL_STR:
					ptr = val->value.vstr.str;
					val->value.vlong = atol( val->value.vstr.str );
					pfree( ptr );
					break;

				case VAL_CSTR:
					val->value.vlong = atol( val->value.vcstr );
					break;
				
				default:
					MSG( "No conversion required" );
					RETURN( FALSE );
			}
			break;
			
		case VAL_ADDR:
			switch( val->type )
			{
				case VAL_LONG:
					val->value.vaddr = (vm_addr)val->value.vlong;
					break;
					
				case VAL_DBL:
					val->value.vaddr = (vm_addr)val->value.vdbl;
					break;
				
				case VAL_STR:
					val->value.vaddr = (vm_addr)atol( val->value.vstr.str );
					pfree( val->value.vstr.str );
					break;

				case VAL_CSTR:
					val->value.vaddr = (vm_addr)atol( val->value.vcstr );
					break;
				
				default:
					MSG( "No conversion required" );
					RETURN( FALSE );
			}
			break;
			
		case VAL_DBL:
			switch( val->type )
			{
				case VAL_LONG:
					val->value.vdbl = (double)val->value.vlong;
					break;
				
				case VAL_ADDR:
					val->value.vdbl = (double)val->value.vaddr;
					break;
				
				case VAL_STR:
					val->value.vdbl = atof( val->value.vstr.str );
					pfree( val->value.vstr.str );
					break;

				case VAL_CSTR:
					val->value.vdbl = atof( val->value.vcstr );
					break;
				
				default:
					MSG( "No conversion required" );
					RETURN( FALSE );
			}
			break;
			
		case VAL_CSTR:
		default:
			MSG( "No conversion supported for this type" );
			RETURN( FALSE );
	}
	
	MSG( "Data type successfully converted" );
	val->type = to_type;

	RETURN( TRUE );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_optimal_value()
	
	Author:			Jan Max Meyer
	
	Usage:			Tests a string if it can be converted to long or double, and
					performs this conversion.

	Parameters:		vm_val*		val				Pointer to value structure

	Returns:		pboolean						TRUE if the string has been
													converted
												FALSE if not
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean rb_vm_optimal_value( vm_val* val )
{
	char*	str;
	int		v[2];
	char	dot, empty;
	
	PROC( "rb_vm_optimal_value" );
	PARMS( "val", "%p", val );
	
	VARS( "val->type", "%d", val->type );
	switch( val->type )
	{
		case VAL_STR:
			str = val->value.vstr.str;
			break;

		case VAL_CSTR:
			str = val->value.vcstr;
			break;
			
		default:
			MSG( "This is not a string" );
			RETURN( TRUE );
	}

	VARS( "str", "%s", str );

	switch( sscanf( str, "%d%c%d%c", &( v[0] ), &dot, &( v[1] ), &empty ) )
	{
		/* One match? This can be converted to integer! */
		case 1:
			MSG( "One match - converting to long" );
			val->type = VAL_LONG;
			val->value.vlong = v[0];
			break;
			
		/* Three matches? This can be converted to double! */
		case 3:
			if( dot != '.' )
			{
				MSG( "No, this is not the double I thought it was" );
				RETURN( FALSE );
			}
			
			if( v[1] != 0 )
			{
				MSG( "Three correct matches - converting to double" );
				val->type = VAL_DBL;
				val->value.vdbl = atof( str );
			}
			else
			{
				MSG( "Three correct matches - but 0 beyond the decimalpoint"
						"This will become long!" );
				val->type = VAL_LONG;
				val->value.vlong = v[0];
			}
			break;
		
		/* Everything else - no convert! */
		default:
			MSG( "This can not be converted" );
			RETURN( FALSE );
	}

	if( val->type == VAL_STR )
		pfree( str );

	RETURN( TRUE );	
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_get_parm_size()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the parameter value byte size a parameter-instruction
					can provide. Returns 0 for all parameter-less VM instruc-
					tions.
					
	Parameters:		vm_code			instr		Instruction code

	Returns:		char						Size in bytes to be returned
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char rb_vm_get_parm_size( vm_code instr )
{
	PROC( "rb_vm_get_parm_size" );
	VARS( "instr", "%d", instr );
	
	if( !( instr >= CCLASS_PARAMETIZED && instr < CCLASS_PARAMETIZED * 2 ) )
		RETURN( 0 );
		
	VARS( "parm_size", "%d", parm_size[
			( instr & ( 0xFF ^ CCLASS_PARAMETIZED ) ) ] );
	RETURN( parm_size[ ( instr & ( 0xFF ^ CCLASS_PARAMETIZED ) ) ] );
}
