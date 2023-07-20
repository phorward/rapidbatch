/* -MODULE----------------------------------------------------------------------
J.M.K S.F. RapidBATCH© 32-Bit 5.1.0 Interpreter 1.5.1 (RBI32.EXE)
Copyright (C) 2006 by J.M.K S.F. Software Technologies, Jan Max Meyer

File:	rb5_helpfct.c
Usage:	Calculation routines for calculating with string-represented values
----------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rb5_base.h"


/* -FUNCTION--------------------------------------------------------------------
	Function:		rbCalculate()

	Usage:			Calculates two values with a desired mathematical operation.
				On success, both operands are automatically freed.

	Parameters:		char**	result		Pointer where the result should be
							stored to
				char*	operand1	First operand
				char*	operand2	Second operand
				int	op		Operator to be applicated.
							This can be one of the mathematical de-
							findes

	Returns:		0			In case the operation succeeded
				-1			if not

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int rbCalculate( char** result, char* operand1, char* operand2, int op )
{
	double	opr1	= 0,
	              opr2	= 0,
	                     res  	= 0;
	int	i   	= 0;
	char	convert	[128];

	if ( operand1 == (char*)NULL || operand2 == (char*)NULL )
		return -1;

	opr1 = atof( operand1 );
	opr2 = atof( operand2 );

	switch ( op )
	{
		case RB_CALC_ADDITION:
			res = opr1 + opr2;
			break;
		case RB_CALC_SUBTRACTION:
			res = opr1 - opr2;
			break;
		case RB_CALC_MULTIPLICATION:
			res = opr1 * opr2;
			break;
		case RB_CALC_DIVISION:
			if ( opr1 != 0 && opr2 != 0 )
				res = opr1 / opr2;
			break;
		case RB_CALC_MODULO:
			if ( opr1 != 0 && opr2 != 0 )
				res = (int)opr1 % (int)opr2;
			break;
		default:
			return -1;
	}

	sprintf( convert, "%lf", res );

	for ( i = strlen( convert ) - 1; i > 0; i--)
	{
		if ( convert[i] == '.' )
		{
			convert[i] = '\0';
			break;
		}
		else if ( convert[i] != '0' )
		{
			convert[i+1] = '\0';
			break;
		}
	}

	free( operand1 );
	free( operand2 );

	*result = strdup( convert );

	return 0;
} /* rbCalculate() */
