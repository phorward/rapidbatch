/* -HEADER----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_comp.h
Author:	Jan Max Meyer
Usage:	Compiler-related structs and definitions
----------------------------------------------------------------------------- */

#ifndef RB_COMP_H
#define RB_COMP_H

/*
 * Includes
 */

/*
 * Defines
 */
/*
#define rb_comp_patch( a, b, c ) rb_comp_patch_d( __FILE__, __LINE__, a, b, c )
*/

/* Region definitions */
#define REGION_GLOBAL			0
#define REGION_ITERATION		1
#define REGION_SELECTION		2

/* Function parameter signature */
#define PARAM_BY_VAL			'v'
#define PARAM_BY_REF			'r'
#define PARAM_VARG_VAL_ONLY		'+'
#define PARAM_VARG_ANY			'*'

/* Error message levels */
#define ERROR_WARNING			0
#define ERROR_FATAL				1

/*EXPORT*/
/* Function exection flags */
#define RB_FUNC_AT_RUNTIME		1
#define RB_FUNC_AT_COMPTIME		2
#define RB_FUNC_IF_POSSIBLE		\
	( RB_FUNC_AT_RUNTIME | RB_FUNC_AT_COMPTIME )
/*ENDEXPORT*/

/*
 * Macros
 */

/*
 * Typedefs
 */
typedef 	struct	_symbol			symbol;
typedef		char					symtype;
typedef		struct	_srcpos			srcpos;
typedef		unsigned long			srcline;

#define SYMBOL 			symbol
#define SYMTYPE 		symtype

/*
 * Enumerators
 */
#define SYM_UNDEFINED	0
#define SYM_VAR			1			/* Variable */
#define SYM_PROC		2			/* User-defined procedure/function */
#define	SYM_LABEL		3			/* Jump Label */

/*
 * Structs
 */

struct _srcpos
{
	char*		filename;			/* Name of file; References
										memory in srcfile */
	srcline		line;				/* Line of occurence */
};
 
struct _symbol
{
	char*		name;				/* Symbol identifier */
	symtype		type;				/* Symbol type */

	union
	{
		long				_long;	/* Variable offset, relative
										to the current frame pointer,
										or proc address in case of 
										procedure */
		vm_addr				_addr;
		
		rb_native_func		function;	/* Native callback function */

		struct
		{
			rb_native_var	get;		/* Native variable getter */
			rb_native_var	set;		/* Native variable setter */
		}					variable;	/* Native callback variable */
		
		char*				constant;	/* Constant */

	} offset;
	
	/* Symbol related additionals */
	plist*		parmsym;		/* For function-calls:
									symbol-pointers to the parameter
										value definitions */
	char*		parmdef;		/* For native function calls:
									a string defining the value
										definition styles */

	/* Flags */
	pboolean		native;				/* Flag for natively implemented functions,
										variables, and constants */

	pboolean		global;				/* Identifier for global or local
										scope */
	int			constant;			/* Flag for constants/compile-time
										functions TODO <rename this> */

	pboolean		defined;			/* Defined flag */
	pboolean		used;				/* Used flag */
	pboolean		patched;			/* Backpatched flag */

	pboolean		pointer;			/* Pointer variable (SYM_VAR only) */
	pboolean		function;			/* Function flag (SYM_PROC only) */
	
	/* List links */
	symbol*		next;				/* Next symbol in bucket */
	symbol*		scope_next;			/* Next symbol in scope */
	
	/* Code positions */
	srcpos		pos;				/* Position of symbol definition */
};

/*
	Structure to pass on variable symbol and access type information
	within the parse tree
*/
struct var_info
{
	char*		varname;			/* Name of variable */
	pboolean		pointer;			/* Pointer indicator */
};

#define VAR_INFO		struct var_info

struct var_info_sym
{
	symbol*		varsym;				/* Symbol of variable */
	pboolean		pointer;			/* Pointer indicator */
};
#define VAR_INFO_SYM	struct var_info_sym

/*
	Structure to stack scope-related pointers within the
	parse tree
*/
struct scope_info
{
	symbol*		scope;				/* Previous scope */
	plist*		implicit_lbl_calls;	/* Previuos list of implicit
										label calls */
};
#define SCOPE_INFO		struct scope_info

/*
	Structure to retrieve function name and starting address
	on call
*/
struct proc_info
{
	symbol*		proc;				
	vm_addr		begin;
};
#define PROC_INFO		struct proc_info

/* 	
	The structs param and proc_call are required to build a procedure
	call signature; They store information about if a parameter can
	be pushed as value or pointer (call-by-value, call-by-reference).
	Using the code pointer, a procedure-call can insert - according
	the procedure call signature - a push as value or of a pointer.
*/
struct param
{
	pboolean		is_const;			/* Flag, for constant parameters */
	vm_addr		code_base;			/* Code-base for a parameter;
										This is used for optimizations
										on constant values */
	symbol*		var;				/* Variable index */
	vm_addr		push_code;			/* Address to code that may cause a
										push of a value or an address */
	pboolean		ptrload;			/* Load as pointer */
	
	srcpos		pos;				/* Position of parameter */
};

struct proc_call
{
	symbol*		proc;				/* Procedure/Function symbol table entry */
	pboolean		as_function;		/* Symbol is called like a function */
	plist*		param_signature;	/* Parameter signature */
	
	/* Code addresses */
	vm_addr		call;				/* Address to code, for backpatching
										implicit function calls */
	vm_addr		drop;				/* Address to code, where to drop
										parameters */
	vm_addr		parm_push;			/* Address to code, where pushing
										of parameters begins */

	srcpos		pos;				/* Position of procedure call */
};

struct lbl_call
{
	symbol*		sym;				/* Symbol table entry */
	vm_addr		call;				/* Address to code, for backpatching
										implicit label calls */
	srcpos		pos;				/* Position of the call */
};

#define PARAM 			struct param
#define PROC_CALL 		struct proc_call
#define LBL_CALL		struct lbl_call

/* Memorizing cache of addresses */
struct addr_cache
{
	vm_addr*	addr;
	vm_addr		addr_cnt;
};
#define ADDR_CACHE		struct addr_cache

/* Region structure */
struct region
{
	byte		type;

	ADDR_CACHE	breaks;
	ADDR_CACHE	conts;

	vm_addr		begin;
};
#define REGION			struct region

/* Compiler information structure */
struct cinfo
{
	srcpos		stmt_begin;
	srcline		line;
	char*		src;
	char*		filename;
	
	symbol*		scope;
	symbol*		proc;
	
	vm_prog 	prog;
	vm_code*	last_stmt;
	
	pboolean 	is_global_scope;
	
	plist*		implicit_proc_calls;
	plist*		implicit_lbl_calls;
	REGION		region;
};
#define CINFO			struct cinfo

#endif

