/*
J.M.K S.F. RapidBATCH© 32-Bit 5.2.0 Runtime Library 3.2 Final (RBRUN32.RTL)
Copyright (C) 2000-2007 by J.M.K S.F., Jan Max Meyer
*/

#define RB5RTL_CPP //A "MUST BE" FOR ANY RAPIDBATCH RUNTIME COMPILE

#define RBATCH_VERSION_INFO "5.2"
#define RBRLSUPPORT 2.2
//#define DEBUG_SESSION ON
#define RUNTIME_COMPILE TRUE
//#define START_FILE "C:\\Documents and Settings\\jmksf\\Desktop\\omega2\\omega_main.coc"
//#define START_FILE "C:\\jmksf\\rb5\\bla.coc"
//#define START_DIR "C:\\Documents and Settings\\jmksf\\Desktop\\omega2"
//#define BETA_INFORMATION TRUE

#ifndef RUNTIME_COMPILE
#define SEEKTO 82 //For testing only
#endif

//------------------------------------------------------------------------------

#include "../rb5/rb5_base.h"

/*
#pragma argsused
#pragma hdrstop

USEUNIT("..\rb5\rb5_str.cpp");
USEUNIT("..\rb5\rb5_varsys.cpp");
USEUNIT("..\rb5\rb5_sendkeys.cpp");
USEUNIT("..\rb5\rb5_helpfct.cpp");
USERC("RBICON.RC");
*/
//------------------------------------------------------------------------------
#ifdef DEBUG_SESSION
bool preVars_Done = false;
char dbg_file[] = "c:\\rbrun5_debug.txt";
FILE* debug = (FILE*) NULL;
int debug_level = 3;

int malloc_cnt = 0;
int free_cnt = 0;
#endif

#define LIBDIR "LIB"

bool rbrunVarName_Extract(char* vname, char** result, int* dim_info)
{
	int dim_pos = 1;
	int onVarFind = 0;
	bool onConstFind = false;
	char op = (char)NULL;

	if (*result != (char*)NULL)
	{
#ifdef DEBUG_SESSION
		FREE_DEBUG(*result);
#endif
		FREE(*result);
	}

	*result = (char*)MALLOC((strlen(vname) + 1) * sizeof(char));
	*result[0] = '\0';

#ifdef DEBUG_SESSION
	MALLOC_DEBUG(*result, (strlen(vname) + 1) * sizeof(char));
#endif

	char* tmp = (char*)MALLOC((strlen(vname) + 1) * sizeof(char));
	*tmp = '\0';

#ifdef DEBUG_SESSION
	MALLOC_DEBUG(tmp, (strlen(vname) + 1) * sizeof(char));
#endif


#ifdef DEBUG_SESSION
	if (debug_level == 3 && debug != (FILE*) NULL) fprintf(debug, "rbrunVarName_Extract() %s\n", vname);
	fflush(debug);
#endif

	for (unsigned int a=0; a<strlen(vname); a++)
	{
		switch (vname[a])
		{
			case ':':
			case ' ':
			case ',':
				break;
			case '[':
				if (!onConstFind)
				{
					onVarFind++;
					if (onVarFind > 1) strcat(tmp, "[");
				}
				break;

			case ']':
				if (onVarFind == 1 && !onConstFind)
				{
					if (op == (char)NULL)
					{
						char* dim_str = (char*)NULL;
						if (rbrunGetVar(tmp, &dim_str, procStack_cnt))
						{
							dim_info[dim_pos] = atoi(dim_str);
							dim_pos++;
						}

#ifdef DEBUG_SESSION
						FREE_DEBUG(dim_str);
#endif

						FREE(dim_str);
					}
					else
					{
						char* dim_str = (char*)NULL;
						if (rbrunGetVar(tmp, &dim_str, procStack_cnt))
						{
							if (strcmp(dim_str, "") != 0)
							{
								switch (op)
								{
									case '+':
										dim_info[dim_pos-1] += atoi(dim_str);
										break;
									case '-':
										dim_info[dim_pos-1] -= atoi(dim_str);
										break;
									case '*':
										dim_info[dim_pos-1] *= atoi(dim_str);
										break;
									case '/':
										dim_info[dim_pos-1] = (int)(dim_info[dim_pos-1] / atoi(dim_str));
										break;
									case '~':
										dim_info[dim_pos-1] = dim_info[dim_pos-1] % atoi(dim_str);
										break;
								}
							}
						}

#ifdef DEBUG_SESSION
						FREE_DEBUG(dim_str);
#endif

						FREE(dim_str);
						op = (char)NULL;
					}

					tmp[0] = '\0';
					onVarFind--;
				}
				else if (onVarFind > 1)
				{
					strcat(tmp, "]");
					onVarFind--;
				}
				break;
			case '\'':
				if (!onConstFind && onVarFind == 0)
				{
					onConstFind = true;
					break;
				}
				else if (onConstFind && onVarFind == 0)
				{
					if (op == (char)NULL)
					{
						dim_info[dim_pos] = atoi(tmp);
						dim_pos++;
					}
					else
					{
						switch (op)
						{
							case '+':
								dim_info[dim_pos-1] += atoi(tmp);
								break;
							case '-':
								dim_info[dim_pos-1] -= atoi(tmp);
								break;
							case '*':
								dim_info[dim_pos-1] *= atoi(tmp);
								break;
							case '/':
								dim_info[dim_pos-1] = (int)(dim_info[dim_pos-1] / atoi(tmp));
								break;
							case '~':
								dim_info[dim_pos-1] = dim_info[dim_pos-1] % atoi(tmp);
								break;
						}
						op = (char)NULL;
					}

					*tmp = '\0';
					onConstFind = false;
					break;
				}
				//Value Calculations -----------------------------------
			case '+':
			case '-':
			case '*':
			case '/':
			case '~':
				if (!onConstFind && onVarFind == 0)
				{
					op = vname[a];
					break;
				}

			default:
				if (onVarFind == 0 && !onConstFind)
				{
					(*result)[strlen(*result)+1] = '\0';
					(*result)[strlen(*result)] = vname[a];
				}
				else
				{
					tmp[strlen(tmp)+1] = '\0';
					tmp[strlen(tmp)] = vname[a];
				}
		}
	}
	dim_info[0] = dim_pos-1;

	for (int a = 1; a < dim_pos;a++)
	{
		dim_info[a]--;
		if (dim_info[a] < 0) dim_info[a] = -1;
	}

#ifdef DEBUG_SESSION
	FREE_DEBUG(tmp);
#endif
	FREE(tmp);

	return true;
}


//------------------------------------------------------------------------------
// rbrunNewVar
// New variable
//------------------------------------------------------------------------------
bool rbrunNewVar(char* vname, int area)
{
#ifdef DEBUG_SESSION
	if (debug_level >= 2 && debug != (FILE*) NULL) fprintf(debug, "rbrunNewVar() %s\n", vname);
	fflush(debug);
#endif

	int dim[MAXDIM];
	char* basename = (char*)NULL;

	if (strcmp(vname, PVAR_NULL) == 0)
		return true;

	if (!rbrunVarName_Extract(vname, &basename, dim))
	{
		FREE(basename);
		return false;
	}

	//Add to last procedure stack if this variable is local
	if (area > 0)
	{
		if (procStack[area-1].var_cnt == 0)
		{
			procStack[area-1].vars = (char**) malloc(sizeof(char*));
#ifdef DEBUG_SESSION
			MALLOC_DEBUG(procStack[area-1].vars, sizeof(char*));
#endif
		}
		else
		{
			procStack[area-1].vars = (char**) realloc((char**)procStack[area-1].vars, (procStack[area-1].var_cnt+1) * sizeof(char*));
#ifdef DEBUG_SESSION
			REALLOC_DEBUG(procStack[area-1].vars, sizeof(char*));
#endif
		}
		procStack[area-1].vars[procStack[area-1].var_cnt] = (char*) malloc((strlen(basename)+1) * sizeof(char));
#ifdef DEBUG_SESSION
		MALLOC_DEBUG(procStack[area-1].vars[procStack[area-1].var_cnt], (strlen(basename)+1) * sizeof(char));
#endif

		strcpy(procStack[area-1].vars[procStack[area-1].var_cnt], basename);
		procStack[area-1].var_cnt++;
	}

	for (int a = 1; a < MAXDIM; a++)
	{
		dim[a]++;
	}

	newVar(&Var, (char*)basename, "", area, dim);

#ifdef DEBUG_SESSION
	FREE_DEBUG(basename);
#endif

	FREE(basename);
	return true;
}

//------------------------------------------------------------------------------
// rbrunDelVar
// Delete variable
//------------------------------------------------------------------------------
bool rbrunDelVar(char* vname, bool completely)
{
#ifdef DEBUG_SESSION
	if (debug_level >= 2 && debug != (FILE*) NULL) fprintf(debug, "rbrunDelVar() %s\n", vname);
	fflush(debug);
#endif

	int dim[MAXDIM];
	char* basename = (char*)NULL;

	if (strcmp(vname, PVAR_NULL) == 0)
		return true;

	if (!rbrunVarName_Extract(vname, &basename, dim))
		return false;

	if (delVar(&Var, (char*)basename, dim, completely) == 0)
	{
		FREE(basename);
		return true;
	}

	FREE(basename);
	return false;
}


//------------------------------------------------------------------------------
// rbrunGetVar
// gets values from variables
//------------------------------------------------------------------------------
bool rbrunGetVar(char* vname, char** retval, int area)
{
#ifdef DEBUG_SESSION
	if (debug_level >= 2 && debug != (FILE*) NULL) fprintf(debug, "rbrunGetVar() %s\n", vname);
	fflush(debug);
#endif

	int dim[MAXDIM];
	char* basename = (char*)NULL;

	if (*retval != (char*)NULL)
	{
		FREE(*retval);
		*retval = (char*)NULL;
	}

	//null-variable
	if (strcmp(vname, PVAR_NULL) == 0)
	{
		*retval = (char*) MALLOC(sizeof(char));
		**retval = '\0';

		return true;
	}

	if (!rbrunVarName_Extract(vname, &basename, dim))
		return false;

	//Look-up build-in variables first
	if (dim[0] == 0 && rbGetBuildInVar(basename, retval))
	{
		FREE(basename);
		return true;
	}

	//Perform standard variable get
	if (dim[0] > 0)
	{
		for (int a = 1; a <= dim[0]; a++)
		{
			if (dim[a] == -1)
			{
				*retval = (char*)MALLOC((STDLINE+1) * sizeof(char));

#ifdef DEBUG_SESSION
				MALLOC_DEBUG(*retval, (STDLINE+1) * sizeof(char));
#endif

				long dimsize = varSize(&Var, (char*)basename, area, dim);
				if (dimsize == -1) dimsize = 0;

				sprintf(*retval, "%ld", dimsize);

				FREE(basename);
				return true;
			}
		}
	}

	int ret = getVar(&Var, (char*)basename, area, dim, retval);
#ifdef DEBUG_SESSION
	if (ret == VARSYS_OKAY)
		malloc_cnt++;
#endif

	FREE(basename);

	if (*retval != (char*)NULL
	        && ret == VARSYS_OKAY)
		return true;

	*retval = (char*) MALLOC(sizeof(char));
	**retval = '\0';

#ifdef DEBUG_SESSION
	MALLOC_DEBUG(*retval, sizeof(char));
#endif


	return false;
}

//------------------------------------------------------------------------------
// rbrunLetVar
// lets values to variables
//------------------------------------------------------------------------------
bool rbrunLetVar(char* vname, char* value, int area)
{
#ifdef DEBUG_SESSION
	if (debug_level >= 2 && debug != (FILE*) NULL) fprintf(debug, "rbrunLetVar() %s\n", vname);
	fflush(debug);
#endif

	int dim[MAXDIM];
	char* basename = (char*)NULL;

	//null variable
	if (strcmp(vname, PVAR_NULL) == 0)
		return true;

	if (!rbrunVarName_Extract(vname, &basename, dim))
		return false;

	//Look-up build-in variables
	if (dim[0] == 0 && rbLetBuildInVar(basename, value))
	{
		FREE(basename);
		return true;
	}

	switch (letVar(&Var, (char*)basename, (char*)value, area, dim))
	{
		case VARSYS_ERR_INDEXOUTOFRANGE:
			resizeVar(&Var, (char*)basename, "", area, dim);
			if (letVar(&Var, (char*)basename, (char*)value, area, dim) == VARSYS_ERR_INDEXOUTOFRANGE)
			{
				char array_information[STDLINE+1];
				sprintf(array_information, "Array reallocation error\n\nvname: %s value: %s area: %d dimension: %d", basename, value, area, dim[0]);
				VARS(array_information);
			}
			break;
		case VARSYS_ERR_VARNOTFOUND:
			rbrunNewVar(vname, area);
			rbrunLetVar(vname, value, area);

			FREE(basename);
			return true;
	}

#ifdef PROFESSIONAL_COMPILE
	if (autoVarDebug)
	{
		rbDebug_AddVar(vname);
	}
#endif

	FREE(basename);
	return true;
}

//------------------------------------------------------------------------------
// rbrunPtrVar
// lets variable/array pointer to another variables
//------------------------------------------------------------------------------
bool rbrunPtrVar(char* target, char* source, int area)
{
	int ret = 0;

#ifdef DEBUG_SESSION
	if (debug_level >= 2 && debug != (FILE*) NULL) fprintf(debug, "rbrunPtrVar() target = %s, source = %s, area = %d\n",
		        target, source, area);
	fflush(debug);
#endif

	int dim[MAXDIM];
	char* basename = (char*)NULL;

	if (!rbrunVarName_Extract(source, &basename, dim))
		return false;

	if ( strcmp( basename, PVAR_NULL ) == 0 )
	{
		rbrunNewVar( target, area );

		FREE(basename);
		return true;
	}

	if ((ret = ptrVar(&Var, (char*)basename, area-1, dim, &Var, (char*)target, area)) != VARSYS_OKAY)
	{
		switch (ret)
		{
			case VARSYS_ERR_VARNOTFOUND:
				rbrunNewVar(source, area-1);
				break;
			case VARSYS_ERR_INDEXOUTOFRANGE:
				resizeVar(&Var, (char*)basename, "", area-1, dim);
				break;
		}

		if (ptrVar(&Var, (char*)basename, area-1, dim, &Var, (char*)target, area) != VARSYS_OKAY)
		{
			FREE(basename);
			return false;
		}
	}

	FREE(basename);
	return true;
}

//------------------------------------------------------------------------------
// rbrunVarExists
// lets variable/array pointer to another variables
//------------------------------------------------------------------------------
bool rbrunVarExists(char* vname, int area)
{
	int dim[MAXDIM];
	char* basename = (char*)NULL;

	if (!rbrunVarName_Extract(vname, &basename, dim))
		return false;

	bool ret = varExists(&Var, (char*)basename, area, dim);

	FREE(basename);
	return ret;
}

//------------------------------------------------------------------------------
// rbrunScanVar
// Scans and returns a variable name
//------------------------------------------------------------------------------
bool rbrunScanVar(char** vname)
{
	int onVarFind = 0;
#ifdef DEBUG_SESSION
	if (debug_level == 3) fprintf(debug, "rbrunScanVar()\n");
#endif

	if (*vname != (char*)NULL)
	{
		FREE(*vname);
	}
	*vname = (char*)NULL;
	int pos = 1;

	(void)rbrunScanChar("[");

	while (!CURRENT_FILEEND)
	{
		char chr = rbrunGetChar();

		switch (chr)
		{
			case '[':
				onVarFind++;
				break;
			case ']':
				if (onVarFind == 0) return true;
				onVarFind--;
				break;
			case ' ':
			case '\t':
				break;
			case '\n':
				CHARACTER_BACK();
				return false;
		}

		if (*vname == (char*)NULL)
		{
			*vname = (char*)MALLOC((pos+1) * sizeof(char));
		}
		else
		{
			*vname = (char*)realloc((char*)*vname, (pos+1) * sizeof(char));
		}
		(*vname)[pos-1] = chr;
		(*vname)[pos] = '\0';
		pos++;
	}

	return false;
}

//------------------------------------------------------------------------------
// rbAddLabel
// Adds a new label to the label information array
//------------------------------------------------------------------------------
void rbAddLabel(char* label_name)
{
	strtrim(label_name);
	char* procedure_name = (char*)NULL;

	if (procStack_cnt > 0)
	{
		procedure_name = procStack[procStack_cnt-1].proc_name;
	}

	for (int a = 0; a < label_cnt; a++)
	{
		if (strcmp(labels[a].label_name, label_name) == 0)
		{
			if ( labels[a].proc_name == (char*)NULL
			        || strcmp( labels[a].proc_name, procedure_name ) == 0 )
			{
				return;
			}
		}
	}

	if (label_cnt == 0)
	{
		labels = (LINFO*) malloc(sizeof(LINFO));
	}
	else
	{
		labels = (LINFO*) realloc((LINFO*)labels, (label_cnt+1) * sizeof(LINFO));
	}

	labels[label_cnt].label_name = (char*) malloc((strlen(label_name)+1) * sizeof(char));
	strcpy(labels[label_cnt].label_name, label_name);

	if (procedure_name != (char*)NULL)
	{
		labels[label_cnt].proc_name = strdup(procedure_name);
	}
	else
	{
		labels[label_cnt].proc_name = (char*)NULL;
	}

	labels[label_cnt].label_pos = file_offset;

	label_cnt++;
}

//------------------------------------------------------------------------------
// rbrunGetChar()
// gets character from current file position
//------------------------------------------------------------------------------
inline char rbrunGetChar()
{
	if (CURRENT_FILEEND) return (char)NULL;
	file_offset++;

#ifdef DEBUG_SESSION
	if (debug_level == 3) fprintf(debug, "rbrunGetChar() : %c\n", filecode[file_offset-1]);
#endif

	if (filecode[file_offset-1] == (char)1) return '\n';
	return filecode[file_offset-1];
}

//------------------------------------------------------------------------------
// rbrunScanChar()
// scans string for character
//------------------------------------------------------------------------------
char rbrunScanChar(char parseto[8+1])
{
#ifdef DEBUG_SESSION
	if (debug_level == 3)
	{
		fprintf(debug, "rbrunScanChar() parseto = >%s<, errormode = >%d\n", parseto);
		fflush(debug);
	}
#endif

	while (!CURRENT_FILEEND)
	{
		char chr = rbrunGetChar();

		for (unsigned int a = 0; a < strlen(parseto); a++)
		{
			if (chr == parseto[a]) return chr;
		}

		switch (chr)
		{
			case ' ':
			case '\t':
				break;
			default:
				return chr;

		}
	}
	return NULL;
}

//------------------------------------------------------------------------------
// rbrunScan()
// scans to a specified character in the file
//------------------------------------------------------------------------------
bool rbrunScan(char parseto, char** retstr)
{
#ifdef DEBUG_SESSION
	if (debug_level >= 3) fprintf(debug, "rbrunScan()\n");
	fflush(debug);
#endif

	if (retstr)
	{
		if (*retstr != (char*)NULL)
		{
			FREE(*retstr);
		}
		*retstr = (char*) MALLOC(sizeof(char));
		(*retstr)[0] = '\0';

#ifdef DEBUG_SESSION
		MALLOC_DEBUG(*retstr, sizeof(char));
#endif
	}

	unsigned long int pos = 1;

	while (!CURRENT_FILEEND)
	{
		char chr = rbrunGetChar();

		//if(chr == parseto || chr == '\n') return restext;
		if (chr == parseto)
		{
			if (!retstr || *retstr != (char*)NULL)
			{
				return true;
			}

			return false;
		}
		if (chr == '\n')
		{
			return false;
		}

		if (retstr)
		{
			*retstr = (char*)realloc((char*) *retstr, (pos+1) * sizeof(char));
			(*retstr)[pos-1] = chr;
			(*retstr)[pos] = '\0';
			pos++;
		}
	}

	return false;
}


//------------------------------------------------------------------------------
// rbrunGetFactor
// parses and executes a factor
//------------------------------------------------------------------------------
bool rbrunGetFactor( char** retval )
{
	char chr;
	char* varname = (char*)NULL;

#ifdef DEBUG_SESSION
	if (debug_level >= 2) fprintf(debug, "rbrunGetFactor()\n");
	fflush(debug);
#endif

	chr = rbrunScanChar( "'[(" );

#ifdef DEBUG_SESSION
	if (debug_level >= 2) fprintf(debug, "rbrunGetFactor() chr = >%c<\n", chr);
	fflush(debug);
#endif
	switch ( chr )
	{
		case '[':
			CHARACTER_BACK();
			rbrunScanVar(&varname);
			rbrunGetVar(varname, retval, procStack_cnt);
			FREE(varname);
			break;

		case '\'':
			rbrunScan('\'', retval);
			break;

		case '(':
			rbrunGetValue( retval );

			if ( rbrunScanChar( ")" ) != ')' )
				return false;

			break;

		default:
			// Error
			return false;
	}

#ifdef DEBUG_SESSION
	if (debug_level >= 2)
		fprintf(debug, "\tsucceeded, read value: >%s<\n", *retval);
	fflush(debug);
#endif

	return true;
}


//------------------------------------------------------------------------------
// rbrunGetTerm
// parses and executes a term
//------------------------------------------------------------------------------
bool rbrunGetTerm( char** retval )
{
	char chr;
	char* operand = (char*)NULL;

#ifdef DEBUG_SESSION
	if (debug_level >= 2) fprintf(debug, "rbrunGetTerm()\n");
	fflush(debug);
#endif

	if ( !rbrunGetFactor( retval ) )
		return false;

	while ( 1 )
	{
		chr = rbrunScanChar( "*/~" );

#ifdef DEBUG_SESSION
		if (debug_level >= 2) fprintf(debug, "\tchr = >%c<\n", chr);
		fflush(debug);
#endif

		operand = (char*)NULL;
		switch ( chr )
		{
			case '*':
				if ( !rbrunGetFactor( &operand ) )
					return false;

				rbCalculate( retval, *retval, operand, RB_CALC_MULTIPLICATION );
				break;

			case '/':
				if ( !rbrunGetFactor( &operand ) )
					return false;

				rbCalculate( retval, *retval, operand, RB_CALC_DIVISION );
				break;

			case '~':
				if ( !rbrunGetFactor( &operand ) )
					return false;

				rbCalculate( retval, *retval, operand, RB_CALC_MODULO );
				break;

			default:
				CHARACTER_BACK();
				return true;
		}
	}

	return false; //Impossible ;)
}


//------------------------------------------------------------------------------
// rbrunGetExpr
// parses and executes an expression
//------------------------------------------------------------------------------
bool rbrunGetExpr( char** retval )
{
	char chr;
	char* operand = (char*)NULL;

#ifdef DEBUG_SESSION
	if (debug_level >= 2) fprintf(debug, "rbrunGetExpr()\n");
	fflush(debug);
#endif

	if ( !rbrunGetTerm( retval ) )
		return false;

	while ( 1 )
	{
		chr = rbrunScanChar( "+-" );

#ifdef DEBUG_SESSION
		if (debug_level >= 2) fprintf(debug, "\tchr = >%c<\n", chr);
		fflush(debug);
#endif

		operand = (char*)NULL;
		switch ( chr )
		{
			case '+':
				if ( !rbrunGetTerm( &operand ) )
					return false;

				rbCalculate( retval, *retval, operand, RB_CALC_ADDITION );
				break;

			case '-':
				if ( !rbrunGetTerm( &operand ) )
					return false;

				rbCalculate( retval, *retval, operand, RB_CALC_SUBTRACTION );
				break;

			default:
				CHARACTER_BACK();
				return true;
		}
	}

	return false; //Impossible ;)
}

//------------------------------------------------------------------------------
// rbrunGetString
// parses and executes a string expression
//------------------------------------------------------------------------------
bool rbrunGetString( char** retval )
{
	char chr;
	char* join = (char*)NULL;

#ifdef DEBUG_SESSION
	if (debug_level >= 2) fprintf(debug, "rbrunGetString()\n");
	fflush(debug);
#endif

	if ( !rbrunGetExpr( retval ) )
		return false;

	while ( 1 )
	{
		chr = rbrunScanChar( "#" );

#ifdef DEBUG_SESSION
		if (debug_level >= 2) fprintf(debug, "\tchr = >%c<\n", chr);
		fflush(debug);
#endif

		join = (char*)NULL;
		switch ( chr )
		{
			case '#':
				if ( !rbrunGetExpr( &join ) )
					return false;

				*retval = (char*)realloc( (char*)(*retval), ( strlen( (*retval) ) + strlen( join ) + 1 ) *
				                          sizeof( char ) );

				strcat( *retval, join );
				free( join );
				break;

			default:
#ifdef DEBUG_SESSION
	if (debug_level >= 2) fprintf(debug, "rbrunGetString() returns with >%s<\n", *retval);
	fflush(debug);
#endif
				CHARACTER_BACK();
				return true;
		}
	}

	return false; //Impossible ;)
}


//------------------------------------------------------------------------------
// rbrunGetCondition
// Performs a condition within an expression, if there is any
//------------------------------------------------------------------------------
bool rbrunGetCondition( char** retval )
{
	char*	checkvalue	[2];
	char	chr;
	bool	ifresult	= false;
	char	res_str		[3];

	enum
	{
	    OPR_NONE	= 0,
	    OPR_EQUAL,
	    OPR_NEQUAL,
	    OPR_GT,
	    OPR_LT,
	    OPR_GTEQUAL,
	    OPR_LTEQUAL
	} opr;


#ifdef DEBUG_SESSION
	if (debug_level >= 2) fprintf(debug, "rbrunGetCondition()\n");
	fflush(debug);
#endif

	if (*retval != (char*)NULL)
	{
		FREE(*retval);
		*retval = (char*)NULL;
	}

	checkvalue[0] = (char*)NULL;
	checkvalue[1] = (char*)NULL;

	if ( !rbrunGetString( &checkvalue[0] ) )
		return false;

	while ( 1 )
	{
		chr = rbrunScanChar( "=!<>" );

		switch ( chr )
		{
			case '=':
				opr = OPR_EQUAL;
				break;
			case '!':
				opr = OPR_NEQUAL;
				break;
			case '>':
				opr = OPR_GT;

				if ( rbrunScanChar( "=" ) == '=' )
					opr = OPR_GTEQUAL;
				else
					CHARACTER_BACK();
				break;
			case '<':
				opr = OPR_LT;

				if ( rbrunScanChar( "=" ) == '=' )
					opr = OPR_LTEQUAL;
				else
					CHARACTER_BACK();
				break;

			default:
				opr = OPR_NONE;
				CHARACTER_BACK();

				*retval = checkvalue[0];
				return true;
		}

		rbrunGetString( &checkvalue[1] );


#ifdef DEBUG_SESSION
		if (debug_level >= 2) fprintf(debug, "\tchr = >%c< opr = %d >%s< >%s<\n", chr, opr, checkvalue[0], checkvalue[1]);
		fflush(debug);
#endif

		ifresult = false;
		switch ( opr )
		{
			case OPR_EQUAL:
				if ( strcmp( checkvalue[0], checkvalue[1] ) == 0 )
					ifresult = true;
				break;

			case OPR_NEQUAL:
				if ( strcmp( checkvalue[0], checkvalue[1] ) != 0 )
					ifresult = true;
				break;

			case OPR_GT:
			case OPR_LT:
			case OPR_GTEQUAL:
			case OPR_LTEQUAL:
				{
					float val1 = atof( checkvalue[0] );
					float val2 = atof( checkvalue[1] );

					switch ( opr )
					{
						case OPR_GT:
							if ( val1 != 0 && val2 != 0 )
							{
								if ( val1 > val2 )
									ifresult = true;
							}
							else
							{
								if ( strcmp( checkvalue[0], checkvalue[1] ) > 0 )
									ifresult = true;
							}
							break;

						case OPR_LT:
							if ( val1 != 0 && val2 != 0 )
							{
								if ( val1 < val2 )
									ifresult = true;
							}
							else
							{
								if ( strcmp( checkvalue[0], checkvalue[1] ) < 0 )
									ifresult = true;
							}
							break;

						case OPR_GTEQUAL:
							if ( val1 != 0 && val2 != 0 )
							{
								if ( val1 >= val2 )
									ifresult = true;
							}
							else
							{
								if ( strcmp( checkvalue[0], checkvalue[1] ) >= 0 )
									ifresult = true;
							}
							break;

						case OPR_LTEQUAL:
							if ( val1 != 0 && val2 != 0 )
							{
								if ( val1 <= val2 )
									ifresult = true;
							}
							else
							{
								if ( strcmp( checkvalue[0], checkvalue[1] ) <= 0 )
									ifresult = true;
							}
							break;
					}
				}
				break;
		}

#ifdef DEBUG_SESSION
		if (debug_level >= 2) fprintf(debug, "\tifresult = %d\n", ifresult);
		fflush(debug);
#endif

		free( checkvalue[0] );
		sprintf( res_str, "%s", ( ifresult ) ? "0" : "-1" );
		checkvalue[0] = strdup( res_str );

		free( checkvalue[1] );
		checkvalue[1] = (char*)NULL;
	}

	return false; //Impossible ;)
}

//------------------------------------------------------------------------------
// rbrunGetANDExpr
// Parses a AND-linked expression from code
//------------------------------------------------------------------------------
bool rbrunGetANDExpr( char** retval )
{
	char*	checkvalue	[2];
	bool	ifresult	= false;
	char	res_str		[3];
	char	chr;

#ifdef DEBUG_SESSION
	if (debug_level >= 2) fprintf(debug, "rbrunGetANDExpr()\n");
	fflush(debug);
#endif

	if (*retval != (char*)NULL)
	{
		FREE(*retval);
		*retval = (char*)NULL;
	}

	checkvalue[0] = (char*)NULL;
	checkvalue[1] = (char*)NULL;
	if ( !rbrunGetCondition( &checkvalue[0] ) )
		return false;

	while ( 1 )
	{
		chr = rbrunScanChar( "&" );

		switch ( chr )
		{
			case '&':
				if ( !rbrunGetCondition( &checkvalue[1] ) )
					return false;

				if ( !IS_FALSE( checkvalue[0] ) && !IS_FALSE( checkvalue[1] )
				       /*  || strcmp( checkvalue[0], checkvalue[1] ) == 0 */ )
					ifresult = true;
				else
					ifresult = false;

				free( checkvalue[0] );
				sprintf( res_str, "%s", ( ifresult ) ? "0" : "-1" );
				checkvalue[0] = strdup( res_str );

				free( checkvalue[1] );
				checkvalue[1] = (char*)NULL;

				break;

			default:
				CHARACTER_BACK();
				*retval = checkvalue[0];
				return true;
		}
	}

	return false; //Impossible ;)
}


//------------------------------------------------------------------------------
// rbrunGetORExpr
// Parses a OR-linked expression from code
//------------------------------------------------------------------------------
bool rbrunGetORExpr( char** retval )
{
	char*	checkvalue	[2];
	bool	ifresult	= false;
	char	res_str		[3];
	char	chr;

#ifdef DEBUG_SESSION
	if (debug_level >= 2) fprintf(debug, "rbrunGetORExpr()\n");
	fflush(debug);
#endif

	checkvalue[0] = (char*)NULL;
	checkvalue[1] = (char*)NULL;
	if ( !rbrunGetANDExpr( &checkvalue[0] ) )
		return false;

	while ( 1 )
	{
		chr = rbrunScanChar( "|" );

		switch ( chr )
		{
			case '|':
				if ( !rbrunGetANDExpr( &checkvalue[1] ) )
					return false;

				if ( !IS_FALSE( checkvalue[0] ) || !IS_FALSE( checkvalue[1] ) )
					ifresult = true;
				else
					ifresult = false;

				free( checkvalue[0] );
				sprintf( res_str, "%s", ( ifresult ) ? "0" : "-1" );
				checkvalue[0] = strdup( res_str );

				free( checkvalue[1] );
				checkvalue[1] = (char*)NULL;

				break;

			default:
				CHARACTER_BACK();
				*retval = checkvalue[0];
				return true;
		}
	}

	return false; //Impossible ;)
}

//------------------------------------------------------------------------------
// rbrunGetValue
// Entry point for expression parser
//------------------------------------------------------------------------------
bool rbrunGetValue( char** retval )
{
#ifdef DEBUG_SESSION
	if (debug_level >= 2) fprintf(debug, "rbrunGetValue()\n");
	fflush(debug);
#endif

	if (*retval != (char*)NULL)
	{
		FREE(*retval);
		*retval = (char*)NULL;
	}

	bool ret = rbrunGetORExpr( retval );

	return ret;
}



// #############################################################################
// rbrunCondition
// #############################################################################
bool rbrunCondition(void)
{
	bool ifResult = false;
	char* val = (char*)NULL;

#ifdef DEBUG_SESSION
	if (debug_level >= 1) fprintf(debug, "rbrunCondition()\n");
	fflush(debug);
#endif

	if ( rbrunGetValue( &val ) )
	{
		if ( !IS_FALSE( val ) )
			ifResult = true;
	}

	if ( val != (char*)NULL )
		free( val );

#ifdef DEBUG_SESSION
	if (debug_level >= 1) fprintf(debug, "rbrunCondition() returns %d\n", ifResult);
	fflush(debug);
#endif

	return ifResult;
}



// #############################################################################
// rbGetSymbol
// #############################################################################
inline bool rbGetSymbol(char** symbol)
{
	if (*symbol != (char*)NULL)
	{
		FREE(*symbol);
		*symbol = (char*)NULL;
	}

	*symbol = (char*)MALLOC((4+1) * sizeof(char));
#ifdef DEBUG_SESSION
	MALLOC_DEBUG(*symbol, (4+1) * sizeof(char));
#endif


	//char code[4+1];
	for (int a = 0; a < 4; a++)
	{
		(*symbol)[a] = rbrunGetChar();
		symbreak = (*symbol)[a];

		if ((*symbol)[a] == '\n') a--;
	}
	(*symbol)[4] = '\0';

#ifdef DEBUG_SESSION
	if (debug_level >= 0) fprintf(debug, "rbGetSymbol() >%s<\n", *symbol);
#endif

	return true;
}

// #############################################################################
// rbrunReadTo
// #############################################################################
bool rbrunReadTo(char* readTo, bool isLabel)
{
#ifdef DEBUG_SESSION
	if (debug_level >= 1) fprintf(debug, "rbrunReadTo() readto = %s isLabel = %d\n", readTo, (int) isLabel);
#endif
	int loops_found = 0;

	if (isLabel)
	{
		//Find this label in label information resource
		for (int a = 0; a < label_cnt; a++)
		{
			/* if(strcmp(labels[a].label_name, readTo) == 0) */

			if (strcmp(labels[a].label_name, readTo) == 0
			        && (labels[a].proc_name == (char*)NULL ||
			            (procStack_cnt > 0 && strcmp(labels[a].proc_name, procStack[procStack_cnt-1].proc_name) == 0) ) )
			{
				file_offset = labels[a].label_pos;

				if (loop_cnt > 0)
				{
					for (int b=0; b < loop_cnt; b++)
					{
						if (loops[b].proc_name != (char*)NULL)
							free(loops[b].proc_name);
					}

					free(loops);
					loop_cnt = 0;
				}

				return true;
			}
		}
	}


	while (!CURRENT_FILEEND)
	{
		rbGetSymbol(&cmd);
		strupr(cmd);

		//MessageBox(NULL, cmd.c_str(), "readTo", 16);
		if (strcmp(cmd, "RLBL") == 0) //Label
		{
			char* labelName = (char*)NULL;

			rbrunScan('\n', &labelName);
			strtrim(labelName);

			rbAddLabel(labelName);

			if (isLabel && strcmp(labelName, readTo) == 0)
			{
				FREE(labelName);
				return true;
			}

			FREE(labelName);
		}
		else
		{
			if (isLabel)
			{
				if (strcmp(cmd, "RRPT") == 0)
				{
					loops_found++;
					if (loop_cnt == 0)
					{
						loops = (LOOPINFO*) malloc(sizeof(LOOPINFO));
					}
					else
					{
						loops = (LOOPINFO*) realloc((LOOPINFO*) loops, (loop_cnt+1)*sizeof(LOOPINFO));
					}

					loops[loop_cnt].loop_begin = file_offset;

					if (procStack_cnt > 0)
					{
						loops[loop_cnt].proc_name = (char*) malloc((strlen(procStack[procStack_cnt-1].proc_name)+1) * sizeof(char));
						strcpy(loops[loop_cnt].proc_name, procStack[procStack_cnt-1].proc_name);
					}
					else
					{
						loops[loop_cnt].proc_name = (char*) NULL;
					}
					loop_cnt++;
				}
				else if (strcmp(cmd, "RUNT") == 0)
				{
					if (loops_found == 0) continue;
					loops_found--;

					(void)rbrunCondition();
					if (loop_cnt == 0) continue;

					loop_cnt--;

					if (loops[loop_cnt].proc_name != (char*)NULL)
						free(loops[loop_cnt].proc_name);

					if (loop_cnt == 0)
					{
						free(loops);
					}
					else
					{
						loops = (LOOPINFO*) realloc((LOOPINFO*) loops, (loop_cnt)*sizeof(LOOPINFO));
					}
				}
			}
			else if (strcmp(cmd, readTo) == 0 && !isLabel)
			{
				return true;
			}

			(void)rbrunScan('\n', NULL);
		}
	}

	return false;
}

//------------------------------------------------------------------------------
// rbrunExecute()
//------------------------------------------------------------------------------
int rbrunExecute(void)
{
#ifdef DEBUG_SESSION
	CHECK_TERMINATOR();

	if (debug_level >= 1) fprintf(debug, "rbrunExecute()\n");

#endif

	rbGetSymbol(&cmd);

#ifdef DEBUG_SESSION
	fflush(debug);
#endif

	// (Label) -------------------------------------------------------------
	if (strcmp(cmd, "RLBL") == 0) //Label
	{
		rbrunScan('\n', &TempStr[0]);
		strtrim(TempStr[0]);

		rbAddLabel(TempStr[0]);
		return RB_OK_IGNORE;
	}


	strupr(cmd);

	// Runtime Languag REVISION SPEC ---------------------------------------
	if (strcmp(cmd, "RRLR") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		TempNum1 = atof(TempStr[0]);
		if (TempNum1 != (float) RBRLSUPPORT)
		{
			MessageBox(NULL, "ERROR: RTL uses newer RBRL-specification!\nThis may result unpredictable results.", "RapidBATCH Runtime Language Conflict", 16);
			return RB_TERMINATE;
		}
#ifdef DEBUG_SESSION
		DEBUG(">>> RRLR Check OK");
#endif

		return RB_OK;
	}
	// ENDIF ---------------------------------------------------------------
	if (strcmp(cmd, "RXEI") == 0) return RB_OK;

	// DEC -----------------------------------------------------------------
	if (strcmp(cmd, "R000") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		RB_NEWVAR(TempStr[0]);
		while (rbrunScanChar("\n") != '\n')
		{
			CHARACTER_BACK();
			rbrunScanVar(&TempStr[0]);
			RB_NEWVAR(TempStr[0]);
		}
		CHARACTER_BACK();
		return RB_OK;
	}
	// RESET ---------------------------------------------------------------
	if (strcmp(cmd, "R052") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunDelVar(TempStr[0], false);

		while (rbrunScanChar("\n") != '\n')
		{
			CHARACTER_BACK();
			rbrunScanVar(&TempStr[0]);
			rbrunDelVar(TempStr[0], false);
		}
		CHARACTER_BACK();
		return RB_OK;
	}
	// LET -----------------------------------------------------------------
	if (strcmp(cmd, "R001") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		RB_LETVAR(TempStr[0], TempStr[1]);
		return RB_OK;
	}
	// IF ------------------------------------------------------------------
	if (strcmp(cmd, "RXIF") == 0
	        || strcmp(cmd, "R0IF") == 0)
	{
		if (rbrunCondition())
		{
			if (strcmp(cmd, "R0IF") == 0)
			{
				return RB_OK_IGNORE;
			}

			return RB_OK;
		}
		else
		{
			if (strcmp(cmd, "RXIF") == 0)
			{
				rbrunScan('\n', NULL);

				ifcount = 0;
				while (!CURRENT_FILEEND)
				{
					rbGetSymbol(&cmd);
					strupr(cmd);

					if (strcmp(cmd, "RXIF") == 0)
					{
						ifcount++;
					}
					else if (strcmp(cmd, "RXIE") == 0)
					{
						if (ifcount == 0)
						{
							if (rbrunCondition()) break;
						}
					}
					else if (strcmp(cmd, "RXEI") == 0)
					{
						if (ifcount > 0)
						{
							ifcount--;
						}
						else
						{
							break;
						}
					}
					else if (strcmp(cmd, "RXEL") == 0)
					{
						if (ifcount == 0) break;
					}
					else if (strcmp(cmd, "RLBL") == 0)
					{
						rbrunScan('\n', &TempStr[0]);
						rbAddLabel(TempStr[0]);
					}

					rbrunScan('\n', NULL);
				}
			}
		}
		return RB_OK;
	}
	// ELSE / ELSEIF -------------------------------------------------------
	if (strcmp(cmd, "RXEL") == 0
	        || strcmp(cmd, "RXIE") == 0)
	{
		if (strcmp(cmd, "RXIE") == 0)
			rbrunScan('\n', NULL);

		while (!CURRENT_FILEEND)
		{
			rbGetSymbol(&cmd);
			strupr(cmd);

			if (strcmp(cmd, "RXIF") == 0)
			{
				ifcount++;
			}
			else if (strcmp(cmd, "RXEI") == 0)
			{
				if (ifcount > 0)
				{
					ifcount--;
				}
				else
				{
					break;
				}
			}
			else if (strcmp(cmd, "RLBL") == 0)
			{
				rbrunScan('\n', &TempStr[0]);
				rbAddLabel(TempStr[0]);
				CHARACTER_BACK();
			}

			rbrunScan('\n', NULL);
		}
		return RB_OK;
	}
	// HALT ----------------------------------------------------------------
	if (strcmp(cmd, "RHLT") == 0
	        || strcmp(cmd, "R003") == 0)
	{
		if (rbrunScanChar("\n") != '\n')
		{
			CHARACTER_BACK();
			rbrunGetValue(&TempStr[0]);
			rb_return_code = atoi( TempStr[0] );
		}

		return RB_TERMINATE;
	}

// *****************************************************************************
// LOOPS
// *****************************************************************************
	// REPEAT --------------------------------------------------------------
	if (strcmp(cmd, "RRPT") == 0)
	{
		if (loop_cnt == 0)
		{
			loops = (LOOPINFO*) malloc(sizeof(LOOPINFO));
		}
		else
		{
			loops = (LOOPINFO*) realloc((LOOPINFO*) loops, (loop_cnt+1)*sizeof(LOOPINFO));
		}

		loops[loop_cnt].loop_begin = file_offset;

		if (procStack_cnt > 0)
		{
			loops[loop_cnt].proc_name = (char*) malloc((strlen(procStack[procStack_cnt-1].proc_name)+1) * sizeof(char));
			strcpy(loops[loop_cnt].proc_name, procStack[procStack_cnt-1].proc_name);
		}
		else
		{
			loops[loop_cnt].proc_name = (char*) NULL;
		}

		loop_cnt++;

		return RB_OK;
	}
	// CONT ----------------------------------------------------------------
	if (strcmp(cmd, "RCNT") == 0)
	{
		if (loop_cnt == 0)
		{
			//If there's an CONT, and loop_cnt is 0, find out all REPEATs in front of this CONT!!!
			//IMMEDIATELLY: GO, GO, GO!!!

			unsigned long int current_pos = file_offset;
			file_offset = 0;

			while (!CURRENT_FILEEND)
			{
				rbGetSymbol(&cmd);
				strupr(cmd);

				if (file_offset >= (current_pos - strlen(cmd)))
					break;

				if (strcmp(cmd, "RRPT") == 0)
				{
					if (loop_cnt == 0)
					{
						loops = (LOOPINFO*) malloc(sizeof(LOOPINFO));
					}
					else
					{
						loops = (LOOPINFO*) realloc((LOOPINFO*) loops, (loop_cnt+1)*sizeof(LOOPINFO));
					}

					loops[loop_cnt].loop_begin = file_offset;

					if (procStack_cnt > 0)
					{
						loops[loop_cnt].proc_name = (char*) malloc((strlen(procStack[procStack_cnt-1].proc_name)+1) * sizeof(char));
						strcpy(loops[loop_cnt].proc_name, procStack[procStack_cnt-1].proc_name);
					}
					else
					{
						loops[loop_cnt].proc_name = (char*) NULL;
					}
					loop_cnt++;
				}
				else if (strcmp(cmd, "RUNT") == 0)
				{
					(void)rbrunCondition();
					if (loop_cnt == 0) continue;
					loop_cnt--;

					if (loops[loop_cnt].proc_name != (char*)NULL)
						free(loops[loop_cnt].proc_name);

					if (loop_cnt == 0)
					{
						free(loops);
					}
					else
					{
						loops = (LOOPINFO*) realloc((LOOPINFO*) loops, (loop_cnt)*sizeof(long));
					}
				}

				if (symbreak != '\n')
					(void)rbrunScan('\n', NULL);
			}
			file_offset = current_pos;

			//Avoid RB_OK here!!
			//return RB_OK;
		}

		if (loop_cnt == 0) return RB_OK;

		if (procStack_cnt > 0)
		{
			if (loops[loop_cnt-1].proc_name == (char*)NULL ||
			        strcmp(loops[loop_cnt-1].proc_name, procStack[procStack_cnt-1].proc_name) != 0)
			{
				return RB_OK;
			}
		}
		file_offset = loops[loop_cnt-1].loop_begin;
		return RB_OK;
	}
	// BREAK ---------------------------------------------------------------
	if (strcmp(cmd, "RBRK") == 0)
	{
		int loops_found = 0;
		while (!CURRENT_FILEEND)
		{
			rbGetSymbol(&cmd);
			strupr(cmd);

			if (strcmp(cmd, "RRPT") == 0)
			{
				loops_found++;
			}
			else if (strcmp(cmd, "RUNT") == 0)
			{
				(void)rbrunCondition();

				if (loops_found == 0)
				{
					if (loop_cnt > 0)
						loop_cnt--;
					break;
				}

				loops_found--;
			}
			else
			{
				if (symbreak != '\n')
					rbrunScan('\n', NULL);
			}
		}

		return RB_OK;
	}
	// UNTIL ---------------------------------------------------------------
	if (strcmp(cmd, "RUNT") == 0)
	{
		if (loop_cnt == 0)
		{
			//If there's an UNTIL, and loop_cnt is 0, find out all REPEATs in front
			//of this UNTIL!!!
			unsigned long int current_pos = file_offset;
			file_offset = 0;

			while (!CURRENT_FILEEND)
			{
				rbGetSymbol(&cmd);
				strupr(cmd);

				if (file_offset >= (current_pos - strlen(cmd)))
					break;

				if (strcmp(cmd, "RRPT") == 0)
				{
					if (loop_cnt == 0)
					{
						loops = (LOOPINFO*) malloc(sizeof(LOOPINFO));
					}
					else
					{
						loops = (LOOPINFO*) realloc((LOOPINFO*) loops, (loop_cnt+1)*sizeof(LOOPINFO));
					}

					loops[loop_cnt].loop_begin = file_offset;

					if (procStack_cnt > 0)
					{
						loops[loop_cnt].proc_name = (char*) malloc((strlen(procStack[procStack_cnt-1].proc_name)+1) * sizeof(char));
						strcpy(loops[loop_cnt].proc_name, procStack[procStack_cnt-1].proc_name);
					}
					else
					{
						loops[loop_cnt].proc_name = (char*) NULL;
					}
					loop_cnt++;
				}
				else if (strcmp(cmd, "RUNT") == 0)
				{
					if (loop_cnt == 0) continue;

					(void)rbrunCondition();
					if (loop_cnt == 0) continue;

					loop_cnt--;

					if (loops[loop_cnt].proc_name != (char*)NULL)
						free(loops[loop_cnt].proc_name);

					if (loop_cnt == 0)
					{
						free(loops);
					}
					else
					{
						loops = (LOOPINFO*) realloc((LOOPINFO*) loops, (loop_cnt)*sizeof(long));
					}
				}

				if (symbreak != '\n')
					rbrunScan('\n', NULL);
			}
			file_offset = current_pos;

			//Avoid RB_OK here!!
			//return RB_OK;
		}

		if (rbrunCondition())
		{
			loop_cnt--;

			if (loops[loop_cnt].proc_name != (char*)NULL)
				free(loops[loop_cnt].proc_name);

			if (loop_cnt == 0)
			{
				free(loops);
			}
			else
			{
				loops = (LOOPINFO*) realloc((LOOPINFO*) loops, (loop_cnt)*sizeof(LOOPINFO));
			}
		}
		else
		{
			if (procStack_cnt > 0)
			{
				if (loops[loop_cnt-1].proc_name == (char*)NULL ||
				        strcmp(loops[loop_cnt-1].proc_name, procStack[procStack_cnt-1].proc_name) != 0)
				{
					return RB_OK;
				}
			}
			file_offset = loops[loop_cnt-1].loop_begin;
		}
		return RB_OK;
	}

// *****************************************************************************
// LABELING / SUB ROUTINES
// *****************************************************************************
	// GOTO ----------------------------------------------------------------
	if (strcmp(cmd, "R004") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		strtrim(TempStr[0]);

		rbrunScan('\n', NULL);

		rbrunReadTo(TempStr[0], true);
		return RB_OK_IGNORE;
	}
	// GOSUB ---------------------------------------------------------------
	if (strcmp(cmd, "R005") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		strtrim(TempStr[0]);

		if (sub_cnt == 0)
		{
			subs = (SUBINFO*) malloc(sizeof(SUBINFO));
		}
		else
		{
			subs = (SUBINFO*) realloc((SUBINFO*) subs, (sub_cnt+1) * sizeof(SUBINFO));
		}
		subs[sub_cnt].call_pos = file_offset;

		sub_cnt++;

		rbrunScan('\n', NULL);

		rbrunReadTo(TempStr[0], true);
		return RB_OK_IGNORE;
	}
	// RESUME --------------------------------------------------------------
	if (strcmp(cmd, "R006") == 0)
	{
		if (sub_cnt == 0) return RB_OK;

		file_offset = subs[sub_cnt-1].call_pos;
		sub_cnt--;

		if (sub_cnt == 0)
		{
			free(subs);
		}
		else
		{
			subs = (SUBINFO*) realloc((SUBINFO*) subs, (sub_cnt) * sizeof(SUBINFO));
		}

		return RB_OK;
	}

// *****************************************************************************
// BUILD-IN FUNCTIONS
// *****************************************************************************
	// UPVAR ---------------------------------------------------------------
	if (strcmp(cmd, "RF03") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strupr(TempStr[1]);

		RB_LETVAR(TempStr[0], TempStr[1]);
		return RB_OK;
	}
	// LOWVAR --------------------------------------------------------------
	if (strcmp(cmd, "RF04") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strlower(TempStr[1]);

		RB_LETVAR(TempStr[0], TempStr[1]);
		return RB_OK;
	}
	// TRIMVAR -------------------------------------------------------------
	if (strcmp(cmd, "RF05") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strtrim(TempStr[1]);

		RB_LETVAR(TempStr[0], TempStr[1]);
		return RB_OK;
	}
	// GETCHR --------------------------------------------------------------
	if (strcmp(cmd, "RF06") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		TempStr[2] = (char*)MALLOC((1+1) * sizeof(char));
#ifdef DEBUG_SESSION
		MALLOC_DEBUG(TempStr[2], (1+1) * sizeof(char));
#endif

		*TempStr[2] = (char)atoi(TempStr[1]);
		TempStr[2][1] = '\0';

		RB_LETVAR(TempStr[0], TempStr[2]);
		return RB_OK;
	}
	// GETASC --------------------------------------------------------------
	if (strcmp(cmd, "RF12") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		sprintf(text, "%d", (unsigned char) TempStr[1][0]);
		RB_LETVAR(TempStr[0], text);
		return RB_OK;
	}
	// GETLEN --------------------------------------------------------------
	if (strcmp(cmd, "RF07") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		sprintf(text, "%d", strlen(TempStr[1]));
		RB_LETVAR(TempStr[0], text);
		return RB_OK;
	}
	// COPYVAR -------------------------------------------------------------
	if (strcmp(cmd, "RF08") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		rbrunGetValue(&TempStr[2]);
		TempInt1 = atoi(TempStr[2]) - 1;

		rbrunGetValue(&TempStr[2]);
		TempInt2 = atoi(TempStr[2]);

		if (TempInt2 < 0) TempInt2 = 1;
		if (TempInt1 <= -1)
		{
			TempInt1 = strlen(TempStr[1]) - TempInt2;
			if (TempInt1 < 0) TempInt1 = 0;
		}

		if (strcmp(TempStr[1], "") != 0)
		{
			RB_LETVAR(TempStr[0], strsubstr(TempStr[1], TempInt1, TempInt2));
		}
		else
		{
			RB_LETVAR(TempStr[0], "");
		}
		return RB_OK;
	}
	// GETCHARAT -----------------------------------------------------------
	if (strcmp(cmd, "RF13") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		rbrunGetValue(&TempStr[2]);
		TempInt1 = atoi(TempStr[2]) - 1;

		if (TempInt1 < 0 || (unsigned int)TempInt1 > strlen(TempStr[1]))
		{
			RB_LETVAR(TempStr[0], "");
			ERRORCODE("-1");
		}
		else
		{
			*(TempStr[1]+TempInt1+1) = '\0';
			RB_LETVAR(TempStr[0], (TempStr[1]+TempInt1));
			ERRORCODE("0");
		}
		return RB_OK;
	}
	// GETPOS --------------------------------------------------------------
	if (strcmp(cmd, "RF02") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		rbrunGetValue(&TempStr[2]);

		rbrunGetValue(&TempStr[3]);
		strtrim(TempStr[3]);
		TempInt1 = atoi(TempStr[3]);

		if (TempInt1 < 1) TempInt1 = 1;

		//Perform case sensitive search?
		rbrunGetVar(PVAR_CASE_SENSITIVITY, &TempStr[3], procStack_cnt);
		if (strcmp(TempStr[3], "0") == 0)
		{
			strupr(TempStr[1]);
			strupr(TempStr[2]);
		}

		TempInt2 = 1;
		for (unsigned int a = 0; a < strlen(TempStr[1]); a++)
		{
			if (strncmp(TempStr[1]+a, TempStr[2], strlen(TempStr[2]) * sizeof(char)) == 0)
			{
				if (TempInt1 == TempInt2)
				{
					sprintf(text, "%d", a+1);
					RB_LETVAR(TempStr[0], text);
					return RB_OK;
				}
				else
				{
					a += strlen(TempStr[2]) - 1;
				}
				TempInt2++;
			}
		}
		RB_LETVAR(TempStr[0], "0");
		return RB_OK;
	}
	// GETTOK --------------------------------------------------------------
	if (strcmp(cmd, "RF10") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		rbrunGetValue(&TempStr[2]); //Token

		rbrunGetValue(&TempStr[3]);
		strtrim(TempStr[3]);
		TempInt1 = atoi(TempStr[3]);

		if (TempInt1 < 1) TempInt1 = 1;

		//Perform case sensitive search?
		bool case_sensitivity = false;
		rbrunGetVar(PVAR_CASE_SENSITIVITY, &TempStr[3], procStack_cnt);
		if (strcmp(TempStr[3], "0") == 0)
		{
			case_sensitivity = true;
			strupr(TempStr[2]);
		}

		if (strlen(TempStr[2]) == 0)
		{
			RB_LETVAR(TempStr[0], TempStr[1]);
			return RB_OK;
		}


		char* tok_text = (char*)NULL;
		if (case_sensitivity)
		{
			tok_text = strdup(TempStr[1]);
			strupr(tok_text);
		}
		else
		{
			tok_text = TempStr[1];
		}


		TempInt2 = 1;
		char* separator = tok_text;
		char* token = tok_text;

		while (separator != (char*)NULL)
		{
			/*
			sprintf(text, "%s %d %d\n", tok_text+a, a, strlen(tok_text));
			VARS(text);
			*/
			separator = strstr(token, TempStr[2]);


			if (separator != (char*)NULL && TempInt1 == TempInt2)
			{
				*separator = '\0';

				RB_LETVAR(TempStr[0], token);

				if (case_sensitivity)
					free(tok_text);

				return RB_OK;
			}
			else if (separator == (char*)NULL && TempInt1 == TempInt2)
			{
				RB_LETVAR(TempStr[0], token);

				if (case_sensitivity)
					free(tok_text);

				return RB_OK;
			}
			else
			{
				TempInt2++;

				token = separator + (strlen(TempStr[2]) * sizeof(char));
			}



			/*
			if(strncmp(tok_text+a, TempStr[2], strlen(TempStr[2])) == 0
				|| a == strlen(tok_text))
			{
				if(TempInt1 == TempInt2)
				{
					RB_LETVAR(TempStr[0], strsubstr(TempStr[1], b, a-b));

					if(case_sensitivity)
						free(tok_text);

					return RB_OK;
				}
				else
				{
					a += strlen(TempStr[2]) - 1;
					b = a + 1;
				}
				TempInt2++;
			}
			*/
		}
		RB_LETVAR(TempStr[0], "");

		if (case_sensitivity)
			free(tok_text);

		return RB_OK;
	}
	// REPLACEVAR ----------------------------------------------------------
	if (strcmp(cmd, "RF11") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		rbrunGetValue(&TempStr[2]);

		rbrunGetValue(&TempStr[3]);

		//Perform case sensitive search?
		bool case_sensitivity = false;
		rbrunGetVar(PVAR_CASE_SENSITIVITY, &TempStr[4], procStack_cnt);
		if (strcmp(TempStr[4], "0") == 0)
		{
			case_sensitivity = true;
		}

		strreplace(&TempStr[1], TempStr[2], TempStr[3], case_sensitivity);

		RB_LETVAR(TempStr[0], TempStr[1]);
		return RB_OK;
	}
	// CNTVAR --------------------------------------------------------------
	if (strcmp(cmd, "RF14") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		rbrunGetValue(&TempStr[2]);


		//Perform case sensitive search?
		rbrunGetVar(PVAR_CASE_SENSITIVITY, &TempStr[3], procStack_cnt);
		if (strcmp(TempStr[3], "0") == 0)
		{
			strupr(TempStr[1]);
			strupr(TempStr[2]);
		}

		long cnt = 0;

		/*
			JMM 25.05.2007:
			Optimierung: Das muss echt so mal schneller werden!!!
		*/
		char* cntstr = TempStr[1] - 1;
		while( cntstr = strstr( cntstr + 1, TempStr[2] ) )
			cnt++;

		sprintf(text, "%ld", cnt);
		RB_LETVAR(TempStr[0], text);
		return RB_OK;
	}
	// RANDVAR -------------------------------------------------------------
	if (strcmp(cmd, "RF09") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		TempInt1 = random(atoi(TempStr[1]));

		if (last_rand == 0)
		{
			//Initialize random number generator
			time_t t;
			srand((unsigned) time(&t));
			last_rand = 10;
		}
		last_rand--;

		if (TempInt1 < 1)
		{
			ERRORCODE("-1");
			RB_LETVAR(TempStr[0], "0");
		}
		else
		{
			ERRORCODE("0");
			sprintf(text, "%d", rand() % TempInt1);
			RB_LETVAR(TempStr[0], text);
		}
		return RB_OK;
	}

// *****************************************************************************
// SYSTEM-SPECIFIC FUNCTIONS
// *****************************************************************************
	// SHELL ---------------------------------------------------------------
	if (strcmp(cmd, "R010") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strtrim(TempStr[1]);
		strupr(TempStr[1]);

		int showmode = -1;

		if (strcmp(TempStr[1], "HIDE") == 0 || strcmp(TempStr[1], "1") == 0)
			showmode = SW_HIDE;
		else if (strcmp(TempStr[1], "MAXIMIZED") == 0 || strcmp(TempStr[1], "2") == 0)
			showmode = SW_SHOWMAXIMIZED;
		else if (strcmp(TempStr[1], "MINIMIZED") == 0 || strcmp(TempStr[1], "3") == 0)
			showmode = SW_SHOWMINIMIZED;
		else
			showmode = SW_SHOW;

		if (showmode > -1)
		{
			sprintf(text, "%d", WinExec(TempStr[0],  showmode));
		}
		else
		{
			strcpy(text, "-1");
		}

		ERRORCODE(text);
		return RB_OK;
	}
	// CHDIR ---------------------------------------------------------------
	if (strcmp(cmd, "R011") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		if (chdir(TempStr[0]) == 0)
		{
			ERRORCODE("0");

			getcwd(text, STDLINE);
			RB_LETVAR(PVAR_CURRENT, text);
		}
		else
		{
			ERRORCODE("-1");
		}
		return RB_OK;
	}
	// MKDIR ---------------------------------------------------------------
	if (strcmp(cmd, "R012") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		TempStr[1] = (char*)MALLOC(sizeof(char));
#ifdef DEBUG_SESSION
		MALLOC_DEBUG(TempStr[1], sizeof(char));
#endif


		*TempStr[1] = '\0';

		int ret = -1;
		char* tok = strtok(TempStr[0], "\\");

		while (tok != (char*)NULL)
		{
			TempStr[1] = (char*)realloc((char*)TempStr[1],
			                            (strlen(TempStr[1]) + strlen(tok) + 2) * sizeof(char)); //Don't forget the \\
			strcat(TempStr[1], tok);

			ret = mkdir(TempStr[1]);

			tok = strtok(NULL, "\\");

			if (tok != (char*)NULL)
				strcat(TempStr[1], "\\");

		}

		sprintf(text, "%d", ret);
		ERRORCODE(text);

		return RB_OK;
	}
	// RMDIR ---------------------------------------------------------------
	if (strcmp(cmd, "R013") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		sprintf(text, "%d", rmdir(TempStr[0]));
		ERRORCODE(text);
		return RB_OK;
	}
	// SYSTEM --------------------------------------------------------------
	if (strcmp(cmd, "R014") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		sprintf(text, "%d", system(TempStr[0]));
		ERRORCODE(text);
		return RB_OK;
	}
	// CALL ----------------------------------------------------------------
	if (strcmp(cmd, "R030") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strtrim(TempStr[1]);
		strupr(TempStr[1]);


		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		SECURITY_ATTRIBUTES saProcess;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );
		si.dwFlags = STARTF_USESHOWWINDOW;

		saProcess.nLength = sizeof(saProcess);
		saProcess.lpSecurityDescriptor = NULL;
		saProcess.bInheritHandle = FALSE;

		if (strcmp(TempStr[1], "HIDE") == 0 || strcmp(TempStr[1], "1") == 0)
			si.wShowWindow = SW_HIDE;
		else if (strcmp(TempStr[1], "MAXIMIZED") == 0 || strcmp(TempStr[1], "2") == 0)
			si.wShowWindow = SW_SHOWMAXIMIZED;
		else if (strcmp(TempStr[1], "MINIMIZED") == 0 || strcmp(TempStr[1], "3") == 0)
			si.wShowWindow = SW_SHOWMINIMIZED;
		else
			si.wShowWindow = SW_SHOW;

		if ( !CreateProcess( NULL, TempStr[0], &saProcess, NULL, FALSE, 0, NULL, NULL, &si, &pi) )
		{
			ERRORCODE("-1");
		}
		else
		{
			MSG message;
			while (WaitForSingleObject(pi.hProcess, 100) == WAIT_TIMEOUT)
			{
				if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}
			if (WaitForSingleObject( pi.hProcess, 0) == WAIT_OBJECT_0)
			{
				DWORD dwCode;
				GetExitCodeProcess(pi.hProcess,&dwCode);
				sprintf(text, "%d", dwCode);
				ERRORCODE(text);
			}
			else
			{
				ERRORCODE("-1");
			}

		}
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return RB_OK;
	}
	// SHORTCUT --------------------------------------------------------------
	if (strcmp(cmd, "R065") == 0)
	{
		rbrunGetValue(&TempStr[0]); //Shortcut path
		TempStr[0] = (char*)realloc((char*)TempStr[0], (strlen(TempStr[0]) + 3 + 1 + 1) * sizeof(char));
		strcat( TempStr[0], ".lnk" );

		rbrunGetValue(&TempStr[1]); //Programm for shortcut

		rbrunGetValue(&TempStr[2]); //Command-line args for shortcut

		rbrunGetValue(&TempStr[3]); // Working directory

		rbrunGetValue(&TempStr[4]); // Icon

		rbrunGetValue(&TempStr[5]); // Show mode
		strupr( TempStr[5] );

		if (strcmp(TempStr[5], "HIDE") == 0 || strcmp(TempStr[1], "1") == 0)
			TempInt1 = SW_HIDE;
		else if (strcmp(TempStr[5], "MAXIMIZED") == 0 || strcmp(TempStr[1], "2") == 0)
			TempInt1 = SW_SHOWMAXIMIZED;
		else if (strcmp(TempStr[5], "MINIMIZED") == 0 || strcmp(TempStr[1], "3") == 0)
			TempInt1 = SW_SHOWMINIMIZED;
		else
			TempInt1 = SW_SHOW;

		LPSHELLLINK pShellLink;
		HRESULT hrCoInit;
		HRESULT hr;

		hrCoInit = CoInitialize( NULL );
		hr = CoCreateInstance(	CLSID_ShellLink,
		                       NULL,
		                       CLSCTX_INPROC_SERVER,
		                       IID_IShellLink,
		                       (void**)&pShellLink );
		if ( SUCCEEDED( hr ) )
		{
			LPPERSISTFILE pPersistFile;

			if (SUCCEEDED(pShellLink->QueryInterface(IID_IPersistFile,(void**)&pPersistFile)))
			{
				wchar_t wsz[ MAX_PATH ];

				hr = pShellLink->SetPath(TempStr[1]);
				if ( SUCCEEDED( hr ) )
					hr = pShellLink->SetArguments(TempStr[2]);


				if ( SUCCEEDED( hr ) )
					hr = pShellLink->SetWorkingDirectory(TempStr[3]);

				if (SUCCEEDED(hr))
					hr = pShellLink->SetIconLocation(TempStr[4],0);

				if ( SUCCEEDED( hr ) )
				{

					hr = pShellLink->SetShowCmd(TempInt1);
				}

				if ( SUCCEEDED( hr ) )
				{
					MultiByteToWideChar( CP_ACP, 0, TempStr[0], -1, wsz, MAX_PATH);
					hr = pPersistFile->Save( wsz, TRUE );
					hr = pPersistFile->SaveCompleted( wsz );
				}
			}

			pShellLink->Release( );
		}

		if ( SUCCEEDED( hrCoInit ) ) CoUninitialize();

		if ( SUCCEEDED( hr ) )
		{
			ERRORCODE("0");
		}
		else
		{
			ERRORCODE("-1");
		}
		return RB_OK;
	}
	// GETENV --------------------------------------------------------------
	if (strcmp(cmd, "R015") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strtrim(TempStr[1]);

		if (getenv(TempStr[1]) == NULL)
		{
			RB_LETVAR(TempStr[0], "");
			ERRORCODE ("-1");
		}
		else
		{
			RB_LETVAR(TempStr[0], getenv(TempStr[1]));
			ERRORCODE ("0");
		}
		return RB_OK;
	}
	// LETENV --------------------------------------------------------------
	if (strcmp(cmd, "R016") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		//Allocating memory for internal environment table
		env_values = (char**)realloc((char*)env_values,
		                             (env_values_cnt+1) * sizeof(char**));
		env_values[env_values_cnt] = (char*)malloc((strlen(TempStr[0])+1) * sizeof(char));
		strcpy(env_values[env_values_cnt],TempStr[0]);

		if (putenv(env_values[env_values_cnt]) == 0)
		{
			ERRORCODE("0");
		}
		else
		{
			ERRORCODE("-1");
		}
		return RB_OK;
	}
	// DELFILE -------------------------------------------------------------
	if (strcmp(cmd, "R017") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		if (remove(TempStr[0]) == 0)
		{
			ERRORCODE("0");
		}
		else
		{
			ERRORCODE("-1");
		}
		return RB_OK;
	}
	// RENAMEFILE ----------------------------------------------------------
	if (strcmp(cmd, "R018") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		if (rename(TempStr[0], TempStr[1]) != 0)
		{
			ERRORCODE("-1");
		}
		else
		{
			ERRORCODE("0");
		}
		return RB_OK;
	}
	// COPYFILE ------------------------------------------------------------
	if (strcmp(cmd, "R026") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		if (CopyFile(TempStr[0], TempStr[1], false))
		{
			DWORD dwAtts = GetFileAttributes(TempStr[0]);
			SetFileAttributes(TempStr[1], dwAtts );

			ERRORCODE("0");
		}
		else
		{
			ERRORCODE("-1");
		}

		return RB_OK;
	}
	// FILEEXISTS ----------------------------------------------------------
	if (strcmp(cmd, "R019") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);
		//VARS(TempStr[1]);
		workingfile = fopen(TempStr[1], "rb");
		if (workingfile == (FILE*)NULL)
		{
			RB_LETVAR(TempStr[0], "-1");
		}
		else
		{
			RB_LETVAR(TempStr[0], "0");
			fclose(workingfile);
		}
		return RB_OK;
	}
	// OPEN ----------------------------------------------------------------
	if (strcmp(cmd, "R020") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		sprintf(text, "%d", ShellExecute(NULL, NULL, TempStr[0], NULL, NULL, SW_NORMAL));
		ERRORCODE(text);
		return RB_OK;
	}
	// NEWFILE -------------------------------------------------------------
	if (strcmp(cmd, "R021") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		workingfile = fopen(TempStr[0], "w");

		if (workingfile == NULL)
		{
			ERRORCODE("-1");
		}
		else
		{
			ERRORCODE("0");
			fclose(workingfile);
		}
		return RB_OK;
	}
	// FILESIZE ------------------------------------------------------------
	if (strcmp(cmd, "R035") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		workingfile = fopen(TempStr[1], "rb");

		if (workingfile == NULL)
		{
			RB_LETVAR(TempStr[0], "-1");
		}
		else
		{
			fseek(workingfile, 0L, SEEK_END);
			sprintf(text, "%ld", ftell(workingfile));
			RB_LETVAR(TempStr[0], text);
			fclose(workingfile);
		}
		return RB_OK;
	}
	// READFILE ------------------------------------------------------------
	if (strcmp(cmd, "R024") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		rbrunGetValue(&TempStr[2]);
		strtrim(TempStr[2]);
		TempInt1 = atoi(TempStr[2]);
		TempInt2 = TempInt1; //for _READFILE_line_cnt

		workingfile = fopen(TempStr[1], "rb");

		if (TempInt1 < 1)
		{
			if (workingfile != (FILE*)NULL)
			{
				fseek( workingfile, 0L, SEEK_END );
				TempStr[3] = (char*)malloc( (ftell( workingfile ) + 1)
				                            * sizeof( char ) );
				char*   nextchar = (char*)NULL;

				fseek( workingfile, 0L, SEEK_SET );
				nextchar = TempStr[3];
				while (!feof(workingfile))
				{
					*nextchar = fgetc( workingfile );
					nextchar++;
				}
				*(nextchar-1) = '\0';

				ERRORCODE("0");
				RB_LETVAR(TempStr[0], TempStr[3]);
				fclose(workingfile);
			}
			else
			{
				ERRORCODE("-1");
			}
		}
		else
		{
			if (workingfile == (FILE*)NULL)
			{
				ERRORCODE("-1");
			}
			else
			{
				if ( _READFILE_file != (char*)NULL &&
				        strcmp( TempStr[1], _READFILE_file ) == 0 &&
				        (_READFILE_line_cnt+1) == TempInt1 )
				{
					fseek( workingfile, _READFILE_offset, SEEK_SET );

					if ( fgetc( workingfile ) != EOF )
						fseek( workingfile, _READFILE_offset, SEEK_SET );
				}
				else
				{
					char chr = (char) NULL;
					if ( TempInt1 > 1 )
					{
						do
						{
							if ( (chr = fgetc( workingfile )) == EOF ) break;
							if ( chr == '\n' ) TempInt1--;
						}
						while ( TempInt1 > 1 );
					}
				}

				if (feof(workingfile))
				{
					RB_LETVAR(TempStr[0], "EOF");
					ERRORCODE("-1");

					fclose(workingfile);
					return RB_OK;
				}

				long line_size = 255;
				long rest_size = 0;
				char* current = (char*)NULL;
				TempStr[3] = (char*)NULL;

				TempStr[3] = (char*)malloc( line_size * sizeof(char));
				*TempStr[3] = '\0';

				rest_size = line_size;
				current = TempStr[3];


				while ( fgets(current, rest_size, workingfile)
				        && current[ strlen(current) - 1 ] != '\n' )
				{
					line_size += 255;
					TempStr[3] = (char*)realloc( TempStr[3], line_size );
					rest_size = line_size - strlen( TempStr[3] );
					current = TempStr[3] + strlen(TempStr[3]);
				}

				switch ( TempStr[3][ strlen(TempStr[3]) - 1 ] )
				{
					case '\n':
					case '\r':
						TempStr[3][ strlen(TempStr[3]) - 1 ] = '\0';

						//Note: At this point, the new value of strlen(TempStr[3])
						//is already strlen(TempStr[3]) - 1!!!
						if (TempStr[3][ strlen(TempStr[3]) - 1 ] == '\r')
							TempStr[3][ strlen(TempStr[3]) - 1 ] = '\0';


						break;
					default:
						break;
				}

				/*
				TempStr[3] = (char*)malloc(sizeof(char));
				int i = 0;
				while(true)
				{
				chr = fgetc(workingfile);
				if(chr == '\n' || feof(workingfile)) break;

				        if(chr != '\r')
				        {
				TempStr[3] = (char*)realloc((char*)TempStr[3], (i + 2) * sizeof(char));
				TempStr[3][i] = chr;
				i++;
				        }
				}
				TempStr[3][i] = '\0';
				*/

				if ( _READFILE_file != (char*)NULL
				        && strcmp( _READFILE_file, TempStr[1] ) != 0 )
				{
					free( _READFILE_file );
				}
				_READFILE_file = strdup( TempStr[1] );
				_READFILE_line_cnt = TempInt2;
				_READFILE_offset = ftell( workingfile );

				ERRORCODE("0");
				RB_LETVAR(TempStr[0], TempStr[3]);

				fclose(workingfile);
			}
		}
		return RB_OK;
	}
	// WRITEFILE -----------------------------------------------------------
	if (strcmp(cmd, "R025") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		workingfile = fopen(TempStr[0], "a+b");

		char* eol_str = (char*)NULL;
		rbrunGetVar(PVAR_EOL, &eol_str, procStack_cnt);

		if (workingfile == (FILE*)NULL)
		{
			ERRORCODE("-1");
		}
		else
		{
			ERRORCODE("0");
			fprintf(workingfile, "%s%s", TempStr[1], eol_str);
			fclose(workingfile);
		}

		if ( eol_str != (char*)NULL )
			free( eol_str );

		return RB_OK;
	}
	// PUTFILE -------------------------------------------------------------
	if (strcmp(cmd, "R050") == 0)
	{
		rbrunGetValue(&TempStr[0]); //Filename
		rbrunGetValue(&TempStr[1]);
		rbrunGetValue(&TempStr[2]); //Data

		unsigned long int byte_pos = 0;
		byte_pos = atol(TempStr[1]); //Byte position

		if (byte_pos > 0) byte_pos--;

		workingfile = fopen(TempStr[0], "r+b");

		if (workingfile == NULL)
		{
			ERRORCODE("-1");
		}
		else
		{
			ERRORCODE("0");

			fseek(workingfile, byte_pos, SEEK_SET);

			char* sep = (char*)NULL;
			rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);

			char* tok = strtoken( TempStr[2], sep );
			while (tok != (char*)NULL)
			{
				putc(atoi(tok), workingfile);
				tok = strtoken( NULL, sep );
			}

			free(sep);

			/*
			for(unsigned int a = 0; a < strlen(TempStr[2]); a++)
				putc(TempStr[2][a], workingfile);
			                     */
			fclose(workingfile);
		}
		return RB_OK;
	}
	// GETFILE -------------------------------------------------------------
	if (strcmp(cmd, "R051") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]); //Filename
		rbrunGetValue(&TempStr[2]);
		rbrunGetValue(&TempStr[3]);


		unsigned long int byte_pos = 0;
		byte_pos = atol(TempStr[2]); //Byte position
		if (byte_pos > 0)
		{
			byte_pos--;
		}
		else
		{
			byte_pos = 0;
		}


		unsigned long int byte_cnt = 0;
		byte_cnt = atol(TempStr[3]); //Byte count
		if (byte_cnt < 1)
		{
			byte_cnt = 1;
		}


		workingfile = fopen(TempStr[1], "rb");
		if (workingfile == (FILE*)NULL)
		{
			ERRORCODE("-1");
		}
		else
		{
			ERRORCODE("0");

			fseek(workingfile, byte_pos, SEEK_SET);

			/*
			BYTE* mem_value = (BYTE*)MALLOC((byte_cnt+1) * sizeof(BYTE));
			memset(mem_value, 0, (STDLINE * sizeof(char)));
			*/

			char tmp[3+1];
			char* sep = (char*)NULL;
			unsigned long int bytes = 0;

			rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);
			sep[1] = '\0';

			while (bytes < byte_cnt)
			{
				if (TempStr[4] == (char*)NULL)
				{
					TempStr[4] = (char*)malloc(3+1+1 * sizeof(char));
					*TempStr[4] = '\0';
				}
				else
				{
					TempStr[4] = (char*)realloc((char*)TempStr[4], (strlen(TempStr[4]) + 3 + 1 + 1) * sizeof(char));
				}

				sprintf(tmp, "%03d", fgetc(workingfile));

				if (feof(workingfile))
				{
					TempStr[4][strlen(TempStr[4])-1] = '\0';
					break;
				}


				strcat(TempStr[4], tmp);

				if (bytes < (byte_cnt-1))
					strcat(TempStr[4], sep);

				bytes++;
			}

			FREE(sep);


			/*
			TempStr[4] = (char*)MALLOC((byte_cnt+1) * sizeof(char));


			for(unsigned long int a = 0; a < byte_cnt; a++)
			{
				if(feof(workingfile)) break;

				TempStr[4][a] = (char)fgetc(workingfile);
				TempStr[4][a+1] = '\0';
			}
			*/


			RB_LETVAR(TempStr[0], TempStr[4]);

			fclose(workingfile);
		}
		return RB_OK;
	}
	// GETFILEDATE ---------------------------------------------------------
	if (strcmp(cmd, "R046") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		workingfile = fopen(TempStr[1], "rb");

		if (workingfile == NULL)
		{
			ERRORCODE("-1");
		}
		else
		{
			struct ftime ft;
			if (getftime(fileno(workingfile), &ft) != 0)
			{
				ERRORCODE("-1")
				RB_LETVAR(TempStr[0], "");
			}
			else
			{
				sprintf(text, "%02d%02d%4d", ft.ft_month, ft.ft_day, (ft.ft_year+1980));
				RB_LETVAR(TempStr[0], text);
				ERRORCODE("0");
			}
			fclose(workingfile);
		}
		return RB_OK;
	}
	// LETFILEDATE ---------------------------------------------------------
	if (strcmp(cmd, "R047") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		workingfile = fopen(TempStr[0], "r+");

		if (workingfile == NULL)
		{
			ERRORCODE("-1");
		}
		else
		{
			struct ftime ft;
			getftime(fileno(workingfile), &ft);

			int day, month, year;
			sscanf(TempStr[1], "%2d%2d%4d", &month, &day, &year);

			if (month > 0 && month <= 12) ft.ft_month = month;
			if (day > 0 && day <= 31) ft.ft_day = day;
			if (year > 0) ft.ft_year = (year-1980);

			if (setftime(fileno(workingfile), &ft) != 0)
			{
				ERRORCODE("-1");
			}
			else
			{
				ERRORCODE("0");
			}
			fclose(workingfile);

		}
		return RB_OK;
	}
	// GETFILETIME ---------------------------------------------------------
	if (strcmp(cmd, "R048") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		workingfile = fopen(TempStr[1], "rb");

		if (workingfile == NULL)
		{
			ERRORCODE("-1");
			RB_LETVAR(TempStr[0], "");
		}
		else
		{
			struct ftime ft;
			if (getftime(fileno(workingfile), &ft) != 0)
			{
				ERRORCODE("-1");
				RB_LETVAR(TempStr[0], "");
			}
			else
			{
				sprintf(text, "%02d%02d%02d", ft.ft_hour, ft.ft_min, ft.ft_tsec*2);
				RB_LETVAR(TempStr[0], text);
				ERRORCODE("0");
			}
			fclose(workingfile);
		}
		return RB_OK;
	}
	// LETFILETIME ---------------------------------------------------------
	if (strcmp(cmd, "R049") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		workingfile = fopen(TempStr[0], "r+");

		if (workingfile == NULL)
		{
			ERRORCODE("-1");
		}
		else
		{
			struct ftime ft;
			getftime(fileno(workingfile), &ft);

			int hours, mins, secs;
			sscanf(TempStr[1], "%2d%2d%2d", &hours, &mins, &secs);

			if (hours >= 0 && hours <= 24) ft.ft_hour = hours;
			if (mins > 0 && mins < 60) ft.ft_min = mins;
			if (secs >= 0 && secs < 60)
			{
				if (secs == 0)
					ft.ft_tsec = 0;
				else
					ft.ft_tsec = secs / 2;
			}

			if (setftime(fileno(workingfile), &ft) != 0)
			{
				ERRORCODE("-1");
			}
			else
			{
				ERRORCODE("0");
			}
			fclose(workingfile);
		}
		return RB_OK;
	}
	// STDOUT/STDIN/STDERR -------------------------------------------------
	if (strcmp(cmd, "R060") == 0
	        || strcmp(cmd, "R061") == 0
	        || strcmp(cmd, "R062") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		FILE* outstream = (FILE*)NULL;

		if (strcmp(cmd, "R061") == 0) outstream = stderr;
		else if (strcmp(cmd, "R062") == 0) outstream = stdin;
		else outstream = stdout;

		fprintf(outstream, TempStr[0]);

		return RB_OK;
	}
	// MCI -----------------------------------------------------------------
	if (strcmp(cmd, "R029") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		if (mciSendString(TempStr[0], text, STDLINE, NULL) == 0)
		{
			if (strcmp(text, "") != 0)
			{
				ERRORCODE(text);
			}
			else
			{
				ERRORCODE("0");
			}
		}
		else
		{
			ERRORCODE("-1");
		}
		return RB_OK;
	}
	// WAIT ----------------------------------------------------------------
	if (strcmp(cmd, "R036") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		TempInt1 = atoi(TempStr[0]);

		if (TempInt1 > 0)
			Sleep(TempInt1);

		return RB_OK;
	}
	// FINDFILES -----------------------------------------------------------
	if (strcmp(cmd, "R034") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strtrim(TempStr[1]);

		rbrunGetValue(&TempStr[2]);
		strtrim(TempStr[2]);
		TempInt1 = atoi(TempStr[2]);

		if (strcmp(TempStr[1], "") == 0)
		{
			FREE(TempStr[1]);
			TempStr[1] = (char*)MALLOC(2 * sizeof(char));
#ifdef DEBUG_SESSION
			MALLOC_DEBUG(TempStr[1], 2 * sizeof(char));
#endif
			strcpy(TempStr[1], "*");
		}

		char* filter = (char*)NULL;
		for (int i = strlen(TempStr[1])-1; i > 0; i--)
		{
			if (TempStr[1][i] == '\\')
			{
				filter = strdup((TempStr[1]+i)+1);
#ifdef DEBUG_SESSION
				malloc_cnt++;
#endif

				TempStr[1][i+1] = '\0';
				if (chdir(TempStr[1]) != 0)
				{
					ERRORCODE("-1");
					FREE(filter);
					return RB_OK;
				}
				break;
			}
		}

		if (filter == (char*)NULL)
		{
			filter = strdup(TempStr[1]);
#ifdef DEBUG_SESSION
			malloc_cnt++;
#endif
		}


		TempInt1--;
		if (TempInt1 == -1) TempInt1 = 255;

		char* sep = (char*)NULL;
		rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);

		readDirectory(&TempStr[3], filter, "", TempInt1, sep);
#ifdef DEBUG_SESSION
		malloc_cnt++;
#endif

		FREE(sep);
		FREE(filter);

		TempStr[3][strlen(TempStr[3])-1] = '\0'; //Cut last pipe from string

		RB_LETVAR(TempStr[0], TempStr[3]);

		rbrunGetVar(PVAR_CURRENT, &TempStr[2], procStack_cnt);
		chdir(TempStr[2]);

		ERRORCODE("0");
		return RB_OK;
	}
	// DISKFREE ------------------------------------------------------------
	if (strcmp(cmd, "R042") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		ULARGE_INTEGER free_bytes;

		if (GetDiskFreeSpaceEx(TempStr[1], NULL, NULL, &free_bytes))
		{
			sprintf(text, "%I64d", free_bytes);
			RB_LETVAR(TempStr[0], text);
		}
		else
		{
			RB_LETVAR(TempStr[0], "-1");
		}

		return RB_OK;
	}
	// DISKSIZE ------------------------------------------------------------
	if (strcmp(cmd, "R057") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		ULARGE_INTEGER disk_size;

		if (GetDiskFreeSpaceEx(TempStr[1], NULL, &disk_size, NULL))
		{
			sprintf(text, "%I64d", disk_size);
			RB_LETVAR(TempStr[0], text);
		}
		else
		{
			RB_LETVAR(TempStr[0], "-1");
		}

		return RB_OK;
	}
	// DISKTYPE ------------------------------------------------------------
	if (strcmp(cmd, "R043") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		TempInt1 = GetDriveType(TempStr[1]);
		switch (TempInt1)
		{
			case DRIVE_NO_ROOT_DIR:
				RB_LETVAR(TempStr[0], "0");
				break;
			case DRIVE_REMOVABLE:
				RB_LETVAR(TempStr[0], "1");
				break;
			case DRIVE_FIXED:
				RB_LETVAR(TempStr[0], "2");
				break;
			case DRIVE_REMOTE:
				RB_LETVAR(TempStr[0], "3");
				break;
			case DRIVE_CDROM:
				RB_LETVAR(TempStr[0], "4");
				break;
			case DRIVE_RAMDISK:
				RB_LETVAR(TempStr[0], "5");
				break;
			default:
				RB_LETVAR(TempStr[0], "-1");
		}
		return RB_OK;
	}
	// SHUTDOWN ------------------------------------------------------------
	if (strcmp(cmd, "R044") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		strupr(TempStr[0]);
		strtrim(TempStr[0]);

		if (strcmp(TempStr[0], "POWEROFF") == 0)
			strcpy(TempStr[0], "1");
		else if (strcmp(TempStr[0], "LOGOFF") == 0)
			strcpy(TempStr[0], "2");
		else if (strcmp(TempStr[0], "REBOOT") == 0)
			strcpy(TempStr[0], "3");
		else
			strcpy(TempStr[0], "0");

		if (strcmp(TempStr[0], "1") == 0)
		{
			TempInt1 = EWX_POWEROFF;
		}
		else if (strcmp(TempStr[0], "2") == 0)
		{
			TempInt1 = EWX_LOGOFF;
		}
		else if (strcmp(TempStr[0], "3") == 0)
		{
			TempInt1 = EWX_REBOOT;
		}
		else
		{
			TempInt1 = EWX_SHUTDOWN;
		}


		HANDLE hToken;
		TOKEN_PRIVILEGES tkp;
		OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken);
		LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
		tkp.PrivilegeCount=1;
		tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken,false,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);

		if (ExitWindowsEx(TempInt1, 0))
		{
			ERRORCODE("0");
		}
		else
		{
			ERRORCODE("-1");
		}
		return RB_OK;
	}
	// LOCKWORKSTATION ------------------------------------------------------
	if (strcmp(cmd, "R045") == 0)
	{
		LockStation LockWorkStation = reinterpret_cast<LockStation>(GetProcAddress(LoadLibrary("user32.dll"), "LockWorkStation"));
		if (LockWorkStation != NULL)
		{
			if (LockWorkStation())
			{
				ERRORCODE("0");
			}
			else
			{
				ERRORCODE("-1");
			}
		}
		else
		{
			ERRORCODE("-1");
		}
		return RB_OK;
	}
	// LETREGVAL -----------------------------------------------------------
	if (strcmp(cmd, "R053") == 0)
	{
		rbrunGetValue(&TempStr[4]);
		strupr(TempStr[4]);
		strtrim(TempStr[4]);

		rbrunGetValue(&TempStr[0]);
		strupr(TempStr[0]);
		strtrim(TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		rbrunGetValue(&TempStr[2]);
		rbrunGetValue(&TempStr[3]);

		HKEY regKey, rootKey;

		if (strcmp(TempStr[0], "CLASSES_ROOT") == 0)
			rootKey = HKEY_CLASSES_ROOT;
		if (strcmp(TempStr[0], "CURRENT_CONFIG") == 0)
			rootKey = HKEY_CURRENT_CONFIG;
		if (strcmp(TempStr[0], "CURRENT_USER") == 0)
			rootKey = HKEY_CURRENT_USER;
		if (strcmp(TempStr[0], "LOCAL_MACHINE") == 0)
			rootKey = HKEY_LOCAL_MACHINE;
		if (strcmp(TempStr[0], "USERS") == 0 ||
		        strcmp(TempStr[0], "DYN_DATA") == 0)
			rootKey = HKEY_USERS;

		if (strcmp(TempStr[1], "") != 0)
		{
			if (RegOpenKey(rootKey, TempStr[1], &regKey) != ERROR_SUCCESS)
			{
				ERRORCODE("-1");
				return RB_OK;
			}
		}
		else
		{
			regKey = rootKey;
		}

		long ret_value;

		if (strcmp(TempStr[4], "DWORD") == 0 ||
		        strcmp(TempStr[4], "1") == 0)
		{
			long mem_value = atol(TempStr[3]);
			ret_value = RegSetValueEx(regKey, TempStr[2], 0, REG_DWORD, (BYTE*)&mem_value, sizeof(long));
		}
		else if (strcmp(TempStr[4], "BINARY") == 0 ||
		         strcmp(TempStr[4], "2") == 0)
		{
			char* mem_value = (char*)NULL;
			char* tok = (char*)NULL;
			int mem_value_cnt = 0;
			char* sep = (char*)NULL;

			rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);

			tok = strtoken(TempStr[3], sep);

			while (tok != (char*)NULL)
			{
				if (mem_value_cnt == 0)
				{
					mem_value = (char*)malloc(2 * sizeof(char));
				}
				else
				{
					mem_value = (char*)realloc((char*)mem_value, (mem_value_cnt + 2) * sizeof(char));
				}
				mem_value[mem_value_cnt] = (char)atoi( tok );
				mem_value[mem_value_cnt+1] = '\0';

				mem_value_cnt++;

				tok = strtoken(NULL, sep);
			}

			if (mem_value == (char*)NULL)
			{
				mem_value = (char*)malloc(sizeof(char));
				*mem_value = '\0';
			}

			ret_value = RegSetValueEx(regKey, TempStr[2], 0, REG_BINARY, (BYTE*)mem_value, mem_value_cnt);

			FREE(sep);
			FREE(mem_value);
		}
		else
		{
			ret_value = RegSetValueEx(regKey, TempStr[2], 0, REG_SZ, (BYTE*)TempStr[3], strlen(TempStr[3])+1);
		}

		if (ret_value == ERROR_SUCCESS)
		{
			ERRORCODE("0");
		}
		else
		{
			ERRORCODE("-1");
		}

		RegCloseKey(regKey);

		return RB_OK;
	}
	// GETREGVAL -----------------------------------------------------------
	if (strcmp(cmd, "R064") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[4]);
		strupr(TempStr[4]);
		strtrim(TempStr[4]);

		rbrunGetValue(&TempStr[1]);
		rbrunGetValue(&TempStr[2]);
		rbrunGetValue(&TempStr[3]);

		HKEY regKey, rootKey;

		if (strcmp(TempStr[1], "CLASSES_ROOT") == 0)
			rootKey = HKEY_CLASSES_ROOT;
		if (strcmp(TempStr[1], "CURRENT_CONFIG") == 0)
			rootKey = HKEY_CURRENT_CONFIG;
		if (strcmp(TempStr[1], "CURRENT_USER") == 0)
			rootKey = HKEY_CURRENT_USER;
		if (strcmp(TempStr[1], "LOCAL_MACHINE") == 0)
			rootKey = HKEY_LOCAL_MACHINE;
		if (strcmp(TempStr[1], "USERS") == 0 ||
		        strcmp(TempStr[1], "DYN_DATA") == 0)
			rootKey = HKEY_USERS;

		if (strcmp(TempStr[2], "") != 0)
		{
			if (RegOpenKey(rootKey, TempStr[2], &regKey) != ERROR_SUCCESS)
			{
				ERRORCODE("-1");
				return RB_OK;
			}
		}
		else
		{
			regKey = rootKey;
		}

#ifdef DEBUG_SESSION
		//MALLOC_DEBUG(regvalue, STDLINE * sizeof(char));
#endif

		char* regvalue = (char*)NULL;
		long retval = 0;

		DWORD strsize = STDLINE;

		if (strcmp(TempStr[4], "DWORD") == 0
		        || strcmp(TempStr[4], "1") == 0)
		{
			long mem_value = 0;
			strsize = sizeof(long);

			if (strcmp(TempStr[3], "") == 0)
			{
				retval = RegQueryValueEx( regKey, NULL, NULL, NULL, (BYTE*)&mem_value, &strsize);
			}
			else
			{
				retval = RegQueryValueEx( regKey, TempStr[3], NULL, NULL, (BYTE*)&mem_value, &strsize);
			}

			regvalue = (char*)MALLOC((sizeof(long) * sizeof(char)) + 1);

			sprintf(regvalue, "%ld", mem_value);
		}
		else if (strcmp(TempStr[4], "BINARY") == 0
		         || strcmp(TempStr[4], "2") == 0)
		{
			BYTE* mem_value = (BYTE*)MALLOC(STDLINE * sizeof(BYTE));
			memset(mem_value, 0, (STDLINE * sizeof(char)));

			if (strcmp(TempStr[3], "") == 0)
			{
				retval = RegQueryValueEx( regKey, NULL, NULL, NULL, mem_value, &strsize);
			}
			else
			{
				retval = RegQueryValueEx( regKey, TempStr[3], NULL, NULL, mem_value, &strsize);
			}

			int regvalue_cnt = 0;
			char tmp[3+1];
			char* sep = (char*)NULL;

			rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);
			sep[1] = '\0';

			while (regvalue_cnt < strsize)
			{
				if (regvalue_cnt == 0)
				{
					regvalue = (char*)malloc(3+1+1 * sizeof(char));
					*regvalue = '\0';
				}
				else
				{
					regvalue = (char*)realloc((char*)regvalue, (strlen(regvalue) + 3 + 1 + 1) * sizeof(char));
				}


				sprintf(tmp, "%03d", (int)mem_value[regvalue_cnt]);

				strcat(regvalue, tmp);

				if (regvalue_cnt < (strsize-1))
					strcat(regvalue, sep);

				regvalue_cnt++;
			}

			FREE(mem_value);
			FREE(sep);
		}
		else
		{
			regvalue = (char*)MALLOC(STDLINE * sizeof(char));
			if (strcmp(TempStr[3], "") != 0)
			{
				while ((retval = RegQueryValueEx( regKey, TempStr[3], NULL, NULL, (LPBYTE) regvalue, &strsize))
				        == ERROR_MORE_DATA)
				{
					strsize += STDLINE;
					regvalue = (char*)realloc(regvalue, strsize);
				}
			}
			else
			{
				while ((retval = RegQueryValueEx( regKey, NULL, NULL, NULL, (LPBYTE) regvalue, &strsize))
				        == ERROR_MORE_DATA)
				{
					strsize += STDLINE;
					regvalue = (char*)realloc(regvalue, strsize);
				}
			}
		}

		if (retval == ERROR_SUCCESS)
		{
			ERRORCODE("0");
			RB_LETVAR(TempStr[0], regvalue);
		}
		else
		{
			ERRORCODE("-1");
		}
		RegCloseKey(regKey);

		if (regvalue != (char*)NULL)
		{
			FREE(regvalue);
		}

		return RB_OK;
	}
	// DELREGVAL -----------------------------------------------------------
	if (strcmp(cmd, "R054") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		strupr(TempStr[0]);
		strtrim(TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strupr(TempStr[0]);
		strtrim(TempStr[0]);

		rbrunGetValue(&TempStr[2]);

		HKEY rootKey, regKey;
		if (strcmp(TempStr[0], "CLASSES_ROOT") == 0)
			rootKey = HKEY_CLASSES_ROOT;
		if (strcmp(TempStr[0], "CURRENT_CONFIG") == 0)
			rootKey = HKEY_CURRENT_CONFIG;
		if (strcmp(TempStr[0], "CURRENT_USER") == 0)
			rootKey = HKEY_CURRENT_USER;
		if (strcmp(TempStr[0], "LOCAL_MACHINE") == 0)
			rootKey = HKEY_LOCAL_MACHINE;
		if (strcmp(TempStr[0], "USERS") == 0 ||
		        strcmp(TempStr[0], "DYN_DATA") == 0)
			rootKey = HKEY_USERS;

		if (strcmp(TempStr[1], "") != 0)
		{
			if (RegOpenKey(rootKey, TempStr[1], &regKey)
			        != ERROR_SUCCESS)
			{
				ERRORCODE("-1");
				return RB_OK;
			}
		}
		else
		{
			regKey = rootKey;
		}

		if (RegDeleteValue(regKey,TempStr[2]) == ERROR_SUCCESS)
		{
			ERRORCODE("0");
		}
		else
		{
			ERRORCODE("-1");
		}
		RegCloseKey(regKey);

		return RB_OK;
	}
	// NEWREGKEY -----------------------------------------------------------
	if (strcmp(cmd, "R055") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		strupr(TempStr[0]);
		strtrim(TempStr[0]);

		rbrunGetValue(&TempStr[1]);


		HKEY regKey, rootKey;

		if (strcmp(TempStr[0], "CLASSES_ROOT") == 0)
			rootKey = HKEY_CLASSES_ROOT;
		if (strcmp(TempStr[0], "CURRENT_CONFIG") == 0)
			rootKey = HKEY_CURRENT_CONFIG;
		if (strcmp(TempStr[0], "CURRENT_USER") == 0)
			rootKey = HKEY_CURRENT_USER;
		if (strcmp(TempStr[0], "LOCAL_MACHINE") == 0)
			rootKey = HKEY_LOCAL_MACHINE;
		if (strcmp(TempStr[0], "USERS") == 0 ||
		        strcmp(TempStr[0], "DYN_DATA") == 0)
			rootKey = HKEY_USERS;

		if (RegCreateKeyEx(rootKey,TempStr[1], 0, 0,
		                   REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&regKey,0)
		        == ERROR_SUCCESS)
		{
			ERRORCODE("0");
		}
		else
		{
			ERRORCODE("-1");
		}
		RegCloseKey(regKey);

		return RB_OK;
	}
	// DELREGKEY -----------------------------------------------------------
	if (strcmp(cmd, "R056") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		strupr(TempStr[0]);
		strtrim(TempStr[0]);

		rbrunGetValue(&TempStr[1]);


		HKEY rootKey;
		if (strcmp(TempStr[0], "CLASSES_ROOT") == 0)
			rootKey = HKEY_CLASSES_ROOT;
		if (strcmp(TempStr[0], "CURRENT_CONFIG") == 0)
			rootKey = HKEY_CURRENT_CONFIG;
		if (strcmp(TempStr[0], "CURRENT_USER") == 0)
			rootKey = HKEY_CURRENT_USER;
		if (strcmp(TempStr[0], "LOCAL_MACHINE") == 0)
			rootKey = HKEY_LOCAL_MACHINE;
		if (strcmp(TempStr[0], "USERS") == 0 ||
		        strcmp(TempStr[0], "DYN_DATA") == 0)
			rootKey = HKEY_USERS;

		if (RegDeleteKey(rootKey, TempStr[1]) == ERROR_SUCCESS)
		{
			ERRORCODE("0");
		}
		else
		{
			ERRORCODE("-1");
		}

		return RB_OK;
	}
	// SENDKEYS ------------------------------------------------------------
	if (strcmp(cmd, "R033") == 0)
	{
		rbrunGetValue(&TempStr[1]);

		HWND prog = FindWindow (NULL, TempStr[1]);

		rbrunGetValue(&TempStr[0]);

		//Let's create a temporary, upper case window identifier ;)...
		TempStr[2] = strdup(TempStr[0]);
		strupr(TempStr[2]);

		if (prog == (HWND)NULL)
		{
			if (strcmp(TempStr[2], "DESKTOP") == 0)
			{
				prog = GetDesktopWindow();
			}
			else if (strcmp(TempStr[2], "FOREGROUND") == 0)
			{
				prog = GetForegroundWindow();
			}
			else if (strcmp(TempStr[2], "ACTIVE") == 0)
			{
				prog = GetActiveWindow();
			}
			else
			{
				prog = FindWindow (TempStr[1], NULL);
				if (prog == (HWND)NULL)
				{
					/*
						JMM 25.05.2007
						So hier soll es jetzt auch
						moeglich sein, eine Fenster-ID
						direkt anzugeben.
					*/
					if( prog == (HWND)NULL )
					{
						prog = (HWND)atol( TempStr[1] );
					}

					if( prog == (HWND)NULL )
					{
						ERRORCODE( "-1" );
						return RB_OK;
					}
				}
			}
		}

		if( SetForegroundWindow(prog) == 0 )
		{
			ERRORCODE("-1");
			return RB_OK;
		}

		ERRORCODE("0");
		ShowWindow(prog, SW_SHOWNORMAL);

		sendkeys( TempStr[0] );

		return RB_OK;
	}
	// MOUSEEVENT ----------------------------------------------------------
	if (strcmp(cmd, "R059") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		strupr(TempStr[0]);
		strtrim(TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		char* sep = (char*)NULL;
		rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);

		TempStr[2] = strtok(TempStr[1], sep);
		if (TempStr[2] == (char*)NULL)
			TempInt1 = 0;
		else
			TempInt1 = atoi(TempStr[2]) - 1;

		TempStr[2] = strtok(NULL, sep);
		if (TempStr[2] == (char*)NULL)
			TempInt2 = 0;
		else
			TempInt2 = atoi(TempStr[2]) - 1;


		FREE(sep);

		SetCursorPos(TempInt1,TempInt2);
		if (strcmp(TempStr[0], "1") == 0
		        || strcmp(TempStr[0], "CLICK_LEFT") == 0)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
			mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
		}
		else if (strcmp(TempStr[0], "2") == 0
		         || strcmp(TempStr[0], "DBLCLICK_LEFT") == 0)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
			mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
			mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
			mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
		}
		else if (strcmp(TempStr[0], "3") == 0
		         || strcmp(TempStr[0], "CLICK_RIGHT") == 0)
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
			mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
		}
		else if (strcmp(TempStr[0], "4") == 0
		         || strcmp(TempStr[0], "DBLCLICK_RIGHT") == 0)
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
			mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
			mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
			mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
		}
		else if (strcmp(TempStr[0], "5") == 0
		         || strcmp(TempStr[0], "DOWN_LEFT") == 0)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		}
		else if (strcmp(TempStr[0], "6") == 0
		         || strcmp(TempStr[0], "UP_LEFT") == 0)
		{
			mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
		}
		else if (strcmp(TempStr[0], "7") == 0
		         || strcmp(TempStr[0], "DOWN_RIGHT") == 0)
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
		}
		else if (strcmp(TempStr[0], "8") == 0
		         || strcmp(TempStr[0], "UP_RIGHT") == 0)
		{
			mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
		}

		return RB_OK;
	}
	// GETFILEATT ----------------------------------------------------------
	if (strcmp(cmd, "R039") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		rbrunGetValue(&TempStr[2]);
		strupr(TempStr[2]);
		strtrim(TempStr[2]);

		if (strcmp(TempStr[2], "NORMAL") == 0 || strcmp(TempStr[2], "0") == 0)
			strcpy(TempStr[2],  "0");
		else if (strcmp(TempStr[2], "ARCHIVE") == 0 || strcmp(TempStr[2], "1") == 0)
			strcpy(TempStr[2],  "1");
		else if (strcmp(TempStr[2], "READONLY") == 0 || strcmp(TempStr[2], "2") == 0)
			strcpy(TempStr[2],  "2");
		else if (strcmp(TempStr[2], "COMPRESSED") == 0 || strcmp(TempStr[2], "3") == 0)
			strcpy(TempStr[2],  "3");
		else if (strcmp(TempStr[2], "DIRECTORY") == 0 || strcmp(TempStr[2], "4") == 0)
			strcpy(TempStr[2],  "4");
		else if (strcmp(TempStr[2], "HIDDEN") == 0 || strcmp(TempStr[2], "5") == 0)
			strcpy(TempStr[2],  "5");
		else if (strcmp(TempStr[2], "SYSTEM") == 0 || strcmp(TempStr[2], "6") == 0)
			strcpy(TempStr[2],  "6");
		else strcpy(TempStr[2],  "7");

		TempInt1 = atoi(TempStr[2]);

		/*
		  0 = FILE_ATTRIBUTE_NORMAL
		  1 = FILE_ATTRIBUTE_ARCHIVE
		  2 = FILE_ATTRIBUTE_READONLY
		  3 = FILE_ATTRIBUTE_COMPRESSED
		  4 = FILE_ATTRIBUTE_DIRECTORY
		                5 = FILE_ATTRIBUTE_HIDDEN
		  6 = FILE_ATTRIBUTE_SYSTEM
		              */
		DWORD dwAtts = GetFileAttributes(TempStr[1]);

		if ( dwAtts == 0xFFFFFFFF )
		{
			TempInt1 = -1;
		}
		else
		{
			ERRORCODE("0");
		}

		switch (TempInt1)
		{
			case 0:
				if ((dwAtts & FILE_ATTRIBUTE_NORMAL))
					RB_LETVAR(TempStr[0], "0");
				break;
			case 1:
				if ((dwAtts & FILE_ATTRIBUTE_ARCHIVE))
					RB_LETVAR(TempStr[0], "0");
				break;
			case 2:
				if ((dwAtts & FILE_ATTRIBUTE_READONLY))
					RB_LETVAR(TempStr[0], "0");
				break;
			case 3:
				if ((dwAtts & FILE_ATTRIBUTE_COMPRESSED))
					RB_LETVAR(TempStr[0], "0");
				break;
			case 4:
				if ((dwAtts & FILE_ATTRIBUTE_DIRECTORY))
					RB_LETVAR(TempStr[0], "0");
				break;
			case 5:
				if ((dwAtts & FILE_ATTRIBUTE_HIDDEN))
					RB_LETVAR(TempStr[0], "0");
				break;
			case 6:
				if ((dwAtts & FILE_ATTRIBUTE_SYSTEM))
					RB_LETVAR(TempStr[0], "0");
				break;

			default:
				ERRORCODE("-1");
				RB_LETVAR(TempStr[0], "-1");
				break;
		}
		return RB_OK;
	}
	// LETFILEATT ----------------------------------------------------------
	if (strcmp(cmd, "R040") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strupr(TempStr[1]);
		strtrim(TempStr[1]);

		if (strcmp(TempStr[1], "NORMAL") == 0 || strcmp(TempStr[1], "0") == 0)
			strcpy(TempStr[1],  "0");
		else if (strcmp(TempStr[1], "ARCHIVE") == 0 || strcmp(TempStr[1], "1") == 0)
			strcpy(TempStr[1],  "1");
		else if (strcmp(TempStr[1], "READONLY") == 0 || strcmp(TempStr[1], "2") == 0)
			strcpy(TempStr[1],  "2");
		else if (strcmp(TempStr[1], "COMPRESSED") == 0 || strcmp(TempStr[1], "3") == 0)
			strcpy(TempStr[1],  "3");
		else if (strcmp(TempStr[1], "DIRECTORY") == 0 || strcmp(TempStr[1], "4") == 0)
			strcpy(TempStr[1],  "4");
		else if (strcmp(TempStr[1], "HIDDEN") == 0 || strcmp(TempStr[1], "5") == 0)
			strcpy(TempStr[1],  "5");
		else if (strcmp(TempStr[1], "SYSTEM") == 0 || strcmp(TempStr[1], "6") == 0)
			strcpy(TempStr[1],  "6");
		else strcpy(TempStr[1],  "7");

		TempInt1 = atoi(TempStr[1]);

		/*
		  0 = FILE_ATTRIBUTE_NORMAL
		  1 = FILE_ATTRIBUTE_ARCHIVE
		  2 = FILE_ATTRIBUTE_READONLY
		  3 = FILE_ATTRIBUTE_COMPRESSED
		  4 = FILE_ATTRIBUTE_DIRECTORY
		  5 = FILE_ATTRIBUTE_HIDDEN
		  6 = FILE_ATTRIBUTE_SYSTEM
		*/

		DWORD dwAtts = GetFileAttributes(TempStr[0]);

		if ( dwAtts == 0xFFFFFFFF )
		{
			TempInt1 = -1;
		}
		else
		{
			ERRORCODE("0");
		}

		switch (TempInt1)
		{
			case 0:
				if (dwAtts != FILE_ATTRIBUTE_NORMAL)
					SetFileAttributes(TempStr[0], FILE_ATTRIBUTE_NORMAL);
				break;
			case 1:
				if (!(dwAtts & FILE_ATTRIBUTE_ARCHIVE))
					SetFileAttributes(TempStr[0], dwAtts | FILE_ATTRIBUTE_ARCHIVE);
				break;
			case 2:
				if (!(dwAtts & FILE_ATTRIBUTE_READONLY))
					SetFileAttributes(TempStr[0], dwAtts | FILE_ATTRIBUTE_READONLY);
				break;
			case 3:
				if (!(dwAtts & FILE_ATTRIBUTE_COMPRESSED))
					SetFileAttributes(TempStr[0], dwAtts | FILE_ATTRIBUTE_COMPRESSED);
				break;
			case 4:
				if (!(dwAtts & FILE_ATTRIBUTE_DIRECTORY))
					SetFileAttributes(TempStr[0], dwAtts | FILE_ATTRIBUTE_DIRECTORY);
				break;
			case 5:
				if (!(dwAtts & FILE_ATTRIBUTE_HIDDEN))
					SetFileAttributes(TempStr[0], dwAtts | FILE_ATTRIBUTE_HIDDEN);
				break;
			case 6:
				if (!(dwAtts & FILE_ATTRIBUTE_SYSTEM))
					SetFileAttributes(TempStr[0], dwAtts | FILE_ATTRIBUTE_SYSTEM);
				break;
			default:
				ERRORCODE("-1");
		}
		return RB_OK;
	}


// *****************************************************************************
// BUILD-IN DIALOGS
// *****************************************************************************
	// ECHO/ECHOLN ---------------------------------------------------------
	if (strcmp(cmd, "R027") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		rbrunGetVar(PVAR_ECHO_TITLE, &TempStr[1], procStack_cnt);

		MessageBox(hInvisibleWindow, TempStr[0], TempStr[1], 64 | MB_SETFOREGROUND);

		SetActiveWindow(GetForegroundWindow());

		return RB_OK;
	}
	// CONFIRM -------------------------------------------------------------
	if (strcmp(cmd, "R028") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);

		rbrunGetVar(PVAR_CONFIRM_TITLE, &TempStr[2], procStack_cnt);

		if (MessageBox(hInvisibleWindow, TempStr[1], TempStr[2], 36 | MB_SETFOREGROUND) == 6)
		{
			RB_LETVAR(TempStr[0], "0");
		}
		else
		{
			RB_LETVAR(TempStr[0], "-1");
		}

		SetActiveWindow(GetForegroundWindow());
		return RB_OK;
	}
	// MSGBOX --------------------------------------------------------------
	if (strcmp(cmd, "R007") == 0
	        || strcmp(cmd, "R063") == 0)
	{
		//This can be called as an instruction or as a function
		if (strcmp(cmd, "R063") == 0)
			rbrunScanVar(&TempStr[3]);

		rbrunGetValue(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		rbrunGetValue(&TempStr[2]);
		strupr(TempStr[2]);
		strtrim(TempStr[2]);

		if (strcmp(TempStr[2], "OK") == 0) strcpy(TempStr[2], "0");
		if (strcmp(TempStr[2], "OKCANCEL") == 0) strcpy(TempStr[2], "1");
		if (strcmp(TempStr[2], "CANCELRETRYIGNORE") == 0) strcpy(TempStr[2], "2");
		if (strcmp(TempStr[2], "YESNOCANCEL") == 0) strcpy(TempStr[2], "3");
		if (strcmp(TempStr[2], "YESNO") == 0) strcpy(TempStr[2], "4");
		if (strcmp(TempStr[2], "RETRYCANCEL") == 0) strcpy(TempStr[2], "5");

		TempInt1 = atoi(TempStr[2]);
		TempInt1 = MessageBox(NULL, TempStr[1], TempStr[0], TempInt1 | MB_SETFOREGROUND);
		sprintf(text, "%d", TempInt1);

		SetActiveWindow(GetForegroundWindow());

		if (strcmp(cmd, "R063") == 0)
		{
			RB_LETVAR(TempStr[3], text);
		}
		else
		{
			ERRORCODE(text);
		}

		return RB_OK;
	}
	// INPUTBOX/PWDBOX -----------------------------------------------------
	if (strcmp(cmd, "R008") == 0 ||
	        strcmp(cmd, "R041") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);
		rbrunGetValue(&TempStr[2]);
		rbrunGetValue(&TempStr[3]);

		//Show the inputbox...
		MSG messages;
		IsEvent = false;
		SendMessage(IB_TEXT, WM_SETTEXT, 0,(long) TempStr[2]);
		SendMessage(IB_WIN, WM_SETTEXT, 0,(long) TempStr[1]);
		if (strcmp(cmd, "R008") == 0)
		{
			ShowWindow(IB_PASSWORD, SW_HIDE);
			SendMessage(IB_EDIT, WM_SETTEXT, 0,(long) TempStr[3]);
			ShowWindow(IB_EDIT, SW_SHOW);
			SetFocus(IB_EDIT);
		}
		else
		{
			ShowWindow(IB_EDIT, SW_HIDE);
			SendMessage(IB_PASSWORD, WM_SETTEXT, 0,(long) TempStr[3]);
			ShowWindow(IB_PASSWORD, SW_SHOW);
			SetFocus(IB_PASSWORD);
		}
		SendMessage(IB_EDIT,EM_SETSEL,strlen(TempStr[3]),strlen(TempStr[3]));

		int width = 300;
		int height = 102;

		rbrunGetVar(PVAR_INPUTBOX_X, &TempStr[4], procStack_cnt);
		int pos_x = atoi(TempStr[4]) - 1;

		rbrunGetVar(PVAR_INPUTBOX_Y, &TempStr[4], procStack_cnt);
		int pos_y = atoi(TempStr[4]) - 1;

		if (pos_x < 0) pos_x = GetSystemMetrics(SM_CXSCREEN)/2 - (width / 2);
		if (pos_y < 0) pos_y = GetSystemMetrics(SM_CYSCREEN)/2 - (height / 2);

		rbrunGetVar(PVAR_INPUTBOX_OK, &TempStr[4], procStack_cnt);
		rbrunGetVar(PVAR_INPUTBOX_CANCEL, &TempStr[5], procStack_cnt);

		if (strcmp(TempStr[4], "") == 0 &&
		        strcmp(TempStr[5], "") == 0)
		{
			MoveWindow(IB_WIN, pos_x, pos_y, width, height + devpix - 20, 1);
		}
		else
		{
			MoveWindow(IB_WIN, pos_x, pos_y, width, height + devpix, 1);
		}

		ShowWindow(IB_WIN, SW_SHOW);

		while (GetMessage(&messages, IB_WIN, 0, 0))
		{
			if (!IsDialogMessage(IB_WIN, &messages))
			{
				TranslateMessage(&messages);
				DispatchMessage(&messages);
			}
			if (IsEvent == true) break; //This line never in if-struct above!!
		}

		if (DlgClick == OKBUTTON_CLICK)
		{
			int textsize = 0;
			char *textbuf = (char*)NULL;

			if (strcmp(cmd, "R008") == 0)
			{
				textsize = SendMessage(IB_EDIT, WM_GETTEXTLENGTH, 0, 0);
				textbuf = (char*) MALLOC((textsize+1)*sizeof(char));

#ifdef DEBUG_SESSION
				MALLOC_DEBUG(textbuf, (textsize+1)*sizeof(char));
#endif

				SendMessage(IB_EDIT, WM_GETTEXT, textsize+1, (long) textbuf ); //Text
			}
			else
			{
				textsize = SendMessage(IB_PASSWORD, WM_GETTEXTLENGTH, 0, 0);
				textbuf = (char*) MALLOC((textsize+1)*sizeof(char));

#ifdef DEBUG_SESSION
				MALLOC_DEBUG(textbuf, (textsize+1)*sizeof(char));
#endif

				SendMessage(IB_PASSWORD, WM_GETTEXT, textsize+1, (long) textbuf ); //Text
			}
			ERRORCODE("0");
			RB_LETVAR(TempStr[0],textbuf);

			FREE(textbuf);
		}
		else
		{
			ERRORCODE("-1");
			RB_LETVAR(TempStr[0],"");
		}
		ShowWindow(IB_WIN, SW_HIDE);
		DlgClick = -1;
		return RB_OK;
	}
	// EDITBOX -------------------------------------------------------------
	if (strcmp(cmd, "R032") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		rbrunGetValue(&TempStr[2]);

		rbrunGetValue(&TempStr[3]);
		strupr(TempStr[3]);
		strtrim(TempStr[3]);


		if (strcmp(TempStr[3], "EDITABLE") == 0
		        || strcmp(TempStr[3], "WRITEABLE") == 0)
		{
			strcpy(TempStr[3], "0");
		}
		else if (strcmp(TempStr[3], "READONLY") == 0)
		{
			strcpy(TempStr[3], "-1");
		}

		TempInt1 = atoi(TempStr[3]);

		//Show EditBox...
		MSG messages;
		IsEvent = false;
		SendMessage(hEditBox_Win, WM_SETTEXT, 0,(long) TempStr[1]);

		//Replace LF with CR/LF
		strreplace(&TempStr[2], "\n", "\r\n", true);

		//Set text to dialog
		SendMessage(hEditBox_Edit, WM_SETTEXT, 0,(long) TempStr[2]);

		if (TempInt1 == -1)
		{
			SendMessage(hEditBox_Edit, EM_SETREADONLY, TRUE, NULL); //Set readonly
		}
		else
		{
			SendMessage(hEditBox_Edit, EM_SETREADONLY, FALSE, NULL); //Set writeable
		}

		//Get dialog positions
		rbrunGetVar(PVAR_EDITBOX_HEIGHT, &TempStr[4], procStack_cnt);
		int height = atoi(TempStr[4]) - 1;

		rbrunGetVar(PVAR_EDITBOX_WIDTH, &TempStr[4], procStack_cnt);
		int width = atoi(TempStr[4]) - 1;

		rbrunGetVar(PVAR_EDITBOX_X, &TempStr[4], procStack_cnt);
		int pos_x = atoi(TempStr[4]) - 1;

		rbrunGetVar(PVAR_EDITBOX_Y, &TempStr[4], procStack_cnt);
		int pos_y = atoi(TempStr[4]) - 1;

		if (pos_x < 0) pos_x = GetSystemMetrics(SM_CXSCREEN)/2 - (width / 2);
		if (pos_y < 0) pos_y = GetSystemMetrics(SM_CYSCREEN)/2 - (height / 2);

		MoveWindow(hEditBox_Win, pos_x, pos_y, width, height + devpix, 1);
		ShowWindow(hEditBox_Win, SW_SHOW);

		RECT win_size;
		GetClientRect(hEditBox_Win, &win_size);
		SendMessage(hEditBox_Win, WM_SIZE, (LPARAM)SIZE_RESTORED, MAKEWPARAM(win_size.right, win_size.bottom));

		SetFocus(hEditBox_Edit);
		while (GetMessage(&messages, hEditBox_Win, 0, 0))
		{
			if (!IsDialogMessage(hEditBox_Win, &messages))
			{
				TranslateMessage(&messages);
				DispatchMessage(&messages);
			}
			if (IsEvent == true) break; //This line never in if-struct above!!
		}

		if (DlgClick == OKBUTTON_CLICK)
		{
			int fieldsize; //Textfeldgrösse
			char *textbuf; //Textpuffer
			fieldsize = SendMessage(hEditBox_Edit, WM_GETTEXTLENGTH, 0, 0);
			textbuf = (char*) MALLOC((fieldsize+1) * sizeof(char));

#ifdef DEBUG_SESSION
			MALLOC_DEBUG(textbuf, (fieldsize+1)*sizeof(char));
#endif

			SendMessage(hEditBox_Edit, WM_GETTEXT, fieldsize+1, (long) textbuf ); //Text

			//Replace LF with CR/LF
			strreplace(&textbuf, "\r\n", "\n", true);

			//Set text to return variable
			RB_LETVAR(TempStr[0],textbuf);

			FREE(textbuf);

			ERRORCODE("0");
		}
		else
		{
			ERRORCODE("-1");
			RB_LETVAR(TempStr[0],"");
		}

		ShowWindow(hEditBox_Win, SW_HIDE);
		DlgClick = -1;

		return RB_OK;
	}
	// LISTBOX -------------------------------------------------------------
	if (strcmp(cmd, "R031") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);
		rbrunGetValue(&TempStr[2]);

		//Show the listbox...
		MSG messages;
		IsEvent = false;

		HWND current_list = (HWND) NULL;

		//Allow multiple selections?
		rbrunGetVar(PVAR_LISTBOX_MULTISEL, &TempStr[3], procStack_cnt);

		bool multiSel = false;
		if (strcmp(TempStr[3], "-1") == 0)
		{
			current_list = hListBox_List;
			ShowWindow(hListBox_List, SW_SHOW);
			ShowWindow(hListBox_MultiList, SW_HIDE);
		}
		else
		{
			current_list = hListBox_MultiList;
			ShowWindow(hListBox_MultiList, SW_SHOW);
			ShowWindow(hListBox_List, SW_HIDE);
			multiSel = true;

		}

		SendMessage(current_list, LB_RESETCONTENT, 0, 0);
		SendMessage(hListBox_Win, WM_SETTEXT, 0,(long) TempStr[1]);

		//Get the items
		char* sep = (char*)NULL;
		rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);

		char* tok = strtoken(TempStr[2], sep);
		while (tok != (char*)NULL)
		{
			SendMessage(current_list, LB_ADDSTRING, NULL, (LPARAM)tok);
			tok = strtoken(NULL, sep);
		}

		//Get dialog positions
		rbrunGetVar(PVAR_LISTBOX_HEIGHT, &TempStr[4], procStack_cnt);
		int height = atoi(TempStr[4]) - 1;

		rbrunGetVar(PVAR_LISTBOX_WIDTH, &TempStr[4], procStack_cnt);
		int width = atoi(TempStr[4]) - 1;

		rbrunGetVar(PVAR_LISTBOX_X, &TempStr[4], procStack_cnt);
		int pos_x = atoi(TempStr[4]) - 1;

		rbrunGetVar(PVAR_LISTBOX_Y, &TempStr[4], procStack_cnt);
		int pos_y = atoi(TempStr[4]) - 1;

		if (pos_x < 0) pos_x = GetSystemMetrics(SM_CXSCREEN)/2 - (width / 2);
		if (pos_y < 0) pos_y = GetSystemMetrics(SM_CYSCREEN)/2 - (height / 2);
		MoveWindow(hListBox_Win, pos_x, pos_y, width, height + devpix, true);

		ShowWindow(hListBox_Win, SW_SHOW);

		RECT win_size;
		GetClientRect(hListBox_Win, &win_size);
		SendMessage(hListBox_Win, WM_SIZE, (LPARAM)SIZE_RESTORED, MAKEWPARAM(win_size.right, win_size.bottom));

		//SetWindowPos(hListBox_Win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		SendMessage(current_list, LB_SETCURSEL, 0, 0);
		SetFocus(current_list);
		while (GetMessage(&messages, hListBox_Win, 0, 0))
		{
			if (!IsDialogMessage(hListBox_Win, &messages))
			{
				TranslateMessage(&messages);
				DispatchMessage(&messages);
			}
			if (IsEvent == true) break; //This line never in if-struct above!!
		}

		if (DlgClick == OKBUTTON_CLICK)
		{
			int lindex, len; //Textfeldgrösse
			char *textbuf; //Textpuffer

			if (!multiSel)
			{
				lindex = SendMessage(current_list, LB_GETCURSEL, 0, 0); //Index
				if (lindex != LB_ERR)
				{
					len = SendMessage(current_list, LB_GETTEXTLEN, lindex, 0 );
					textbuf = (char*)MALLOC((len+1)*sizeof(char));
#ifdef DEBUG_SESSION
					MALLOC_DEBUG(textbuf, (len+1)*sizeof(char));
#endif
					SendMessage(current_list, LB_GETTEXT, lindex, (long)textbuf);
					RB_LETVAR(TempStr[0],textbuf);
					FREE(textbuf);
				}
				else
				{
					RB_LETVAR(TempStr[0],"");
				}
			}
			else
			{
				int items_Pos[1024];
				int selItemCnt = SendMessage(current_list, LB_GETSELCOUNT, 0, 0);
				SendMessage(current_list, LB_GETSELITEMS, selItemCnt, (LPARAM) items_Pos);

				char* ret_string = (char*)MALLOC(sizeof(char));
#ifdef DEBUG_SESSION
				MALLOC_DEBUG(ret_string, sizeof(char));
#endif
				*ret_string = '\0';

				for (int a = 0; a<selItemCnt; a++)
				{
					len = SendMessage(current_list, LB_GETTEXTLEN, items_Pos[a], 0 );
					textbuf = (char*)MALLOC((len+1)*sizeof(char));

#ifdef DEBUG_SESSION
					MALLOC_DEBUG(textbuf, (len+1) * sizeof(char));
#endif

					SendMessage(current_list, LB_GETTEXT, items_Pos[a], (long)textbuf);

					ret_string = (char*) realloc((char*)ret_string,
					                             (strlen(ret_string) + strlen(textbuf) + strlen(sep) + 1) * sizeof(char));

					strcat(ret_string, textbuf);

					if ((a+1) < selItemCnt)
						strcat(ret_string, sep);

					FREE(textbuf);
				}
				RB_LETVAR(TempStr[0],ret_string);
				FREE(ret_string);
			}
			ERRORCODE("0");
		}
		else
		{
			RB_LETVAR(TempStr[0],"");
			ERRORCODE("-1");
		}
		ShowWindow(hListBox_Win, SW_HIDE);
		DlgClick = -1;

		FREE(sep);
		return RB_OK;
	}
	// TRAYMENU ------------------------------------------------------------
	if (strcmp(cmd, "R058") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);
		rbrunGetValue(&TempStr[2]);

		MSG messages;
		IsEvent = false;

		//Set system tray icon
		rbrunGetVar(PVAR_APP_ICON, &TempStr[3], procStack_cnt);

		if (strcmp(TempStr[3], "") == 0)
		{
			hTrayIcon = (HICON)LoadImage(hInstance, TempStr[3], IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		}

		if (hTrayIcon == (HICON)NULL)
		{
			hTrayIcon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(1000));
		}

		//Get the items
		char* sep = (char*)NULL;
		rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);

		if (strcmp(TempStr[2], "") != 0)
		{
			hTrayMenu = CreatePopupMenu();

			//Copy current string to get the chosen menu item later
			TempStr[4] = (char*)MALLOC((strlen(TempStr[2])+1) * sizeof(char));
#ifdef DEBUG_SESSION
			MALLOC_DEBUG(TempStr[4], (strlen(TempStr[2])+1) * sizeof(char));
#endif

			strcpy(TempStr[4], TempStr[2]);

			//Put items into the menu bar
			int menu_pos = 1;
			char* tok = strtok(TempStr[2], sep);

			while (tok != (char*)NULL)
			{
				if (strcmp(tok, "-") != 0)
				{
					AppendMenu(hTrayMenu, MF_STRING, menu_pos, tok);
				}
				else
				{
					AppendMenu(hTrayMenu, MF_SEPARATOR, menu_pos, "");
				}
				menu_pos++;

				tok = strtok(NULL, sep);
			}
		}
		else
		{
			hTrayMenu = (HMENU)NULL;
		}

		trayIconToolTip = TempStr[1];
		s_uTaskbarRestart = RegisterWindowMessage ("TaskbarCreated");
		iconToTaskbar (hInvisibleWindow, hTrayIcon, trayIconToolTip, false);

		//Show the icon
		onTrayMenu = true;
		trayMenuSelect = -1;
		while (GetMessage(&messages, NULL, 0, 0))
		{
			TranslateMessage(&messages);
			DispatchMessage(&messages);

			if (IsEvent == true) break;
		}

		onTrayMenu = false;
		iconToTaskbar (hInvisibleWindow, hTrayIcon, "", true);

		if (trayMenuSelect > -1)
		{
			int menu_pos = 1;
			char* tok = strtok(TempStr[4], sep);

			while (tok != (char*)NULL)
			{
				if (menu_pos == trayMenuSelect)
				{
					RB_LETVAR(TempStr[0], tok);
					break;
				}

				//if(strcmp(tok, "-") != 0)
				menu_pos++;

				tok = strtok(NULL, sep);
			}
		}
		else
		{
			RB_LETVAR(TempStr[0], "");
		}

		DestroyIcon(hTrayIcon);
		DestroyMenu(hTrayMenu);
		hTrayIcon = (HICON)NULL;

		FREE(sep);

		DlgClick = -1;
		return RB_OK;
	}
	// OPENBOX/SAVEBOX -----------------------------------------------------
	if (strcmp(cmd, "R022") == 0
	        || strcmp(cmd, "R023") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);
		rbrunGetValue(&TempStr[2]);

		char filenames[STDLINE+1];
		OPENFILENAME FileDialog; //Structure for Open/Save-Dialogs
		ZeroMemory(&FileDialog, sizeof(FileDialog));
		FileDialog.lStructSize          = sizeof(FileDialog);

		FileDialog.lpstrFile            = filenames;
		FileDialog.lpstrFile[0]         = '\0';

		FileDialog.nMaxFile             = sizeof(filenames);
		FileDialog.nFilterIndex         = 0;
		FileDialog.lpstrTitle           = TempStr[1];
		FileDialog.nMaxFileTitle        = STDLINE;

		getcwd(text, STDLINE);
		FileDialog.lpstrInitialDir      = text;
		FileDialog.hwndOwner            = hInvisibleWindow;


		//Set or unset multiselection
		bool multiSel = false;
		if (strcmp(cmd, "R022") == 0)
		{
			rbrunGetVar(PVAR_OPENBOX_MULTISEL, &TempStr[3], procStack_cnt);
			if (strcmp(TempStr[3], "-1") != 0)
			{
				FileDialog.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY;
				multiSel = true;
			}
			else
			{
				FileDialog.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
			}

			rbrunGetVar(PVAR_OPENBOX_DEFFILEEXT, &TempStr[4], procStack_cnt);
			FileDialog.lpstrDefExt = TempStr[4];
		}
		else
		{
			rbrunGetVar(PVAR_SAVEBOX_MULTISEL, &TempStr[3], procStack_cnt);
			if (strcmp(TempStr[3], "-1") != 0)
			{
				FileDialog.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY;
				multiSel = true;
			}
			else
			{
				FileDialog.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
			}

			rbrunGetVar(PVAR_SAVEBOX_DEFFILEEXT, &TempStr[4], procStack_cnt);
			FileDialog.lpstrDefExt = TempStr[4];
		}

		char* sep = (char*)NULL;
		rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);

		if (strcmp(TempStr[2], "") != 0)
		{
			TempStr[2] = (char*)realloc((char*)TempStr[2],
			                            (strlen(TempStr[2]) + 2) * sizeof(char));

			unsigned long int len = strlen(TempStr[2]);
			for (unsigned long int i=0; i < len; i++)
			{
				if (TempStr[2][i] == sep[0])
				{
					TempStr[2][i] = '\0';
				}
			}

			TempStr[2][len+1] = '\0';
		}

		FileDialog.lpstrFilter = TempStr[2];

		if (strcmp(cmd, "R022") == 0)
		{
			if (GetOpenFileName(&FileDialog) == false)
			{
				ERRORCODE("-1");
			}
			else
			{
				ERRORCODE("0");
			}
		}
		else
		{
			if (GetSaveFileName(&FileDialog) == false)
			{
				ERRORCODE("-1");
			}
			else
			{
				ERRORCODE("0");
			}
		}

		if (multiSel)
		{
			for (int i=0;;i++)
			{
				if (FileDialog.lpstrFile[i] == '\0')
				{
					if (FileDialog.lpstrFile[i+1] != '\0')
					{
						FileDialog.lpstrFile[i] = sep[0];
					}
					else
					{
						break;
					}
				}
			}
		}


		FREE(sep);
		RB_LETVAR(TempStr[0], FileDialog.lpstrFile);

		rbrunGetVar( PVAR_CURRENT, &TempStr[1], procStack_cnt );
		chdir( TempStr[1] );

		return RB_OK;
	}
	// FOLDERBOX -----------------------------------------------------------
	if (strcmp(cmd, "R037") == 0)
	{
		rbrunScanVar(&TempStr[0]);
		rbrunGetValue(&TempStr[1]);
		rbrunGetValue(&TempStr[2]);

		if (BrowseForFolder((char*) TempStr[1], (char*) TempStr[2], text) != 0)
		{
			RB_LETVAR(TempStr[0], text);
			ERRORCODE ("0");
		}
		else
		{
			RB_LETVAR(TempStr[0], "");
			ERRORCODE ("-1");
		}
		return RB_OK;
	}
	// INFOBOX -------------------------------------------------------------
	if (strcmp(cmd, "R038") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strupr(TempStr[1]);

		//Get dialog positions
		rbrunGetVar(PVAR_INFOBOX_HEIGHT, &TempStr[2], procStack_cnt);
		int height = atoi(TempStr[2]) - 1;

		rbrunGetVar(PVAR_INFOBOX_WIDTH, &TempStr[2], procStack_cnt);
		int width = atoi(TempStr[2]) - 1;

		rbrunGetVar(PVAR_INFOBOX_X, &TempStr[2], procStack_cnt);
		int pos_x = atoi(TempStr[2]) - 1;

		rbrunGetVar(PVAR_INFOBOX_Y, &TempStr[2], procStack_cnt);
		int pos_y = atoi(TempStr[2]) - 1;

		if (pos_x < 0) pos_x = GetSystemMetrics(SM_CXSCREEN)/2 - (width / 2);
		if (pos_y < 0) pos_y = GetSystemMetrics(SM_CYSCREEN)/2 - (height / 2);

		MoveWindow(hInfoBox_Win, pos_x, pos_y, width, height, 1);
		MoveWindow(hInfoBox_Info, 0, 3, width, height, TRUE);

		SendMessage(hInfoBox_Info, WM_SETTEXT,0, (long) TempStr[0]);
		SendMessage(hInfoBox_Win, WM_SETTEXT,0, (long) TempStr[0]);

		if (strcmp(TempStr[1], "-1") == 0
		        || strcmp(TempStr[1], "HIDE") == 0)
		{
			RB_LETVAR(PVAR_INFOBOX_ENABLED, "-1");
		}
		else
		{
			RB_LETVAR(PVAR_INFOBOX_ENABLED, "0");
		}
		return RB_OK;
	}

// -----------------------------------------------------------------------------
// HOTKEYS
// -----------------------------------------------------------------------------
	// NEWHOTKEY -----------------------------------------------------------
	if (strcmp(cmd, "NEWHOTKEY") == 0)
	{
		rbrunGetValue(&TempStr[0]);
		rbrunScanChar(",:");
		rbrunGetValue(&TempStr[1]);
		rbrunScanChar(",");
		rbrunGetValue(&TempStr[2]);

		for (int i=0; i < hotkey_info_cnt; i++)
		{
			if (hotkey_info[i] != (char*)NULL &&
			        strcmp(hotkey_info[i], TempStr[0]) == 0)
			{
				ERRORCODE("-1");
				return RB_OK;
			}
		}

		char ch = TempStr[1][0];
		int keycode = 0;

		if (ch == '~')
		{
			ch = TempStr[1][1];
			switch (ch)
			{
				case 'r':
					keycode = VK_RETURN;
					break;
				case 't':
					keycode = VK_TAB;
					break;
				case 'a':
					keycode = VK_MENU;
					break;
				case 'c':
					keycode = VK_CAPITAL;
					break;
				case 'b':
					keycode = VK_BACK;
					break;

				case 'U':
					keycode = VK_UP;
					break;
				case 'D':
					keycode = VK_DOWN;
					break;
				case 'L':
					keycode = VK_LEFT;
					break;
				case 'R':
					keycode = VK_RIGHT;
					break;

				case '1':
					keycode = VK_F1;
					break;
				case '2':
					keycode = VK_F2;
					break;
				case '3':
					keycode = VK_F3;
					break;
				case '4':
					keycode = VK_F4;
					break;
				case '5':
					keycode = VK_F5;
					break;
				case '6':
					keycode = VK_F6;
					break;
				case '7':
					keycode = VK_F7;
					break;
				case '8':
					keycode = VK_F8;
					break;
				case '9':
					keycode = VK_F9;
					break;
				case 'A':
					keycode = VK_F10;
					break;
				case 'B':
					keycode = VK_F11;
					break;
				case 'C':
					keycode = VK_F12;
					break;
			}
		}

		if (keycode == 0)
		{
			keycode = VkKeyScan(ch);
		}

		int mods = 0;
		if (strcmp(TempStr[2], "0") != 0
		        && atoi(TempStr[2]) == 0)
		{
			char* tok = strtok(TempStr[2], ",+&");
			while (tok != (char*)NULL)
			{
				if (strcmp(strupr(tok), "ALT") == 0)
					mods |= MOD_ALT;
				else if (strcmp(strupr(tok), "CTRL") == 0)
					mods |= MOD_CONTROL;
				else if (strcmp(strupr(tok), "SHIFT") == 0)
					mods |= MOD_SHIFT;
				else if (strcmp(strupr(tok), "WIN") == 0)
					mods |= MOD_WIN;

				tok = strtok((char*)NULL, ",+&");
			}
		}
		else
		{
			int mod_val = atoi(TempStr[2]);

			if (mod_val & 2) mods |= MOD_ALT;
			if (mod_val & 4) mods |= MOD_CONTROL;
			if (mod_val & 8) mods |= MOD_SHIFT;
			if (mod_val & 16) mods |= MOD_WIN;
		}

		if (!RegisterHotKey(hInvisibleWindow, hotkey_info_cnt, mods, keycode))
		{
			ERRORCODE("-1");
			return RB_OK;
		}


		if (hotkey_info_cnt == 0)
		{
			hotkey_info = (char**)malloc(sizeof(char**));
		}
		else
		{
			hotkey_info = (char**)realloc((char**)hotkey_info, (hotkey_info_cnt+1) * sizeof(char*));
		}

		hotkey_info[hotkey_info_cnt] = (char*)malloc((strlen(TempStr[0])+1) * sizeof(char));
		(void)strcpy(hotkey_info[hotkey_info_cnt], TempStr[0]);
		hotkey_info_cnt++;

		ERRORCODE("0");
		return RB_OK;
	}
	// DELHOTKEY -----------------------------------------------------------
	if (strcmp(cmd, "DELHOTKEY") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		if (hotkey_info_cnt == 0)
		{
			ERRORCODE("-1");
			return RB_OK;
		}

		for (int i=0; i < hotkey_info_cnt; i++)
		{
			if (hotkey_info[i] != (char*)NULL &&
			        strcmp(hotkey_info[i], TempStr[0]) == 0)
			{
				UnregisterHotKey(hInvisibleWindow, i);

				free(hotkey_info[i]);
				hotkey_info[i] = (char*)NULL;
				break;
			}
		}

		ERRORCODE("-1");
		return RB_OK;
	}

// -----------------------------------------------------------------------------
// DIALOG EXTENSIONS
// -----------------------------------------------------------------------------
	// NEWDIALOG -----------------------------------------------------------
	if (strcmp(cmd, "RD00") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strtrim(TempStr[1]);
		strupr(TempStr[1]);

		rbrunGetValue(&TempStr[2]);

		bool 	dialog_isParent = false;
		int 	dialog_type;
		HWND 	dialog_handle;

		HWND parent = (HWND)NULL;
		char* parent_win = (char*)NULL;

		//Check if the widget already exits
		for (int a = 0; a < dialog_info_cnt; a++)
		{
			if (dialog_info[a].label != (char*)NULL
			        && strcmp(dialog_info[a].label, TempStr[0]) == 0)
			{
				ERRORCODE("-1");
				return RB_OK;
			}
		}

		//Get the parent window
		for (int a=strlen(TempStr[0]); a >= 0; a--)
		{
			if (TempStr[0][a] == ':')
			{
				parent_win = strdup(TempStr[0]);
#ifdef DEBUG_SESSION
				malloc_cnt++;
#endif

				parent_win[a] = '\0';
				break;
			}
		}

		if (parent_win != (char*)NULL)
		{
			for (int a=0; a < dialog_info_cnt; a++)
			{
				if (dialog_info[a].label != (char*)NULL
				        && strcmp(dialog_info[a].label, parent_win) == 0)
				{
					parent = dialog_info[a].handle;
					break;
				}
			}
		}

		if (parent_win != (char*)NULL)
		{
			FREE(parent_win);
		}

		char* sep = (char*)NULL;
		rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);

		char 	*x = (char*)NULL,
		           *y = (char*)NULL,
		                *width = (char*)NULL,
		                         *height = (char*)NULL;

		x = strtok((char*)TempStr[2], sep);
		y = strtok(NULL, sep);
		width = strtok(NULL, sep);
		height = strtok(NULL, sep);

		FREE(sep);

		if (x == (char*)NULL || y == (char*)NULL
		        || width == (char*)NULL || height == (char*)NULL)
		{
			ERRORCODE("-1");
			return RB_OK;
		}


		//Create new dialog element
		int di_pos = rbDlg_CreateDialogInfo(TempStr[0]);

		//Special case: If there is a group of radio buttons that should
		//be created, and the new widget is not a RADIO-button, open a
		//new group with the next radio button!
		if (!new_group && (strcmp(TempStr[1], "RADIO") != 0
		                   && strcmp(TempStr[1], "7") != 0))
		{
			//VARS(TempStr[1]);
			new_group = true;
		}

		//Perform dialog element creation method based on the type
		if (strcmp(TempStr[1], "BUTTON") == 0
		        || strcmp(TempStr[1], "1") == 0)
		{
			dialog_handle =
			    CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_NOTIFY |WS_TABSTOP | BS_PUSHBUTTON, atoi(x)-1, atoi(y)-1,  atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_BUTTON;
		}
		else if (strcmp(TempStr[1], "IMAGEBUTTON") == 0
		         || strcmp(TempStr[1], "12") == 0)
		{
			dialog_handle =
			    CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_NOTIFY |WS_TABSTOP | BS_PUSHBUTTON | BS_BITMAP, atoi(x)-1, atoi(y)-1,  atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_BUTTON;
		}
		else if (strcmp(TempStr[1], "INPUT") == 0
		         || strcmp(TempStr[1], "2") == 0)
		{
			dialog_handle =
			    CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "", WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_TABSTOP, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_INPUT;
		}
		else if (strcmp(TempStr[1], "EDIT") == 0
		         || strcmp(TempStr[1], "3") == 0)
		{
			dialog_handle =
			    CreateWindowEx (WS_EX_CLIENTEDGE, "edit", "", WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_EDIT;

			SendMessage(dialog_handle, EM_SETLIMITTEXT, -1, 0);
		}
		else if (strcmp(TempStr[1], "EDIT_LINEWRAP") == 0
		         || strcmp(TempStr[1], "4") == 0)
		{
			dialog_handle =
			    CreateWindowEx (WS_EX_CLIENTEDGE, "edit", "", WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_TABSTOP | WS_VSCROLL, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_EDIT;

			SendMessage(dialog_handle, EM_SETLIMITTEXT, -1, 0);
		}
		else if (strcmp(TempStr[1], "LABEL") == 0
		         || strcmp(TempStr[1], "5") == 0)
		{
			dialog_handle =
			    CreateWindow("static", "",WS_CHILD | WS_VISIBLE | SS_NOTIFY, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_LABEL;
		}
		else if (strcmp(TempStr[1], "OPTION") == 0
		         || strcmp(TempStr[1],"6") == 0)
		{
			dialog_handle =
			    CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_NOTIFY |WS_TABSTOP | BS_AUTOCHECKBOX, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_OPTION;
		}
		else if (strcmp(TempStr[1], "RADIO") == 0
		         || strcmp(TempStr[1], "7") == 0)
		{
			if (new_group)
			{
				dialog_handle =
				    CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_NOTIFY |WS_TABSTOP|BS_AUTORADIOBUTTON|WS_GROUP, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			}
			else
			{
				dialog_handle =
				    CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_NOTIFY |WS_TABSTOP|BS_AUTORADIOBUTTON, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			}

			dialog_type = WIN_RADIO;

			new_group = false;
		}
		else if (strcmp(TempStr[1], "LIST") == 0
		         || strcmp(TempStr[1], "8") == 0)
		{
			dialog_handle =
			    CreateWindowEx(WS_EX_CLIENTEDGE,"LISTBOX", "",WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_LIST;
			//LBS_NOTIFY
		}
		else if (strcmp(TempStr[1], "LIST_SORTED") == 0
		         || strcmp(TempStr[1], "9") == 0)
		{
			dialog_handle =
			    CreateWindowEx(WS_EX_CLIENTEDGE,"LISTBOX", "",WS_CHILD | LBS_SORT | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_LIST;
		}
		else if (strcmp(TempStr[1], "LIST_MULTI") == 0
		         || strcmp(TempStr[1], "10") == 0)
		{
			dialog_handle =
			    CreateWindowEx(WS_EX_CLIENTEDGE,"LISTBOX", "",WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_MULTIPLESEL, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_MULTILIST;
		}
		else if (strcmp(TempStr[1], "LIST_MULTISORTED") == 0
		         || strcmp(TempStr[1], "11") == 0)
		{
			dialog_handle =
			    CreateWindowEx(WS_EX_CLIENTEDGE,"LISTBOX", "",WS_CHILD | LBS_SORT | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_MULTIPLESEL, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_MULTILIST;
		}
		else if (strcmp(TempStr[1], "COMBO") == 0
		         || strcmp(TempStr[1], "12") == 0)
		{
			dialog_handle =
			    CreateWindowEx(WS_EX_CLIENTEDGE,"COMBOBOX", "", WS_CHILD | WS_VISIBLE| CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP| CBS_NOINTEGRALHEIGHT, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_COMBO;
		}
		else if (strcmp(TempStr[1], "STATIC_COMBO") == 0
		         || strcmp(TempStr[1], "13") == 0)
		{
			dialog_handle =
			    CreateWindowEx(WS_EX_CLIENTEDGE,"COMBOBOX", "", WS_CHILD | WS_VISIBLE| CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP| CBS_NOINTEGRALHEIGHT, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_STATIC_COMBO;
		}
		else if (strcmp(TempStr[1], "GROUP") == 0
		         || strcmp(TempStr[1], "14") == 0)
		{
			dialog_handle =
			    CreateWindow("button", "",WS_CHILD | WS_VISIBLE | ES_LEFT | BS_GROUPBOX, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU) (di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_GROUP;
		}
		else if (strcmp(TempStr[1], "IMAGE") == 0
		         || strcmp(TempStr[1], "15") == 0)
		{
			dialog_handle =
			    CreateWindow("static", "",WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU) (di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_IMAGE;
		}
		else if (strcmp(TempStr[1], "PWD") == 0
		         || strcmp(TempStr[1], "16") == 0)
		{
			dialog_handle =
			    CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "", WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_TABSTOP | ES_PASSWORD, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_PWD;
		}
		else if (strcmp(TempStr[1], "PROGRESS") == 0
		         || strcmp(TempStr[1], "17") == 0)
		{
			dialog_handle =
			    CreateWindowEx(WS_EX_CLIENTEDGE, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, parent, (HMENU)(di_pos + DIALOG_ID), hInstance, NULL);
			dialog_type = WIN_PROGRESS;
		}

		else //as default, create a dialog
		{
			dialog_handle =
			    CreateWindowEx(0, szClassName, "", WS_OVERLAPPEDWINDOW, atoi(x)-1, atoi(y)-1,  atoi(width)-1, atoi(height)-1 + devpix, HWND_DESKTOP, NULL, hInstance, NULL);
			dialog_isParent = true;
			dialog_type = WIN_DIALOG;
		}


		if (!dialog_isParent)
		{
			SendMessage(dialog_handle, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		}

		//Fill the rest information into dialog_info structure
		dialog_info[di_pos].isParent = dialog_isParent;
		dialog_info[di_pos].handle = dialog_handle;
		dialog_info[di_pos].window_type = dialog_type;

		//Set default events
		switch (dialog_type)
		{
			case WIN_DIALOG:
				//RB_DLG_LETEVENT(di_pos, DLGEVENT_OK, true);
				//RB_DLG_LETEVENT(di_pos, DLGEVENT_CANCEL, true);
				RB_DLG_LETEVENT(di_pos, DLGEVENT_WINDOWCLOSE, true);
				//RB_DLG_LETEVENT(di_pos, DLGEVENT_WINDOWSIZE, true);
				//RB_DLG_LETEVENT(di_pos, DLGEVENT_MOVE, true);
				break;
			case WIN_BUTTON:
			case WIN_OPTION:
			case WIN_RADIO:
				RB_DLG_LETEVENT(di_pos, DLGEVENT_CLICK, true);
				break;

		}

		ERRORCODE("0");

		return RB_OK;
	}
	// LETDIALOG -----------------------------------------------------------
	if (strcmp(cmd, "RD01") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		strtrim(TempStr[1]);
		strupr(TempStr[1]);

		rbrunGetValue(&TempStr[2]);

		if (dialog_info_cnt == 0)
		{
			ERRORCODE("-1");
			return RB_OK;
		}

		HWND handle = (HWND)NULL;
		int handle_pos = -1;

		for (int a = 0; a < dialog_info_cnt; a++)
		{
			if (dialog_info[a].label != (char*)NULL
			        && strcmp(dialog_info[a].label, TempStr[0]) == 0)
			{
				handle = dialog_info[a].handle;
				handle_pos = a;
				break;
			}
		}

		ERRORCODE("-1");
		if (handle == (HWND)NULL)
		{
			return RB_OK;
		}

		if (strcmp(TempStr[1], "CAPTION") == 0
		        || strcmp(TempStr[1], "TEXT") == 0
		        || strcmp(TempStr[1], "SELECTION") == 0)
		{
			//Replace LF with CR/LF
			strreplace(&TempStr[2], "\r", "", true);
			strreplace(&TempStr[2], "\n", "\r\n", true);

			if (strcmp(TempStr[1], "SELECTION") == 0)
			{
				if (dialog_info[handle_pos].window_type == WIN_LIST)
				{
					SendMessage(handle, LB_SELECTSTRING, 0, (LPARAM) TempStr[2]);
					/*
					int sel = SendMessage(handle, LB_GETCURSEL, 0, 0);

					if(sel != LB_ERR)
					{
						if(SendMessage( handle, LB_SETITEMDATA, (WPARAM)sel, (LPARAM) TempStr[2]) != LB_ERR)
						{
							UpdateWindow(handle);
							ERRORCODE_0_AND_RB_OK;
						}
					}
					return RB_OK;
					*/
				}
				else if (dialog_info[handle_pos].window_type == WIN_STATIC_COMBO)
				{
					SendMessage(handle, CB_SELECTSTRING, 0, (LPARAM) TempStr[2]);
				}
				else if (dialog_info[handle_pos].window_type == WIN_MULTILIST)
				{
					int item_cnt = 0;

					//Retrieving number of items
					item_cnt = SendMessage(handle, LB_GETCOUNT, 0, 0);

					rbrunGetVar(PVAR_STD_SEP, &TempStr[3], procStack_cnt);
					char* item = strtoken(TempStr[2], TempStr[3]);

					while (item != (char*)NULL)
					{
						for (int a = 0; a < item_cnt; a++)
						{
							int len = 0;

							len = SendMessage(handle, LB_GETTEXTLEN, a, 0 );

							char* textbuf = (char*)MALLOC((len+1)*sizeof(char));

							SendMessage(handle, LB_GETTEXT, a, (long)textbuf);
							if ( strcmp( textbuf, item ) == 0 )
							{
								SendMessage( handle, LB_SETSEL, (WPARAM)TRUE, (LPARAM) a );
								FREE(textbuf);
								break;
							}

							FREE(textbuf);
						}
						item = strtoken(NULL, TempStr[3]);
					}
				}
				else
				{
					SendMessage(handle, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)TempStr[2]);
				}
			}
			else
			{
				if (RB_WINDOW_TYPE(handle_pos, WIN_MENU))
				{
					ModifyMenu( GetMenu( handle ), handle_pos + DIALOG_ID, MF_BYCOMMAND | MF_STRING,
					            handle_pos + DIALOG_ID, (LPCTSTR) TempStr[2] );
				}
				else
				{
					SendMessage(handle, WM_SETTEXT, 0,(long) TempStr[2]);
				}
			}

			UpdateWindow(handle);
			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "VISIBLE") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			if (strcmp(TempStr[2], "-1") == 0)
			{
				ShowWindow(handle, SW_HIDE);
			}
			else
			{
				ShowWindow(handle, SW_SHOW);
			}

			UpdateWindow(handle);
			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "ENABLED") == 0)
		{
			if (RB_WINDOW_TYPE(handle_pos, WIN_MENU))
			{
				if (strcmp(TempStr[2], "-1") == 0)
				{
					EnableMenuItem(GetMenu(handle), handle_pos + DIALOG_ID, MF_GRAYED);
				}
				else
				{
					EnableMenuItem(GetMenu(handle), handle_pos + DIALOG_ID, MF_ENABLED);
				}
				DrawMenuBar(handle);
			}
			else
			{
				if (strcmp(TempStr[2], "-1") == 0)
				{
					EnableWindow(handle, FALSE);
				}
				else
				{
					EnableWindow(handle, TRUE);
				}
			}

			UpdateWindow(handle);
			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "CHECKED") == 0)
		{
			if (RB_WINDOW_TYPE(handle_pos, WIN_MENU))
			{
				if (strcmp(TempStr[2], "-1") == 0)
				{
					CheckMenuItem(GetMenu(handle), handle_pos + DIALOG_ID, MF_UNCHECKED);
				}
				else
				{
					CheckMenuItem(GetMenu(handle), handle_pos + DIALOG_ID, MF_CHECKED);
				}
				DrawMenuBar(handle);
			}
			else
			{
				if (strcmp(TempStr[2], "-1") == 0)
				{
					SendMessage(handle, BM_SETCHECK, BST_UNCHECKED, 0);
				}
				else
				{
					SendMessage(handle, BM_SETCHECK, BST_CHECKED, 0);
				}
			}

			UpdateWindow(handle);
			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "READONLY") == 0)
		{
			/*
			if(RB_WINDOW_TYPE(handle_pos, WIN_COMBO))
			{
				SetWindowLong(handle, GWL_STYLE, NULL);

				if(strcmp(TempStr[2], "-1") == 0)
				{
					SetWindowLong(handle, GWL_STYLE, WS_CHILD | WS_VISIBLE| CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP | CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT);

				}
				else
				{
					SetWindowLong(handle, GWL_STYLE, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT | CBS_SIMPLE);
				}
			}
			else
			{
			*/
			if (strcmp(TempStr[2], "-1") == 0)
			{
				SendMessage(handle, EM_SETREADONLY, FALSE, NULL);
			}
			else
			{
				SendMessage(handle, EM_SETREADONLY, TRUE, NULL);
			}

			UpdateWindow(handle);
			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "SELINDEX") == 0)
		{
			if (RB_WINDOW_TYPE(handle_pos, WIN_LIST))
			{
				if (SendMessage(handle, LB_SETCURSEL, atoi(TempStr[2])-1, 0) !=  LB_ERR)
				{
					ERRORCODE_0_AND_RB_OK;
				}
			}
			else if (RB_WINDOW_TYPE(handle_pos, WIN_MULTILIST))
			{
				rbrunGetVar(PVAR_STD_SEP, &TempStr[3], procStack_cnt);
				char* item = strtoken(TempStr[2], TempStr[3]);

				while (item != (char*)NULL)
				{
					SendMessage(handle, LB_SETSEL, (WPARAM)TRUE, (LPARAM)atoi(item));
					item = strtoken(NULL, TempStr[3]);
				}
				ERRORCODE_0_AND_RB_OK;
			}
			return RB_OK;
		}
		/*
		else if(strcmp(TempStr[1], "SCROLLBARS") == 0)
		{
			if(RB_WINDOW_TYPE(handle_pos, WIN_EDIT))
			{
				DWORD styles = 0;
				styles = GetWindowLong(handle, GWL_STYLE);

				DWORD dw = GetWindowLong(handle ,GWL_STYLE);
				dw = dw & ~WS_VSCROLL & ~WS_HSCROLL & ~ES_AUTOVSCROLL & ~ES_AUTOHSCROLL;

				SetWindowLong( handle, GWL_STYLE, dw);
				InvalidateRect( handle, NULL, TRUE);

				UpdateWindow(handle);
				SetWindowPos(handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
				ERRORCODE_0_AND_RB_OK;
			}
			return RB_OK;
		}
		*/
		else if (strcmp(TempStr[1], "FOCUS") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			SetFocus(handle);

			UpdateWindow(handle);
			ERRORCODE_0_AND_RB_OK;
		}
		/* <TESTWEISE> */
		else if (strcmp(TempStr[1], "FOCUS_NEXT") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			SendMessage(GetFocus(), WM_NEXTDLGCTL, 0, FALSE);

			UpdateWindow(GetFocus());
			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "FOCUS_PREV") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			SendMessage(GetFocus(), WM_NEXTDLGCTL, 1, FALSE);

			UpdateWindow(GetFocus());
			ERRORCODE_0_AND_RB_OK;
		}
		/* </TESTWEISE> */
		else if (strcmp(TempStr[1], "TOOLTIP") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			DIALOG_EXTINFO* tt_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_TOOLTIP);
			if (tt_info == (DIALOG_EXTINFO*)NULL)
			{
				tt_info = rbDlg_CreateDialogExtInfo(handle_pos);

				//Set the extinfo-type
				tt_info->ext_type = DIALOG_EXTINFO_TOOLTIP;

				//Create the tooltip
				TOOLINFO* ti = (TOOLINFO*)malloc(sizeof(TOOLINFO));
				memset(ti, 0, sizeof(TOOLINFO));

				ti->cbSize = sizeof(TOOLINFO);
				ti->uFlags = TTF_SUBCLASS | TTF_IDISHWND;
				ti->hwnd = handle;
				ti->hinst = NULL;
				ti->uId = (unsigned int) handle;

				//(HWND)((*tt_info).ext_handle)  	(HWND)(ti->uId)
				(HWND)((*tt_info).ext_handle) = CreateWindowEx(WS_EX_TOPMOST,
				                                TOOLTIPS_CLASS, NULL,  WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
				                                NULL, NULL, NULL, NULL, handle,	NULL, hInstance, NULL);

				SetWindowPos((HWND)((*tt_info).ext_handle), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

				/*
				RECT WinSize;

				GetWindowRect(handle, &WinSize);
				ti->rect.left = 0;
				ti->rect.top = 0;
				ti->rect.right = WinSize.right;
				ti->rect.bottom = WinSize.bottom;
				*/
				ti->lpszText = strdup( TempStr[2] );

				SendMessage((HWND)((*tt_info).ext_handle), TTM_ADDTOOL, 0, (LPARAM) ti);
				SendMessage((HWND)((*tt_info).ext_handle), TTM_SETDELAYTIME, TTDT_AUTOMATIC, 2000 );

				tt_info->ext_data = ti;
			}
			else
			{
				if (strcmp(TempStr[2], "") != 0)
				{
					//Update the tooltip's text
					TOOLINFO *ti = (TOOLINFO*)tt_info->ext_data;

					if (ti->lpszText != (char*)NULL)
						free(ti->lpszText);

					ti->lpszText = strdup( TempStr[2] );
					SendMessage((HWND)((*tt_info).ext_handle), TTM_SETTOOLINFO, 0, (LPARAM) ti);
				}
				else
				{
					TOOLINFO *ti = (TOOLINFO*)tt_info->ext_data;

					if (ti->lpszText != (char*)NULL)
						free(ti->lpszText);

					//Remove the tooltip from the window
					SendMessage(handle, TTM_DELTOOL, 0, (LPARAM) ti);


					DestroyWindow((HWND)((*tt_info).ext_handle));
					tt_info->ext_handle = (void*)NULL;

					rbDlg_DeleteDialogExtInfo(handle_pos, DIALOG_EXTINFO_TOOLTIP);
				}
			}

			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "STATUSBAR") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			if (dialog_info[handle_pos].window_type != WIN_DIALOG)
				return RB_OK;

			DIALOG_EXTINFO* statusbar = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_STATUSBAR);

			//Create the window if it does not exist
			if (statusbar == (DIALOG_EXTINFO*)NULL)
			{
				statusbar = rbDlg_CreateDialogExtInfo(handle_pos);

				//Set the extinfo-type
				statusbar->ext_type = DIALOG_EXTINFO_STATUSBAR;

				//Create the statusbar
				(HWND)((*statusbar).ext_handle) = CreateWindowEx(0, STATUSCLASSNAME, "", WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP,
				                                  0, 0, 0, 0, handle, NULL, hInstance, NULL);

			}

			//Take action/fill data
			if (statusbar != (DIALOG_EXTINFO*)NULL)
			{
				if (strcmp(TempStr[2], "") != 0)
				{
					//Copy statusbar defintion to extinfo
					if (statusbar->ext_data != (void*)NULL)
						free(statusbar->ext_data);

					statusbar->ext_data = strdup(TempStr[2]);

					//Set as default styled statusbar
					SendMessage((HWND)((*statusbar).ext_handle), SB_SIMPLE, 0, 0);

					rbrunGetVar(PVAR_STD_SEP, &TempStr[3], procStack_cnt);

					char* sb_part = (char*)NULL;
					char** sb_texts = (char**)NULL;
					int* sb_size = (int*)NULL;
					int sb_cnt = 0;
					bool is_size = false;

					sb_part = strtok(TempStr[2], TempStr[3]);
					while (sb_part != (char*)NULL)
					{
						if (!is_size)
						{
							if (sb_cnt == 0)
							{
								sb_texts = (char**)malloc(sizeof(char*));
							}
							else
							{
								sb_texts = (char**)realloc((char**)sb_texts, (sb_cnt+1) * sizeof(char*));
							}
							sb_texts[sb_cnt] = sb_part;

							is_size = true;
						}
						else
						{
							if (sb_cnt == 0)
							{
								sb_size = (int*)malloc(sizeof(int));
							}
							else
							{
								sb_size = (int*)realloc((int*)sb_size, (sb_cnt+1) * sizeof(int));
							}
							sb_size[sb_cnt] = atoi(sb_part);
							sb_cnt++;
							is_size = false;
						}
						sb_part = strtok(NULL, TempStr[3]);
					}

					//Set the parts
					if (sb_size != (int*)NULL)
					{
						SendMessage ((HWND)((*statusbar).ext_handle), SB_SETPARTS, sb_cnt, (LPARAM)sb_size);
						free(sb_size);
					}

					//Fill the blocks with text
					if (sb_texts != (char**)NULL)
					{
						if (sb_cnt > 0)
						{
							for (int a=0;a<sb_cnt;a++)
							{
								SendMessage ((HWND)((*statusbar).ext_handle), SB_SETTEXT , a, (LPARAM)sb_texts[a]);
							}
						}
						else
						{
							int size = GetSystemMetrics(SM_CXFULLSCREEN);
							SendMessage((HWND)((*statusbar).ext_handle), SB_SETPARTS, 1, (LPARAM)&size);

							SendMessage((HWND)((*statusbar).ext_handle), SB_SETTEXT , 0, (LPARAM)*sb_texts);
						}
						free(sb_texts);
					}
				}
				else
				{
					//Destroy the statusbar
					DestroyWindow((HWND)((*statusbar).ext_handle));
					statusbar->ext_handle = (void*)NULL;

					//Delete the other information
					rbDlg_DeleteDialogExtInfo(handle_pos, DIALOG_EXTINFO_STATUSBAR);
				}
			}

			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "IMAGE") == 0
		         || strcmp(TempStr[1], "IMAGE_TRANSPARENT") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			if (dialog_info[handle_pos].window_type == WIN_DIALOG)
				return RB_OK;

			HBITMAP image = (HBITMAP)NULL;
			int image_type;

			DIALOG_EXTINFO* image_info = (DIALOG_EXTINFO*)NULL;
			if (strcmp(TempStr[1], "IMAGE_TRANSPARENT") == 0)
			{
				image_type = DIALOG_EXTINFO_IMAGE_TRANS;
			}
			else
			{
				image_type = DIALOG_EXTINFO_IMAGE;
			}

			image_info = rbDlg_GetDialogExtInfo(handle_pos, image_type);

			if (strcmp(TempStr[2], "") != 0)
			{
				if (strcmp(TempStr[1], "IMAGE_TRANSPARENT") == 0)
				{
					image = (HBITMAP)LoadImage(NULL,TempStr[2],IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_SHARED | LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS | LR_DEFAULTSIZE);
				}
				else
				{
					image = (HBITMAP)LoadImage(NULL,TempStr[2],IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_SHARED | LR_DEFAULTSIZE);
				}


				if (image != (HBITMAP)NULL)
				{
					//First get position of handle
					RECT WinSize;
					GetWindowRect(handle, &WinSize);
					ScreenToClient(GetParent(handle), (POINT*)&WinSize.left);
					ScreenToClient(GetParent(handle), (POINT*)&WinSize.right);

					//Set the image
					if (dialog_info[handle_pos].window_type == WIN_BUTTON)
					{
						SendMessage(handle, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)image);
					}
					else
					{
						SendMessage(handle, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)image);
					}

					MoveWindow(handle, WinSize.left, WinSize.top, WinSize.right - WinSize.left, WinSize.bottom - WinSize.top, TRUE);
					//SetWindowPos(handle, HWND_TOP, WinSize.left, WinSize.top, WinSize.right, WinSize.bottom, SWP_FRAMECHANGED );
					UpdateWindow(handle);
					InvalidateRect(handle, NULL, TRUE);

					if (image_info == (DIALOG_EXTINFO*)NULL)
					{
						image_info = rbDlg_CreateDialogExtInfo(handle_pos);

						//Set the extinfo-type
						image_info->ext_type = image_type;

						//Set the bitmap handle
						image_info->ext_handle = (HBITMAP*) malloc(sizeof(HBITMAP));
						memcpy(image_info->ext_handle, &image, sizeof(HBITMAP));

						//Set the bitmap filename
						image_info->ext_data = strdup(TempStr[2]);
					}
					else
					{
						//Reset the bitmap handle
						if (image_info->ext_handle != (void*)NULL)
							DeleteObject(image_info->ext_handle);

						(HBITMAP)(image_info->ext_handle) = image;

						//Reset the bitmap filename
						if (image_info->ext_data != (void*)NULL)
							free(image_info->ext_data);

						image_info->ext_data = strdup(TempStr[2]);
					}
				}
				else
				{
					if (image_info != (DIALOG_EXTINFO*)NULL)
					{
						if (image_info->ext_handle != (void*)NULL)
							DeleteObject(image_info->ext_handle);
					}

					SendMessage(handle, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)NULL);
					return RB_OK;
				}
			}
			else
			{
				if (image_info != (DIALOG_EXTINFO*)NULL)
				{
					if (image_info->ext_handle != (void*)NULL)
						DeleteObject(image_info->ext_handle);

					SendMessage(handle, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)NULL);
					rbDlg_DeleteDialogExtInfo(handle_pos, image_type);
				}
			}

			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "ICON") == 0)
		{
			if (dialog_info[handle_pos].window_type == WIN_DIALOG)
			{
				DIALOG_EXTINFO* icon_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_ICON);

				if (strcmp( TempStr[2], "" ) != 0)
				{
					HICON dialog_icon;
					dialog_icon = (HICON)LoadImage(hInstance, TempStr[2], IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
					if (dialog_icon == (HICON)NULL)
					{
						*TempStr[2] = '\0';
					}
					else
					{
						SendMessage(handle, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)dialog_icon);

						//Insert/Update Attribute in Extended Dialog Information
						if (icon_info == (DIALOG_EXTINFO*)NULL)
						{
							icon_info = rbDlg_CreateDialogExtInfo(handle_pos);

							//Set the extinfo-type
							icon_info->ext_type = DIALOG_EXTINFO_ICON;

							//Set the bitmap handle
							icon_info->ext_handle = (HICON*) malloc(sizeof(HICON));
							memcpy(icon_info->ext_handle, &dialog_icon, sizeof(HICON));

							//Set the bitmap filename
							icon_info->ext_data = strdup(TempStr[2]);
						}
						else
						{
							//Reset the bitmap handle
							if (icon_info->ext_handle != (void*)NULL)
								DeleteObject(icon_info->ext_handle);

							(HICON)(icon_info->ext_handle) = dialog_icon;

							//Reset the bitmap filename
							if (icon_info->ext_data != (void*)NULL)
								free(icon_info->ext_data);

							icon_info->ext_data = strdup(TempStr[2]);
						}
					}
				}


				if (strcmp( TempStr[2], "" ) == 0)
				{
					if (icon_info != (DIALOG_EXTINFO*)NULL)
					{
						if (icon_info->ext_handle != (void*)NULL)
						{
							SendMessage(handle, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)NULL);
							DeleteObject(icon_info->ext_handle);
						}
						rbDlg_DeleteDialogExtInfo(handle_pos, DIALOG_EXTINFO_ICON);
					}
				}


				ERRORCODE("0");
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[1], "MENU") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			TempStr[3] = strdup(TempStr[2]);

			if (rbDlg_CreateMenu(handle, TempStr[0], TempStr[2]))
			{
				DIALOG_EXTINFO* menu_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_MENU);
				if (strcmp(TempStr[3], "") != 0)
				{
					//Insert/Update Attribute in Extended Dialog Information
					if (menu_info == (DIALOG_EXTINFO*)NULL)
					{
						menu_info = rbDlg_CreateDialogExtInfo(handle_pos);

						//Set the extinfo-type
						menu_info->ext_type = DIALOG_EXTINFO_MENU;
					}
					else
					{
						if (menu_info->ext_data != (void*)NULL)
							free( menu_info->ext_data );

					}

					//Set the menu descriptor
					menu_info->ext_data = strdup(TempStr[3]);
				}
				else
				{
					rbDlg_DeleteDialogExtInfo(handle_pos, DIALOG_EXTINFO_MENU);
				}

				ERRORCODE_0_AND_RB_OK;
			}
		}
		else if (strcmp(TempStr[1], "STYLE") == 0)
		{
			if (dialog_info[handle_pos].window_type == WIN_DIALOG)
			{
				strupr(TempStr[2]);
				SetWindowLong(handle, GWL_EXSTYLE, NULL );

				bool save_extended_info = true;


				if (strcmp(TempStr[2], "DIALOG") == 0
				        || strcmp(TempStr[2], "0") == 0)
				{
					SetWindowLong(handle, GWL_STYLE, WS_CAPTION | WS_SYSMENU );
				}
				else if (strcmp(TempStr[2], "SINGLE") == 0
				         || strcmp(TempStr[2], "1") == 0)
				{
					SetWindowLong(handle, GWL_STYLE, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU );
				}
				else if (strcmp(TempStr[2], "TOOL") == 0
				         || strcmp(TempStr[2], "2") == 0)
				{
					SetWindowLong(handle, GWL_EXSTYLE, WS_EX_TOOLWINDOW );
					SetWindowLong(handle, GWL_STYLE, WS_OVERLAPPEDWINDOW );
				}
				else if (strcmp(TempStr[2], "STATIC_TOOL") == 0
				         || strcmp(TempStr[2], "3") == 0)
				{
					SetWindowLong(handle, GWL_EXSTYLE, WS_EX_PALETTEWINDOW );
					SetWindowLong(handle, GWL_STYLE, WS_CAPTION | WS_SYSMENU );
				}
				else
				{
					SetWindowLong(handle, GWL_STYLE, WS_OVERLAPPEDWINDOW );
					save_extended_info = false;
				}

				SetWindowPos(handle, HWND_TOP, 0, 0, 0, 0,
				             SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER );

				UpdateWindow(handle);

				if (IsWindowVisible(GetParent(handle)))
				{
					ShowWindow(handle, SW_SHOW);
				}

				DIALOG_EXTINFO* style_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_STYLE);
				if (save_extended_info)
				{
					//Insert/Update Attribute in Extended Dialog Information
					if (style_info == (DIALOG_EXTINFO*)NULL)
					{
						style_info = rbDlg_CreateDialogExtInfo(handle_pos);

						//Set the extinfo-type
						style_info->ext_type = DIALOG_EXTINFO_STYLE;
					}
					else
					{
						if (style_info->ext_data != (void*)NULL)
							free( style_info->ext_data );

					}

					//Set the current style name
					style_info->ext_data = strdup(TempStr[2]);
				}
				else
				{
					rbDlg_DeleteDialogExtInfo(handle_pos, DIALOG_EXTINFO_STYLE);
				}

				ERRORCODE("0");
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[1], "ORDER") == 0)
		{
			if (dialog_info[handle_pos].window_type == WIN_DIALOG)
			{
				strupr(TempStr[2]);
				HWND insertafter = (HWND)NULL;

				if (strcmp(TempStr[2], "BOTTOM") == 0
				        || strcmp(TempStr[2], "1") == 0)
				{
					insertafter = HWND_BOTTOM;
				}
				else if (strcmp(TempStr[2], "TOP") == 0
				         || strcmp(TempStr[2], "2") == 0)
				{
					insertafter = HWND_TOPMOST;
				}
				else
				{
					SetWindowPos(handle, HWND_NOTOPMOST, 0, 0, 0, 0,
					             SWP_NOMOVE | SWP_NOSIZE );
					insertafter = HWND_TOP;
					strcpy( TempStr[2], "" );
				}

				SetWindowPos(handle, insertafter, 0, 0, 0, 0,
				             SWP_NOMOVE | SWP_NOSIZE );

				/*
				                            UpdateWindow(handle);

				if(IsWindowVisible(GetParent(handle)))
				{
					ShowWindow(handle, SW_SHOW);
				}
				                            */

				DIALOG_EXTINFO* order_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_ORDER);

				//Insert/Update Attribute in Extended Dialog Information
				if (order_info == (DIALOG_EXTINFO*)NULL)
				{
					order_info = rbDlg_CreateDialogExtInfo(handle_pos);

					//Set the extinfo-type
					order_info->ext_type = DIALOG_EXTINFO_ORDER;
				}
				else
				{
					if (order_info->ext_data != (void*)NULL)
						free( order_info->ext_data );

				}

				//Set the current order type
				order_info->ext_data = strdup(TempStr[2]);

				ERRORCODE("0");
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[1], "ANIMATION") == 0)
		{
			if (dialog_info[handle_pos].window_type == WIN_ANIMATION)
			{
				if ( Animate_Open( handle, TempStr[2] ) )
				{
					ShowWindow( handle, SW_SHOW );
					DIALOG_EXTINFO* anim_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_ANIMATION);

					//Insert/Update Attribute in Extended Dialog Information
					if (anim_info == (DIALOG_EXTINFO*)NULL)
					{
						anim_info = rbDlg_CreateDialogExtInfo(handle_pos);

						//Set the extinfo-type
						anim_info->ext_type = DIALOG_EXTINFO_ANIMATION;
					}
					else
					{
						if (anim_info->ext_data != (void*)NULL)
							free( anim_info->ext_data );
					}

					//Set the current order type
					anim_info->ext_data = strdup(TempStr[2]);

					ERRORCODE("0");
				}
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[1], "PROGRESS") == 0)
		{
			if (dialog_info[handle_pos].window_type == WIN_PROGRESS)
			{
				SendMessage( handle, PBM_SETPOS, atoi( TempStr[2] ), 0 );
				ERRORCODE("0");
			}
		}
		/*
		else if(strcmp(TempStr[1], "COLOR") == 0)
		{
		        dialog_info[handle_pos].hBrush = (HBRUSH)CreateSolidBrush( RGB( 0, 0, 128 ) );


		        RECT WinSize;
		        GetWindowRect(handle, &WinSize);
		        ScreenToClient(GetParent(handle), (POINT*)&WinSize.left);
		        ScreenToClient(GetParent(handle), (POINT*)&WinSize.right);

		        MoveWindow(handle, WinSize.left, WinSize.top, WinSize.right - WinSize.left, WinSize.bottom - WinSize.top, TRUE);
		        
		        UpdateWindow(handle);
		        InvalidateRect(handle, NULL, TRUE);

		        return RB_OK;
		}
		*/
		else if (strncmp(TempStr[1], "EVENT_", 6) == 0)
		{
			bool set_value = false;
			if (strcmp(TempStr[2], "0") == 0) set_value = true;

			if (strncmp(TempStr[1]+6, "CLICK", 5) == 0)
			{
				RB_DLG_LETEVENT(handle_pos, DLGEVENT_CLICK, set_value);
			}
			else if (strncmp(TempStr[1]+6, "DBLCLICK", 8) == 0)
			{
				RB_DLG_LETEVENT(handle_pos, DLGEVENT_DBLCLICK, set_value);
			}
			else if (strncmp(TempStr[1]+6, "SIZE", 4) == 0)
			{
				RB_DLG_LETEVENT(handle_pos, DLGEVENT_WINDOWSIZE, set_value);
			}
			else if (strncmp(TempStr[1]+6, "CLOSE", 5) == 0)
			{
				RB_DLG_LETEVENT(handle_pos, DLGEVENT_WINDOWCLOSE, set_value);
			}
			else if (strncmp(TempStr[1]+6, "CHANGE", 6) == 0)
			{
				RB_DLG_LETEVENT(handle_pos, DLGEVENT_CHANGE, set_value);
			}
			else if (strncmp(TempStr[1]+6, "OK", 2) == 0)
			{
				RB_DLG_LETEVENT(handle_pos, DLGEVENT_OK, set_value);
			}
			else if (strncmp(TempStr[1]+6, "CANCEL", 6) == 0)
			{
				RB_DLG_LETEVENT(handle_pos, DLGEVENT_CANCEL, set_value);
			}
			else if (strncmp(TempStr[1]+6, "MOVE", 4) == 0)
			{
				RB_DLG_LETEVENT(handle_pos, DLGEVENT_MOVE, set_value);
			}
			else if (strncmp(TempStr[1]+6, "GETFOCUS", 8) == 0)
			{
				RB_DLG_LETEVENT(handle_pos, DLGEVENT_GETFOCUS, set_value);
			}
			else if (strncmp(TempStr[1]+6, "LEAVEFOCUS", 10) == 0)
			{
				RB_DLG_LETEVENT(handle_pos, DLGEVENT_LEAVEFOCUS, set_value);
			}
			else if (strncmp(TempStr[1]+6, "MOUSEMOVE", 9) == 0)
			{
				RB_DLG_LETEVENT(handle_pos, DLGEVENT_MOUSEMOVE, set_value);
			}
			else
			{
				return RB_OK;
			}

			ERRORCODE_0_AND_RB_OK;
		}

		RB_DLG_NOMENU(handle_pos);

		rbrunGetVar(PVAR_STD_SEP, &TempStr[3], procStack_cnt);

		if (strcmp(TempStr[1], "POSITION") == 0)
		{
			char 	*x = (char*)NULL,
			           *y = (char*)NULL,
			                *width = (char*)NULL,
			                         *height = (char*)NULL;

			x = strtok(TempStr[2], TempStr[3]);
			y = strtok(NULL, TempStr[3]);
			width = strtok(NULL, TempStr[3]);
			height = strtok(NULL, TempStr[3]);

			if (x == (char*)NULL || y == (char*)NULL
			        || width == (char*)NULL || height == (char*)NULL)
			{
				ERRORCODE("-1");
				return RB_OK;
			}

			MoveWindow(handle, atoi(x)-1, atoi(y)-1, atoi(width)-1, atoi(height)-1, true);
			InvalidateRect(handle, NULL, TRUE);
			UpdateWindow(handle);

			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "ITEMS") == 0)
		{
			char* tmp_text = (char*)NULL;

			//Reset existing content
			if ( RB_WINDOW_TYPE(handle_pos, WIN_LIST) || RB_WINDOW_TYPE(handle_pos, WIN_MULTILIST) )
				SendMessage(handle, LB_RESETCONTENT, 0, 0);

			if (dialog_info[handle_pos].window_type == WIN_COMBO
			        || dialog_info[handle_pos].window_type == WIN_STATIC_COMBO)
			{
				int len = SendMessage(handle, WM_GETTEXTLENGTH, 0, 0 );
				if (len > 0)
				{
					tmp_text = (char*)MALLOC((len+1)*sizeof(char));
					SendMessage(handle, WM_GETTEXT, len+1, (long) tmp_text );
				}

				SendMessage(handle, CB_RESETCONTENT, 0, 0);
			}

			//Insert items
			char* item = strtoken(TempStr[2], TempStr[3]);

			while (item != (char*)NULL)
			{
				if (RB_WINDOW_TYPE(handle_pos, WIN_LIST) || RB_WINDOW_TYPE(handle_pos, WIN_MULTILIST))
					SendMessage(handle, LB_ADDSTRING, NULL, (LPARAM)item);
				else if (RB_WINDOW_TYPE(handle_pos, WIN_COMBO) ||
				         RB_WINDOW_TYPE(handle_pos, WIN_STATIC_COMBO))
					SendMessage(handle, CB_ADDSTRING, NULL, (LPARAM)item);

				item = strtoken(NULL, TempStr[3]);
			}
			UpdateWindow(handle);

			if ( (dialog_info[handle_pos].window_type == WIN_COMBO
			        || dialog_info[handle_pos].window_type == WIN_STATIC_COMBO ) && tmp_text != (char*)NULL)
			{
				SendMessage(handle, WM_SETTEXT, 0, (long) tmp_text);
				FREE(tmp_text);
			}

			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "FONT") == 0)
		{
			DIALOG_EXTINFO* font_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_FONT);

			if ( strcmp( TempStr[2], "" ) != 0 )
			{
				bool f_bold, f_italic, f_underline, f_strikeout;
				int f_weight, f_style;
				char* font = (char*)NULL;
				char* size = (char*)NULL;
				char* style = (char*)NULL;

				//Insert/Update Attribute in Extended Dialog Information
				if (font_info == (DIALOG_EXTINFO*)NULL)
				{
					font_info = rbDlg_CreateDialogExtInfo(handle_pos);

					//Set the extinfo-type
					font_info->ext_type = DIALOG_EXTINFO_FONT;

					//Set the font descriptor
					font_info->ext_data = strdup(TempStr[2]);
				}
				else
				{
					//Reset the font descriptor
					if (font_info->ext_data != (void*)NULL)
						free(font_info->ext_data);

					font_info->ext_data = strdup(TempStr[2]);
				}

				font = strtok((char*)TempStr[2], TempStr[3]);
				if (font == (char*)NULL)
					return RB_OK;

				size = strtok(NULL, TempStr[3]);
				if (size == (char*)NULL)
					return RB_OK;

				style = strtok(NULL, TempStr[3]);
				if (style == (char*)NULL)
					return RB_OK;

				HFONT hFont = (HFONT)NULL;

				f_style = atoi(style);

				f_bold = f_style & FF_BOLD;
				f_italic = f_style & FF_ITALIC;
				f_underline = f_style & FF_UNDERLINE;
				f_strikeout = f_style & FF_STRIKEOUT;
				f_weight = f_bold ? FW_BOLD : FW_NORMAL;

				hFont = CreateFont(-MulDiv(atoi(size), GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72), 0, 0, 0,
				                   f_weight, f_italic, f_underline, f_strikeout, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
				                   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, font);
				SendMessage(handle, WM_SETFONT, (WPARAM)hFont, 0);
			}
			else
			{
				SendMessage(handle, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

				rbDlg_DeleteDialogExtInfo(handle_pos, DIALOG_EXTINFO_FONT);
			}

			UpdateWindow(handle);
			InvalidateRect(handle, NULL, TRUE);

			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "SELPOS") == 0)
		{
			if (RB_WINDOW_TYPE(handle_pos, WIN_EDIT)
			        || RB_WINDOW_TYPE(handle_pos, WIN_INPUT)
			        || RB_WINDOW_TYPE(handle_pos, WIN_PWD))
			{
				char* sel_start = (char*)NULL;
				char* sel_length = (char*)NULL;

				sel_start = strtok(TempStr[2], TempStr[3]);
				sel_length = strtok(NULL, TempStr[3]);

				if (sel_start == (char*)NULL || sel_length == (char*)NULL)
					return RB_OK;

				SendMessage(handle, EM_SETSEL, (WPARAM)atoi(sel_start)-1, (LPARAM)(atoi(sel_start)-1)+atoi(sel_length));
				SendMessage(handle, EM_SCROLLCARET, 0, 0);

				UpdateWindow(handle);

				ERRORCODE_0_AND_RB_OK;
			}
		}

		//----------------------------------------------------------------------------
		//The following attributes (X, Y, WIDTH, HEIGHT) always need the widget's size
		//----------------------------------------------------------------------------
		RECT WinSize;
		if (RB_WINDOW_TYPE(handle_pos, WIN_DIALOG))
		{
			GetWindowRect(handle, &WinSize);
		}
		else
		{
			GetWindowRect(handle, &WinSize);
			ScreenToClient(GetParent(handle), (POINT*)&WinSize.left);
			ScreenToClient(GetParent(handle), (POINT*)&WinSize.right);
		}

		if (strcmp(TempStr[1], "X") == 0)
		{
			MoveWindow(handle, atoi( TempStr[2] )-1, WinSize.top,
			           WinSize.right - WinSize.left, WinSize.bottom - WinSize.top, true);
			InvalidateRect(handle, NULL, TRUE);
			UpdateWindow(handle);

			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "Y") == 0)
		{
			MoveWindow(handle, WinSize.left, atoi( TempStr[2] )-1,
			           WinSize.right - WinSize.left, WinSize.bottom - WinSize.top, true);
			InvalidateRect(handle, NULL, TRUE);
			UpdateWindow(handle);

			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "WIDTH") == 0)
		{
			MoveWindow(handle, WinSize.left, WinSize.top,
			           atoi( TempStr[2] )-1, WinSize.bottom - WinSize.top, true);
			InvalidateRect(handle, NULL, TRUE);
			UpdateWindow(handle);

			ERRORCODE_0_AND_RB_OK;
		}
		else if (strcmp(TempStr[1], "HEIGHT") == 0)
		{
			MoveWindow(handle, WinSize.left, WinSize.top,
			           WinSize.right - WinSize.left, atoi( TempStr[2] )-1, true);
			InvalidateRect(handle, NULL, TRUE);
			UpdateWindow(handle);

			ERRORCODE_0_AND_RB_OK;
		}

		return RB_OK;
	}
	// GETDIALOG -----------------------------------------------------------
	if (strcmp(cmd, "RD02") == 0)
	{
		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);

		rbrunGetValue(&TempStr[2]);
		strtrim(TempStr[2]);
		strupr(TempStr[2]);

		HWND handle = (HWND)NULL;
		int handle_pos = -1;

		if (dialog_info_cnt == 0)
		{
			ERRORCODE("-1");
			return RB_OK;
		}

		for (int a = 0; a < dialog_info_cnt; a++)
		{
			if (dialog_info[a].label != (char*)NULL
			        && strcmp(dialog_info[a].label, TempStr[1]) == 0)
			{
				handle = dialog_info[a].handle;
				handle_pos = a;
				break;
			}
		}

		if (handle == (HWND)NULL)
		{
			ERRORCODE("-1");
			return RB_OK;
		}

		ERRORCODE("0");

		if (strcmp(TempStr[2], "CAPTION") == 0
		        || strcmp(TempStr[2], "TEXT") == 0
		        || strcmp(TempStr[2], "SELECTION") == 0)
		{
			/*
			The SELECTION-attribute of a static_combo is internally
			handled as a TEXT-attribute!
			*/
			if ( RB_WINDOW_TYPE( handle_pos, WIN_STATIC_COMBO ) )
			{
				strcpy( TempStr[2], "TEXT" );
			}

			if (strcmp(TempStr[2], "SELECTION") == 0
			        && (RB_WINDOW_TYPE(handle_pos, WIN_LIST) || RB_WINDOW_TYPE(handle_pos, WIN_MULTILIST)) )
			{
				if (RB_WINDOW_TYPE(handle_pos, WIN_MULTILIST))
				{
					int len = 0;
					char* textbuf = (char*)NULL;
					char* sep = (char*)NULL;
					int items_Pos[1024];

					//Get number of selected items
					int selItemCnt = SendMessage(handle, LB_GETSELCOUNT, 0, 0);
					SendMessage(handle, LB_GETSELITEMS, selItemCnt, (LPARAM) items_Pos);

					char* ret_string = (char*)MALLOC(sizeof(char));
					*ret_string = '\0';

					//Get standard separator
					rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);

					for (int a = 0; a<selItemCnt; a++)
					{
						len = SendMessage(handle, LB_GETTEXTLEN, items_Pos[a], 0 );
						textbuf = (char*)MALLOC((len+1)*sizeof(char));
						SendMessage(handle, LB_GETTEXT, items_Pos[a], (long)textbuf);

						ret_string = (char*) realloc((char*)ret_string,
						                             (strlen(ret_string) + strlen(textbuf) + strlen(sep) + 1) * sizeof(char));

						strcat(ret_string, textbuf);

						if ((a+1) < selItemCnt)
							strcat(ret_string, sep);

						FREE(textbuf);
					}

					RB_LETVAR(TempStr[0],ret_string);
					FREE(ret_string);
					FREE(sep);
				}
				else
				{
					int position = SendMessage(handle, LB_GETCURSEL, 0, 0); //Index
					if (position != LB_ERR)
					{
						int len = SendMessage(handle, LB_GETTEXTLEN, position, 0 );
						char* textbuf = (char*)MALLOC((len+1)*sizeof(char));
						SendMessage(handle, LB_GETTEXT, position, (long)textbuf);

						RB_LETVAR(TempStr[0], textbuf);
						FREE(textbuf);
					}
					else
					{
						RB_LETVAR(TempStr[0], "");
					}
				}
			}
			else if (RB_WINDOW_TYPE(handle_pos, WIN_MENU))
			{
				*text = '\0';
				GetMenuString( GetMenu( handle ), handle_pos + DIALOG_ID, text, STDLINE, MF_BYCOMMAND );

				RB_LETVAR(TempStr[0], text);
			}
			else
			{
				int fieldsize;
				char *textbuf = (char*)NULL;
				char* selstr = (char*)NULL;
				fieldsize = SendMessage(handle, WM_GETTEXTLENGTH, 0, 0);
				textbuf = (char*)MALLOC((fieldsize+1) * sizeof(char));
				SendMessage(handle, WM_GETTEXT, fieldsize+1, (long) textbuf ); //Text

				if (strcmp(TempStr[2], "SELECTION") == 0 )
				{
					DWORD dwStart = 0, dwEnd = 0;
					SendMessage(handle, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);

					selstr = strsubstr(textbuf, (int)dwStart, (int)dwEnd - (int)dwStart);
					RB_LETVAR(TempStr[0], selstr);
				}
				else
				{
					//Replace CR/LF to LF only
					strreplace(&textbuf, "\r\n", "\n", true);
					RB_LETVAR(TempStr[0], textbuf);
				}

				FREE(textbuf);
			}

			return RB_OK;
		}
		else if (strcmp(TempStr[2], "VISIBLE") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			if (IsWindowVisible(handle))
			{
				RB_LETVAR(TempStr[0], "0");
			}
			else
			{
				RB_LETVAR(TempStr[0], "-1");
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "ENABLED") == 0)
		{
			if (RB_WINDOW_TYPE(handle_pos, WIN_MENU))
			{
				if ((GetMenuState(GetMenu(handle), handle_pos + DIALOG_ID, MF_BYCOMMAND)
				        &(MF_DISABLED | MF_GRAYED)))
				{
					RB_LETVAR(TempStr[0], "-1");
				}
				else
				{
					RB_LETVAR(TempStr[0], "0");
				}
			}
			else
			{
				if (IsWindowEnabled(handle))
				{
					RB_LETVAR(TempStr[0], "0");
				}
				else
				{
					RB_LETVAR(TempStr[0], "-1");
				}
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "CHECKED") == 0)
		{
			if (RB_WINDOW_TYPE(handle_pos, WIN_MENU))
			{
				if (!(GetMenuState(GetMenu(handle), handle_pos + DIALOG_ID, MF_BYCOMMAND)
				        &MF_CHECKED))
				{
					RB_LETVAR(TempStr[0], "-1");
				}
				else
				{
					RB_LETVAR(TempStr[0], "0");
				}
			}
			else
			{
				if (SendMessage(handle, BM_GETCHECK, 0, 0) == BST_CHECKED)
				{
					RB_LETVAR(TempStr[0], "0");
				}
				else
				{
					RB_LETVAR(TempStr[0], "-1");
				}
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "FOCUS") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			if (handle == GetFocus())
			{
				RB_LETVAR(TempStr[0], "0");
			}
			else
			{
				RB_LETVAR(TempStr[0], "-1");
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "READONLY") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			long styles = 0;
			styles = GetWindowLong(handle, GWL_STYLE);

			if (styles & ES_READONLY)
			{
				RB_LETVAR(TempStr[0], "0");
			}
			else
			{
				RB_LETVAR(TempStr[0], "-1");
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "SELINDEX") == 0)
		{
			if (RB_WINDOW_TYPE(handle_pos, WIN_LIST))
			{
				int sel = SendMessage(handle, LB_GETCURSEL, 0, 0);

				if (sel != LB_ERR)
				{
					sprintf(text, "%d", sel+1);
					RB_LETVAR(TempStr[0], text);
				}
				else
				{
					RB_LETVAR(TempStr[0], "0");
				}
			}
			else if (RB_WINDOW_TYPE(handle_pos, WIN_MULTILIST))
			{
				int items_Pos[1024];

				char* sep = (char*)NULL;
				rbrunGetVar(PVAR_STD_SEP, &sep, procStack_cnt);

				//Get number of selected items
				int selItemCnt = SendMessage(handle, LB_GETSELCOUNT, 0, 0);
				if (selItemCnt == 0)
				{
					RB_LETVAR(TempStr[0], "0");
					free( sep );

					return RB_OK;
				}
				SendMessage(handle, LB_GETSELITEMS, selItemCnt, (LPARAM) items_Pos);

				char* retstr = (char*)NULL;
				int retstr_cnt = 0;

				char itempos[10+1];

				for (int a=0; a < selItemCnt; a++)
				{
					sprintf(itempos, "%d", items_Pos[a]+1);

					if (retstr_cnt == 0)
					{
						retstr = (char*)malloc((strlen(itempos)+1+1) * sizeof(char));
						*retstr = '\0';
					}
					else
					{
						retstr = (char*)realloc((char*)retstr, (strlen(retstr) + strlen(itempos)+1+1) * sizeof(char));
						strcat( retstr, sep );
					}
					strcat( retstr, itempos );
					retstr_cnt++;
				}

				RB_LETVAR(TempStr[0], retstr);

				free( sep );
				free( retstr );
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "TOOLTIP") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			DIALOG_EXTINFO* tt_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_TOOLTIP);
			if (tt_info == (DIALOG_EXTINFO*)NULL)
			{
				RB_LETVAR(TempStr[0], "");
			}
			else
			{
				TOOLINFO *ti = (TOOLINFO*)tt_info->ext_data;

				if (ti->lpszText != (char*)NULL)
				{
					RB_LETVAR(TempStr[0], ti->lpszText);
				}
				else
				{
					RB_LETVAR(TempStr[0], "");
				}
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "STATUSBAR") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			if (dialog_info[handle_pos].window_type == WIN_DIALOG)
			{
				DIALOG_EXTINFO* statusbar = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_STATUSBAR);
				if (statusbar == (DIALOG_EXTINFO*)NULL)
				{
					RB_LETVAR(TempStr[0], "");
				}
				else
				{
					if (statusbar->ext_data != (char*)NULL)
					{
						RB_LETVAR(TempStr[0], (char*)statusbar->ext_data);
					}
					else
					{
						RB_LETVAR(TempStr[0], "");
					}
				}
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "IMAGE") == 0
		         || strcmp(TempStr[2], "IMAGE_TRANSPARENT") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			if (dialog_info[handle_pos].window_type != WIN_DIALOG)
			{
				DIALOG_EXTINFO* image_info;

				if ( strcmp(TempStr[2], "IMAGE_TRANSPARENT") == 0)
				{
					image_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_IMAGE_TRANS);
				}
				else
				{
					image_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_IMAGE);
				}

				if (image_info != (DIALOG_EXTINFO*) NULL)
				{
					if (image_info->ext_data != (void*)NULL)
					{
						RB_LETVAR(TempStr[0], (char*)image_info->ext_data);
					}
					else
					{
						RB_LETVAR(TempStr[0], "");
					}
				}
				else
				{
					RB_LETVAR(TempStr[0], "");
				}
			}
			else
			{
				RB_LETVAR(TempStr[0], "");
			}

			return RB_OK;
		}
		else if (strcmp(TempStr[2], "ICON") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			if (dialog_info[handle_pos].window_type == WIN_DIALOG)
			{
				DIALOG_EXTINFO* icon_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_ICON);
				if (icon_info != (DIALOG_EXTINFO*) NULL)
				{
					if (icon_info->ext_data != (void*)NULL)
					{
						RB_LETVAR(TempStr[0], (char*)icon_info->ext_data);
					}
					else
					{
						RB_LETVAR(TempStr[0], "");
					}
				}
				else
				{
					RB_LETVAR(TempStr[0], "");
				}
			}
			else
			{
				RB_LETVAR(TempStr[0], "");
			}

			return RB_OK;
		}
		else if (strcmp(TempStr[2], "MENU") == 0)
		{
			if (dialog_info[handle_pos].window_type == WIN_DIALOG)
			{
				DIALOG_EXTINFO* menu_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_MENU);
				if (menu_info != (DIALOG_EXTINFO*) NULL)
				{
					if (menu_info->ext_data != (void*)NULL)
					{
						RB_LETVAR(TempStr[0], (char*)menu_info->ext_data);
					}
					else
					{
						RB_LETVAR(TempStr[0], "");
					}
				}
				else
				{
					RB_LETVAR(TempStr[0], "");
				}
			}
			else
			{
				RB_LETVAR(TempStr[0], "");
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "STYLE") == 0)
		{
			if (dialog_info[handle_pos].window_type == WIN_DIALOG)
			{
				DIALOG_EXTINFO* style_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_STYLE);
				if (style_info != (DIALOG_EXTINFO*) NULL)
				{
					if (style_info->ext_data != (void*)NULL)
					{
						RB_LETVAR(TempStr[0], (char*)style_info->ext_data);
					}
					else
					{
						RB_LETVAR(TempStr[0], "");
					}
				}
				else
				{
					RB_LETVAR(TempStr[0], "");
				}
			}
			else
			{
				RB_LETVAR(TempStr[0], "");
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "ORDER") == 0)
		{
			if (dialog_info[handle_pos].window_type == WIN_DIALOG)
			{
				DIALOG_EXTINFO* order_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_ORDER);
				if (order_info != (DIALOG_EXTINFO*) NULL)
				{
					if (order_info->ext_data != (void*)NULL)
					{
						RB_LETVAR(TempStr[0], (char*)order_info->ext_data);
					}
					else
					{
						RB_LETVAR(TempStr[0], "");
					}
				}
				else
				{
					RB_LETVAR(TempStr[0], "");
				}
			}
			else
			{
				RB_LETVAR(TempStr[0], "");
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "ANIMATION") == 0)
		{
			if (dialog_info[handle_pos].window_type == WIN_ANIMATION)
			{
				DIALOG_EXTINFO* anim_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_ANIMATION);
				if (anim_info != (DIALOG_EXTINFO*) NULL)
				{
					if (anim_info->ext_data != (void*)NULL)
					{
						RB_LETVAR(TempStr[0], (char*)anim_info->ext_data);
					}
					else
					{
						RB_LETVAR(TempStr[0], "");
					}
				}
				else
				{
					RB_LETVAR(TempStr[0], "");
				}
			}
			else
			{
				RB_LETVAR(TempStr[0], "");
			}
			return RB_OK;
		}
		else if (strcmp(TempStr[2], "PROGRESS") == 0)
		{
			if (dialog_info[handle_pos].window_type == WIN_PROGRESS)
			{
				sprintf(text, "%d", SendMessage( handle, PBM_GETPOS, 0, 0 ) );
				RB_LETVAR(TempStr[0], text);
				return RB_OK;
			}
		}
		else if (strcmp(TempStr[2], "HWND") == 0)
		{
			sprintf(text, "%ld", handle);
			RB_LETVAR( TempStr[0], text );

			return RB_OK;
		}
		else if (strcmp(TempStr[2], "TYPE") == 0)
		{
			switch (dialog_info[handle_pos].window_type)
			{
				case WIN_DIALOG:
					strcpy(text, "DIALOG");
					break;
				case WIN_BUTTON:
					strcpy(text, "BUTTON");
					break;
				case WIN_INPUT:
					strcpy(text, "INPUT");
					break;
				case WIN_EDIT:
					strcpy(text, "EDIT");
					break;
				case WIN_LABEL:
					strcpy(text, "LABEL");
					break;
				case WIN_OPTION:
					strcpy(text, "OPTION");
					break;
				case WIN_RADIO:
					strcpy(text, "RADIO");
					break;
				case WIN_LIST:
					strcpy(text, "LIST");
					break;
				case WIN_COMBO:
					strcpy(text, "COMBO");
					break;
				case WIN_STATIC_COMBO:
					strcpy(text, "STATIC_COMBO");
					break;
				case WIN_GROUP:
					strcpy(text, "GROUP");
					break;
				case WIN_IMAGE:
					strcpy(text, "IMAGE");
					break;
				case WIN_PWD:
					strcpy(text, "PWD");
					break;
				case WIN_MENU:
					strcpy(text, "MENU");
					break;
				case WIN_MULTILIST:
					strcpy(text, "MULTILIST");
					break;
				case WIN_PROGRESS:
					strcpy(text, "PROGRESS");
					break;
				default:
					strcpy(text, "");
					break;
			}

			RB_LETVAR( TempStr[0], text );

			return RB_OK;
		}
		else if (strncmp(TempStr[2], "EVENT_", 6) == 0)
		{
			bool get_value = false;

			if (strncmp(TempStr[2]+6, "CLICK", 5) == 0)
			{
				get_value = RB_DLG_GETEVENT(handle_pos, DLGEVENT_CLICK);
			}
			else if (strncmp(TempStr[2]+6, "DBLCLICK", 8) == 0)
			{
				get_value = RB_DLG_GETEVENT(handle_pos, DLGEVENT_DBLCLICK);
			}
			else if (strncmp(TempStr[2]+6, "SIZE", 4) == 0)
			{
				get_value = RB_DLG_GETEVENT(handle_pos, DLGEVENT_WINDOWSIZE);
			}
			else if (strncmp(TempStr[2]+6, "CLOSE", 5) == 0)
			{
				get_value = RB_DLG_GETEVENT(handle_pos, DLGEVENT_WINDOWCLOSE);
			}
			else if (strncmp(TempStr[2]+6, "CHANGE", 6) == 0)
			{
				get_value = RB_DLG_GETEVENT(handle_pos, DLGEVENT_CHANGE);
			}
			else if (strncmp(TempStr[2]+6, "OK", 2) == 0)
			{
				get_value = RB_DLG_GETEVENT(handle_pos, DLGEVENT_OK);
			}
			else if (strncmp(TempStr[2]+6, "CANCEL", 6) == 0)
			{
				get_value = RB_DLG_GETEVENT(handle_pos, DLGEVENT_CANCEL);
			}
			else if (strncmp(TempStr[2]+6, "MOVE",4 ) == 0)
			{
				get_value = RB_DLG_GETEVENT(handle_pos, DLGEVENT_MOVE);
			}
			else if (strncmp(TempStr[2]+6, "GETFOCUS", 8) == 0)
			{
				get_value = RB_DLG_GETEVENT(handle_pos, DLGEVENT_GETFOCUS);
			}
			else if (strncmp(TempStr[2]+6, "LEAVEFOCUS", 10) == 0)
			{
				get_value = RB_DLG_GETEVENT(handle_pos, DLGEVENT_LEAVEFOCUS);
			}
			else if (strncmp(TempStr[2]+6, "MOUSEMOVE", 10) == 0)
			{
				get_value = RB_DLG_GETEVENT(handle_pos, DLGEVENT_MOUSEMOVE);
			}

			if (get_value)
			{
				RB_LETVAR(TempStr[0], "0");
			}
			else
			{
				RB_LETVAR(TempStr[0], "-1");
			}

			return RB_OK;
		}

		RB_DLG_NOMENU(handle_pos);

		rbrunGetVar(PVAR_STD_SEP, &TempStr[3], procStack_cnt);

		if (strcmp(TempStr[2], "ITEMS") == 0)
		{
			int item_cnt = 0;

			//Reset existing content
			if (RB_WINDOW_TYPE(handle_pos, WIN_LIST) || RB_WINDOW_TYPE(handle_pos, WIN_MULTILIST))
				item_cnt = SendMessage(handle, LB_GETCOUNT, 0, 0);

			if (RB_WINDOW_TYPE(handle_pos, WIN_COMBO) || RB_WINDOW_TYPE(handle_pos, WIN_STATIC_COMBO))
				item_cnt = SendMessage(handle, CB_GETCOUNT, 0, 0);

			TempStr[4] = (char*)MALLOC(sizeof(char));
			*TempStr[4] = '\0';
			for (int a = 0; a < item_cnt; a++)
			{
				int len = 0;

				if (RB_WINDOW_TYPE(handle_pos, WIN_LIST) || RB_WINDOW_TYPE(handle_pos, WIN_MULTILIST))
					len = SendMessage(handle, LB_GETTEXTLEN, a, 0 );
				else if (RB_WINDOW_TYPE(handle_pos, WIN_COMBO) ||
				         RB_WINDOW_TYPE(handle_pos, WIN_STATIC_COMBO))
					len = SendMessage(handle, CB_GETLBTEXTLEN, a, 0 );

				char* textbuf = (char*)MALLOC((len+1)*sizeof(char));


				if (RB_WINDOW_TYPE(handle_pos, WIN_LIST) || RB_WINDOW_TYPE(handle_pos, WIN_MULTILIST))
					SendMessage(handle, LB_GETTEXT, a, (long)textbuf);
				else if (RB_WINDOW_TYPE(handle_pos, WIN_COMBO) ||
				         RB_WINDOW_TYPE(handle_pos, WIN_STATIC_COMBO))
					SendMessage(handle, CB_GETLBTEXT, a, (long)textbuf);

				TempStr[4] = (char*)realloc((char*)TempStr[4],
				                            (strlen(TempStr[4]) + len + strlen(TempStr[3]) + 1) * sizeof(char));
				strcat(TempStr[4], textbuf);
				if (a < item_cnt-1) strcat(TempStr[4], TempStr[3]);

				FREE(textbuf);
			}

			RB_LETVAR(TempStr[0], TempStr[4]);

			return RB_OK;
		}
		else if (strcmp(TempStr[2], "FONT") == 0)
		{
			RB_DLG_NOMENU(handle_pos);

			if (dialog_info[handle_pos].window_type != WIN_DIALOG)
			{
				DIALOG_EXTINFO* font_info = rbDlg_GetDialogExtInfo(handle_pos, DIALOG_EXTINFO_FONT);
				if (font_info != (DIALOG_EXTINFO*) NULL)
				{
					if (font_info->ext_data != (void*)NULL)
					{
						RB_LETVAR(TempStr[0], (char*)font_info->ext_data);
					}
					else
					{
						RB_LETVAR(TempStr[0], "");
					}
				}
				else
				{
					RB_LETVAR(TempStr[0], "");
				}
			}
			else
			{
				RB_LETVAR(TempStr[0], "");
			}

			return RB_OK;
		}
		else if (strcmp(TempStr[2], "SELPOS") == 0)
		{
			if (RB_WINDOW_TYPE(handle_pos, WIN_EDIT)
			        || RB_WINDOW_TYPE(handle_pos, WIN_INPUT)
			        || RB_WINDOW_TYPE(handle_pos, WIN_PWD))
			{
				DWORD dwStart = 0, dwEnd = 0;
				SendMessage(handle, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);

				sprintf(text, "%d%c%d", (int)dwStart+1, TempStr[3][0], (int)dwStart - dwEnd);

				RB_LETVAR(TempStr[0], text);
				return RB_OK;
			}
		}

		//--------------------------------------------------------------------------------------
		//The following attributes (POSITION, X, Y, WIDTH, HEIGHT) always need the widget's size
		//--------------------------------------------------------------------------------------
		RECT WinSize;
		if (RB_WINDOW_TYPE(handle_pos, WIN_DIALOG))
		{
			GetWindowRect(handle, &WinSize);
		}
		else
		{
			GetWindowRect(handle, &WinSize);
			ScreenToClient(GetParent(handle), (POINT*)&WinSize.left);
			ScreenToClient(GetParent(handle), (POINT*)&WinSize.right);
		}

		if (strcmp(TempStr[2], "POSITION") == 0)
		{
			/*
			sprintf(text, "%d%c%d%c%d%c%d", dialog_info[handle_pos].pos.left+1, sep,
				 dialog_info[handle_pos].pos.top+1, sep,
					dialog_info[handle_pos].pos.right+1, sep,
						dialog_info[handle_pos].pos.bottom+1);
			*/
			(void)sprintf(text, "%d%c%d%c%d%c%d", WinSize.left + 1, TempStr[3][0],
			              WinSize.top + 1, TempStr[3][0], WinSize.right - WinSize.left + 1, TempStr[3][0],
			              WinSize.bottom - WinSize.top + 1);

			RB_LETVAR(TempStr[0], text);

			return RB_OK;
		}
		else if (strcmp(TempStr[2], "X") == 0)
		{
			sprintf(text, "%d", WinSize.left + 1 );
			RB_LETVAR(TempStr[0], text);

			return RB_OK;
		}
		else if (strcmp(TempStr[2], "Y") == 0)
		{
			sprintf(text, "%d", WinSize.top + 1 );
			RB_LETVAR(TempStr[0], text);

			return RB_OK;
		}
		else if (strcmp(TempStr[2], "WIDTH") == 0)
		{
			sprintf(text, "%d", WinSize.right - WinSize.left + 1 );
			RB_LETVAR(TempStr[0], text);

			return RB_OK;
		}
		else if (strcmp(TempStr[2], "HEIGHT") == 0)
		{
			sprintf(text, "%d", WinSize.bottom - WinSize.top + 1 );
			RB_LETVAR(TempStr[0], text);

			return RB_OK;
		}


		ERRORCODE("-1");
		return RB_OK;
	}
	// RUNDIALOG/WAITFORHOTKEY ---------------------------------------------
	if (strcmp(cmd, "RD03") == 0
	        || strcmp(cmd, "WAITFORHOTKEY") == 0)
	{
		bool onHotKey = false;
		if (strcmp(cmd, "WAITFORHOTKEY") == 0) onHotKey = true;

		rbrunScanVar(&TempStr[0]);

		rbrunGetValue(&TempStr[1]);
		TempInt1 = atoi(TempStr[1]);

		/*
		HWND handle = (HWND)NULL;
		if(dialog_info_cnt > 0)
		{
			for(int a = 0; a < dialog_info_cnt; a++)
			{
				if(strcmp(dialog_info[a].label, TempStr[1].c_str()) == 0)
				{
					if(dialog_info[a].window_type != WIN_DIALOG) break;

					handle = dialog_info[a].handle;
					break;
				}
			}
		}

		if(handle == (HWND)NULL)
		{
			ERRORCODE("-1");
			return RB_OK;
		}

		//Set focus if no object has it
		for(int a = 0; a < dialog_info_cnt; a++)
		{
			if(dialog_info[a].window_type != WIN_DIALOG &&
				dialog_info[a].handle == GetFocus()) break;
			if(a == dialog_info_cnt-1)
			{
				for(int b = 0; b < dialog_info_cnt; b++)
				{
					if(dialog_info[b].window_type != WIN_DIALOG
						&& dialog_info[b].window_type != WIN_LABEL)
					{
						SetFocus(dialog_info[b].handle);
						break;
					}
				}
			}
		}
		*/
		//HWND handle = GetActiveWindow();
		if (TempInt1 > 0)
		{
			SetTimer(hInvisibleWindow,RUNDIALOG_TIMER_ID, TempInt1,NULL);
		}

		IsEvent = false;
		MSG messages;
		while (GetMessage(&messages, NULL, 0, 0))
		{
			if (!IsDialogMessage(GetActiveWindow(), &messages))
			{
				TranslateMessage(&messages);
				DispatchMessage(&messages);
			}
			if (!onHotKey && IsEvent) break; //This line never in if-struct above!!
			else if (onHotKey && IsEvent
			         && (dialog_event == SPECIALEVENT_HOTKEY
			             || dialog_event == DLGEVENT_TIMEOUT)) break;
		}

		if (TempInt1 > 0)
		{
			KillTimer(hInvisibleWindow,RUNDIALOG_TIMER_ID);
		}

		char event_id[30+1];
		*event_id = '\0';

		switch (dialog_event)
		{
			case DLGEVENT_TIMEOUT:
				break;
			case DLGEVENT_CLICK:
				strcpy(event_id, "click");
				break;
			case DLGEVENT_DBLCLICK:
				strcpy(event_id, "dblclick");
				break;
			case DLGEVENT_WINDOWSIZE:
				strcpy(event_id, "size");
				break;
			case DLGEVENT_WINDOWCLOSE:
				strcpy(event_id, "close");
				break;
			case DLGEVENT_CHANGE:
				strcpy(event_id, "change");
				break;
			case DLGEVENT_OK:
				strcpy(event_id, "ok");
				break;
			case DLGEVENT_CANCEL:
				strcpy(event_id, "cancel");
				break;
			case DLGEVENT_MOVE:
				strcpy(event_id, "move");
				break;
			case DLGEVENT_MOUSEMOVE:
				strcpy(event_id, "mousemove");
				break;

			case SPECIALEVENT_HOTKEY:
				{
					strcpy(event_id, "hotkey");

					char* event_string = (char*)MALLOC((strlen(hotkey_info[DlgClick])
					                                    + strlen(event_id) + 1 + 1) * sizeof(char));
#ifdef DEBUG_SESSION
					MALLOC_DEBUG(event_string, (strlen(hotkey_info[DlgClick])
					                            + strlen(event_id) + 1 + 1) * sizeof(char));
#endif



					sprintf(event_string, "%s_%s", event_id, hotkey_info[DlgClick]);

					RB_LETVAR(TempStr[0], event_string);

					FREE(event_string);

					return RB_OK;
				}
			default:
				strcpy(event_id, "undefined");
		}

		if (strcmp(event_id, "") != 0
		        && DlgClick >= DIALOG_ID)
		{
			char* event_string = (char*)MALLOC((strlen(dialog_info[DlgClick - DIALOG_ID].label)
			                                    + strlen(event_id) + 1 + 1) * sizeof(char));

#ifdef DEBUG_SESSION
			MALLOC_DEBUG(event_string, (strlen(dialog_info[DlgClick - DIALOG_ID].label)
			                            + strlen(event_id) + 1 + 1) * sizeof(char));
#endif

			sprintf(event_string, "%s_%s", event_id, dialog_info[DlgClick - DIALOG_ID].label);

			RB_LETVAR(TempStr[0], event_string);

			FREE(event_string);
		}
		else
		{
			RB_LETVAR(TempStr[0], "");
		}
		return RB_OK;
	}
	// DELDIALOG -----------------------------------------------------------
	if (strcmp(cmd, "RD04") == 0)
	{
		rbrunGetValue(&TempStr[0]);

		ERRORCODE("-1");

		bool delete_children = false;

		for (int a = 0; a < dialog_info_cnt; a++)
		{

			/* sprintf(text, "%s %s", dialog_info[a].label, TempStr[0]);
			VARS( text ); */


			if (dialog_info[a].label != (char*)NULL &&
			        ((!delete_children && strcmp(dialog_info[a].label, TempStr[0]) == 0)
			         || (delete_children && strncmp(dialog_info[a].label, TempStr[0],
			                                        strlen(TempStr[0]) * sizeof(char)) == 0)))
			{
				if (!delete_children) DestroyWindow(dialog_info[a].handle);

				dialog_info[a].handle = (HWND)NULL;
				free(dialog_info[a].label);
				dialog_info[a].label = (char*)NULL;

				ERRORCODE("0");

				if (!delete_children)
				{
					if (dialog_info[a].window_type == WIN_DIALOG)
					{
						//Children can only follow behind the dialog in the stack
						delete_children = true;
					}

					TempStr[0] = (char*)realloc((char*)TempStr[0],
					                            (strlen(TempStr[0]) + 1 + 1) * sizeof(char));
					strcat(TempStr[0], ":");
				}
			}
		}

		return RB_OK;
	}

// *****************************************************************************
// PROCEDURE MANAGEMENT
// *****************************************************************************
	// PROC/FUNC -----------------------------------------------------------
	if (strcmp(cmd, "PROC") == 0 ||
	        strcmp(cmd, "FUNC") == 0)
	{
		if (loop_cnt > 0)
		{
			return RB_OK;
		}

		char* procName = (char*)NULL;

		rbrunGetValue(&procName);

		strtrim(procName);
		strupr(procName);

#ifdef DEBUG_SESSION
		fprintf(debug, "procName = %s\n", procName);
#endif

		//If this procedure already exists in the procedure information, ignore all code
		for (int a = 0; a < proc_cnt; a++)
		{
			if (strcmp(procedures[a].proc_name, procName) == 0)
			{
				(void)rbrunScan('\n', NULL);

				if (file_offset != procedures[a].proc_pos)
				{
					FREE(procName);
					return RB_OK;
				}

				if (strcmp(cmd, "PROC") == 0)
				{
					rbrunReadTo("ENDP", false);
				}
				else
				{
					rbrunReadTo("ENDF", false);
				}

				FREE(procName);
				return RB_OK;
			}
		}

		if (proc_cnt == 0)
		{
			procedures = (PROCINFO*)malloc(sizeof(PROCINFO));
		}
		else
		{
			procedures = (PROCINFO*)realloc((PROCINFO*) procedures, (proc_cnt+1) * sizeof(PROCINFO));
		}

		procedures[proc_cnt].proc_name = (char*)malloc((strlen(procName)+1) * sizeof(char));
		strcpy(procedures[proc_cnt].proc_name, procName);

		int parm_cnt = 0;
		if (rbrunGetChar() != '0')
		{
			CHARACTER_BACK();

			char* var;

			do
			{
				if (parm_cnt == 0)
				{
					procedures[proc_cnt].parms = (char**) malloc(sizeof(char*));
				}
				else
				{
					procedures[proc_cnt].parms = (char**) realloc((char**)procedures[proc_cnt].parms,
					                             (parm_cnt+1) * sizeof(char*));

					CHARACTER_BACK(); //Because we read \n at end of this loop!
				}

				var = (char*)NULL;

				rbrunScanVar(&var);

				procedures[proc_cnt].parms[parm_cnt] = (char*) malloc((strlen(var)+1) * sizeof(char));
				strcpy(procedures[proc_cnt].parms[parm_cnt], var);

				FREE(var);

				parm_cnt++;
			}
			while (rbrunScanChar("\n") != '\n');

			//CHARACTER_BACK(); //have to go one character back because rbrunScanChar("\n") has been called
			procedures[proc_cnt].parm_cnt = parm_cnt;
		}
		else
		{
			procedures[proc_cnt].parm_cnt = 0;
			rbrunScan('\n', NULL);
		}

		procedures[proc_cnt].proc_pos = file_offset;
		procedures[proc_cnt].hasReturnValue = false;

		if (strcmp(cmd, "FUNC") == 0)
			procedures[proc_cnt].hasReturnValue = true;

		proc_cnt++;

		//well, before rbReadTo is called, we create a pseudo procedure call,
		//so that rbAddLabel can access the current procedure name!
		if (procStack_cnt == 0)
		{
			procStack = (PSTACK*) malloc(sizeof(PSTACK));
		}
		else
		{
			procStack = (PSTACK*) realloc((PSTACK*)procStack, (procStack_cnt+1) * sizeof(PSTACK));
		}

		procStack[procStack_cnt].proc_name = procName;
		procStack[procStack_cnt].vars = (char**)NULL;

		procStack_cnt++;

		//go to the end of the procedure
		if (strcmp(cmd, "PROC") == 0)
		{
			rbrunReadTo("ENDP", false);
		}
		else
		{
			rbrunReadTo("ENDF", false);
		}

		//Decrement procStack_cnt
		procStack_cnt--;

		if (procStack_cnt == 0)
		{
			free(procStack);
		}
		else
		{
			procStack = (PSTACK*) realloc((PSTACK*)procStack, procStack_cnt * sizeof(PSTACK));
		}

		FREE(procName);
		return RB_OK;
	}
	// ENDPROC/ENDFUNC -----------------------------------------------------
	if (strcmp(cmd, "ENDP") == 0
	        || strcmp(cmd, "ENDF") == 0)
	{
		if (procStack_cnt == 0)
		{
			return RB_OK;
		}

		if (procStack[procStack_cnt-1].ret_var != (char*) NULL)
		{
			RB_LETVAR(procStack[procStack_cnt-1].ret_var, "");
			free(procStack[procStack_cnt-1].ret_var);
		}

		for (int b = 0; b < procStack[procStack_cnt-1].var_cnt; b++)
		{
			rbrunDelVar(procStack[procStack_cnt-1].vars[b], true);
			free(procStack[procStack_cnt-1].vars[b]);
		}
		if (procStack[procStack_cnt-1].vars != (char**)NULL)
			free(procStack[procStack_cnt-1].vars);

		procStack_cnt--;

		//Set offset to calling position
		file_offset = procStack[procStack_cnt].call_pos;

		//Free strings
		free(procStack[procStack_cnt].proc_name);


		if (procStack_cnt == 0)
		{
			free(procStack);
		}
		else
		{
			procStack = (PSTACK*) realloc((PSTACK*)procStack, (procStack_cnt) * sizeof(PSTACK));
		}
		return RB_OK;
	}
	// RET -----------------------------------------------------------------
	if (strcmp(cmd, "RRET") == 0)
	{
		if (procStack_cnt == 0)
		{
			return RB_OK;
		}

		if (procStack[procStack_cnt-1].ret_var != (char*) NULL)
		{
			//first get the value BEFORE deleting procedure variables
			if (rbrunScanChar("\n") != '\n')
			{
				CHARACTER_BACK();
				rbrunGetValue(&TempStr[0]);
			}
			else
			{
				TempStr[0] = strdup("");
			}
		}

		for (int b = 0; b < procStack[procStack_cnt-1].var_cnt; b++)
		{
			rbrunDelVar(procStack[procStack_cnt-1].vars[b],true);
			free(procStack[procStack_cnt-1].vars[b]);
		}
		if (procStack[procStack_cnt-1].vars != (char**)NULL)
			free(procStack[procStack_cnt-1].vars);

		//Delete loop information from loop stack
		for (int a = loop_cnt-1; a > -1; a--)
		{
			if (loops[a].proc_name != (char*) NULL
			        && strcmp(loops[a].proc_name, procStack[procStack_cnt-1].proc_name) == 0)
			{
				free(loops[a].proc_name);

				if (a == 0)
				{
					free(loops);
				}
				else
				{
					loops = (LOOPINFO*) realloc((LOOPINFO*)loops, a * sizeof(LOOPINFO));
				}
				loop_cnt--;
			}
		}

		procStack_cnt--;
		//Assign return variable with value NOW after deleting the variables

		if (procStack[procStack_cnt].ret_var != (char*) NULL)
		{
			RB_LETVAR(procStack[procStack_cnt].ret_var, TempStr[0]);
			free(procStack[procStack_cnt].ret_var);
		}

		//Set offset to calling position
		file_offset = procStack[procStack_cnt].call_pos;

		//Free strings
		free(procStack[procStack_cnt].proc_name);

		if (procStack_cnt == 0)
		{
			free(procStack);
		}
		else
		{
			procStack = (PSTACK*) realloc((PSTACK*)procStack, (procStack_cnt) * sizeof(PSTACK));
		}
		return RB_OK;
	}

// *****************************************************************************
// EXTERNAL FUNCTION CALLS
// *****************************************************************************
	// EXT -----------------------------------------------------------------
	if (strcmp(cmd, "REXT") == 0)
	{
		EXTERNFUNC*     ExternalFunc = (EXTERNFUNC*)NULL;
		HINSTANCE       hLib = (HINSTANCE)NULL;

		//Return value
		rbrunScanVar(&TempStr[0]);

		//DLL Name
		rbrunGetValue(&TempStr[1]);

		//DLL Function
		rbrunGetValue(&TempStr[2]);

		//Parameters
		char** dll_parm = (char*)NULL;
		int dll_parm_cnt = 0;
		int reterr = 0;
		char chr;

		while ( rbrunScanChar("\n") != '\n' )
		{
			CHARACTER_BACK();

			if ( dll_parm_cnt == 0 )
			{
				dll_parm = (char**)malloc( sizeof( char** ) );
			}
			else
			{
				dll_parm = (char**)realloc( (char**) dll_parm,
				                            (dll_parm_cnt + 1) * sizeof( char** ) );
			}

			dll_parm[dll_parm_cnt] = (char*)NULL;
			rbrunGetValue(&dll_parm[dll_parm_cnt] );
			dll_parm_cnt++;
		}
		CHARACTER_BACK();

		char* retval            = (char*)NULL;
		HINSTANCE dll_handle    = (HINSTANCE)NULL;

		for ( int a = 0; a < dll_cnt; a++ )
		{
			if ( strcmp( dll[a].dll_name, TempStr[1] ) == 0 )
			{
				dll_handle = dll[a].dll_handle;
				break;
			}
		}

		if ( dll_handle == (HINSTANCE)NULL )
		{
			dll_handle = LoadLibrary (TempStr[1]);
			if ( dll_handle )
			{
				if ( dll_cnt == 0 )
				{
					dll = (DLL_INFO*)malloc( sizeof( DLL_INFO ) );
				}
				else
				{
					dll = (DLL_INFO*)realloc( (DLL_INFO*) dll,
					                          (dll_cnt + 1) * sizeof( DLL_INFO ) );
				}

				dll[dll_cnt].dll_name = TempStr[1];
				TempStr[1] = (char*)NULL;

				dll[dll_cnt].dll_handle = dll_handle;

				dll_cnt++;
			}
		}

		if ( dll_handle )
		{
			ExternalFunc = (EXTERNFUNC*)GetProcAddress (dll_handle, TempStr[2] );
			if (ExternalFunc != 0)
			{
				reterr = ExternalFunc( &retval, dll_parm, dll_parm_cnt );
				sprintf( text, "%d", reterr );
				ERRORCODE( text );
			}
			else
			{
				ERRORCODE("-1");
			}
		}
		else
		{
			ERRORCODE("-1");
		}

		if ( retval )
		{
			RB_LETVAR( TempStr[0], retval );
			LocalFree (retval);
		}
		else
		{
			RB_LETVAR( TempStr[0], "" );
		}

		for ( int i = 0; i < dll_parm_cnt; i++ )
		{
			if (dll_parm[i])
				free( dll_parm[i] );
		}

		if ( dll_parm_cnt )
			free( dll_parm );

		return RB_OK;
	}

	// PROCEDURE CALL ------------------------------------------------------
	if (strcmp(cmd, "CALL") == 0)
	{
		rbrunGetValue(&cmd);
#ifdef DEBUG_SESSION
		fprintf(debug, "Procedure: %s\n", cmd);
		fflush(debug);
#endif

		for (int a = 0; a<proc_cnt; a++)
		{
			if (strcmp(procedures[a].proc_name, cmd) == 0)
			{
				char* parm_value = (char*)NULL;

				if (procStack_cnt == 0)
				{
					procStack = (PSTACK*) malloc(sizeof(PSTACK));
				}
				else
				{
					procStack = (PSTACK*) realloc((PSTACK*)procStack,
					                              (procStack_cnt+1) * sizeof(PSTACK));
				}

				procStack[procStack_cnt].proc_name = (char*)malloc((strlen(procedures[a].proc_name)+1) * sizeof(char));
				strcpy(procStack[procStack_cnt].proc_name, procedures[a].proc_name);

				procStack[procStack_cnt].vars = (char**)NULL;
				procStack[procStack_cnt].var_cnt = 0;

				if (procedures[a].hasReturnValue)
				{
					rbrunScanVar(&TempStr[0]);

					procStack[procStack_cnt].ret_var =
					    (char*)malloc((strlen(TempStr[0])+1) * sizeof(char));
					strcpy(procStack[procStack_cnt].ret_var, TempStr[0]);
				}
				else
				{
					procStack[procStack_cnt].ret_var = (char*) NULL;
				}

				//Important for rbrunNewVar(): The procedure stack counter!

				//Because we read out and create the variables now, we need to know the current
				//procedure's name
				if (procedures[a].parm_cnt > 0)
				{
					for (int b=0; b<procedures[a].parm_cnt; b++)
					{
						if (*procedures[a].parms[b] == '*')
						{
							rbrunScanVar(&TempStr[0]);

							/*
							//Create pointer target variable if it does not exist
							if(!rbrunVarExists(TempStr[0], procStack_cnt))
								rbrunNewVar( TempStr[0], procStack_cnt );
							*/

							//Create locale pointer of procedure
							rbrunNewVar(procedures[a].parms[b]+1, procStack_cnt+1);
							rbrunPtrVar(procedures[a].parms[b]+1,
							            TempStr[0], procStack_cnt+1);


							if (TempStr[0] != (char*)NULL)
							{
								FREE(TempStr[0]);
							}
							TempStr[0] = (char*)NULL;
						}
						else
						{
							parm_value = (char*)NULL;

							rbrunGetValue(&parm_value);

							rbrunNewVar(procedures[a].parms[b], procStack_cnt+1);
							rbrunLetVar(procedures[a].parms[b],
							            parm_value, procStack_cnt+1);

							FREE(parm_value);
						}
					}
				}

				//Get position ONLY here because here we already read the parameters ;)
				procStack[procStack_cnt].call_pos = file_offset;

				file_offset = procedures[a].proc_pos;

				procStack_cnt++;

				return RB_OK_IGNORE;
			}
		}
	}

	return RB_ERROR;
}

//------------------------------------------------------------------------------
// WindowProcedure
//------------------------------------------------------------------------------
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT point;
	PAINTSTRUCT ps;

	switch (message)
	{
		case WM_CLOSE:
			if (hwnd == hDebug_Win)
			{
				PostQuitMessage(0);
				//fclose(infile[infileStack[infileStack_cnt]].file);
				exit(0);
			}
			else if (RB_BUILD_IN_DIALOG)
			{
				DlgClick = CANCELBUTTON_CLICK;
				IsEvent = true;
			}
			else
			{
				dialog_event = DLGEVENT_WINDOWCLOSE;
				for (int a = 0; a < dialog_info_cnt; a++)
				{
					if (dialog_info[a].handle == hwnd)
					{
						if (RB_DLG_GETEVENT(a, DLGEVENT_WINDOWCLOSE))
						{
							DlgClick = a + DIALOG_ID;
							IsEvent = true;
						}
						break;
					}
				}
			}
			break;
		case WM_HOTKEY:
			DlgClick = (int)wParam;
			IsEvent = true;
			dialog_event = SPECIALEVENT_HOTKEY;
			break;
		case WM_TIMER:
			IsEvent = true;
			dialog_event = DLGEVENT_TIMEOUT;
			break;
		case WM_MOUSEMOVE:
			//First check if this is a dialog window
			if (RB_BUILD_IN_DIALOG)
				break;

			for (int a = 0; a < dialog_info_cnt; a++)
			{
				if (dialog_info[a].handle == hwnd)
				{
					if (RB_DLG_GETEVENT(a, DLGEVENT_MOUSEMOVE))
					{
						dialog_event = DLGEVENT_MOUSEMOVE;
						DlgClick = a + DIALOG_ID;
						IsEvent = true;
					}
					break;
				}
			}
			return 0;
		case WM_DESTROY:
			//First check if this is a dialog window
			if (RB_BUILD_IN_DIALOG)
				return 0;

			for (int a = 0; a < dialog_info_cnt; a++)
			{
				if (dialog_info[a].handle == hwnd)
					return 0;
			}

			PostQuitMessage(0);
			break;
		case WM_ICONCLICK:
			switch (lParam)
			{
				case WM_LBUTTONUP:
					IsEvent = true;
					trayMenuSelect = -1;
					SetForegroundWindow(hInvisibleWindow);
					PostMessage(hInvisibleWindow, WM_NULL, 0, 0);
					break;
				case WM_RBUTTONUP:
					if (hTrayMenu != (HMENU)NULL)
					{
						GetCursorPos(&point);

						SetForegroundWindow(hInvisibleWindow);
						trayMenuSelect = (int)TrackPopupMenu (hTrayMenu, TPM_LEFTBUTTON | TPM_RETURNCMD, point.x, point.y, 0, hwnd, NULL);
						if (trayMenuSelect == 0) trayMenuSelect = -1;

						PostMessage(hInvisibleWindow, WM_NULL, 0, 0);

						IsEvent = true;
					}
					break;
			}
			return 0;
		case WM_SIZE:
			{
				int dev_value = 0;

				if (hwnd == hEditBox_Win)
				{
					if (IsWindowVisible(hEditBox_OK) || IsWindowVisible(hEditBox_Cancel))
					{
						dev_value = 35;
						MoveWindow(hEditBox_OK, 5, HIWORD(lParam) - 28, 100, 25, TRUE);
						MoveWindow(hEditBox_Cancel, LOWORD(lParam) - 105, HIWORD(lParam) - 28, 100, 25, TRUE);
					}
					MoveWindow(hEditBox_Edit, 0, 0, LOWORD(lParam), HIWORD(lParam) - dev_value, TRUE);
				}
				else if (hwnd == hListBox_Win)
				{
					if (IsWindowVisible(hListBox_OK))
					{
						dev_value = 35;
						MoveWindow(hListBox_OK, 3, HIWORD(lParam) - 28, LOWORD(lParam) - 5, 25, TRUE);
					}
					MoveWindow(hListBox_MultiList, 0, 0, LOWORD(lParam), HIWORD(lParam) - dev_value, TRUE);
					MoveWindow(hListBox_List, 0, 0, LOWORD(lParam), HIWORD(lParam) - dev_value, TRUE);
				}
				else if (hwnd == hDebug_Win)
				{
					MoveWindow(hDebug_List, 0, 0, LOWORD(lParam)-1, HIWORD(lParam) - 50, TRUE);
					MoveWindow(hDebug_CodeLine, 0, HIWORD(lParam) - 47, LOWORD(lParam)-1, 21, TRUE);
					MoveWindow(hDebug_Step, 0, HIWORD(lParam) - 22, 50, 22, TRUE);
					MoveWindow(hDebug_Run, 52, HIWORD(lParam) - 22, 50, 22, TRUE);
					MoveWindow(hDebug_ValLine, 104, HIWORD(lParam) - 22, 132, 22, TRUE);
					MoveWindow(hDebug_Set, 236, HIWORD(lParam) - 22, 40, 22, TRUE);
					MoveWindow(hDebug_Halt, LOWORD(lParam)-61, HIWORD(lParam) - 22, 60, 22, TRUE);
				}
				else
				{
					if (!RB_BUILD_IN_DIALOG)
					{
						dialog_event = DLGEVENT_WINDOWSIZE;
						for (int a = 0; a < dialog_info_cnt; a++)
						{
							if (dialog_info[a].handle == hwnd)
							{
								//Resize the statusbar, if one does exist
								DIALOG_EXTINFO* statusbar = rbDlg_GetDialogExtInfo(a, DIALOG_EXTINFO_STATUSBAR);

								if (statusbar != (DIALOG_EXTINFO*)NULL)
								{
									RECT rSB;
									GetWindowRect((HWND)((*statusbar).ext_handle), &rSB);
									MoveWindow((HWND)((*statusbar).ext_handle), 0, HIWORD(lParam) - 10, LOWORD(lParam), HIWORD(lParam), TRUE);
									MoveWindow((HWND)((*statusbar).ext_handle), 0, rSB.bottom - rSB.top, LOWORD(lParam), HIWORD(lParam) - (rSB.bottom - rSB.top) + 1, TRUE );
								}

								//Is event set?
								if (RB_DLG_GETEVENT(a, DLGEVENT_WINDOWSIZE))
								{
									DlgClick = a + DIALOG_ID;
									IsEvent = true;
								}
								break;
							}
						}
					}
				}

				return 0;
			}
		case WM_MOVE:
			if (!RB_BUILD_IN_DIALOG)
			{
				dialog_event = DLGEVENT_MOVE;
				for (int a = 0; a < dialog_info_cnt; a++)
				{
					if (dialog_info[a].handle == hwnd)
					{
						if (RB_DLG_GETEVENT(a, DLGEVENT_MOVE))
						{
							DlgClick = a + DIALOG_ID;
							IsEvent = true;
						}
						break;
					}
				}
			}
			break;
		case WM_PAINT:
			BeginPaint (hwnd, &ps);
			EndPaint (hwnd, &ps);
			return 0;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			if (!RB_BUILD_IN_DIALOG)
			{
				dialog_event = DLGEVENT_CLICK;
				for (int a = 0; a < dialog_info_cnt; a++)
				{
					if (dialog_info[a].handle == hwnd)
					{
						if (RB_DLG_GETEVENT(a, DLGEVENT_CLICK))
						{
							DlgClick = a + DIALOG_ID;
							IsEvent = true;
						}
						break;
					}
				}
			}
			break;
		case WM_COMMAND:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				if (RB_BUILD_IN_DIALOG)
				{
					if (LOWORD(wParam) == IB_OKBUTTON || LOWORD(wParam) == EDITBOX_OK  || LOWORD(wParam) == LISTBOX_OK)
					{
						IsEvent = true;
						DlgClick = OKBUTTON_CLICK;
					}
					else if (LOWORD(wParam) == IB_CANCELBUTTON || LOWORD(wParam) == EDITBOX_CANCEL)
					{
						IsEvent = true;
						DlgClick = CANCELBUTTON_CLICK;
					}
				}
				else
				{
					if (LOWORD(wParam) >= DIALOG_ID &&
					        RB_DLG_GETEVENT(LOWORD(wParam) - DIALOG_ID, DLGEVENT_CLICK))
					{
						dialog_event = DLGEVENT_CLICK;
						DlgClick = LOWORD(wParam);
						IsEvent = true;
					}
					else
					{
						switch (LOWORD(wParam))
						{
							case IDOK:
								dialog_event = DLGEVENT_OK;
								break;
							case IDCANCEL:
								dialog_event = DLGEVENT_CANCEL;
								break;
						}

						//Get ID if the current hwnd
						for (int a = 0; a < dialog_info_cnt; a++)
						{
							if (dialog_info[a].handle == hwnd)
							{
								if (RB_DLG_GETEVENT(a, dialog_event))
								{
									DlgClick = a + DIALOG_ID;
									IsEvent = true;
								}
								break;
							}
						}
					}
				}
			}
			else if (HIWORD(wParam) == LBN_SELCHANGE ||
			         HIWORD(wParam) == EN_CHANGE)
			{
				if (!RB_BUILD_IN_DIALOG && hwnd != hDebug_Win)
				{
					if (RB_DLG_GETEVENT(LOWORD(wParam) - DIALOG_ID, DLGEVENT_CHANGE))
					{
						dialog_event = DLGEVENT_CHANGE;
						DlgClick = LOWORD(wParam);
						IsEvent = true;
					}
				}
			}
			else if (HIWORD(wParam) == LBN_DBLCLK)
			{
				if (LOWORD(wParam) == LISTBOX_LIST)
				{
					IsEvent = true;
					DlgClick = OKBUTTON_CLICK;
				}
				else
				{
					if (!RB_BUILD_IN_DIALOG &&
					        RB_DLG_GETEVENT(LOWORD(wParam) - DIALOG_ID, DLGEVENT_DBLCLICK))
					{
						dialog_event = DLGEVENT_DBLCLICK;
						DlgClick = LOWORD(wParam);
						IsEvent = true;
					}
				}
			}

			return 0;
		default:
			if (onTrayMenu && message == s_uTaskbarRestart)
				iconToTaskbar(hwnd, hTrayIcon, trayIconToolTip, false);

			return DefWindowProc(hwnd, message, wParam, lParam);

	}
	return 0;
}


//------------------------------------------------------------------------------
// WinMain (RapidBATCH)
//------------------------------------------------------------------------------
#pragma argsused
int WINAPI WinMain(HINSTANCE tInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nFunsterStil)
{
	int res;
	long int seek_to = 0;
	FILE* infile = (FILE*)NULL;

	hInstance = tInstance;
	MSG messages;

	wincl.cbSize        = sizeof(WNDCLASSEX);
	wincl.hInstance = hInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wincl.hIcon  = (HICON) LoadIcon(hInstance, MAKEINTRESOURCE(1000)); //(HICON) LoadIcon(hInstance,MAKEINTRESOURCE(1000));
	wincl.hIconSm = wincl.hIcon;
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
	if (!RegisterClassEx(&wincl)) return 0;

	//Püfung, ob System Windows XP ist
	IsThemeActive = reinterpret_cast<WinXPStyle>(GetProcAddress(LoadLibrary("UxTheme.dll"), "IsThemeActive"));
	devpix = 0;
	if (IsThemeActive != NULL)
	{
		if (IsThemeActive() == true) devpix = XPDEVPIX;
	}


#ifdef DEBUG_SESSION
	preVars_Done = true;
	remove(dbg_file);
	debug = fopen(dbg_file, "wt");

	fprintf(debug, "RapidBATCH v5.0 Runtime Library v3.0 TRACE LOGGING\n");

	struct time tinfo;
	gettime(&tinfo);
	struct date dinfo;
	getdate(&dinfo);

	fprintf(debug, "Created on %02d.%02d.%04d at %02d:%02d:%02d\n", dinfo.da_day, dinfo.da_mon, dinfo.da_year, tinfo.ti_hour, tinfo.ti_min, tinfo.ti_sec);
	fprintf(debug, "Trace file: %s\nDebug Level: %d\n-----------------\n", dbg_file,debug_level);
	fflush(debug);
#endif


#ifdef START_DIR
	chdir(START_DIR);
#endif


	// Initialize Window Info...
	//INPUTBOX DIALOG
	IB_WIN = CreateWindowEx(0,szClassName, "", WS_CAPTION | WS_SYSMENU, GetSystemMetrics(SM_CXFULLSCREEN)/2-150, GetSystemMetrics(SM_CYFULLSCREEN)/2-55, 300, 102 + devpix, HWND_DESKTOP, NULL, hInstance, NULL); // ggf: WS_SYSMENU

	IB_TEXT = CreateWindow("static", "",WS_CHILD | WS_VISIBLE, 5, 5, 280, 20, IB_WIN, NULL, hInstance, NULL);
	SendMessage(IB_TEXT, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	IB_EDIT = CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "",WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_TABSTOP, 5, 22, 285, 24, IB_WIN, NULL, hInstance, NULL);
	SendMessage(IB_EDIT, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	IB_PASSWORD = CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "",WS_CHILD | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_PASSWORD | WS_TABSTOP, 5, 22, 285, 24, IB_WIN, NULL, hInstance, NULL);
	SendMessage(IB_PASSWORD, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

	IB_OK = CreateWindow("button", "&Ok",WS_CHILD | WS_VISIBLE | BS_NOTIFY |WS_TABSTOP, 15, 50, 80, 25, IB_WIN, (HMENU) IB_OKBUTTON, hInstance, NULL);
	SendMessage(IB_OK, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	IB_CANCEL = CreateWindow("button", "&Cancel",WS_CHILD | WS_VISIBLE | BS_NOTIFY | WS_TABSTOP, 200, 50, 80, 25, IB_WIN, (HMENU) IB_CANCELBUTTON, hInstance, NULL);
	SendMessage(IB_CANCEL, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

	//EDITBOX DIALOG
	hEditBox_Win = CreateWindowEx (0, szClassName, "", WS_THICKFRAME|WS_SYSMENU,
	                               GetSystemMetrics(SM_CXSCREEN)/2 - 250, GetSystemMetrics(SM_CYSCREEN)/2 - 200 + devpix, 500, 400,
	                               NULL, NULL, hInstance, NULL);
	hEditBox_Edit = CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "", WS_CHILD | WS_VISIBLE | ES_LEFT | WS_VSCROLL |
	                               ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN,
	                               1, 1, 342, 180, hEditBox_Win, (HMENU) EDITBOX_EDIT, hInstance, NULL) ;

	SendMessage(hEditBox_Edit, EM_SETLIMITTEXT, -1, 0);

	SendMessage(hEditBox_Edit, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

	hEditBox_OK = CreateWindowEx(0, "button", "&Ok", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
	                             1, 185, 100, 25, hEditBox_Win, (HMENU) EDITBOX_OK, hInstance, NULL) ;
	SendMessage(hEditBox_OK, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

	hEditBox_Cancel = CreateWindowEx(0, "button", "&Cancel", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                                 242, 185, 100, 25, hEditBox_Win, (HMENU) EDITBOX_CANCEL, hInstance, NULL) ;
	SendMessage(hEditBox_Cancel, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

	//LISTBOX DIALOG
	hListBox_Win = CreateWindowEx (0, szClassName, "", //WS_BORDER | WS_CAPTION | WS_SYSMENU,
	                               WS_THICKFRAME|WS_SYSMENU|WS_CLIPCHILDREN,
	                               GetSystemMetrics(SM_CXSCREEN)/2 - 65, GetSystemMetrics(SM_CYSCREEN)/2 - 88 + devpix, 130, 176, NULL, NULL, hInstance, NULL);
	hListBox_List = CreateWindowEx(WS_EX_CLIENTEDGE ,"listbox", NULL, WS_CHILD | WS_VISIBLE | ES_LEFT | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | LBS_NOTIFY,
	                               1, 1, 122, 140, hListBox_Win, (HMENU) LISTBOX_LIST, hInstance, NULL) ;
	SendMessage(hListBox_List, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

	hListBox_MultiList = CreateWindowEx(WS_EX_CLIENTEDGE ,"listbox", NULL, WS_CHILD | WS_VISIBLE | ES_LEFT | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | LBS_MULTIPLESEL,
	                                    1, 1, 122, 140, hListBox_Win, (HMENU) LISTBOX_MULTILIST, hInstance, NULL) ;
	SendMessage(hListBox_MultiList, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

	hListBox_OK = CreateWindowEx(0 ,"button", "&Ok", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
	                             1, 125, 122, 24, hListBox_Win, (HMENU) LISTBOX_OK, hInstance, NULL) ;
	SendMessage(hListBox_OK, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

	//INFOBOX DIALOG
	hInfoBox_Win = CreateWindowEx (WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, szClassName, "", WS_POPUPWINDOW ,
	                               GetSystemMetrics(SM_CXSCREEN)/2 - 150, GetSystemMetrics(SM_CYSCREEN)/2 - 12, 300, 24,
	                               NULL, NULL, hInstance, NULL);
	hInfoBox_Info = CreateWindowEx(0, "static", "", WS_CHILD | WS_VISIBLE | SS_CENTER,
	                               0, 2, 300, 30, hInfoBox_Win, (HMENU) INFOBOX_INFO, hInstance, NULL) ;
	SendMessage(hInfoBox_Info, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

	//TRAYMENU HIDDEN WINDOW and parent window for OPENBOX/SAVEBOX to set their positions
	hInvisibleWindow = CreateWindowEx (0, szClassName, "", WS_BORDER, GetSystemMetrics(SM_CXSCREEN)/2 - 280, GetSystemMetrics(SM_CYSCREEN)/2 - 205, 0, 0, NULL, (HMENU)NULL, hInstance, NULL);


	//PRE-DECLARED VARIABLES -----------------------------------------------
	RB_NEWVAR(PVAR_ERRORCODE);

	strtrim(szCmdLine);
	if (szCmdLine != (char*)NULL)
	{
		RB_LETVAR(PVAR_COMMAND, szCmdLine);

	}
	else
	{
		RB_NEWVAR(PVAR_COMMAND);
	}

	RB_LETVAR(PVAR_ECHO_TITLE, "RapidBATCH Echo");
	RB_LETVAR(PVAR_CONFIRM_TITLE, "RapidBATCH Confirmation");
	RB_LETVAR(PVAR_WINDIR, getenv("windir"));
	RB_LETVAR(PVAR_INPUTBOX_OK, "&OK");
	RB_LETVAR(PVAR_INPUTBOX_CANCEL, "&Cancel");


	//Current directory
	getcwd(text, STDLINE);
	RB_LETVAR(PVAR_CURRENT, text);

	//User name...
	DWORD unamesize = STDLINE;
	if (GetUserName(text, &unamesize) == true)
	{
		RB_LETVAR(PVAR_CURUSER, text);
	}
	else
	{
		RB_LETVAR(PVAR_CURUSER, "unknown");
	}

	RB_LETVAR(PVAR_WINVERS, GetOSName());
	RB_NEWVAR(PVAR_DAY);
	RB_NEWVAR(PVAR_YEAR);
	RB_NEWVAR(PVAR_MONTH);
	RB_NEWVAR(PVAR_TIME);
	RB_LETVAR(PVAR_OPENBOX_MULTISEL, "-1");
	RB_LETVAR(PVAR_SAVEBOX_MULTISEL, "-1");

	RB_LETVAR(PVAR_EDITBOX_OK, "&Ok");
	RB_LETVAR(PVAR_EDITBOX_CANCEL, "&Cancel");

	RB_LETVAR(PVAR_EDITBOX_WIDTH, "450");
	RB_LETVAR(PVAR_EDITBOX_HEIGHT, "300");

	RB_LETVAR(PVAR_LISTBOX_WIDTH, "150");
	RB_LETVAR(PVAR_LISTBOX_HEIGHT, "200");
	RB_LETVAR(PVAR_LISTBOX_OK, "&Ok");
	RB_LETVAR(PVAR_LISTBOX_MULTISEL, "-1");

	RB_LETVAR(PVAR_INFOBOX_WIDTH, "300");
	RB_LETVAR(PVAR_INFOBOX_HEIGHT, "24");
	RB_LETVAR(PVAR_INFOBOX_ENABLED, "-1");

	RB_LETVAR(PVAR_NEW_LINE, "\n");

	RB_LETVAR(PVAR_RB_VERSION, RBATCH_VERSION_INFO);
#ifdef PROFESSIONAL_COMPILE
	RB_LETVAR(PVAR_RB_PRO, "0");
#else
	RB_LETVAR(PVAR_RB_PRO, "-1");
#endif
	RB_NEWVAR(PVAR_SCREENWIDTH);
	RB_NEWVAR(PVAR_SCREENHEIGHT);
	RB_LETVAR(PVAR_CRLF, "\r\n");
	RB_LETVAR(PVAR_TAB, "\t");
	RB_LETVAR(PVAR_PIPE, "|");
	RB_LETVAR(PVAR_QUOT, "\'");

	RB_NEWVAR(PVAR_APP_ICON);

	RB_NEWVAR(PVAR_MOUSE_X);
	RB_NEWVAR(PVAR_MOUSE_Y);

	RB_LETVAR(PVAR_TRUE, "0");
	RB_LETVAR(PVAR_FALSE, "-1");

	RB_LETVAR(PVAR_STD_SEP, "|");

	RB_LETVAR(PVAR_INFOBOX_X, "0");
	RB_LETVAR(PVAR_INFOBOX_Y, "0");
	RB_LETVAR(PVAR_LISTBOX_X, "0");
	RB_LETVAR(PVAR_LISTBOX_Y, "0");
	RB_LETVAR(PVAR_INPUTBOX_X, "0");
	RB_LETVAR(PVAR_INPUTBOX_Y, "0");
	RB_LETVAR(PVAR_EDITBOX_X, "0");
	RB_LETVAR(PVAR_EDITBOX_Y, "0");

	RB_LETVAR(PVAR_CASE_SENSITIVITY, "-1");

	RB_NEWVAR(PVAR_XPSTYLEACTIVE);

	RB_NEWVAR(PVAR_METRIC_EDGE_WIDTH);
	RB_NEWVAR(PVAR_METRIC_EDGE_HEIGHT);
	RB_NEWVAR(PVAR_METRIC_CAPTION_HEIGHT);
	RB_NEWVAR(PVAR_METRIC_MENU_HEIGHT);

	RB_LETVAR(PVAR_EOL, "\r\n");
	//PRE-DECLARED VARIABLES --------------------------------------------END


	//Open file :) ---------------------------------------------------------
#ifdef RUNTIME_COMPILE
	GetModuleFileName(hInstance, openfile, STDLINE);
#else
#ifdef START_FILE
	strcpy(openfile, START_FILE);
#else
	strcpy(openfile, szCmdLine);
#endif
	if (strcmp(openfile,"") == 0)
	{
		return 0;
	}
#endif

	RB_LETVAR(PVAR_OWNNAME, openfile);

	if ((infile = fopen(openfile, "rb")) == (FILE*)NULL)
	{
		MessageBox(NULL, "RapidBATCH Runtime Error", "Fatal runtime error", 16);
		exit(0);
	}

#ifdef RUNTIME_COMPILE
	fseek(infile, 0L, SEEK_END);
	fseek(infile, ftell(infile) - sizeof(long int),SEEK_SET);

	fread(&seek_to, sizeof(long int), 1, infile);
#else
	fseek(infile, 0L, SEEK_END);
	seek_to = SEEKTO;
#endif

	if (seek_to == 0) return 0;

	//Get file length
	filecode = (char*)malloc(((ftell(infile) - seek_to) + 1) * sizeof(char));

#ifdef DEBUG_SESSION
	fprintf(debug, "filecode = %p %ld\n", filecode, ((ftell(infile) - seek_to) + 1));
#endif

	fseek(infile, seek_to, SEEK_SET);

	if (filecode == (char*)NULL)
	{
		MessageBox(NULL, "RapidBATCH Runtime Error", "Your system ran out of memory", 16);
		exit(0);
	}


	file_size = 0;
	while (!feof(infile))
	{
		char ch = fgetc(infile);

		if (feof(infile))
			break;

		filecode[file_size] = ch;

		file_size++;
	}

	file_size -= sizeof( long int );
	filecode[file_size] = '\0';
		
	fclose(infile);

	if( filecode[file_size-1] == '1' )
	{		
		char key[128+1];
		char* sp,
			* kp;
		long int cnt = 0;
		
		sprintf( key, "%ld", file_size - 1 );
		
		for( sp = filecode, kp = key; cnt < file_size; sp++, kp++, cnt++ )
		{
			if( *kp == '\0' )
				kp = key;
			
			*sp = *sp ^ *kp;
		}
	}
	
	
	FILE* x = fopen( "hallo.bla", "ab" );
	
	for( long n = 0; n < file_size; n++ )	
		fputc( filecode[n], x );
		
	fclose( x );
		
	
	filecode[file_size-1] = '\0';


#ifdef DEBUG_SESSION
	char debug_text[STDLINE+1];
	sprintf(debug_text, "RB5RTL :: DEBUG VERSION\n\nRB5RTL DEBUG_SESSION is ON\nFile: %s\ndebug_level: %d\n\nENJOY DEBUGGING ;)", dbg_file, debug_level);

	VARS(debug_text);
#endif

	res = RB_OK;
	while (!CURRENT_FILEEND && res != RB_TERMINATE)
	{
		res = rbrunExecute();

		for (int i = 0; i < TEMPVARS; i++)
		{
			if (TempStr[i] != (char*)NULL)
			{
				FREE(TempStr[i]);
			}
			TempStr[i] = (char*)NULL;
		}
		if (cmd != (char*)NULL)
		{
			FREE(cmd);
			cmd = (char*)NULL;
		}

#ifdef DEBUG_SESSION
		fprintf(debug, "--------------------------------- MALLOC STACK: %d\n", malloc_cnt);
		fflush(debug);
		malloc_cnt = 0;
#endif


		if (res == RB_ERROR)
			break;
		if (res == RB_OK)
			rbrunScan('\n', NULL);
	}

#ifdef DEBUG_SESSION
	printVar(&Var, dbg_file);
	fclose(debug);
#endif

	rbrunShutdown();

	return rb_return_code;
}

#ifndef NBATCH
//------------------------------------------------------------------------------
//DIALOG HELP FUNCTIONS
//------------------------------------------------------------------------------
bool rbDlg_CreateMenu(HWND win, char* win_name, char* menudef)
{
	if (GetMenu(win) != (HMENU)NULL
	        || strcmp(menudef, "") == 0)
	{
		DestroyMenu(GetMenu(win));
		SetMenu(win, (HMENU)NULL);

		for (int a = 0; a < dialog_info_cnt; a++)
		{
			if (dialog_info[a].handle == win
			        && dialog_info[a].window_type == WIN_MENU)
			{
				dialog_info[a].handle = (HWND)NULL;
				free(dialog_info[a].label);
			}
		}

		if (strcmp(menudef, "") == 0)
		{
			DrawMenuBar(win);
			return true;
		}

	}

	char* std_sep = (char*)NULL;
	rbrunGetVar(PVAR_STD_SEP, &std_sep, procStack_cnt);

	HMENU *levels;
	char** parent_menu = (char**)NULL;
	int parent_menu_cnt = 0;

	int levels_cnt = 0;
	int levels_pos = 0;
	char* menu_label = (char*)malloc(sizeof(char));
	*menu_label = '\0';

	levels = (HMENU*)malloc(sizeof(HMENU));
	levels[0] = CreateMenu();

	for (unsigned int a = 0; a < strlen(menudef); a++)
	{
		if (menudef[a] == ':')
		{
			if (parent_menu_cnt == 0)
			{
				parent_menu = (char**)malloc(sizeof(char**));
			}
			else
			{
				parent_menu = (char**)realloc((char**)parent_menu, (parent_menu_cnt+1)*sizeof(char**));
			}
			parent_menu[parent_menu_cnt] = (char*)malloc(strlen(menu_label)+1 * sizeof(char));
			(void)strcpy(parent_menu[parent_menu_cnt], menu_label);
			parent_menu_cnt++;

			free(menu_label);
			menu_label = (char*)malloc(sizeof(char));
			*menu_label = '\0';

			levels_pos++;
			levels_cnt++;

			levels = (HMENU*)realloc((HMENU*)levels, (levels_cnt + 1) * sizeof(HMENU));

			levels[levels_pos] = CreateMenu();

		}
		else if (menudef[a] == ';'
		         || menudef[a] == *std_sep)
		{
			if (*menu_label != '\0')
			{
				if (strcmp(menu_label, "-") != 0)
				{
					char* menu_tmp = (char*) malloc((strlen(win_name) +
					                                 strlen(":Menu_") + strlen(menu_label) + 1) * sizeof(char));
					sprintf(menu_tmp, "%s:Menu_%s", win_name, menu_label);

					int di_pos = rbDlg_CreateDialogInfo(menu_tmp);
					dialog_info[di_pos].handle = win;
					dialog_info[di_pos].window_type = WIN_MENU;

					free(menu_tmp);

					RB_DLG_LETEVENT(di_pos, DLGEVENT_CLICK, true);

					AppendMenu(levels[levels_pos], MF_STRING,
					           (di_pos + DIALOG_ID), menu_label);
				}
				else
				{
					AppendMenu(levels[levels_pos], MF_SEPARATOR, NULL, menu_label);
				}

				free(menu_label);
				menu_label = (char*)malloc(sizeof(char));
				*menu_label = '\0';
			}

			if (menudef[a] == ';')
			{
				if (levels_pos > 0)
				{
					AppendMenu(levels[levels_pos-1], MF_POPUP, (UINT)levels[levels_pos], parent_menu[parent_menu_cnt-1]);

					parent_menu_cnt--;
					if ((parent_menu_cnt) > 0)
					{
						parent_menu = (char**)realloc((char**)parent_menu, parent_menu_cnt*sizeof(char**));
					}
					else
					{
						free(parent_menu);

					}

				}
				levels_pos--;
			}
		}
		else
		{
			if (menudef[a] == '~' && a < strlen(menudef)-1) a++;

			menu_label = (char*)realloc((char*)menu_label, (strlen(menu_label)+1+1) * sizeof(char));
			menu_label[strlen(menu_label)+1] = '\0';
			menu_label[strlen(menu_label)] = menudef[a];
			//VARS(menu_label);
		}
	}
	SetMenu(win, levels[0]);
	DrawMenuBar(win);

	free(levels);
	free(std_sep);

	return true;
}


int rbDlg_CreateDialogInfo(char* label)
{
	int dialog_info_pos = -1;
	for (int x = 0; x < dialog_info_cnt; x++)
	{
		if (dialog_info[x].handle == (HWND)NULL)
		{
			dialog_info_pos = x;
			break;
		}
	}
	if (dialog_info_pos == -1)
	{
		if (dialog_info_cnt == 0)
		{
			dialog_info = (DIALOG_INFO*)malloc(sizeof(DIALOG_INFO));
		}
		else
		{
			dialog_info = (DIALOG_INFO*)realloc((DIALOG_INFO*)dialog_info, (dialog_info_cnt+1) * sizeof(DIALOG_INFO));
		}
		dialog_info_pos = dialog_info_cnt;
		dialog_info_cnt++;
	}

	(void)memset(&dialog_info[dialog_info_pos], 0, sizeof(DIALOG_INFO));

	dialog_info[dialog_info_pos].label = (char*)malloc((strlen(label)+1) * sizeof(char));
	(void)strcpy(dialog_info[dialog_info_pos].label, label);

	dialog_info[dialog_info_pos].extInfo_cnt = 0;
	dialog_info[dialog_info_pos].extInfo = (DIALOG_EXTINFO*)NULL;

	//dialog_info[dialog_info_pos].has_own_colors = false;

	return dialog_info_pos;
}

DIALOG_EXTINFO* rbDlg_CreateDialogExtInfo(int dialog_info_pos)
{
	int free_pos = -1;
	if (dialog_info[dialog_info_pos].extInfo_cnt == 0)
	{
		dialog_info[dialog_info_pos].extInfo = (DIALOG_EXTINFO*)malloc(sizeof(DIALOG_EXTINFO));
		free_pos = dialog_info[dialog_info_pos].extInfo_cnt;
	}
	else
	{
		for (int i = 0; i < dialog_info[dialog_info_pos].extInfo_cnt; i++)
		{
			if (dialog_info[dialog_info_pos].extInfo[i].ext_type == DIALOG_EXTINFO_VOID)
			{
				free_pos = i;
				break;
			}
		}

		if (free_pos == -1)
		{
			dialog_info[dialog_info_pos].extInfo = (DIALOG_EXTINFO*)realloc((DIALOG_EXTINFO*)dialog_info[dialog_info_pos].extInfo,
			                                       (dialog_info[dialog_info_pos].extInfo_cnt + 1) * sizeof(DIALOG_EXTINFO));
			free_pos = dialog_info[dialog_info_pos].extInfo_cnt;
		}
	}

	//Initialize everything to 0
	memset(&dialog_info[dialog_info_pos].extInfo[free_pos], 0, sizeof(DIALOG_EXTINFO));

	if (free_pos == dialog_info[dialog_info_pos].extInfo_cnt)
		dialog_info[dialog_info_pos].extInfo_cnt++;

	return &dialog_info[dialog_info_pos].extInfo[free_pos];
}

DIALOG_EXTINFO* rbDlg_GetDialogExtInfo(int dialog_info_pos, short type)
{
	if (dialog_info[dialog_info_pos].extInfo_cnt == 0)
	{
		return (DIALOG_EXTINFO*)NULL;
	}
	else
	{
		for (int i = 0; i < dialog_info[dialog_info_pos].extInfo_cnt; i++)
		{
			if (dialog_info[dialog_info_pos].extInfo[i].ext_type == type)
			{
				return &dialog_info[dialog_info_pos].extInfo[i];
			}
		}
	}

	return (DIALOG_EXTINFO*)NULL;
}

void rbDlg_DeleteDialogExtInfo(int dialog_info_pos, short type)
{
	if (type == -1)
	{
		//Delete all...
	}
	else
	{
		if (dialog_info[dialog_info_pos].extInfo_cnt == 0)
		{
			return;
		}
		else
		{
			for (int i = 0; i < dialog_info[dialog_info_pos].extInfo_cnt; i++)
			{
				if (dialog_info[dialog_info_pos].extInfo[i].ext_type == type)
				{
					if (dialog_info[dialog_info_pos].extInfo[i].ext_handle != (void*)NULL)
						free(dialog_info[dialog_info_pos].extInfo[i].ext_handle);
					if (dialog_info[dialog_info_pos].extInfo[i].ext_data != (void*)NULL)
						free(dialog_info[dialog_info_pos].extInfo[i].ext_data);

					dialog_info[dialog_info_pos].extInfo[i].ext_type = DIALOG_EXTINFO_VOID;
					return;
				}
			}
		}
	}

	return;
}


//------------------------------------------------------------------------------
//ADVANCED FUNCTIONS
//------------------------------------------------------------------------------
static int CALLBACK BrowseForFolderHook(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
		case BFFM_INITIALIZED:
			if (lpData)
				SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);

			break;
		default:
			break;
	}

	return(0);
}


BOOL BrowseForFolder(char* lpTitle, char* lpInitialFolder, char* lpFolder)
{
	ITEMIDLIST* pidl;
	BROWSEINFO bi;
	IMalloc* pmalloc = NULL;
	HRESULT hrIni, hr;
	char szName[MAX_PATH * 2];

	hrIni = CoInitialize(NULL);  // COM Laufzeitumgebung initialisieren

	if (SUCCEEDED((hr = SHGetMalloc(&pmalloc))))
	{
		bi.hwndOwner        = hInvisibleWindow;
		bi.pidlRoot         = NULL;
		bi.pszDisplayName   = szName;
		bi.lpszTitle        = lpTitle;
		bi.ulFlags          = 0;
		bi.lpfn             = BrowseForFolderHook;
		bi.lParam           = (LPARAM)lpInitialFolder;
		bi.iImage           = 0;

		if (NULL == (pidl = SHBrowseForFolder(&bi)))
		{
			hr = S_OK;
			lpFolder[0] = '\0';
		}
		else
		{
			if (!SHGetPathFromIDList(pidl, szName))
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			else
			{
				strcpy(lpFolder, szName);
				lpFolder[strlen(szName)] = '\0';
			}
			pmalloc->Free(pidl);
		}
		pmalloc->Release();
	}

	if (SUCCEEDED(hrIni))
		CoUninitialize();

	return(SUCCEEDED(hr));
}
#endif

bool readDirectory(char** file_list, char* filter, char* dirname, int depth, char* sep)
{
	WIN32_FIND_DATA fileinfo;
	char* newdirname = (char*)NULL;

	if (*file_list == (char*)NULL)
	{
		*file_list = (char*)malloc(sizeof(char));
		**file_list = '\0';
	}

	HANDLE hFiles = FindFirstFile(filter, &fileinfo);
	do
	{
		if (hFiles == INVALID_HANDLE_VALUE) break;
		if (strcmp(fileinfo.cFileName, ".") != 0 &&
		        strcmp(fileinfo.cFileName, "..") != 0)
		{
			*file_list = (char*)realloc((char*)*file_list,
			                            (strlen(*file_list) + strlen(dirname) + 1 + strlen(fileinfo.cFileName) +
			                             strlen(sep) + 1) * sizeof(char));

			if (*dirname != '\0')
			{
				strcat(*file_list, dirname);
				strcat(*file_list, "\\");
			}
			strcat(*file_list, fileinfo.cFileName);
			strcat(*file_list, sep);
		}
	}
	while (FindNextFile(hFiles, &fileinfo));

	if ((depth-1) > -1)
	{
		hFiles = FindFirstFile("*", &fileinfo);
		do
		{
			if (hFiles == INVALID_HANDLE_VALUE) break;

			DWORD dwAtts = GetFileAttributes(fileinfo.cFileName);

			if ((dwAtts & FILE_ATTRIBUTE_DIRECTORY) &&
			        strcmp(fileinfo.cFileName, ".") != 0 &&
			        strcmp(fileinfo.cFileName, "..") != 0 &&
			        !(dwAtts & FILE_ATTRIBUTE_SYSTEM))
			{
				newdirname = (char*)malloc((strlen(dirname) + 1 +
				                            strlen(fileinfo.cFileName) + 1) * sizeof(char));

				*newdirname = '\0';

				if (*dirname != '\0')
				{
					strcpy(newdirname, dirname);
					strcat(newdirname, "\\");
				}

				strcat(newdirname, fileinfo.cFileName);

				if (chdir(fileinfo.cFileName) == 0)
				{
					readDirectory(file_list, filter, newdirname, depth-1, sep);
					chdir("..");
				}

				if (newdirname != (char*)NULL)
					free(newdirname);
			}
		}
		while (FindNextFile(hFiles, &fileinfo));
	}
	FindClose(hFiles);
	return true;
}

//------------------------------------------------------------------------------
// GetOSName
//------------------------------------------------------------------------------
char* GetOSName()
{
	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&osinfo)) return "unknown";

	int major = osinfo.dwMajorVersion;
	int minor = osinfo.dwMinorVersion;
	int build = osinfo.dwBuildNumber;

	switch (osinfo.dwPlatformId)
	{
		case VER_PLATFORM_WIN32_WINDOWS :
			if (major == 4)
			{
				if (minor == 0 || minor == 3)
				{
					switch (LOWORD(build))
					{
						case  951:
							return "95A";
						case 1111:
						case 1212:
							return "95B";
						case 1214:
							return "95C";
					}
				}
				if (minor == 10)
				{
					switch (LOWORD(build))
					{
						case 1998:
							return "98";
						case 2000:
						case 2222:
							return "98SE";
					}
				}
				if (minor == 90)
				{
					switch (LOWORD(build))
					{
						case 3000:
							return "ME";
					}
				}
			}

		case VER_PLATFORM_WIN32_NT:

			if (major == 4) return "NT4";

			if (major == 5)
			{
				switch (minor)
				{
					case 0:
						if (build == 2195) return "2000";

					case 1:
						switch (build)
						{
							case 2505:
							case 2600:
								return "XP";
						}
					case 2:
						return "2003";


				}
			}

			if ( major == 6 )
			{
				return "Vista";
			}
	}
	return "unknown";
}


#ifndef NBATCH
void iconToTaskbar(HWND hWnd, HICON icon, char* tooltip, bool destroy)
{
	NOTIFYICONDATA tsym;

	ZeroMemory (&tsym, sizeof (NOTIFYICONDATA));

	if (!destroy)
	{
		tsym.cbSize = sizeof (NOTIFYICONDATA);
		tsym.hWnd   = hWnd;
		tsym.uID    = 1;
		tsym.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
		tsym.uCallbackMessage = WM_ICONCLICK;
		tsym.hIcon  = icon;
		strcpy (tsym.szTip, tooltip);
		Shell_NotifyIcon (NIM_ADD, &tsym);
	}
	else
	{
		tsym.cbSize = sizeof (NOTIFYICONDATA);
		tsym.hWnd   = hWnd;
		tsym.uID    = 1;
		tsym.uFlags = 0;
		Shell_NotifyIcon (NIM_DELETE, &tsym);
	}
}

//------------------------------------------------------------------------------
// BuildFont
//------------------------------------------------------------------------------
HFONT BuildFont(LPCTSTR lpszName, INT Size, WORD FontFormat)
{
	BOOL Bold, It, Ul, Strikeout;
	INT Weight;
	HFONT Font;
	Bold = FontFormat & FF_BOLD;
	It = FontFormat & FF_ITALIC;
	Ul = FontFormat & FF_UNDERLINE;
	Strikeout = FontFormat & FF_STRIKEOUT;
	Weight = Bold ? FW_BOLD : FW_NORMAL;
	Font = CreateFont(-MulDiv(Size, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72), 0, 0, 0, Weight, It, Ul, Strikeout, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, lpszName);
	return Font;
}

//------------------------------------------------------------------------------
// SetFont
//------------------------------------------------------------------------------
HFONT SetFont(HWND hwnd, LPCTSTR lpszName, int Size, WORD FontFormat)
{
	HFONT Font = BuildFont(lpszName, Size, FontFormat);
	SendMessage(hwnd, WM_SETFONT, (WPARAM)Font, 0);
	UpdateWindow(hwnd);
	return Font;
}

#endif

//------------------------------------------------------------------------------
//rbGetBuildInVar()
//------------------------------------------------------------------------------
inline bool rbGetBuildInVar(char* vname, char** value)
{
	//Because of performance problems, we will get and return the time/date
	//only if required
	if (strcmp(vname, PVAR_TIME) == 0)
	{
		//get time
		struct time tinfo;
		gettime(&tinfo);

		*value = (char*)MALLOC(((2+1+2+1+2)+1) * sizeof(char));

		sprintf(*value, "%02d:%02d:%02d", tinfo.ti_hour, tinfo.ti_min, tinfo.ti_sec);
		return true;
	}
	else if (strcmp(vname, PVAR_YEAR) == 0)
	{
		//get year
		struct date dinfo;
		getdate(&dinfo);

		*value = (char*)MALLOC((4+1) * sizeof(char));

		sprintf(*value, "%04d", dinfo.da_year);
		return true;
	}
	else if (strcmp(vname, PVAR_MONTH) == 0)
	{
		//get month
		struct date dinfo;
		getdate(&dinfo);

		*value = (char*)MALLOC((2+1) * sizeof(char));

		sprintf(*value, "%02d", dinfo.da_mon);
		return true;
	}
	else if (strcmp(vname, PVAR_DAY) == 0)
	{
		//get day
		struct date dinfo;
		getdate(&dinfo);

		*value = (char*)MALLOC((2+1) * sizeof(char));
		sprintf(*value, "%02d", dinfo.da_day);
		return true;
	}
	else if (strcmp(vname, PVAR_CLIPBOARD) == 0)
	{
		if (OpenClipboard(NULL))
		{
			if ((char*)GetClipboardData(CF_TEXT) != (char*) NULL)
			{
				*value = strdup((char*)GetClipboardData(CF_TEXT));
			}
			else
			{
				*value = (char*)MALLOC((1+1) * sizeof(char));
				**value = '\0';
			}
			CloseClipboard();
		}
		return true;
	}
	else if (strcmp(vname, PVAR_SCREENWIDTH) == 0)
	{
		*value = (char*)MALLOC((10+1) * sizeof(char));
		sprintf(*value, "%d", GetSystemMetrics(SM_CXSCREEN));
		return true;
	}
	else if (strcmp(vname, PVAR_SCREENHEIGHT) == 0)
	{
		*value = (char*)MALLOC((10+1) * sizeof(char));
		sprintf(*value, "%d", GetSystemMetrics(SM_CYSCREEN));
		return true;
	}
	else if (strcmp(vname, PVAR_ACTIVE_WINDOW) == 0)
	{
		HWND active = GetForegroundWindow();

		*value = (char*)MALLOC((STDLINE+1) * sizeof(char));
		if (active != (HWND)NULL)
		{
			GetWindowText(active, *value, STDLINE);
		}
		else
		{
			(void)strcpy(*value, "");
		}
		return true;
	}
	else if (strcmp(vname, PVAR_MOUSE_X) == 0)
	{
		POINT pt;
		*value = (char*)MALLOC((10+1) * sizeof(char));

		if (GetCursorPos(&pt))
		{
			sprintf(*value, "%ld", pt.x+1);
		}
		else
		{
			strcpy(*value, "-1");
		}
		return true;
	}
	else if (strcmp(vname, PVAR_MOUSE_Y) == 0)
	{
		POINT pt;
		*value = (char*)MALLOC((10+1) * sizeof(char));

		if (GetCursorPos(&pt))
		{
			sprintf(*value, "%ld", pt.y+1);
		}
		else
		{
			strcpy(*value, "-1");
		}
		return true;
	}
	else if (strcmp(vname, PVAR_XPSTYLEACTIVE) == 0)
	{
		*value = (char*)MALLOC((2+1) * sizeof(char));

		strcpy(*value, "-1");

		if (IsThemeActive != NULL)
		{
			if (IsThemeActive())
			{
				strcpy(*value, "0");
			}
		}
		return true;
	}
	else if (strcmp(vname, PVAR_METRIC_EDGE_WIDTH) == 0)
	{
		*value = (char*)MALLOC((10+1) * sizeof(char));

		sprintf(*value, "%d", GetSystemMetrics(SM_CXEDGE) );
		return true;
	}
	else if (strcmp(vname, PVAR_METRIC_EDGE_HEIGHT) == 0)
	{
		*value = (char*)MALLOC((10+1) * sizeof(char));

		sprintf(*value, "%d", GetSystemMetrics(SM_CYEDGE) );
		return true;
	}
	else if (strcmp(vname, PVAR_METRIC_CAPTION_HEIGHT) == 0)
	{
		*value = (char*)MALLOC((10+1) * sizeof(char));

		sprintf(*value, "%d", GetSystemMetrics(SM_CYCAPTION) );
		return true;
	}
	else if (strcmp(vname, PVAR_METRIC_MENU_HEIGHT) == 0)
	{
		*value = (char*)MALLOC((10+1) * sizeof(char));

		sprintf(*value, "%d", GetSystemMetrics(SM_CYMENU) );
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
//rbLetBuildInVar()
//------------------------------------------------------------------------------
inline bool rbLetBuildInVar(char* vname, char* value)
{
	if (strcmp(vname, PVAR_INPUTBOX_OK) == 0)
	{
		if (*value != '\0')
		{
			SendMessage(IB_OK, WM_SETTEXT,0, (long) value);
			ShowWindow(IB_OK, SW_SHOW);
		}
		else
		{
			ShowWindow(IB_OK, SW_HIDE);
		}
	}
	//Setzen der Buttonbeschriftungen für InputBox
	else if (strcmp(vname, PVAR_INPUTBOX_CANCEL) == 0)
	{
		if (*value != '\0')
		{
			SendMessage(IB_CANCEL, WM_SETTEXT,0, (long) value);
			ShowWindow(IB_CANCEL, SW_SHOW);
		}
		else
		{
			ShowWindow(IB_CANCEL, SW_HIDE);
		}
		vname = "InputBox_Cancel";
	}
	//Set button caption for EDITBOX
	else if (strcmp(vname, PVAR_EDITBOX_OK) == 0)
	{
		if (*value != '\0')
		{
			SendMessage(hEditBox_OK, WM_SETTEXT,0, (long) value);
			ShowWindow(hEditBox_OK, SW_SHOW);
		}
		else
		{
			ShowWindow(hEditBox_OK, SW_HIDE);
		}
	}
	else if (strcmp(vname, PVAR_EDITBOX_CANCEL) == 0)
	{
		if (*value != '\0')
		{
			SendMessage(hEditBox_Cancel, WM_SETTEXT,0, (long) value);
			ShowWindow(hEditBox_Cancel, SW_SHOW);
		}
		else
		{
			ShowWindow(hEditBox_Cancel, SW_HIDE);
		}
	}
	//Set button caption for LISTBOX
	else if (strcmp(vname, PVAR_LISTBOX_OK) == 0)
	{
		if (*value != '\0')
		{
			SendMessage(hListBox_OK, WM_SETTEXT,0, (long) value);
			ShowWindow(hListBox_OK, SW_SHOW);
		}
		else
		{
			ShowWindow(hListBox_OK, SW_HIDE);
		}
	}
	//Clipboard variable
	else if (strcmp(vname, PVAR_CLIPBOARD) == 0)
	{
		if (OpenClipboard(NULL))
		{
			HGLOBAL clipboardbuffer;
			char *clipboard_text;
			EmptyClipboard();
			clipboardbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(value)+1);
			clipboard_text = (char*)GlobalLock(clipboardbuffer);
			strcpy(clipboard_text, value);
			SetClipboardData(CF_TEXT,clipboardbuffer);
			GlobalUnlock(clipboardbuffer);
			CloseClipboard();
		}
	}
	else if (strcmp(vname, PVAR_INFOBOX_ENABLED) == 0)
	{
		strupr(value);

		if (strcmp(value, "-1") == 0
		        || strcmp(value, "HIDE") == 0
		        || strcmp(value, "HIDDEN") == 0)
		{
			ShowWindow(hInfoBox_Win, SW_HIDE);
			SetWindowPos(hInfoBox_Win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			UpdateWindow(hInfoBox_Win);
		}
		else
		{
			ShowWindow(hInfoBox_Win, SW_SHOW);
			UpdateWindow(hInfoBox_Win);
		}
	}
	else if (strcmp(vname, PVAR_ACTIVE_WINDOW) == 0)
	{
		HWND prog = FindWindow (NULL, value);

		if (prog != (HWND)NULL)
		{
			ShowWindow(prog, SW_SHOWNORMAL);
			SetForegroundWindow(prog);
		}
	}
	else if (strcmp(vname, PVAR_APP_ICON) == 0)
	{
		HICON app_icon = (HICON)NULL;
		if (*value != '\0')
		{
			app_icon = (HICON)LoadImage(hInstance, value, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		}

		if (app_icon == (HICON)NULL)
		{
			app_icon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(1000));
		}

		SetClassLong(IB_WIN, GCL_HICONSM, (long)app_icon);
		//DestroyIcon(app_icon);
	}
	else if (strcmp(vname, PVAR_MOUSE_X) == 0)
	{
		POINT pt;
		if (GetCursorPos(&pt))
		{
			SetCursorPos(atoi(value)+1,pt.y);
		}
	}
	else if (strcmp(vname, PVAR_MOUSE_Y) == 0)
	{
		POINT pt;
		if (GetCursorPos(&pt))
		{
			SetCursorPos(pt.x, atoi(value)+1);
		}
	}

	return false;
}

//------------------------------------------------------------------------------
//rbrunShutdown()
//------------------------------------------------------------------------------
void rbrunShutdown( void )
{
	free(filecode);

	//Shutdown procedure stack if available
	if (procStack_cnt > 0)
	{
		for (int i=0; i < procStack_cnt; i++)
		{
			if (procStack[i].proc_name != (char*)NULL)
				free(procStack[i].proc_name);
			if (procStack[i].ret_var != (char*)NULL)
				free(procStack[i].ret_var);

			for (int j = 0; j < procStack[i].var_cnt; j++)
			{
				free(procStack[i].vars[j]);
			}
		}
		free(procStack);
	}

	//Shutdown loop information system if available
	if (loop_cnt > 0)
	{
		for (int i=0; i < loop_cnt; i++)
		{
			if (loops[i].proc_name != (char*)NULL)
				free(loops[i].proc_name);
		}
		free(loops);
	}

	//Shutdown sub information system if available
	if (sub_cnt > 0)
	{
		free(subs);
	}

	//Shutdown procedure information system if available
	if (proc_cnt > 0)
	{
		for (int i=0; i < proc_cnt; i++)
		{
			if (procedures[i].proc_name != (char*)NULL)
				free(procedures[i].proc_name);
			for (int j = 0; j < procedures[i].parm_cnt; j++)
			{
				free(procedures[i].parms[j]);
			}
		}
		free(procedures);
	}

	//Shutdown debug variables
	if (debugvar_cnt > 0)
	{
		for (int i = 0; i < debugvar_cnt; i++)
			if (debugvar[i] != (char*)NULL) free(debugvar[i]);
		free(debugvar);
	}

	//Shutdown environment values
	if (env_values_cnt > 0)
	{
		for (int i = 0; i < env_values_cnt; i++)
			if (env_values[i] != (char*)NULL) free(env_values[i]);
		free(env_values);
	}

	//Shutdown dialog informations
	if (dialog_info_cnt > 0)
	{
		for (int i = 0; i < dialog_info_cnt; i++)
		{
			if (dialog_info[i].label != (char*)NULL) free(dialog_info[i].label);
		}

		free(dialog_info);
	}


	//Shutdown hotkey information system
	if (hotkey_info != (char**)NULL)
	{
		for (int i=0; i < hotkey_info_cnt; i++)
		{
			if (hotkey_info[i] != (char*)NULL)
			{
				UnregisterHotKey(hInvisibleWindow, i);
				if (hotkey_info[i] != (char*)NULL)
					free(hotkey_info[i]);
			}
		}
		free(hotkey_info);
	}

	//Shutdown label information system
	if (labels != (LINFO*)NULL)
	{
		free(labels);
	}

	//Externa DLL boundings
	if ( dll_cnt > 0 )
	{
		for ( int i=0; i < dll_cnt; i++ )
		{
			FreeLibrary( dll[i].dll_handle );
			free( dll[i].dll_name );
		}

		free( dll );
	}

	//Shutdown variable system
	shutdownVarSys(&Var);
}






