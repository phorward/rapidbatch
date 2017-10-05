/* * * * * * * * * * * * * * * * * * * * * * *
 * W A R N I N G :: THIS IS ONLY A TEMPLATE! *
 * * * * * * * * * * * * * * * * * * * * * * */

/* -HEADER----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_native.h
Author:	Jan Max Meyer
Usage:	Header Template for the Native Function SDK
		
		WARNING:	THIS IS A HEADER FILE THAT IS USED TO GENERATED THE NATIVE
					FUNCTION SDK FOR BOTH THE EXTERNAL AND INTERNAL BUILD.
					
					DO NEVER USE THIS FILE DIRECTLY!
----------------------------------------------------------------------------- */

/* * * * * * * * * * * * * * * * * * * * * * *
 * W A R N I N G :: THIS IS ONLY A TEMPLATE! *
 * * * * * * * * * * * * * * * * * * * * * * */

/*EXPORT:internal*/
#define rb_param		vm_stackitem*
#define rb_var			vm_var*
/*ENDEXPORT*/

/*EXPORT*/
/*
 * Macros
 */
 
/********************************
	NATIVE FUNCTION RELATED
********************************/

/* Native function parameters */
#define RB_FCT_PARMS \
	rb_param _RB_RET, rb_param _RB_PARMS, int _RB_PARMS_COUNT
#define RB_FCT_PARMS_PASS_THROUGH \
	_RB_RET, _RB_PARMS, _RB_PARMS_COUNT

/* Native function header macros */
#define RB_FCT( name )	\
	int name( RB_FCT_PARMS )

/* Native function param access */
#define RB_FCT_PARM_COUNT() \
	( _RB_PARMS_COUNT )
/*ENDEXPORT*/
/*EXPORT:internal*/
#define RB_FCT_PARM_ACCESS( num ) \
	( ( ( num ) < _RB_PARMS_COUNT ) ? \
		_RB_PARMS + (num) : (rb_param)NULL )
/*ENDEXPORT*/
/*EXPORT:external*/
#define RB_FCT_PARM_ACCESS( num ) \
	rb_vmwrap_GET_PARAM( _RB_PARMS, (num), _RB_PARMS_COUNT )
/*ENDEXPORT*/
/*EXPORT*/
#define RB_FCT_RET \
	( _RB_RET )

#ifdef __WITH_TRACE
#define RB_FCT_DUMP_PARMS() \
	PARMS( "_RB_RET", "%p", _RB_RET ), \
	PARMS( "_RB_PARMS", "%p", _RB_PARMS ), \
	PARMS( "_RB_PARMS_COUNT", "%d", _RB_PARMS_COUNT )
#else
	#define RB_FCT_DUMP_PARMS()
#endif

/********************************
	NATIVE VARIABLE RELATED
********************************/

/* Native function parameters */
#define RB_VARFCT_PARMS \
	rb_param _RB_VAL
#define RB_VARFCT_PARMS_PASS_TROUGH \
	_RB_VAL

/* System variable header macros */
#define RB_VARFCT( name ) \
	int name( RB_VARFCT_PARMS )

/* Native function param access */
#define RB_VARFCT_VAL \
	( _RB_VAL )

#ifdef __WITH_TRACE
#define RB_VARFCT_DUMP_PARMS() \
	PARMS( "_RB_VAL", "%p", _RB_VAL )
#else
	#define RB_VARFCT_DUMP_PARMS()
#endif

/********************************
	PARAMETER VALUE ACCESS
********************************/

/*ENDEXPORT*/
/*EXPORT:internal*/
#define RB_PARM_VAL( parm )			ITEM_GET_VAL( parm )
#define RB_PARM_VAR( parm )			ITEM_GET_VAR( parm )
/*ENDEXPORT*/
/*EXPORT:external*/
#define RB_PARM_VAL( parm )			rb_vmwrap_ITEM_GET_VAL( parm )
#define RB_PARM_VAR( parm )			rb_vmwrap_ITEM_GET_VAR( parm )
/*ENDEXPORT*/

/*EXPORT*/
#define RB_PARM_IS_VAL( parm )		RB_PARM_VAL( parm )
#define RB_PARM_IS_VAR( parm )		RB_PARM_VAR( parm )

/* Value structure access (getter/setter macros) */
#define RB_PARM_VAL_GET_ULONG( parm )	( RB_PARM_VAL( parm ) ? \
											RB_VAL_GET_ADDR( \
												RB_PARM_VAL( parm ) ) : \
													(unsigned long)0 )
#define RB_PARM_VAL_GET_LONG( parm ) 	( RB_PARM_VAL( parm ) ? \
											RB_VAL_GET_LONG( \
												RB_PARM_VAL( parm ) ) : 0L )
#define RB_PARM_VAL_GET_DBL( parm ) 	( RB_PARM_VAL( parm ) ? \
											RB_VAL_GET_DBL( \
												RB_PARM_VAL( parm ) ) : 0.0 )
#define RB_PARM_VAL_GET_STR( parm ) 	( RB_PARM_VAL( parm ) ? \
											RB_VAL_GET_STR( \
												RB_PARM_VAL( parm ) ) : \
													(uchar*)NULL )

#define RB_PARM_VAL_SET_ULONG( parm, v ) ( RB_PARM_VAL( parm ) ? \
											RB_VAL_SET_ADDR( RB_PARM_VAL( \
 												parm ), v ), RB_ERR_OK : \
 													RB_ERR_FAILURE )
#define RB_PARM_VAL_SET_LONG( parm, v ) ( RB_PARM_VAL( parm ) ? \
											RB_VAL_SET_LONG( RB_PARM_VAL( \
												parm ), v ), RB_ERR_OK : \
													RB_ERR_FAILURE )
#define RB_PARM_VAL_SET_DBL( parm, v ) 	( RB_PARM_VAL( parm ) ? \
											RB_VAL_SET_DBL( RB_PARM_VAL( \
												parm ), v ), RB_ERR_OK : \
													RB_ERR_FAILURE )
#define RB_PARM_VAL_SET_CSTR( parm, v )	( RB_PARM_VAL( parm ) ? \
											RB_VAL_SET_CSTR( RB_PARM_VAL( \
												parm ), v ), RB_ERR_OK : \
													RB_ERR_FAILURE )
#define RB_PARM_VAL_SET_STR( parm, v ) 	( RB_PARM_VAL( parm ) ? \
											RB_VAL_SET_STR( RB_PARM_VAL( \
												parm ), v ), RB_ERR_OK : \
													RB_ERR_FAILURE )

/* Variable structure access (getter/setter macros) */
#define RB_PARM_VAR_GET_ULONG( parm )	( RB_PARM_VAR( parm ) ? \
											RB_VAR_GET_ADDR( RB_PARM_VAR( \
												parm ) ) : (unsigned long)0 )
#define RB_PARM_VAR_GET_LONG( parm ) 	( RB_PARM_VAR( parm ) ? \
											RB_VAR_GET_LONG( RB_PARM_VAR( \
												parm ) ) : 0L )
#define RB_PARM_VAR_GET_DBL( parm ) 	( RB_PARM_VAR( parm ) ? \
											RB_VAR_GET_DBL( RB_PARM_VAR( \
												parm ) ) : 0.0 )
#define RB_PARM_VAR_GET_STR( parm ) 	( RB_PARM_VAR( parm ) ? \
											RB_VAR_GET_STR( RB_PARM_VAR( \
												parm ) ) : (uchar*)NULL )

#define RB_PARM_VAR_SET_ULONG( parm, v ) ( RB_PARM_VAR( parm ) ? \
											RB_VAR_SET_ADDR( RB_PARM_VAR( \
												parm ), v ), \
												RB_ERR_OK : RB_ERR_FAILURE )
#define RB_PARM_VAR_SET_LONG( parm, v ) ( RB_PARM_VAR( parm ) ? \
											RB_VAR_SET_LONG( RB_PARM_VAR( \
												parm ), v ), \
												RB_ERR_OK : RB_ERR_FAILURE )
#define RB_PARM_VAR_SET_DBL( parm, v ) 	( RB_PARM_VAR( parm ) ? \
											RB_VAR_SET_DBL( RB_PARM_VAR( \
												parm ), v ), \
												RB_ERR_OK : RB_ERR_FAILURE )
#define RB_PARM_VAR_SET_STR( parm, v ) 	( RB_PARM_VAR( parm ) ? \
											RB_VAR_SET_STR( RB_PARM_VAR( \
												parm ), v ), \
												RB_ERR_OK : RB_ERR_FAILURE )
/*ENDEXPORT*/
