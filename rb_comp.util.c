/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_comp.util.c
Author:	Jan Max Meyer
Usage:	Utility functions for the RapidBATCH compiler module;
		This module works closely together with the semantic actions
		defined in the parser rb6.par, and is especially used for
		semantic code that needs to be run from different productions.
----------------------------------------------------------------------------- */

/*
 * Includes
 */
/* #define __WITH_TRACE 1 */
#include "rb_comp.h"
#ifndef MAKE_PROTOTYPES
#include "rb_proto.h"
#endif

/*
 * Global variables
 */
extern CINFO	cur;

/*
 * Functions
 */

/*
 	Semantic action function:
 	Create procedure/function symbol table entry
 	and calculate variable addresses
*/
/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_get_symbol()
	
	Author:			Jan Max Meyer
	
	Usage:			Retrieves a symbol, creates new ones, and fills some
					symbol-dependent informations.
					
	Parameters:		symbol*		ident			Identifier for the symbol
					symtype		type			The symbol type
					boolean		auto_create		Automatically create symbol
												if it does not exist
	
	Returns:		symbol*						pointer to symbol on success,
												(symbol*)NULL on failure.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
symbol* rb_comp_get_symbol( char* ident, symtype type, boolean auto_create )
{
	symbol*	sym;

	if( !( sym = rb_symtab_find( ident, type ) ) && auto_create )
	{
		if( !( sym = rb_symtab_new( ident, type ) ) )
		{
			RB_OUT_OF_MEMORY;
			return (symbol*)NULL;
		}
		
		/* If this is a new variable, add it to the
			current scope! */
		if( type == SYM_VAR )
		{
			sym->offset._long = rb_comp_get_var_count_of_scope(
				cur.scope );
			sym->global = cur.is_global_scope;
			
			cur.scope = rb_symtab_engage_scope(
				cur.scope, sym );
		}
	}
	
	return sym;
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_get_var_count_of_scope()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the number of variables within one scope, excluding
					parameter variables.
					
	Parameters:		symbol*		begin			The scope begin node
	
	Returns:		int							The scope length

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
long rb_comp_get_var_count_of_scope( symbol* begin )
{
	long	size	= 0L;

	/* Get highest address */
	for( ; begin; begin = begin->scope_next )
		if( begin->type == SYM_VAR && begin->offset._long >= 0 )
			size++;

	return size;
}

/*
	Semantic action function:
	Fill symbol table entry with procedure-related information
*/
symbol* rb_comp_proc_header( char* ident,
	boolean is_function, plist* parameters )
{
	char*		parmdef;
	symbol*		sym			= (symbol*)NULL;
	symbol* 	parm;
	long		parm_cnt;
	plistel*	e;
					
	sym = rb_comp_get_symbol( ident, SYM_PROC, TRUE );
	if( !( sym->defined ) )
	{
		memcpy( &( sym->pos ), &cur.stmt_begin,
			sizeof( srcpos ) );

		sym->function = is_function;
		sym->defined = TRUE;
		sym->parmsym = parameters;
		
		/* Build parameter signature */
		if( !( parmdef = sym->parmdef = (char*)pmalloc(
				( plist_count( parameters ) + 1 ) * sizeof( char ) ) ) )
		{
			RB_OUT_OF_MEMORY;
		}
		
		plist_for( parameters, e )
		{
			parm = plist_access( e );
			if( parm->pointer )
				*(parmdef++) = PARAM_BY_REF;
			else
				*(parmdef++) = PARAM_BY_VAL;
		}

		*parmdef = '\0';
		/* VARS( "parmdef", "%s", sym->parmdef ); */
		
		/* This is a place holder and the local variable
			allocation part */
		sym->offset._addr = rb_comp_next_addr( &cur.prog, VMC_RESERVE );
		
		parm_cnt = -( rb_comp_get_var_count_of_scope( cur.scope ) + 1 );
		for( parm = cur.scope; parm; parm = parm->scope_next )
			parm->offset._long = parm_cnt++;
	}
	else
		rb_comp_error( &( cur.stmt_begin ), "func_already_defined",
				"function", ident, (char*)NULL );
		
	pfree( ident );
	
	cur.is_global_scope = FALSE;
	cur.proc = sym;
	
	return sym;
}

/*
	Semantic action function:
	Complete procedure definition and reset local variable
	scope
*/
void rb_comp_proc_complete( symbol* proc,
	symbol* last_param, SCOPE_INFO* prev_scope, vm_addr jmp )
{
	int		size;
	
	if( last_param )
		size = rb_comp_get_var_count_of_scope( last_param->scope_next );
	else
		size = rb_comp_get_var_count_of_scope( cur.scope );

	rb_comp_patch( VM_GET_CODE( &cur.prog, proc->offset._addr ),
					VMC_RESERVE, &size );
	
	/* TODO: Proc call default return */
	rb_comp_proc_return( proc );

	/* Drop the scope parameters behind
		the last parameter */
	if( last_param )
	{
		last_param->scope_next = rb_symtab_drop_scope(
			last_param->scope_next, TRUE );
		
		rb_symtab_drop_scope( cur.scope, FALSE );
	}
	else
		rb_symtab_drop_scope( cur.scope, TRUE );
	
	/* Check for implicit label calls */
	rb_comp_backpatch_label_calls( (symbol*)NULL );
	
	/* Replace old variable scope */
	cur.scope = prev_scope->scope;
	cur.implicit_lbl_calls = prev_scope->implicit_lbl_calls;
	
	cur.is_global_scope = TRUE;
	cur.proc = (symbol*)NULL;
	
	/* Generate jump over procedure */
	rb_comp_patch_jmp( &cur.prog, VMC_JMP, jmp );
}

/*
	Semantic action function:
	Return from a procedure/function
*/
void rb_comp_proc_return( symbol* proc )
{
	PROC( "rb_comp_proc_return" );
	PARMS( "proc", "%p", proc );

	/* Avoid double return code generation */
	if( cur.prog.code[ cur.prog.cur_addr ] == VMC_RET
		|| cur.prog.code[ cur.prog.cur_addr ] == VMC_RETV )
	{
		MSG( "There is already a return!" );
		VOIDRET;
	}
	
	if( proc->function )
		rb_comp_gen( &cur.prog, VMC_RETV, (void*)NULL );
	else	
		rb_comp_gen( &cur.prog, VMC_RET, (void*)NULL );
		
	VOIDRET;
}


/*
	Semantic action function:
	Validates a procedure call and backpatches code
	according to the procedure's signature and
	address.
*/
int rb_comp_proc_call_perform( struct proc_call* pc, boolean free_call )
{
	int				error	= 0;
	int				i;
	plistel*		e;
	struct	param*	cpar;
	char			parm	= (char)0;
	
	PROC( "rb_comp_proc_call_perform" );
	PARMS( "pc", "%p", pc );
	PARMS( "free_call", "%s", BOOLEAN_STR( free_call ) );
	
	if( !( pc->proc->defined ) )
	{
		if( pc->as_function )
			rb_comp_error( &( pc->pos ), "func_undefined",
				"name", pc->proc->name, (char*)NULL );
		else
			rb_comp_error( &( pc->pos ), "proc_undefined",
				"name", pc->proc->name, (char*)NULL );
	}
	else
	{
		if( !( pc->proc->function ) && pc->as_function )
		{
			rb_comp_error( &( pc->pos ), "proc_called_as_func",
				"name", pc->proc->name, (char*)NULL );
		}
		
		for( e = plist_first( pc->param_signature ), i = 0;
				e; e = plist_next( e ) )
		{
			cpar = (struct param*)plist_access( e );
			
			parm = pc->proc->parmdef[ i ];
			if( !( parm == PARAM_VARG_VAL_ONLY
					|| parm == PARAM_VARG_ANY ) )
				i++;
			
			VARS( "cpar", "%p", cpar );
			VARS( "parm", "%c", parm );
			
			if( parm )
			{				
				if( parm == PARAM_BY_REF )
				{
					if( !( cpar->var ) )
					{
						char	num[ 10 + 1];

						sprintf( num, "%d", i+1 );
						rb_comp_error( &( pc->pos ), "pointer_required",
							"name", pc->proc->name, "parm", num, (char*)NULL );
					}
					else
					{
						MSG( "Patching pointer code" );

						/* Patch the code */
						rb_comp_patch( VM_GET_CODE( &cur.prog,
														cpar->push_code ),
							VMC_PTRLOAD, (void*)NULL );
					}
				}
				else if( cpar->ptrload )
				{
					MSG( "Patching pointer code / Copy variable" );
					rb_comp_patch( VM_GET_CODE( &cur.prog, cpar->push_code ),
							VMC_CPYLOAD, (void*)NULL );
				}
			}
			else
			{
				rb_comp_error( &( pc->pos ), "too_many_args",
					"name", pc->proc->name, (char*)NULL );
				
				break;
			}
		}
		
		if( !e && i < strlen( pc->proc->parmdef ) &&
				!( pc->proc->parmdef[ i ] == PARAM_VARG_VAL_ONLY
					|| pc->proc->parmdef[ i ] == PARAM_VARG_ANY ) )
		{
			rb_comp_error( &( pc->pos ), "too_less_args",
				"name", pc->proc->name, (char*)NULL );
		}
	}
	
	VARS( "pc->proc->native", "%d", pc->proc->native );
	VARS( "pc->proc->constant", "%d", pc->proc->constant );
		
	/* Call function/procedure */
	if( pc->proc->native )
	{
		MSG( "Generating native procedure call" );
		rb_comp_patch( VM_GET_CODE( &cur.prog, pc->call ),
			VMC_NCALL, &( pc->proc->offset.function ) );
	}
	else
	{
		MSG( "Generating procedure call" );
		rb_comp_patch( VM_GET_CODE( &cur.prog, pc->call ),
			VMC_CALL, &( pc->proc->offset._addr ) );
	}

	if( !( pc->as_function ) )
	{
		vm_addr one = 1;
		rb_comp_patch( VM_GET_CODE( &cur.prog, pc->drop ),
			VMC_DROP, &one );
	}
	
	/* Free used memory? */
	if( free_call )
	{
		MSG( "Freeing up memory" );
		
		plist_for( pc->param_signature, e )
		{
			cpar = (struct param*)plist_access( e );
			pfree( cpar );
		}

		plist_free( pc->param_signature );
	}
	
	RETURN( error );
}

/*
	Semantic action function:
	Backpatches all implicit procedure calls
	(if possible!) to a given procedure, or all.
	This function reports errors, when calls to
	undefined procedures exist.
*/
void rb_comp_backpatch_proc_calls( symbol* proc )
{
	plistel* 	e;
	struct 
	proc_call*	pc_ptr;
	
	PROC( "rb_comp_backpatch_proc_calls" );
	PARMS( "proc", "%p", proc );
	
	do
	{
		plist_for( cur.implicit_proc_calls, e )
		{
			pc_ptr = (struct proc_call*)plist_access( e );
			if( !proc || pc_ptr->proc == proc );
				break;
		}
		
		if( e )
		{
			rb_comp_proc_call_perform( pc_ptr, TRUE );
			
			plist_remove( cur.implicit_proc_calls,
				plist_get_by_ptr( cur.implicit_proc_calls, pc_ptr ) );
			pfree( pc_ptr );
		}
	}
	while( e );
	
	VOIDRET;
}

/*
	Semantic action function:
	Validates a procedure signature, or remembers
	signatures to be processed later.
*/
void rb_comp_proc_call( symbol* proc, boolean as_function,
	plist* params, vm_addr begin )
{
	struct 	proc_call*	pc_ptr;
	struct 	proc_call	pc;

	/* TODO Check for calls with more than 128 parameters!! */
	char				param_count = (char)plist_count( params );
	
	PROC( "rb_comp_proc_call" );
	PARMS( "proc", "%p", proc );
	PARMS( "params", "%p", params );
	
	proc->used = TRUE;
	
	/* TODO: Maybe this longer? */
	rb_comp_gen( &cur.prog, VMC_PUSHCHR, &param_count );
	
	pc.proc = proc;
	pc.param_signature = params;
	pc.as_function = as_function;
	pc.call = rb_comp_next_addr( &cur.prog, VMC_CALL );
	
	memcpy( &( pc.pos ), &cur.stmt_begin, sizeof( srcpos ) );
	
	if( !as_function )
		pc.drop = rb_comp_next_addr( &cur.prog, VMC_DROP );

	VARS( "proc->defined", "%d", proc->defined );
	if( !( proc->defined ) )
	{
		MSG( "It seems, that this procedure is not defined, yet" );
		MSG( "No problem here, we save it as implicit procedure call" );
		
		if( !( pc_ptr = (struct proc_call*)rb_memdup(
				&pc, sizeof( struct proc_call ) ) ) )
			RB_OUT_OF_MEMORY;
		
		if( !cur.implicit_proc_calls )
			cur.implicit_proc_calls = plist_create( 0, PLIST_MOD_PTR );
					
		plist_push( cur.implicit_proc_calls, (void*)pc_ptr );
	}
	else
	{
		MSG( "okidoki this procedure is defined, let's verify it!" );
		rb_comp_proc_call_perform( &pc, TRUE );
		
		if( !proc->patched )
			rb_comp_backpatch_proc_calls( proc );
		
		proc->patched = TRUE;
	
		if( proc->constant & RB_FUNC_AT_COMPTIME && proc->native )
		{
			if( !rb_comp_opt_test( cur.prog.code + begin, pc.call - begin ) )
			{
				if( proc->constant == RB_FUNC_AT_COMPTIME )
				{				
					rb_comp_error( &( pc.pos ), "compiletime_func_with_var_parm",
						"name", pc.proc->name, (char*)NULL );
					VOIDRET;
				}
			}
			else
			{	
				rb_vm_dump( stderr, &cur.prog, begin );
				rb_comp_opt_expr( &cur.prog, begin, as_function );
			}
		}
	}
	
	VOIDRET;
}

/*
	Semantic action function:
	Adds an address to an collection array for addresses
*/
void rb_comp_cache_address( ADDR_CACHE* cache, vm_addr addr )
{
	PROC( "rb_comp_pick_up_address" );
	PARMS( "cache", "%p", cache );
	PARMS( "addr", "%ld", addr );
	
	if( cache->addr_cnt == 0 )
		cache->addr = (vm_addr*)pmalloc(
						MALLOC_STEP * sizeof( vm_addr ) );
	else if( cache->addr_cnt % MALLOC_STEP == 0 )
		cache->addr = (vm_addr*)prealloc(
						cache->addr, ( cache->addr_cnt + MALLOC_STEP )
							* sizeof( vm_addr ) );

	if( !( cache->addr ) )
	{
		MSG( "Out of memory, shit" );
		RB_OUT_OF_MEMORY;
	}

	cache->addr[ cache->addr_cnt++ ] = addr;
	
	VOIDRET;
}

/*
	Semantic action function:
	Adds a CONT or BREAK-jump to the current region.
	
	Parameter cont is TRUE for a CONT, and FALSE for a BREAK.
*/
boolean rb_comp_region_branch( boolean cont )
{
	vm_addr		jmp;

	PROC( "rb_comp_region_branch" );
	PARMS( "cont", "%d", cont );
	
	jmp = rb_comp_next_addr( &cur.prog, VMC_JMP );

	/* Check if this region allows for BREAK and/or CONTs */
	switch( cur.region.type )
	{
		case REGION_GLOBAL:
			/* Never! */
			break;

		case REGION_ITERATION:
			/* Both! */
			if( cont )
				rb_comp_cache_address( &( cur.region.conts ), jmp );
			else
				rb_comp_cache_address( &( cur.region.breaks ), jmp );

			RETURN( TRUE );

		case REGION_SELECTION:
			/* Only BREAK! */
			if( cont )
				break;
			
			rb_comp_cache_address( &( cur.region.breaks ), jmp );

			RETURN( TRUE );
			
		default:
			MSG( "!!!Unhandled region type!!!" ); 
			break;
	}

	if( cont )
		rb_comp_error( &( cur.stmt_begin ), "cont_in_illegal_context", (char*)NULL );
	else
		rb_comp_error( &( cur.stmt_begin ), "break_in_illegal_context", (char*)NULL );
	
	RETURN( FALSE );
}

/*
	Semantic action function:
	Closes the current region
*/
void rb_comp_region_close( REGION* prev,
	vm_addr break_target, vm_addr cont_target )
{
	vm_addr		i;
	
	PROC( "rb_comp_region_drop" );
	PARMS( "prev", "%p", prev );
	
	MSG( "Patching BREAKs" );
	for( i = 0; i < cur.region.breaks.addr_cnt; i++ )
	{
		rb_comp_patch( VM_GET_CODE( &cur.prog, cur.region.breaks.addr[i] ),
			VMC_JMP, &break_target );
	}
	pfree( cur.region.breaks.addr );
	
	MSG( "Patching CONTs" );
	for( i = 0; i < cur.region.conts.addr_cnt; i++ )
	{
		rb_comp_patch( VM_GET_CODE( &cur.prog, cur.region.conts.addr[i] ),
			VMC_JMP, &cont_target );
	}
	pfree( cur.region.conts.addr );
	
	MSG( "Replacing previous region" );	
	memcpy( &cur.region, prev, sizeof( REGION ) );	
	VOIDRET;
}

/*
	Semantic action function:
	Add label to symbol table
*/
symbol* rb_comp_get_label( char* ident, boolean at_definition )
{
	symbol*		label;
	
	label = rb_symtab_find_in_scope(
				cur.scope, ident, SYM_LABEL );

	if( at_definition && label && label->defined )
	{
		rb_comp_error( &( cur.stmt_begin ),
			"label_already_defined",
				"label", ident, (char*)NULL );
	}
	else
	{
		if( !label )
		{
			label = rb_symtab_new( ident, SYM_LABEL );
			cur.scope = rb_symtab_engage_scope(
							cur.scope, label );
		}
		
		if( at_definition )
		{
			label->defined = TRUE;
			label->offset._addr =  cur.prog.code_next;
		}
		else
			label->used = TRUE;
	}
	
	pfree( ident );	
	return label;
}

/*
	Semantic action function:
	Call label, or remember call on implicit call
*/
void rb_comp_label_call( char* ident )
{
	symbol*		label;
	LBL_CALL	lc;
	LBL_CALL*	lcp;
	
	PROC( "rb_comp_label_call" );
	PARMS( "ident", "%s", ident );
	
	label = rb_comp_get_label( ident, FALSE );
	
	lc.sym = label;
	lc.call = rb_comp_next_addr( &cur.prog, VMC_JMP );
	memcpy( &( lc.pos ), &cur.stmt_begin, sizeof( srcpos ) );
	
	if( label->defined )
	{
		MSG( "Label is defined - call it now!" );
		rb_comp_label_call_perform( &lc );
	}
	else
	{
		MSG( "It seems, that this label is not defined, yet" );
		MSG( "No problem here, we save it as implicit label call" );
		
		if( !( lcp = (LBL_CALL*)rb_memdup( &lc, sizeof( LBL_CALL ) ) ) )
		{
			RB_OUT_OF_MEMORY;
			VOIDRET;
		}
		
		if( !cur.implicit_lbl_calls )
			cur.implicit_lbl_calls = plist_create( 0, PLIST_MOD_PTR );

		plist_push( cur.implicit_lbl_calls, (void*)lcp );
	}
	
	VOIDRET;
}

/*
	Semantic action function:
	Perform label call
*/
void rb_comp_label_call_perform( LBL_CALL*	lc )
{
	PROC( "rb_comp_label_call_perform" );
	
	if( !( lc->sym->defined ) )
	{
		rb_comp_error( &lc->pos, "label_not_defined",
			"label", lc->sym->name, (char*)NULL );
			
		rb_comp_patch( VM_GET_CODE( &cur.prog, lc->call ),
			VMC_NOP, 0 );

		VOIDRET;
	}
	
	rb_comp_patch( VM_GET_CODE( &cur.prog, lc->call ),
			VMC_JMP, &( lc->sym->offset._addr ) );
	
	VOIDRET;
}

/*
	Semantic action function:
	Backpatch implicit label calls
*/
void rb_comp_backpatch_label_calls( symbol* label )
{
	plistel*	e;
	LBL_CALL*	lc_ptr;
	
	PROC( "rb_comp_backpatch_label_calls" );
	PARMS( "label", "%p", label );
	
	do
	{
		plist_for( cur.implicit_lbl_calls, e )
		{
			lc_ptr = (LBL_CALL*)plist_access( e );
			if( !label || lc_ptr->sym == label );
				break;
		}
		
		if( e )
		{
			rb_comp_label_call_perform( lc_ptr );
			
			plist_remove( cur.implicit_lbl_calls,
				plist_get_by_ptr( cur.implicit_lbl_calls, lc_ptr ) );
			pfree( lc_ptr );
		}
	}
	while( e );
	
	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_optimize_operator()
	
	Author:			Jan Max Meyer
	
	Usage:			Performs an operator optimization. Used by the parser in
					several productions.

	Parameters:		PARAM*		ret			Return parameter
					PARAM*		operand1	First Operand
					PARAM*		operand2	Second operand

	Returns:		void
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_comp_optimize_operator( PARAM* ret, PARAM* operand1, PARAM* operand2 )
{
	PROC( "rb_comp_optimize_operator" );
	PARMS( "ret", "%p", ret );
	PARMS( "operand1", "%p", operand1 );
	PARMS( "operand2", "%p", operand2 );
	
	if( ( ret->is_const = operand1->is_const && operand2->is_const ) )
	{
		rb_comp_opt_expr( &cur.prog, operand1->code_base, TRUE );
		/* rb_vm_dump( stderr, &cur.prog, operand1->code_base ); */						
		ret->is_const = TRUE;
	}
	
	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_comp_push_constant()
	
	Author:			Jan Max Meyer
	
	Usage:			Generates a push on a constant value as the most suitable
					value.

	Parameters:		char*		constval		The constant to be pushed

	Returns:		void
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_comp_push_constant( char* constval )
{
	long	v[2];
	char	dot, empty;
	char*	str;
	
	PROC( "rb_comp_push_constant" );
	PARMS( "constval", "%s", constval );
	
	switch( sscanf( constval, "%ld%c%ld%c", &( v[0] ), &dot, &( v[1] ), &empty ) )
	{
		/* Three matches? This can possibly be converted to double! */
		case 3:
			if( dot != '.' )
			{
				MSG( "No, this is not the double I thought it was" );
				break;
			}
			else if( v[1] != 0 )
			{
				double tmp_d;

				MSG( "Three correct matches - converting to double" );
				
				tmp_d = atof( constval );
				VARS( "tmp_d", "%lf", tmp_d );
				
				rb_comp_gen( &cur.prog, VMC_PUSHDBL, (void*)&tmp_d );
				
				VOIDRET;
			}
			
			MSG( "Three correct matches - but 0 beyond the decimalpoint"
					"This will become long!" );
			
		/* One match? This can be converted to integer! */
		case 1:
			MSG( "One match - converting to long" );
			
			if( v[0] >= 0 && v[0] <= UCHAR_MAX )
			{
				empty = (char)v[0];
				rb_comp_gen( &cur.prog, VMC_PUSHCHR, (void*)&empty );				
			}
			else
				rb_comp_gen( &cur.prog, VMC_PUSHINT, (void*)&v[0] );
			
			VOIDRET;

		
		/* Everything else - no convert! */
		default:
			break;
	}
	
	MSG( "Now we had gone to here... that means... PUSH A STRING!!!" );
	
	str = rb_str_pool_append( &( cur.prog.cstr ), constval, TRUE );
	rb_comp_gen( &cur.prog, VMC_PUSHSTR, (void*)&str );
	
	VOIDRET;
}
