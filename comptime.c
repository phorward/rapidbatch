/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	comptime.c
Author:	Jan Max Meyer
Usage:	Compile-time functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */

#include "rb_global.h"

/*
 * Global variables
 */
struct srcfile
{
	char*		filename;				/* Name of file */
	char*		content;				/* File content */
};

static	struct srcfile*		all_incl;
static	uint				all_incl_cnt;

#define SRCFILE_MALLOC_STEP			16	/* Hold 16 files in line */

/*
 * Functions
 */

/* Free up this module */

/* Execution function to include a file */
static int include_file( char* filename, boolean once )
{
	uint	i;
	char*	src		= (char*)NULL;
	struct
	srcfile*	sf	= (struct srcfile*)NULL;

	PROC( "include_file" );
	PARMS( "filename", "%s", filename );
	PARMS( "once", "%d", once );

	for( i = 0; i < all_incl_cnt; i++ )
	{
		if( strcmp( all_incl[ i ].filename, filename ) == 0 )
		{
			MSG( "This file has already been included... check" );
			if( once )
			{
				MSG( "once flag set - no more include of this file!" );
				RETURN( 0 );
			}

			sf = &( all_incl[ i ] );
			break;
		}
	}

	if( !sf )
	{
		MSG( "File has not been included yet, mapping now" );
		if( !pfiletostr( &src, filename ) )
		{
			MSG( "Printing file-not-found error" );

			/*
			rb_error( rb_comp_cur_pos(),
				"include_not_found", "filename", filename,
					(char*)NULL );
			*/
			fprintf( stderr, "Include file not found: %s\n", filename );

			RETURN( 0 ); /* Compiler gets: All right ;) */
		}

		/* Put this file into cache */
		if( !all_incl )
		{
			MSG( "Allocating new array" );
			all_incl = (struct srcfile*)pmalloc( ( SRCFILE_MALLOC_STEP )
							* sizeof( struct srcfile ) );
		}
		else if( all_incl_cnt % SRCFILE_MALLOC_STEP == 0 )
		{
			MSG( "Extending existing array" );
			all_incl = (struct srcfile*)prealloc( (struct srcfile*)all_incl,
						( all_incl_cnt + SRCFILE_MALLOC_STEP )
							* sizeof( struct srcfile ) );
		}

		if( !all_incl )
		{
			RB_OUT_OF_MEMORY;
			RETURN( 1 );
		}

		/* Save pointers */
		sf = &( all_incl[ all_incl_cnt++ ] );
		sf->content = src;

		if( !( sf->filename = pstrdup( filename ) ) )
		{
			RB_OUT_OF_MEMORY;
			RETURN( 1 );
		}
	}

	rb_comp_compile( sf->filename, sf->content );
		/* Number of errors does not matter here */

	RETURN( 0 );
}

/*
 * RapidBATCH Functions
 */

RB_FCT( include )
{
/*RBAUTOIMPORT function include v comptime*/

/*RBDOC

	%%function
	include( file )
	%%desc:en
	TODO
	%%desc:ge
	TODO

	%%parm:en
	str		Relative or absolute file path to the file to be included.
	%%parm:ge
	str		Der relative oder absolute Dateipfad zur einzubindenden Datei.

	%%return:en
	%%return:ge

	%%notice:en
	%%notice:ge

RBDOC*/
	char*	filename;

	PROC( "include" );
	RB_FCT_DUMP_PARMS();

	filename = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	PARMS( "filename", "%s", filename );

	RETURN( include_file( filename, FALSE ) );
}

RB_FCT( include_once )
{
/*RBAUTOIMPORT function include_once v comptime*/

/*RBDOC

	%%function
	include_once( file )
	%%desc:en
	TODO
	%%desc:ge
	TODO

	%%parm:en
	str		Relative or absolute file path to the file to be included.
	%%parm:ge
	str		Der relative oder absolute Dateipfad zur einzubindenden Datei.

	%%return:en
	%%return:ge

	%%notice:en
	%%notice:ge

RBDOC*/
	char*	filename;

	PROC( "include_once" );
	RB_FCT_DUMP_PARMS();

	filename = RB_PARM_VAL_GET_STR( RB_FCT_PARM_ACCESS( 0 ) );
	PARMS( "filename", "%s", filename );

	RETURN( include_file( filename, TRUE ) );
}

