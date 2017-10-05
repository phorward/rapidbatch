/* -HEADER----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_val.h
Author:	Jan Max Meyer
Usage:	Universal value structure
----------------------------------------------------------------------------- */

#ifndef RB_VAL_H
#define RB_VAL_H

/*
 * Includes
 */

/*
 * Defines
 */
#define VAL_UNDEFINED		0
#define VAL_LONG			1
#define VAL_ADDR			2
#define VAL_DBL				4
#define VAL_STR				8
#define VAL_CSTR			8+16

/*
 * Macros
 */
 
/*EXPORT*/
/* Value access macros and functions */
/*ENDEXPORT*/

#if 0
/*EXPORT:internal*/
#include "rb_val.h"
/*ENDEXPORT*/
#endif

/*EXPORT:internal*/
#define RB_VAL_TYPE( val )		( (val)->type )
/*ENDEXPORT*/

#if 0
/*EXPORT:external*/
#define RB_VAL_TYPE( val )		rb_vmwrap_VAL_TYPE( val )
/*ENDEXPORT*/
#endif

/*EXPORT:external*/
#define RB_VAL_FREE( val )		rb_vm_free_val( val )
/*ENDEXPORT*/

/*EXPORT*/
#define RB_VAL_GET_ADDR( val ) 	rb_vm_get_addr_val( ( val ), TRUE )
#define RB_VAL_GET_LONG( val ) 	rb_vm_get_long_val( ( val ), TRUE )								
#define RB_VAL_GET_DBL( val ) 	rb_vm_get_dbl_val( ( val ), TRUE )
#define RB_VAL_GET_STR( val ) 	rb_vm_get_str_val( ( val ), TRUE )
/*ENDEXPORT*/
/*EXPORT:internal*/
#define RB_VAL_SET_ADDR( val, v ) \
								( val )->value.vaddr = (v), \
								( val )->type = VAL_ADDR
#define RB_VAL_SET_LONG( val, v ) \
								( val )->value.vlong = (v), \
								( val )->type = VAL_LONG
#define RB_VAL_SET_DBL( val, v ) \
								( val )->value.vdbl = (v), \
								( val )->type = VAL_DBL
#define RB_VAL_SET_CSTR( val, v ) \
								( val )->value.vcstr = (v), \
								( val )->type = VAL_CSTR
#define RB_VAL_SET_STR( val, v ) \
								( val )->value.vstr.str = (v), \
								( val )->value.vstr.len = pstrlen( \
											( val )->value.vstr.str ), \
								( val )->type = VAL_STR
/*ENDEXPORT*/
#if 0
/*EXPORT:external*/
#define RB_VAL_SET_ADDR( val, v ) \
								rb_vmwrap_VAL_SET_ADDR( (val), (v) )
#define RB_VAL_SET_LONG( val, v ) \
								rb_vmwrap_VAL_SET_LONG( (val), (v) )
#define RB_VAL_SET_DBL( val, v ) \
								rb_vmwrap_VAL_SET_DBL( (val), (v) )
#define RB_VAL_SET_CSTR( val, v ) \
								rb_vmwrap_VAL_SET_CSTR( (val), (v) )
#define RB_VAL_SET_STR( val, v ) \
								rb_vmwrap_VAL_SET_STR( (val), (v) )
/*ENDEXPORT*/
#endif

/* Internal, old-style macro names */
#define VAL_GET_ADDR			RB_VAL_GET_ADDR		
#define VAL_GET_LONG			RB_VAL_GET_LONG						
#define VAL_GET_DBL    			RB_VAL_GET_DBL
#define VAL_GET_STR    			RB_VAL_GET_STR
#define VAL_SET_ADDR   			RB_VAL_SET_ADDR
#define VAL_SET_LONG   			RB_VAL_SET_LONG
#define VAL_SET_DBL    			RB_VAL_SET_DBL
#define VAL_SET_CSTR   			RB_VAL_SET_CSTR
#define VAL_SET_STR				RB_VAL_SET_STR

#define VAL_TYPE				RB_VAL_TYPE
#define VAL_FREE				RB_VAL_FREE

/*
 * Typedefs
 */
typedef	struct	_vm_val	vm_val;
#define VM_VAL			vm_val

/*
 * Enumerators
 */

/*
 * Structs & Unions
 */

struct _vm_val
{
	uchar	type;

	union
	{
		long	vlong;		/* long */
		vm_addr	vaddr;		/* adress */
		double	vdbl;		/* double */
		uchar*	vcstr;		/* constant string */

		struct
		{
			uchar*	str;
			int		len;
		} vstr;				/* dynamic string */
	} value;
};

#endif

