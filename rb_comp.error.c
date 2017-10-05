/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_comp.error.c
Author:	Jan Max Meyer
Usage:	Compiler error reporting
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "rb_global.h"
#include "rb_comp.h"
#include "rb_vm.h"
#ifndef MAKE_PROTOTYPES
#include "rb_proto.h"
#endif

/*
 * Global variables
 */
static XML_T	errors;

/*
 * Functions
 */


/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_error()
	
	Author:			Jan Max Meyer
	
	Usage:			Compile-time error reporting function.
					
	Parameters:		uchar*				file		Filename
					int					line		Line number in filename
					uchar*				id			Error message catalog
													identifier
					...								Parameter pairs of uchar*:
													- Wildcard-Identifier
													- Value
													
													Last parameter must be
													(uchar*)NULL;

	Returns:		void
							
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_comp_error( srcpos* pos, uchar* id, ... )
{
	struct
	{
		uchar	wildcard[ 80 + 1 ];
		uchar*	value;
		uchar*	_match;
	} values[64];

	XML_T	ident;

	va_list	args;
	int		i;
	int		vcount;
	int		match;
	uchar*	tpl_ptr;
	uchar*	result			= (uchar*)NULL;
	long	copy_size;
	long	prev_size;
	long	size			= 0L;

	PROC( "rb_error" );
	PARMS( "pos", "%p", pos );
	PARMS( "id", "%s", id );

	va_start( args, id );

	for( vcount = 0; vcount < 64; vcount++ )
	{
		if( !( values[vcount].value = va_arg( args, uchar* ) ) )
			break;
		
		sprintf( values[vcount].wildcard, "##%.78s", values[vcount].value );
		values[vcount].value = va_arg( args, uchar* );

		if( !values[vcount].value )
			values[vcount].value = pstrdup( "" );
	}

	/* Do I already have a language catalogue? */
	if( !errors )
	{
		if( !( errors = xml_parse_file( "errors.xml" ) ) )
		{
			MSG( "Can't read error message catalogue" );
			VOIDRET;
		}
	}
	
	/* Find error message */
	for( ident = xml_child( errors, "msg" ); ident; ident = xml_next( ident ) )
	{
		if( !pstrcmp( (uchar*)xml_attr( ident, "id" ), id ) )
		{
			tpl_ptr = xml_txt( ident );

			do
			{
				for( i = 0; i < vcount; i++ )
					values[i]._match = strstr( tpl_ptr, values[i].wildcard );
		
				match = vcount;
				for( i = 0; i < vcount; i++ )
				{
					if( values[i]._match != (uchar*)NULL )
					{
						if( match == vcount ||
								values[match]._match > values[i]._match )
							match = i;
					}
				}
		
				prev_size = size;
				if( match < vcount )
				{
					copy_size = (long)( values[match]._match - tpl_ptr );
					size += (long)strlen( values[match].value );
				}
				else
					copy_size = (long)strlen( tpl_ptr );
		
				size += copy_size;
		
				if( result )
					result = (uchar*)prealloc( (uchar*)result, 
						( size + 1 ) * sizeof( uchar ) );
				else
					result = (uchar*)pmalloc( ( size + 1 ) * sizeof( uchar ) );
		
				memcpy( result + prev_size, tpl_ptr,
					copy_size * sizeof( uchar ) );
		
				if( match < vcount )
					memcpy( result + prev_size + copy_size,
						values[match].value, strlen( values[match].value )
							* sizeof( uchar ) );
		
				result[size] = '\0';
		
				if( match < vcount )
					tpl_ptr += copy_size + strlen( values[match].wildcard );
			}
			while( match < vcount );
		
			va_end( args );
			
			/* Print error message */
			if( pos )
			{
				if( pos->filename )
				{
					fprintf( stderr, "%s, line %ld, error: %s\n",
						pos->filename, pos->line, result );
				}
				else
					fprintf( stderr, "line %ld, error: %s\n",
						pos->line, result );
			}
			else
			{
				fprintf( stderr, "error: %s\n", result );
			}
			fflush( stderr );

			pfree( result );
			break;
		}
	}
	
	if( !ident )
	{
		fprintf( stderr, "undefined error message: %s\n", id );
		for( i = 0; i < vcount; i++ )
		{
			fprintf( stderr, "  >%s< = >%s<\n",
				values[i].wildcard, values[i].value );
		}
	}

	VOIDRET;
}

