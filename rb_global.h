/* -HEADER----------------------------------------------------------------------
RapidBATCH 6.0
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_global.h
Author:	Jan Max Meyer
Usage:	Global declarations
----------------------------------------------------------------------------- */

#ifndef RB_GLOBAL_H
#define RB_GLOBAL_H

/*
 * Includes
 */
/*EXPORT*/
/* System includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>

/* Phorward DevEnv includes */
#include <uchar.h>
#include <boolean.h>

/*ENDEXPORT*/
#include <llist.h>
#include <dbg.h>
#include <pmalloc.h>
#include <pstring.h>
#include <xml.h>
/*EXPORT*/
/*ENDEXPORT*/

/*
 * Defines
 */

/*EXPORT::Several defines*/
#define RB_TRUE				0
#define RB_FALSE			-1
 
#define RB_ERR_OK			0		/* OK */
#define RB_ERR_FAILURE		-1		/* Failure */
#define RB_ERR_PARMS		-2		/* Wrong parameters */
#define RB_ERR_MEM			-3		/* Memory allocation error */
#define RB_ERR_SYSTEM		-4		/* System call error */
#define RB_ERR_UNIMPL		-5		/* Unimplemented */
#define RB_ERR_OTHER		-6		/* Any other error */
/*ENDEXPORT*/

/* Default allocation step size in bytes */
#define SYMTAB_SIZE			64
#define	MALLOC_STEP			128

/* Number of user function parameters */
#define MAX_USERFUNC_PARMS	64

typedef 	char			status;
#define STATUS				status
typedef		char			byte;
#define BYTE				byte
typedef		unsigned long	vm_addr;
#define VM_ADDR				vm_addr

#if 0
/*EXPORT:internal*/
#define		vm_addr			unsigned long
/*ENDEXPORT*/
#endif

/*
 * Macro definitions
 */

/* memory management */
#define RB_OUT_OF_MEMORY			do {} while( 0 ) //Will be defined later!

#endif

