/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_comp.symtab.c
Author:	Jan Max Meyer
Usage:	Symbol table management functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "rb_comp.h"
#ifndef MAKE_PROTOTYPES
#include "rb_proto.h"
#endif

/*
 * Global variables
 */
static symbol*		symtab[ SYMTAB_SIZE ];

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		pfree_symbol()
	
	Author:			Jan Max Meyer
	
	Usage:			Frees a symbol.
					
	Parameters:		symbol*		sym				Symbol pointer to be freed.
	
	Returns:		symbol*						Always (symbol*)NULL
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
static symbol* pfree_symbol( symbol* sym )
{
	if( !sym )
		return (symbol*)NULL;
		
	if( sym->type == SYM_VAR && sym->constant )
		pfree( sym->offset.constant );
	
	pfree( sym->name );
	pfree( sym );

	return (symbol*)NULL;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_symtab_engage()
	
	Author:			Jan Max Meyer
	
	Usage:			Engages/inserts a symbol pointer into the symbol database.
					
	Parameters:		symbol*		sym				The symbol to be inserted.
	
	Returns:		int							The bucket index of the symbol
												table that got the entry.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
static int rb_symtab_engage( symbol* sym )
{
	symbol*		link;
	symbol*		last;
	int			idx;
	
	PROC( "rb_symtab_engage" );
	PARMS( "sym", "%p", sym );

	idx = hash_from_str( sym->name, SYMTAB_SIZE );

	/* If no symbol table exists, yet... */
	if( !( link = symtab[ idx ] ) )
	{
		symtab[ idx ] = sym;
		RETURN( idx );
	}

	/* Insert symbol with same name before existing item, if available! */
	for( last = (symbol*)NULL; link; link = link->next )
	{
		if( !strcmp( link->name, sym->name ) )
		{
			sym->next = link;

			if( last )
				last->next = sym;
			else
				symtab[ idx ] = sym;

			RETURN( idx );
		}

		last = link;
	}

	/* Ok, then add this to the last one! */
	last->next = sym;
	
	RETURN( idx );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_symtab_drop()
	
	Author:			Jan Max Meyer
	
	Usage:			Drops a symbol pointer from the symbol database.
					
	Parameters:		symbol*		sym				The symbol to be dropped.
					boolean		drop_sym		If true, the sym pointer is
												freed.

	Returns:		boolean						TRUE, if the symbol was dropped,
												FALSE if ot could not be found.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
static BOOLEAN rb_symtab_drop( symbol* sym, boolean drop_sym )
{
	symbol*		prev	= (symbol*)NULL;
	symbol*		link;
	int			idx;
	
	PROC( "rb_symtab_drop" );
	PARMS( "sym", "%p", sym );
	PARMS( "drop_sym", "%d", drop_sym );
	
	if( !sym )
		RETURN( FALSE );

	/* Get hash table index */
	idx = hash_from_str( sym->name, SYMTAB_SIZE );
	VARS( "idx", "%d", idx );

	for( link = symtab[ idx ]; link; prev = link, link = link->next )
	{
		if( link == sym )
		{
			if( !prev )
				symtab[ idx ] = link->next;
			else
				prev->next = link->next;
				
			if( drop_sym )
				pfree_symbol( sym );

			RETURN( TRUE );
		}
	}
	
	if( drop_sym )
		pfree_symbol( sym );
	
	RETURN( FALSE );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_symtab_find()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns a symbol with the same name and type from the
					symbol table, or creates a new symbol table entry and
					returns this entry.
					
	Parameters:		char*		name			The symbol's identifying name
					symtype		type			The symbol type
	
	Returns:		symbol*						Pointer to the symbol,
												(symbol*)NULL if the symbol
												does not exist.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
symbol* rb_symtab_find( char* name, symtype type )
{
	symbol*		link;

	PROC( "rb_symtab_find" );
	PARMS( "name", "%s", name );
	PARMS( "type", "%d", type );
	
	if( !( link = symtab[ hash_from_str( name, SYMTAB_SIZE ) ] ) )
	{
		MSG( "found nothing" );
		RETURN( (symbol*)NULL );
	}

	for( ; link; link = link->next )
	{
		if( !strcmp( link->name, name ) && link->type == type )
			RETURN( link );
	}
	
	MSG( "found nothing" );
	RETURN( (symbol*)NULL );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_symtab_find_in_scope()
	
	Author:			Jan Max Meyer
	
	Usage:			Searches for a symbol with the given name and type within
					a scope, that is defined by begin.
					
	Parameters:		symbol*		begin			Begin node of the scope.
					char*		name			The symbol's identifying name
					symtype		type			The symbol type
	
	Returns:		symbol*						Pointer to the symbol,
												(symbol*)NULL if the symbol
												does not exist in the scope.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
symbol* rb_symtab_find_in_scope( symbol* begin, char* name, symtype type )
{
	PROC( "rb_symtab_find_in_scope" );
	PARMS( "begin", "%p", begin );
	PARMS( "name", "%s", name );
	PARMS( "type", "%d", type );

	for( ; begin; begin = begin->next )
	{
		if( !strcmp( begin->name, name ) && begin->type == type )
		{
			MSG( "symbol found!" );
			RETURN( begin );
		}
	}
	
	MSG( "found nothing" );
	RETURN( (symbol*)NULL );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_symtab_new()
	
	Author:			Jan Max Meyer
	
	Usage:			Creates a new symbol table entry and inserts it into the
					symbol database.
					
	Parameters:		char*		name			The symbol's identifying name
					symtype		type			The symbol type
	
	Returns:		symbol*						Pointer to the desired symbol,
												(symbol*)NULL if the symbol
												could not be created.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
symbol* rb_symtab_new( char* name, symtype type )
{
	symbol*		sym;
	
	PROC( "rb_new_symbol" );
	PARMS( "name", "%s", name );
	PARMS( "type", "%d", type );
		
	MSG( "Creating new symbol" );
	
	if( !( sym = (symbol*)pmalloc( sizeof( symbol ) ) ) )
		RETURN( (symbol*)NULL );

	memset( sym, 0, sizeof( symbol ) );
	sym->name = pstrdup( name );
	sym->type = type;
	
	MSG( "Engaging symbol" );
	rb_symtab_engage( sym );

	RETURN( sym );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_symtab_engage_scope()
	
	Author:			Jan Max Meyer
	
	Usage:			Engages a symbol to a scope. The scope is defined by the
					begin node. If begin is (symbol*)NULL, engage becomes the
					leading node of the scope.
					
	Parameters:		symbol*		begin			The scope begin node
					symbol*		engange			The new symbol node to be
												engaged into the scope
	
	Returns:		symbol*						Always the pointer to the
												scope's beginning node.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
symbol* rb_symtab_engage_scope( symbol* begin, symbol* engage )
{
	symbol*		sym;
	
	PROC( "rb_symtab_engage_scope" );
	PARMS( "begin", "%p", begin );
	PARMS( "engage", "%p", engage );
	
	if( !begin )
	{
		MSG( "Defining begin of scope" );
		begin = engage;
	}
	else
	{
		MSG( "Moving to end of scope list" );
		for( sym = begin; sym->scope_next; sym = sym->scope_next )
			;
		
		sym->scope_next = engage;
	}

	RETURN( begin );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_symtab_drop_scope()
	
	Author:			Jan Max Meyer
	
	Usage:			Drops an entire scope.
					
	Parameters:		symbol*		begin			The scope begin node
					boolean		drop_sym		Drop the symbol's memory, if
												TRUE.
	
	Returns:		symbol*						Always a pointer to
												(symbol*)NULL

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
symbol* rb_symtab_drop_scope( symbol* begin, boolean drop_sym )
{
	symbol*	del;
	
	PROC( "rb_symtab_drop_scope" );
	PARMS( "begin", "%p", begin );
	PARMS( "drop_sym", "%p", drop_sym );

	while( ( del = begin ) )
	{
		begin = begin->scope_next;
		rb_symtab_drop( del, drop_sym );
	}
	
	RETURN( (symbol*)NULL );
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_symtab_scope_pos()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the position of a symbol in the scope.
					This is required to locate variable entry points
					and push their address onto the stack, e.g. at
					var-calls.
					
	Parameters:		symbol*		begin			The scope begin node
					symbol*		find			The symbol pointer to find
	
	Returns:		int							The scope length

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int rb_symtab_scope_pos( symbol* begin, symbol* find )
{
	int	pos		= 0;

	for( ; begin; begin = begin->scope_next, pos++ )
		if( begin == find )
			return pos;
	
	return -1;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_symtab_dump()
	
	Author:			Jan Max Meyer
	
	Usage:			Dumps the symbol table content.
					
	Parameters:		FILE*		stream			Output stream. If (FILE*)NULL,
												stderr will be used.
	
	Returns:		void

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_symtab_dump( FILE* stream )
{
	int		i;
	symbol*	sym;
	symbol* parm;
	LIST*	l;
	
	if( !stream )
		stream = stderr;
		
	for( i = 0; i < SYMTAB_SIZE; i++ )
	{
		if( ! symtab[i] )
			continue;
			
		fprintf( stream, "bucket %02d  ", i );

		for( sym = symtab[i]; sym; sym = sym->next )
		{
			if( sym != symtab[i] )
				fprintf( stream, "           " );
			
			if( !( sym->native ) )
			{			
				if( sym->used )
				{
					if( !( sym->defined ) )
						fprintf( stream, "[implicit] " );
				}
				else
					fprintf( stream, "[unused] " );
			}
			else
				fprintf( stream, "[native] " );

			switch( sym->type )
			{
				case SYM_VAR:
					fprintf( stream, "%sVariable %s (offset %ld, %s scope)",
						sym->pointer ? "Pointer-" : "", sym->name,
							sym->offset._long, sym->global ?
								"global" : "local" );
					break;
			
				case SYM_PROC:
					fprintf( stream, "%s %s",
						sym->function ? "Function" : "Procedure",
							sym->name );
					fprintf( stream, "\n           Signature >%s<",
								sym->parmdef );

					for( l = sym->parmsym; l; l = list_next( l ) )
					{
						parm = (symbol*)list_access( l );

						fprintf( stream, "\n           Parameter [%s%s]",
							parm->pointer ? "*" : "", parm->name );
					}

					break;
				case SYM_LABEL:
					fprintf( stream, "Label %s", sym->name );
					break;
				
				default:
					fprintf( stream, "[Unknown symbol]" );
					break;
			}
			
			fprintf( stream, "\n" );
		}
	}
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_symtab_drop_all()
	
	Author:			Jan Max Meyer
	
	Usage:			Drop entire symbol table database.
					
	Parameters:		keep_native				Keep natives
	
	Returns:		void

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_symtab_drop_all( boolean keep_native )
{
	int		i;
	symbol*	sym;
		
	for( i = 0; i < SYMTAB_SIZE; i++ )
	{
		if( ! symtab[i] )
			continue;
			
		for( sym = symtab[i]; sym; sym = sym->next )
		{
			if( sym->native && keep_native )
				continue;
			
			
			rb_symtab_drop( sym, TRUE );
		}
	}
}
