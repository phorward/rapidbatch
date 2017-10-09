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

/* System includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>

/* Phorward DevEnv includes */
#include <phorward.h>
#include "xml.h"

/*
 * Defines
 */

#define RB_TRUE				0
#define RB_FALSE			-1

/* Default allocation step size in bytes */
#define SYMTAB_SIZE			64
#define	MALLOC_STEP			128

/* Number of user function parameters */
#define MAX_USERFUNC_PARMS	64

typedef 	char			status;
typedef		char			byte;
typedef		unsigned long	vm_addr;

#include "rb_val.h"
#include "rb_var.h"
#include "rb_vm.h"
#include "rb_comp.h"
#include "rb_comp.parser.h"
#include "rb_native.h"
#include "xml.h"

#include "rb_proto.h"

#endif

