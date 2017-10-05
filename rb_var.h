/* -HEADER----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_var.h
Author:	Jan Max Meyer
Usage:	Variable system
----------------------------------------------------------------------------- */

#ifndef RB_VAR_H
#define RB_VAR_H

#include "rb_val.h"

/*
 * Includes
 */

/*
 * Defines
 */
#define VM_ASSOC_HASH_SIZE	64
#define COPY_STRING			FALSE
#define DONT_COPY_STRING	TRUE

/*
 * Macros
 */
 
/*EXPORT*/
/* Variable access macros and functions */
/*ENDEXPORT*/

#if 0
/*EXPORT:internal*/
#include "rb_var.h"
/*ENDEXPORT*/
#endif

/*EXPORT:internal*/
#define RB_VAR_VAL_STRUCT( var )	( &( (var)->value ) )
#define RB_VAR_VAL_TYPE( var )		( (var)->value.type )
/*ENDEXPORT*/

#if 0
/*EXPORT:external*/
#define RB_VAR_VAL_STRUCT( var )	rb_vmwrap_VAR_VAL_STRUCT( var )
#define RB_VAR_VAL_TYPE( var )		rb_vmwrap_VAR_VAL_TYPE( var )
/*ENDEXPORT*/
#endif

/*EXPORT*/
#define RB_VAR_GET_ADDR( var ) 		RB_VAL_GET_ADDR( RB_VAR_VAL_STRUCT( var ) )
#define RB_VAR_GET_LONG( var ) 		RB_VAL_GET_LONG( RB_VAR_VAL_STRUCT( var ) )
#define RB_VAR_GET_DBL( var ) 		RB_VAL_GET_DBL( RB_VAR_VAL_STRUCT( var ) )
#define RB_VAR_GET_STR( var ) 		RB_VAL_GET_STR( RB_VAR_VAL_STRUCT( var ) )

#define RB_VAR_SET_ADDR( var, v )	RB_VAL_SET_ADDR( RB_VAR_VAL_STRUCT( var ), v )
#define RB_VAR_SET_LONG( var, v )	RB_VAL_SET_LONG( RB_VAR_VAL_STRUCT( var ), v )
#define RB_VAR_SET_DBL( var, v ) 	RB_VAL_SET_DBL( RB_VAR_VAL_STRUCT( var ), v )
#define RB_VAR_SET_CSTR( var, v )	RB_VAL_SET_CSTR( RB_VAR_VAL_STRUCT( var ), v )
#define RB_VAR_SET_STR( var, v )	RB_VAL_SET_STR( RB_VAR_VAL_STRUCT( var ), v )
/*ENDEXPORT*/

/* Internal, old-style macro names */
#define VAR_VAL_GET_ADDR			RB_VAR_GET_ADDR
#define VAR_VAL_GET_LONG			RB_VAR_GET_LONG
#define VAR_VAL_GET_DBL				RB_VAR_GET_DBL
#define VAR_VAL_GET_STR				RB_VAR_GET_STR

#define VAR_VAL_SET_ADDR			RB_VAR_SET_ADDR
#define VAR_VAL_SET_LONG			RB_VAR_SET_LONG
#define VAR_VAL_SET_DBL				RB_VAR_SET_DBL
#define VAR_VAL_SET_CSTR			RB_VAR_SET_CSTR
#define VAR_VAL_SET_STR				RB_VAR_SET_STR

#define VAR_VAL_STRUCT				RB_VAR_VAL_STRUCT
#define VAR_VAL_TYPE				RB_VAR_VAL_TYPE

/*
 * Typedefs
 */
typedef	struct	_vm_var	vm_var;
#define VM_VAR			vm_var

/*
 * Enumerators
 */

/*
 * Structs & Unions
 */

struct _vm_var
{
	vm_val			value;		/* The variable's value */
	uchar*			key;		/* Pointer to key value
									(only associative array) */

	vm_addr*		ht;			/* Hash-table for subsequent elements */
	vm_var*			begin;		/* Dynamic array of subsequent elements */
	vm_addr			begin_cnt;	/* Size of current dimension */

	vm_addr			next;		/* Next element in hash table
									collision list */
};

#endif

