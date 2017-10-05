/* -MODULE----------------------------------------------------------------------
RapidBATCH
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	rb_vm.dbg.c
Author:	Jan Max Meyer
Usage:	Virtual machine debug facilities
----------------------------------------------------------------------------- */

/*
 * Includes
 */
/* #define __WITH_TRACE */
#include "rb_global.h"
#include "rb_comp.h"
#include "rb_vm.h"

#ifndef MAKE_PROTOTYPES
#include "rb_proto.h"
#endif

/*
 * Global variables
 */

/*
 * Defines
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		rb_vm_dump()
	
	Author:			Jan Max Meyer
	
	Usage:			Dumps a virtual machine program.
					
	Parameters:		FILE*		stream			Stream to dump to
					vm_prog*	prog			Program to be dumped

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void rb_vm_dump( FILE* stream, vm_prog* prog, vm_addr begin_at )
{
	char*	instr;
	char	undefstr[ 32+1 ];
	vm_addr	i;
	int		jmp;
	
	PROC( "rb_vm_dump" );
	PARMS( "prog", "%p", prog );

	for( i = begin_at; i < prog->code_next; i++ )
	{
		switch( prog->code[i] )
		{
			case VMC_NOP:
				instr = "NOP";
				break;
		
			case VMC_PUSHINT:
				instr = "PUSHINT";
				break;
		
			case VMC_PUSHADR:
				instr = "PUSHADR";
				break;
		
			case VMC_PUSHCHR:
				instr = "PUSHCHR";
				break;
		
			case VMC_PUSHDBL:
				instr = "PUSHDBL";
				break;
		
			case VMC_PUSHSTR:
				instr = "PUSHSTR";
				break;
		
			case VMC_RESERVE:
				instr = "RESERVE";
				break;
		
			case VMC_JMP:
				instr = "JMP";
				break;
		
			case VMC_JMPTRUE:
				instr = "JMPTRUE";
				break;

			case VMC_JMPFALSE:
				instr = "JMPFALSE";
				break;
		
			case VMC_CALL:
				instr = "CALL";
				break;
		
			case VMC_NCALL:
				instr = "NCALL";
				break;
		
			case VMC_DROP:
				instr = "DROP";
				break;
				
			case VMC_NSTORE:
				instr = "NSTORE";
				break;
				
			case VMC_NLOAD:
				instr = "NLOAD";
				break;

			case VMC_STORE:
				instr = "STORE";
				break;
		
			case VMC_LOAD:
				instr = "LOAD";
				break;
		
			case VMC_PTRLOAD:
				instr = "PTRLOAD";
				break;
				
			case VMC_CPYLOAD:
				instr = "CPYLOAD";
				break;
		
			case VMC_MOV:
				instr = "MOV";
				break;
		
			case VMC_RET:
				instr = "RET";
				break;

			case VMC_RETV:
				instr = "RETV";
				break;

			case VMC_PUSHFRM:
				instr = "PUSHFRM";
				break;
		
			case VMC_INIT:
				instr = "INIT";
				break;

			case VMC_FREE:
				instr = "FREE";
				break;

			case VMC_JOIN:
				instr = "JOIN";
				break;
		
			case VMC_ADD:
				instr = "ADD";
				break;
		
			case VMC_SUB:
				instr = "SUB";
				break;
		
			case VMC_DIV:
				instr = "DIV";
				break;
		
			case VMC_MUL:
				instr = "MUL";
				break;
		
			case VMC_MOD:
				instr = "MOD";
				break;
		
			case VMC_NOT:
				instr = "NOT";
				break;
		
			case VMC_EQU:
				instr = "EQU";
				break;
		
			case VMC_NEQ:
				instr = "NEQ";
				break;
		
			case VMC_GRT:
				instr = "GRT";
				break;
		
			case VMC_LWR:
				instr = "LWR";
				break;
		
			case VMC_GRE:
				instr = "GRE";
				break;
		
			case VMC_LWE:
				instr = "LWE";
				break;
		
			case VMC_NEG:
				instr = "NEG";
				break;
		
			case VMC_AND:
				instr = "AND";
				break;
		
			case VMC_OR:
				instr = "OR";
				break;
		
			case VMC_XOR:
				instr = "XOR";
				break;
		
			case VMC_LAND:
				instr = "LAND";
				break;
		
			case VMC_LOR:
				instr = "LOR";
				break;
				
			default:
				
				instr = undefstr;
				sprintf( undefstr, "undefined %d", prog->code[i] );
				break;
		}

		fprintf( stream, "%06lx %s", i, instr );
		
		if( ( jmp = VM_GET_PARAM_SIZE( prog->code[i] ) ) )
		{
			switch( prog->code[i] )
			{
				case VMC_PUSHINT:
					fprintf( stream, " (long) %ld",
								*( (long*)( prog->code + i + 1 ) ) );
					break;
					
				case VMC_PUSHCHR:
					fprintf( stream, " (char) %d",
								*( (char*)( prog->code + i + 1 ) ) );
					break;
			
				case VMC_PUSHDBL:
					fprintf( stream, " (double) %f",
								*( (double*)( prog->code + i + 1 ) ) );
					break;
			
				case VMC_PUSHSTR:
					fprintf( stream, " (string) >%s<", 
								*( (char**)( prog->code + i + 1 ) ) );
					break;

				default:
					fprintf( stream, " (address) %06lx", *( (vm_addr*)(
											prog->code + i + 1 ) ) );
					break;
			}
			
			i += jmp;
		}

		fprintf( stream, "\n" );
	}
	
	VOIDRET;
}
