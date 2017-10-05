/* -HEADER----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_vm.h
Author:	Jan Max Meyer
Usage:	Virtual machine related defintions
----------------------------------------------------------------------------- */

#ifndef RB_VM_H
#define RB_VM_H

/*
 * Includes
 */
/*EXPORT:internal*/
#include "rb_val.h"
#include "rb_var.h"
/*ENDEXPORT*/

/*
 * Defines
 */
 
/* Size of an activation record (begin of local variables!) */
#define VM_ACTREC_SIZE		2				/* Activation record exists of:
												[ * Return value ]
												* Parameters
												* Number of parameters
												0 Return address
												1 Previous Frame Pointer
												* Local variables
											*/
 
/* Command class definitions */
#define CCLASS_UNDEFINED	0				/* Undefined command */
#define CCLASS_PROGRAM_FLOW	16				/* Program flow/Memory
												management */
#define CCLASS_PARAMETIZED	32				/* Command with parameter bytes */
#define CCLASS_ONE_OPERAND	64				/* Operations just taking
												one operand */
#define CCLASS_TWO_OPERAND	64+16			/* Operations just taking
												two operands */
#define CCLASS_TWO_DOPERAND	64+16+32		/* Double operations taking
												two operands */
#define CCLASS_TWO_LOPERAND	64+64			/* Long operations taking
												two operands */
/*EXPORT:internal*/
/* Stackitem type definitions */
#define ITEM_NULL			0				/* Undefined stackitem */
#define ITEM_VAL			1				/* Stacked item is a variable */
#define ITEM_VAR			2				/* Stacked item is a variable */
#define ITEM_PTR			4				/* Stacked item is a pointer */
#define ITEM_ADDR			8				/* Stacked item is a
												fixed address */
/*ENDEXPORT*/

/*
 * Macros
 */

/*
 * Typedefs
 */
typedef		unsigned char			vm_code;
#define VM_CODE		vm_code
/*EXPORT:internal*/
typedef		struct	_vm_prog		vm_prog;
#define VM_PROG		vm_prog
typedef		struct	_vm_stack		vm_stack;
#define VM_STACK	vm_stack
typedef		struct	_vm_stackitem	vm_stackitem;
#define VM_STACKITEM vm_stackitem
/*ENDEXPORT*/

/*EXPORT:internal*/
typedef		vm_val					rb_val;
typedef		vm_var					rb_var;
typedef		vm_stackitem			rb_param;
typedef		vm_prog					rb_prog;
typedef		vm_stack				rb_stack;
/*ENDEXPORT*/

#if 0
/*EXPORT:external*/
typedef void* rb_val;
typedef void* rb_var;
typedef void* rb_param;
typedef void* rb_prog;
typedef void* rb_stack;
/*ENDEXPORT*/
#endif

typedef		enum	_vm_cmd			vm_cmd;
#define VM_CMD		vm_cmd

/*EXPORT:internal*/
/* Callback function prototypes */
typedef		int 					(*rb_native_func)( vm_stackitem*,
										vm_stackitem*, int );
typedef		int 					(*rb_native_var)( vm_stackitem* );
/*ENDEXPORT*/

#if 0
/*EXPORT:external*/
/* Callback function prototypes */
typedef		int 					(*rb_native_func)( rb_param,
										rb_param, int );
typedef		int 					(*rb_native_var)( rb_param );
/*ENDEXPORT*/
#endif

/*
 * Enumerators
 */
#define	VMC_NOP			CCLASS_UNDEFINED		/* No operation */

#define VMC_STORE		CCLASS_PROGRAM_FLOW+0	/* Program flow */
#define VMC_LOAD 		CCLASS_PROGRAM_FLOW+1
#define VMC_PTRLOAD 	CCLASS_PROGRAM_FLOW+2
#define VMC_CPYLOAD		CCLASS_PROGRAM_FLOW+3
#define VMC_INIT		CCLASS_PROGRAM_FLOW+4
#define VMC_FREE		CCLASS_PROGRAM_FLOW+5
#define VMC_MOV			CCLASS_PROGRAM_FLOW+6
#define VMC_RET			CCLASS_PROGRAM_FLOW+7
#define VMC_RETV		CCLASS_PROGRAM_FLOW+8
#define VMC_PUSHFRM		CCLASS_PROGRAM_FLOW+9

#define VMC_PUSHINT		CCLASS_PARAMETIZED+0	/* Parametized */
#define VMC_PUSHADR		CCLASS_PARAMETIZED+1
#define VMC_PUSHCHR		CCLASS_PARAMETIZED+2
#define VMC_PUSHDBL		CCLASS_PARAMETIZED+3
#define VMC_PUSHSTR		CCLASS_PARAMETIZED+4
#define VMC_RESERVE		CCLASS_PARAMETIZED+5
#define VMC_JMP			CCLASS_PARAMETIZED+6
#define VMC_JMPTRUE		CCLASS_PARAMETIZED+7
#define VMC_JMPFALSE	CCLASS_PARAMETIZED+8
#define VMC_CALL		CCLASS_PARAMETIZED+9
#define VMC_NCALL		CCLASS_PARAMETIZED+10
#define VMC_DROP		CCLASS_PARAMETIZED+11
#define VMC_NSTORE		CCLASS_PARAMETIZED+12
#define VMC_NLOAD		CCLASS_PARAMETIZED+13
/* When adding new instructions with parameters here, don't forget to
	extend array parm_size in file rb_vm.util.c, else may get too short
		or long addresses */

#define VMC_NOT			CCLASS_ONE_OPERAND+0	/* Operation, one operand */
#define VMC_NEG			CCLASS_ONE_OPERAND+1

#define	VMC_JOIN		CCLASS_TWO_OPERAND+0	/* Operation, two operands */
#define VMC_EQU			CCLASS_TWO_OPERAND+1
#define VMC_NEQ			CCLASS_TWO_OPERAND+2
#define VMC_GRT			CCLASS_TWO_OPERAND+3
#define VMC_LWR			CCLASS_TWO_OPERAND+4
#define VMC_GRE			CCLASS_TWO_OPERAND+5
#define VMC_LWE			CCLASS_TWO_OPERAND+6

#define VMC_ADD			CCLASS_TWO_DOPERAND+0	/* Operation, two operands,
													double conversion
														required */
#define	VMC_SUB			CCLASS_TWO_DOPERAND+1
#define VMC_DIV			CCLASS_TWO_DOPERAND+2
#define VMC_MUL			CCLASS_TWO_DOPERAND+3
#define VMC_AND			CCLASS_TWO_DOPERAND+4
#define VMC_OR			CCLASS_TWO_DOPERAND+5
#define VMC_XOR			CCLASS_TWO_DOPERAND+6
#define VMC_LAND		CCLASS_TWO_DOPERAND+7
#define VMC_LOR			CCLASS_TWO_DOPERAND+8

#define VMC_MOD			CCLASS_TWO_LOPERAND+0	/* Operation, two operands,
														long conversion
															required */

/*
 * Macros
 */
#define VM_GET_PARAM_SIZE( instr ) \
	rb_vm_get_parm_size( instr )

#define VM_GET_CODE( prog, addr ) \
	( (prog)->code + (addr) )
	
#define VM_NEXT_OF( prog, addr ) \
	( addr + rb_vm_get_parm_size( \
		*VM_GET_CODE( prog, addr ) ) + 1 )

#define VM_NEXT( prog ) \
	( (prog)->cur_addr )

/*
 * Structs & Unions
 */
/*EXPORT:internal*/
struct _vm_stackitem
{
	char			type;

	union
	{
		vm_val		val;
		vm_var		var;
		vm_var*		var_ptr;
		vm_addr		addr;
	} attribute;
};

#define ITEM_GET_VAR( item )		( ( (item)->type == ITEM_VAR ) ? \
										&( (item)->attribute.var ) : \
										( ( (item)->type == ITEM_PTR ) ? \
											(item)->attribute.var_ptr : \
												(vm_var*)NULL ) )
#define ITEM_GET_VAL( item )		( ( (item)->type == ITEM_VAL ) ? \
										&( (item)->attribute.val ) : \
											(vm_val*)NULL )
/*ENDEXPORT*/

#define ITEM_SET_INVALID( item )	( (item)->type = ITEM_NULL )

#define ITEM_VAL_TYPE( item )		( (item)->type == ITEM_VAL ? \
										(item)->attribute.val.type \
											: VAL_UNDEFINED )
#define ITEM_VAL_STRUCT( item )		( &( (item)->attribute.val ) )


#define ITEM_VAL_GET_ADDR( item ) 	( ( (item)->type == ITEM_VAL ) ? \
										VAL_GET_ADDR( \
											ITEM_VAL_STRUCT( item ) ) : \
										( ( (item)->type == ITEM_ADDR ) ? \
											(item)->attribute.addr : 0L ) )
#define ITEM_VAL_GET_LONG( item ) 	( ( (item)->type == ITEM_VAL ) ? \
										VAL_GET_LONG( \
											ITEM_VAL_STRUCT( item ) ) : 0L )
#define ITEM_VAL_GET_DBL( item ) 	( ( (item)->type == ITEM_VAL ) ? \
										VAL_GET_DBL( \
											ITEM_VAL_STRUCT( item ) ) : 0.0 )
#define ITEM_VAL_GET_STR( item ) 	( ( (item)->type == ITEM_VAL ) ? \
										VAL_GET_STR( \
											ITEM_VAL_STRUCT( item ) ) : \
												(char*)NULL )

#define ITEM_VAL_SET_ADDR( item, v ) \
									(item)->type = ITEM_VAL, \
									VAL_SET_ADDR( ITEM_VAL_STRUCT( item ), v )
#define ITEM_VAL_SET_LONG( item, v ) \
									(item)->type = ITEM_VAL, \
									VAL_SET_LONG( ITEM_VAL_STRUCT( item ), v )
#define ITEM_VAL_SET_DBL( item, v ) \
									(item)->type = ITEM_VAL, \
									VAL_SET_DBL( ITEM_VAL_STRUCT( item ), v )
#define ITEM_VAL_SET_CSTR( item, v ) \
									(item)->type = ITEM_VAL, \
									VAL_SET_CSTR( ITEM_VAL_STRUCT( item ), v )
#define ITEM_VAL_SET_STR( item, v ) \
									(item)->type = ITEM_VAL, \
									VAL_SET_STR( ITEM_VAL_STRUCT( item ), v )

struct _vm_stack
{
	vm_stackitem*	stack;
	vm_addr			stack_cnt;
	vm_addr			stack_top;
};

#define rb_vm_stack_init( STACK )	memset( (STACK), 0, sizeof( vm_stack ) )

struct _vm_prog
{
	vm_code*		code;		/* VM Code */
	vm_addr			code_cnt;
	vm_addr			cur_addr;
	vm_addr			code_next;

	char**			cstr;		/* String constants */
};

#endif

