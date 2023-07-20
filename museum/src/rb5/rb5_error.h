#ifndef ERROR_H
#define ERROR_H

/*
string error_msg[] =
{
	"Parse error - Misplaced character",
	"Unknown instruction: ",
	"Parameter expected at ",
	"Label out of range: ",
	"Label not found: ",
	"ENDPROC without PROC",
	"ENDFUNC without FUNC",
	"Include file not found: ",
	"Use of RET out of a function or procedure",
	"PROC without closing ENDPROC",
	"FUNC without closing ENDFUNC",
	"Double definition of Procedure or Function: ",
	"UNTIL without REPEAT",
	"CONT without REPEAT",
	"Misplaced procedure definition",
};
*/

#define ERR_PARSE_ERROR 		0
#define ERR_UNKNOWN_INSTRUCTION 	1
#define ERR_PARAMETER_EXPECTED 		2
#define ERR_LABEL_OUT_OF_RANGE	 	3
#define ERR_LABEL_NOT_FOUND 		4
#define ERR_ENDPROC_WITHOUT_PROC 	5
#define ERR_ENDFUNC_WITHOUT_FUNC 	6
#define ERR_INCLUDEFILE_NOT_FOUND 	7
#define ERR_RET_OUTER_PROC		8
#define ERR_NO_ENDPROC			9
#define ERR_NO_ENDFUNC			10
#define ERR_DOUBLE_PROC_DEFINITION	11
#define ERR_UNTIL_WITHOUT_REPEAT	12
#define ERR_CONT_WITHOUT_REPEAT		13
#define ERR_MISPLACED_PROCDEF		14
#define ERR_INVALID_VARNAME		15

#define ERROR_NULL ""


#endif