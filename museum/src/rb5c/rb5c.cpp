/*
J.M.K S.F. RapidBATCH v5.2 Compiler 2.4
Copyright (C) 2000-2007 by J.M.K S.F., Jan Max Meyer
*/

#include "rbc5.h"

#pragma argsused
#pragma hdrstop

//#define ICONPOS "000000"
#define BETA_COMPILE
#define RBRL_VERSION "2.2"
#define RB_VERSION "5.2"
#define RBC_VERSION "2.4"
bool createCOC3_Header = true;

#define NOSTOPCHAR 99
//#define START_FILE "C:\\Documents and Settings\\jmksf\\Desktop\\converter.rb"
//#define START_FILE "C:\\jmksf\\rb5\\rbctest1.rb"

#define LIBDIR "LIB"
#define DEFAULT_INCLUDE "LIB\\default.rb"
#define ETCDIR "ETC"
#define LANGFILE "rb5c.lng"

// Global variables
string codeline;
string cmd;
char symbreak;

string compilerpath;
string TempStr1;
string TempStr2;
string TempStr3;
string TempStr4;
string TempStr5;

string TempAdress;
string Procedure;
string declare;
string noequal;
string openfile;
string outfile;
string tempfile;
string tempcode;
string varcode;
string progicon = "none";
char tvarcode[30];
int IsSubMenu = 0;

char ch;

bool editorcall = false;
bool autolink = false;
bool runafterhalt = false;
bool encrypt = false;

int errorno = 0;
int lmres;
int extIf = 0;
float TempNum1;
float TempNum2;

int total_lines = 0;

bool behindscan;
//FILE *testfile;

FILE *cocfile = (FILE*)NULL;
unsigned long int start_pos = 0;

int loop_cnt = 0;

//File information structure ---------------------------------------------------
struct FINFO
{
	char* filename;
	FILE* file;
	int line;
};

int infiles_pos = 0;
FINFO* infiles = (FINFO*)NULL;

char** filenames;
int infiles_cnt = 0;

//Compiler internal variable information system --------------------------------
int rbcVarInfo_cnt = 0;
char** rbcVarInfo = (char**)NULL;

//Compiler internal label information system -----------------------------------
struct LINFO
{
	char* label;
	char* procedure;
	int line;
	int file_number;
};

LINFO* go_labels = (LINFO*)NULL;
int go_labels_cnt = 0;

LINFO* labels = (LINFO*)NULL;
int labels_cnt = 0;

//Compiler internal procedure information system -------------------------------
struct PINFO
{
	char* proc_name;
	char** parms;
	bool hasReturnValue;
	int parm_cnt;
};

PINFO* procinfo = (PINFO*)NULL;
PINFO* current_proc = (PINFO*)NULL;
int procinfo_cnt = 0;

char** once_included = (char**)NULL;
int once_included_cnt = 0;

//------------------------------------------------------------------------------
// rbcMapFile()
// Maps a file to memory.
//------------------------------------------------------------------------------
char* rbcMapFile( const char* filename )
{
	FILE*	f;
	char*	retstr		= (char*)NULL,
		*	strend;
	
	if( ( f = fopen( filename, "rb" ) ) != (FILE*)NULL )
	{
		fseek( f, 0L, SEEK_END );
		
		strend = retstr = (char*)malloc( ( ftell( f ) + 1 ) * sizeof( char ) );
		
		fseek( f, 0L, SEEK_SET );
		while( !feof( f ) )
		{
			*strend = fgetc( f );
			strend++;
		}		
		*(strend-1) = '\0';
		
		fclose( f );
	}
	
	return retstr;
}


//------------------------------------------------------------------------------
// rbcEncrypt()
// Encrypts a string using its length as the key.
//------------------------------------------------------------------------------
char* rbcEncrypt( char* str )
{
	char	key[ 255+1 ],
		*	sp,
		*	kp;
	
	sprintf( key, "%ld", strlen( str ) );
	
	for( sp = str, kp = key; ; sp++, kp++ )
	{
		if( *sp == '\0' )
			break;
		if( *kp == '\0' )
			kp = key;

		*sp = *sp ^ *kp;
	}
	
	return str;
}

//------------------------------------------------------------------------------
// rbcOpenSourceFile()
// Opens a new source input file in the stack
//------------------------------------------------------------------------------
bool rbcOpenSourceFile(string fname)
{
	if (infiles_pos == 0)
	{
		infiles = (FINFO*) malloc(sizeof(FINFO));
	}
	else
	{
		infiles = (FINFO*) realloc((FINFO*) infiles, (infiles_pos+1) * sizeof(FINFO));
	}

	//Remember the filename
	if ( infiles_cnt == 0 )
	{
		filenames = (char**)malloc(sizeof(char*));
	}
	else
	{
		filenames = (char**)realloc((char**)filenames, (infiles_cnt+1) * sizeof(char*));
	}
	filenames[infiles_cnt] = (char*)malloc((fname.length()+1) * sizeof(char));
	(void)strcpy(filenames[infiles_cnt], fname.c_str());

	infiles[infiles_pos].filename = filenames[infiles_cnt];

	infiles_cnt++;

	infiles[infiles_pos].file = fopen(infiles[infiles_pos].filename, "rb");
	if (infiles[infiles_pos].file == (FILE*) NULL)
	{
		if (infiles_pos == 0)
		{
			free(infiles);
		}
		else
		{
			infiles = (FINFO*) realloc((FINFO*) infiles, (infiles_pos) * sizeof(FINFO));
		}
		return false;
	}

	infiles_pos++;

	infiles_pos = infiles_pos;

	return true;
}

//------------------------------------------------------------------------------
// rbcCloseSourceFile()
// Closes and frees last source input file
//------------------------------------------------------------------------------
void rbcCloseSourceFile(void)
{
	if (infiles_pos == 0) return;

	infiles_pos--;

	fclose(infiles[infiles_pos].file);

	if (infiles_pos == 0)
	{
		free(infiles);
	}
	else
	{
		infiles = (FINFO*) realloc((FINFO*) infiles, (infiles_pos) * sizeof(FINFO));
	}
}

//------------------------------------------------------------------------------
// rbcFreeMemory()
// Free allocated memory
//------------------------------------------------------------------------------
void rbcFreeMemory(void)
{
	if (infiles_cnt > 0)
	{
		free(filenames);
	}

	if (labels != (LINFO*)NULL) free(labels);
	if (go_labels != (LINFO*)NULL) free(go_labels);
	if (procinfo != (PINFO*)NULL) free(procinfo);
}

//------------------------------------------------------------------------------
// rbcGetChar()
// gets character from current file position
//------------------------------------------------------------------------------
char rbcGetChar()
{
	char chr;
	chr = fgetc(infiles[infiles_pos-1].file);

	if (chr == '\r')
	{
		infiles[infiles_pos-1].line++;
		total_lines++;

		fgetc(infiles[infiles_pos-1].file);
		chr = '\n';
	}

	return chr;
}

// #############################################################################
// rbcError
// #############################################################################
void rbcError(int terrno)
{
	char errstr[STDLINE];
	printf("\n");
	switch (terrno)
	{
		case 0:
			strcpy 	(errstr, "Parse error");
			break;
		case 1:
			sprintf	(errstr, "Unknown instruction: %s", cmd.c_str());
			break;
		case 2:
			sprintf	(errstr, "Label not found/out of range: %s", TempStr1.c_str());
			break;
		case 3:
			sprintf	(errstr, "Double label definition: %s", TempStr1.c_str());
			break;
		case 4:
			sprintf	(errstr, "Double procedure definition: %s", TempStr1.c_str());
			break;
		case 5:
			sprintf	(errstr, "Include file not found: %s", TempStr1.c_str());
			break;
		case 6:
			strcpy 	(errstr, "REPEAT without ending UNTIL");
			break;
		case 7:
			strcpy 	(errstr, "Misplaced procedure definition.");
			break;
		case 8:
			strcpy 	(errstr, "UNTIL without beginning REPEAT.");
			break;
		case 9:
			strcpy 	(errstr, "Unknown conditional operation.");
			break;

		default:
			strcpy (errstr, "Unknown error; Errorcode");
			break;
	}

	int line = 1;

	fseek(infiles[infiles_pos-1].file, 0L, SEEK_SET);
	while (!feof(infiles[infiles_pos-1].file) && ftell(infiles[infiles_pos-1].file) < start_pos)
	{
		if (rbcGetChar() == '\n') line++;
	}

	printf("Error in \'%s\'\n(Line %d, Error %02d) %s\n\n", infiles[infiles_pos-1].filename, line, terrno, errstr);

	if (editorcall == false)
	{
		printf("Compiling process failed.\nPress any key...");
		getch();
	}

	fclose(cocfile);
	remove(outfile.c_str());
	rbcFreeMemory();
	exit(1);
}

//------------------------------------------------------------------------------
// rbcScanVar
// Scans and returns a variable name
//------------------------------------------------------------------------------
string rbcScanVar()
{
	int onVarFind = 0;
	string varname;

	rbcScanChar("[", true);

	while (!CURRENT_FILEEND)
	{
		char chr = rbcGetChar();
		switch (chr)
		{
			case '[':
				onVarFind++;
				break;
			case ']':
				if (onVarFind == 0) return varname;
				onVarFind--;
				break;
		}
		varname+=chr;

	}
	return varname;
}

//------------------------------------------------------------------------------
// rbcScanChar()
// scans file for character
//------------------------------------------------------------------------------
char rbcScanChar(char parseto[8+1], bool errormode)
{
	char chr;
	while (!CURRENT_FILEEND)
	{
		chr = rbcGetChar();
		for (int a = 0; a < strlen(parseto); a++)
		{
			if (chr == parseto[a]) return chr;
		}

		if (chr == '\n')
		{
			return '\n';
		}
		else if (chr != ' ' && chr != '\t')
		{
			if (errormode == true) rbcError(0);
			return chr;
		}
	}
	return (char)NULL;
}


//------------------------------------------------------------------------------
// rbcScan()
// scans file to character
//------------------------------------------------------------------------------
string rbcScan(char parseto)
{
	char chr;
	string restext;
	string str_chr;
	while (str_chr[0] != parseto && !CURRENT_FILEEND)
	{
		str_chr = rbcGetChar();
		if (str_chr == "\n") return restext;
		restext = restext + str_chr;
	}
	return restext.substr(0, restext.length() - 1);
}

// #############################################################################
// rbcVarName_Extract
// #############################################################################
string rbcVarName_Extract(string vname, string* dimvar_values, int* dimension_cnt)
{
	int dim_pos = 0;
	int onVarFind = 0;
	bool onConstFind = false;
	string result, tmp;
	char op = (char)NULL;

	//printf("Start %s ... \n",vname.c_str());
	for (int a=0; a<vname.length(); a++)
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
					if (onVarFind > 1) tmp += "[";
				}
				break;
			case ']':
				if (onVarFind == 1 && !onConstFind)
				{
					if (op == (char)NULL)
					{
						dimvar_values[dim_pos] = rbcGetVar(tmp, false);
						//printf("GetVar: %s => %s\n", tmp.c_str(),  dimvar_values[dim_pos].c_str());
						dim_pos++;
					}
					else
					{
						char op_convert[1+1];
						op_convert[0] = op;
						op_convert[1] = '\0';
						dimvar_values[dim_pos-1] += ((string)op_convert) + rbcGetVar(tmp, false);
						op = (char)NULL;
					}

					tmp = "";
					onVarFind--;
				}
				else if (onVarFind > 1)
				{
					tmp += "]";
					onVarFind--;
				}
				break;
			case '\'':
				if (!onConstFind && onVarFind == 0)
				{
					onConstFind = true;
					break;
				}
				else if (onConstFind)
				{
					if (op == (char)NULL)
					{
						dimvar_values[dim_pos] = "'" + tmp + "'";
						dim_pos++;
					}
					else
					{
						char op_convert[1+1];
						op_convert[0] = op;
						op_convert[1] = '\0';

						dimvar_values[dim_pos-1] += ((string)op_convert) + "'" + tmp + "'";
						op = (char)NULL;
					}

					tmp = "";
					onConstFind = false;
					break;
				}

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
					result += vname[a];
				else
					tmp += vname[a];
		}
		//printf("%d: %s %s @ %d\n", a, result.c_str(), tmp.c_str(), dim_pos);
	}
	*dimension_cnt = dim_pos;

	return result;
}

/*
//------------------------------------------------------------------------------
// rbcNewVar
// New variable
//------------------------------------------------------------------------------
string rbcNewVar(string vname)
{
        char code[STDLINE+1];
        char tmp_code[STDLINE+1];
        int index_values[MAXDIM];
        bool index_types[MAXDIM];

        vname = rbcVarName_Extract(vname, index_values, index_types);

        if(rbcVarInfo_cnt == 0)
        {
                rbcVarInfo = (char**) malloc(sizeof(char*));
        }
        else
        {
                rbcVarInfo = (char**) realloc((char**)rbcVarInfo, (rbcVarInfo_cnt+1) * sizeof(char*));
        }
        rbcVarInfo[rbcVarInfo_cnt] = (char*)malloc((vname.length()+1) * sizeof(char));
        (void)strcpy(rbcVarInfo[rbcVarInfo_cnt], vname.c_str());

        sprintf(code, "[%d", rbcVarInfo_cnt);
        printf("%d %d\n", rbcVarInfo_cnt, index_values[0]);
        for(int a = 1; a<=index_values[0]; a++)
        {
                if(index_types[a])
                        (void)sprintf(tmp_code, "[%d]", index_values[a]);
                else
                        (void)sprintf(tmp_code, "'%d'", index_values[a]);
                (void)strcat(code, tmp_code);
        }
        (void)strcat(code, "]");

        rbcVarInfo_cnt++;
        return code;
}
*/

//------------------------------------------------------------------------------
// rbcDelVar
// Delete a variable
//------------------------------------------------------------------------------
bool rbcDelVar(string vname)
{
	//vname = vname.substr(1, vname.length()-2);
	for (int a = rbcVarInfo_cnt-1; a > -1; a--)
	{
		if (strcmp(rbcVarInfo[a], vname.c_str()) == 0)
		{
			if (a < rbcVarInfo_cnt-1)
			{
				free(rbcVarInfo[a]);
				rbcVarInfo[a] = (char*)malloc((strlen(rbcVarInfo[rbcVarInfo_cnt-1])+1) * sizeof(char));
				(void)strcpy(rbcVarInfo[a], rbcVarInfo[rbcVarInfo_cnt-1]);
			}
			free(rbcVarInfo[rbcVarInfo_cnt-1]);
			if (rbcVarInfo_cnt-1 == 0)
			{
				free(rbcVarInfo);
			}
			else
			{
				rbcVarInfo = (char**) realloc((char**)rbcVarInfo, rbcVarInfo_cnt * sizeof(char*));
			}
			rbcVarInfo_cnt--;
			break;
		}
	}
	return true;
}


//------------------------------------------------------------------------------
// rbcGetVar
// Get code for a variable
//------------------------------------------------------------------------------
string rbcGetVar(string vname, bool force_createNew)
{
	//printf("rbcgetvar %s\n", vname.c_str());
	char code[STDLINE+1];
	char tmp_code[STDLINE+1];
	string index_values[MAXDIM];
	int dim_cnt = 0;
	bool isPointer = false;

	if (vname[0] == '*')
	{
		vname = vname.substr(1, vname.length());
		isPointer = true;
	}

	if (vname == "null")
	{
		return "[-1]";
	}

	vname = rbcVarName_Extract(vname, index_values, &dim_cnt);

	if (!force_createNew)
	{
		for (int a = rbcVarInfo_cnt-1; a >= 0; a--)
		{
			//printf("SEARCHING: %s %s\n", rbcVarInfo[a], vname.c_str());
			if (strcmp(rbcVarInfo[a], vname.c_str()) == 0)
			{
				if (isPointer)
					(void)sprintf(code, "[*%d", a);
				else
					(void)sprintf(code, "[%d", a);

				for (int a = 0; a<dim_cnt; a++)
				{
					(void)strcat(code, index_values[a].c_str());
				}
				(void)strcat(code, "]");

				//printf("found: %d %s %s\n", force_createNew, vname.c_str(), code);
				return code;
			}
		}
	}

	//Create new variable
	if (rbcVarInfo_cnt == 0)
	{
		rbcVarInfo = (char**) malloc(sizeof(char*));
	}
	else
	{
		rbcVarInfo = (char**) realloc((char**)rbcVarInfo, (rbcVarInfo_cnt+1) * sizeof(char*));
	}
	rbcVarInfo[rbcVarInfo_cnt] = (char*)malloc((vname.length()+1) * sizeof(char));
	(void)strcpy(rbcVarInfo[rbcVarInfo_cnt], vname.c_str());

	if (isPointer)
		(void)sprintf(code, "[*%d", rbcVarInfo_cnt);
	else
		(void)sprintf(code, "[%d", rbcVarInfo_cnt);

	for (int a = 0; a<dim_cnt; a++)
	{
		(void)strcat(code, index_values[a].c_str());
	}
	(void)strcat(code, "]");

	rbcVarInfo_cnt++;

	//printf("new: %d %s %s\n",force_createNew,  vname.c_str(), code);
	return code;
}

//------------------------------------------------------------------------------
// GetValue
// gets any ident from code
//------------------------------------------------------------------------------
string rbcGetValue(int allowUnknown)
{
	char chr;
	//char code[STDLINE+1];

	string multiValue;
	char op = (char)NULL;
	bool firstrun = true;

	do
	{
		if (firstrun == false) multiValue += op;

		// allowUnknown: 0 = No, 1 = Yes
		if (allowUnknown == 0)
		{
			chr = rbcScanChar("'[(", true);
		}
		else
		{
			chr = rbcScanChar("'[(", false);
		}

		if (chr == '\'')
		{
			multiValue += "'" + rbcScan('\'') + "'";
		}
		else if (chr == '[')
		{
			CHARACTER_BACK();
			multiValue += rbcGetVar(rbcScanVar(), false);
		}
		else if (chr == '(')
		{
			multiValue += "(" + rbcGetValue(0) + ")";
		}
		else
		{
			//Return whole line
			multiValue = chr + rbcScan('\n');
			multiValue = "'" + multiValue + "'";
			return multiValue;
		}
		firstrun = false;

		op = rbcScanChar("#=!<>&|+-/*~)", false);
	}
	while (op == '#' ||
	        op == '=' ||
	        op == '!' ||
	        op == '<' ||
	        op == '>' ||
	        op == '&' ||
	        op == '|' ||
	        op == '+' ||
	        op == '-' ||
	        op == '/' ||
	        op == '*' ||
	        op == '~');

	if (op != ')')
	{
		CHARACTER_BACK();
	}

	return multiValue;
}


// #############################################################################
// rbcGetSymbol
// #############################################################################
string rbcGetSymbol(void)
{
	char chr;
	string symbol;

	chr = rbcGetChar();
	while (chr == '\n' || chr == '\t' || chr == ' ')
	{
		chr = rbcGetChar();
	}


	start_pos = ftell(infiles[infiles_pos-1].file);

	do
	{
		if (chr == '%') return "%";
		if (chr == '[')
		{
			CHARACTER_BACK();
			symbol = "LET";
			break;
		}
		symbol = symbol + chr;


		chr = rbcGetChar();
	}
	while (chr != '\n' && chr != ' ' && chr != '\t' && !CURRENT_FILEEND);

	symbreak = chr;

	if (CURRENT_FILEEND) symbol = "END";
	//printf("sym %s %d: %s\n", infiles[infiles_pos-1].filename, infiles[infiles_pos-1].line, symbol.c_str());
	return symbol;
}


// #############################################################################
// rbcCompile
// #############################################################################
int rbcCompile(void)
{
	char chr;
	cmd = rbcGetSymbol();
	//printf("%s %d\n", cmd.c_str(), extIf);

	if (cmd == "%") //Label
	{
		TempStr1 = Trim(rbcScan('\n'));
		fprintf(cocfile, "RLBL%s", TempStr1.c_str());

		if (labels_cnt == 0)
		{
			labels = (LINFO*) malloc(sizeof(LINFO));
		}
		else
		{
			labels = (LINFO*) realloc((LINFO*)labels, (labels_cnt+1) * sizeof(LINFO));
		}
		labels[labels_cnt].label = (char*)malloc((TempStr1.length()+1) * sizeof(char));
		(void)strcpy(labels[labels_cnt].label, TempStr1.c_str());
		labels[labels_cnt].line = infiles[infiles_pos-1].line;
		labels[labels_cnt].file_number = infiles_pos;

		if (current_proc != (PINFO*)NULL)
		{
			labels[labels_cnt].procedure = (char*)malloc((strlen(current_proc->proc_name)+1) * sizeof(char));
			(void)strcpy(labels[labels_cnt].procedure, current_proc->proc_name);
		}
		else
		{
			labels[labels_cnt].procedure = (char*)malloc(sizeof(char));
			(void)strcpy(labels[labels_cnt].procedure, "");
		}

		labels_cnt++;

		return 0;
	}

	cmd = toUpper(cmd);

	// PROC/FUNC -----------------------------------------------------------
	if (cmd == "PROC" || cmd == "FUNC")
	{
		string procName;

		while (!CURRENT_FILEEND)
		{
			chr = rbcScanChar(":\n", false);
			if (chr != ':' && chr != '\n')
			{
				procName += chr;
			}
			else
			{
				break;
			}
		}

		procName = toUpper(Trim(procName));

		/*
		if(procinfo_cnt > 0)
		{
			for(int a = 0; a < procinfo_cnt; a++)
			{
				if(strcmp(procinfo[a].proc_name, procName.c_str()) == 0)
		                              {
		                                      TempStr1 = procName;
		                                      rbcError(4);
		                                      return RB_TERMINATE;
		                              }
		                      }
		}
		              if(procinfo_cnt == 0)
		{
		                      procinfo = (PINFO*)malloc(sizeof(PINFO));
		              }
		              else
		              {
			procinfo = (PINFO*)realloc((PINFO*) procinfo, (procinfo_cnt+1) * sizeof(PINFO));
		              }

		              procinfo[procinfo_cnt].proc_name = (char*) malloc((procName.length()+1) * sizeof(char));
		strcpy(procinfo[procinfo_cnt].proc_name, procName.c_str());

		procinfo[procinfo_cnt].parm_cnt = 0;



		              if(chr == ':')
		              {
		                      do
			{
				string varname = rbcScanVar();
		                              string var = rbcGetVar(varname, true);

				procinfo[procinfo_cnt].parms[procinfo[procinfo_cnt].parm_cnt] = (char*) malloc(varname.length()+1);
				strcpy(procinfo[procinfo_cnt].parms[procinfo[procinfo_cnt].parm_cnt], varname.c_str());

				procinfo[procinfo_cnt].parm_cnt++;
			}
			while (rbcScanChar(",", true) != '\n');
		}

		procinfo[procinfo_cnt].hasReturnValue = false;
		if(cmd == "FUNC") procinfo[procinfo_cnt].hasReturnValue = true;
		*/

		if (procinfo_cnt > 0)
		{
			for (int a = 0; a < procinfo_cnt; a++)
			{
				if (strcmp(procinfo[a].proc_name, procName.c_str()) == 0)
				{
					//Current procedure pointer
					current_proc = &procinfo[a];
					break;
				}
			}
		}

		if ( current_proc == (PINFO*)NULL )
			return RB_ERROR;

		//Write RBRL code
		if (current_proc->hasReturnValue)
			fprintf(cocfile, "FUNC'%s'",procName.c_str());
		else
			fprintf(cocfile, "PROC'%s'",procName.c_str());

		if (current_proc->parm_cnt > 0)
		{
			for (int a = 0; a < current_proc->parm_cnt; a++)
			{
				fprintf(cocfile, "%s", rbcGetVar(current_proc->parms[a], true).c_str());
			}
			rbcScan('\n');
		}
		else
		{
			fprintf(cocfile, "0");
		}

		if (loop_cnt > 0) rbcError(7);

		return RB_OK;
	}
	// ENDPROC/ENDFUNC -----------------------------------------------------
	if (cmd == "ENDPROC" || cmd == "ENDFUNC")
	{
		if (current_proc == (PINFO*)NULL)
		{
			return RB_TERMINATE;
		}
		for (int b = 0; b < current_proc->parm_cnt; b++)
		{
			if (current_proc->parms[b][0] == '*')
				rbcDelVar(current_proc->parms[b]+1);
			else
				rbcDelVar(current_proc->parms[b]);
		}

		//Write RBRL code
		if (current_proc->hasReturnValue)
			fprintf(cocfile, "ENDF");
		else
			fprintf(cocfile, "ENDP");



		current_proc = (PINFO*)NULL;

		if (loop_cnt > 0) rbcError(6);

		return RB_OK;
	}
	// RET -----------------------------------------------------------------
	if (cmd == "RET")
	{
		if (current_proc == (PINFO*)NULL)
		{
			return RB_TERMINATE;
		}

		if (current_proc->hasReturnValue)
		{
			if (rbcScanChar("'[(\n", true) == '\n')
			{
				fprintf(cocfile, "RRET''");
			}
			else
			{
				CHARACTER_BACK();
				fprintf(cocfile, "RRET%s", rbcGetValue(1).c_str());
			}
		}
		else
		{
			fprintf(cocfile, "RRET''");
		}
		return RB_OK;
	}
	// INCLUDE -------------------------------------------------------------
	if (cmd == "INCLUDE")
	{
		if (rbcScanChar("'", false) != '\'')
		{
			CHARACTER_BACK();
			TempStr1 = rbcScan('\n');
		}
		else
		{
			TempStr1 = rbcScan('\'');
		}

		if (!rbcOpenSourceFile(TempStr1))
		{
			if (TempStr1[0] != '\\')
			{
				/*
				string bla = compilerpath + "\\" + LIBDIR + "\\" + TempStr1;
				printf("My File: %s\n", bla.c_str());
				*/
				if (!rbcOpenSourceFile(compilerpath + "\\" + LIBDIR + "\\" + TempStr1))
					rbcError(5);
			}
			else
			{
				rbcError(5);
			}
		}
		return RB_OK_IGNORE;
	}
	// INCLUDE_ONCE --------------------------------------------------------
	if (cmd == "INCLUDE_ONCE")
	{
		if (rbcScanChar("'", false) != '\'')
		{
			CHARACTER_BACK();
			TempStr1 = rbcScan('\n');
		}
		else
		{
			TempStr1 = rbcScan('\'');
		}


		for (int i=0; i<once_included_cnt;i++)
		{
			if (strcmp(once_included[i], TempStr1.c_str()) == 0)
			{
				RB_OK_IGNORE;
			}
		}

		if (once_included_cnt == 0)
		{
			once_included = (char**)malloc(sizeof(char**));
		}
		else
		{
			once_included = (char**)realloc((char**)once_included,
			                                (once_included_cnt+1) * sizeof(char));
		}
		once_included[once_included_cnt] = strdup(TempStr1.c_str());
		once_included_cnt++;

		if (!rbcOpenSourceFile(TempStr1))
		{
			if (TempStr1[0] != '\\')
			{
				if (!rbcOpenSourceFile(compilerpath + "\\" + LIBDIR + "\\" + TempStr1)) rbcError(5);
			}
			else
			{
				rbcError(5);
			}
		}
		return RB_OK_IGNORE;
	}
	// DEC -----------------------------------------------------------------
	if (cmd == "DEC")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		fprintf(cocfile, "R000%s", TempStr1.c_str());
		while (rbcScanChar(",", true) != '\n')
		{
			fprintf(cocfile, "%s", rbcGetVar(rbcScanVar(), false).c_str());
		}
		return RB_OK;
	}
	// RESET ---------------------------------------------------------------
	if (cmd == "RESET")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		fprintf(cocfile, "R052%s", TempStr1.c_str());

		while (rbcScanChar(",", true) != '\n')
		{
			fprintf(cocfile, "%s", rbcGetVar(rbcScanVar(), false).c_str());
		}
		return RB_OK;
	}
	// LET -----------------------------------------------------------------
	if (cmd == "LET")
	{
		unsigned long start = ftell(infiles[infiles_pos-1].file);
		TempStr1 = rbcGetVar(rbcScanVar(), false);

		chr = rbcScanChar("=#+-/*~", true);

		switch (chr)
		{
			case '#':
			case '+':
			case '-':
			case '*':
			case '/':
			case '~':
				fseek(infiles[infiles_pos-1].file, start, SEEK_SET);
		}

		TempStr2 = rbcGetValue(1);

		fprintf(cocfile, "R001%s%s", TempStr1.c_str(), TempStr2.c_str());
		return RB_OK;
	}
	// GOTO ----------------------------------------------------------------
	if (cmd == "GOTO")
	{
		TempStr1 = Trim(rbcGetValue(1));
		if (toUpper(TempStr1) == "") rbcError(0);

		if (go_labels_cnt == 0)
		{
			go_labels = (LINFO*) malloc(sizeof(LINFO));
		}
		else
		{
			go_labels = (LINFO*) realloc((LINFO*)go_labels, (go_labels_cnt+1) * sizeof(LINFO));
		}
		go_labels[go_labels_cnt].label = (char*)malloc((TempStr1.length()+1) * sizeof(char));
		(void)strcpy(go_labels[go_labels_cnt].label, TempStr1.c_str());

		go_labels[go_labels_cnt].line = infiles[infiles_pos-1].line;
		go_labels[go_labels_cnt].file_number = infiles_pos;

		if (current_proc != (PINFO*)NULL)
		{
			go_labels[go_labels_cnt].procedure = (char*)malloc((strlen(current_proc->proc_name)+1) * sizeof(char));
			(void)strcpy(go_labels[go_labels_cnt].procedure, current_proc->proc_name);
		}
		else
		{
			go_labels[go_labels_cnt].procedure = (char*)malloc(sizeof(char));
			(void)strcpy(go_labels[go_labels_cnt].procedure, "");
		}

		go_labels_cnt++;
		fprintf(cocfile, "R004%s", TempStr1.c_str());
		return RB_OK;
	}
	// GOSUB ---------------------------------------------------------------
	if (cmd == "GOSUB")
	{
		TempStr1 = Trim(rbcGetValue(1));
		if (toUpper(TempStr1) == "") rbcError(0);

		if (go_labels_cnt == 0)
		{
			go_labels = (LINFO*) malloc(sizeof(LINFO));
		}
		else
		{
			go_labels = (LINFO*) realloc((LINFO*)go_labels, (go_labels_cnt+1) * sizeof(LINFO));
		}
		go_labels[go_labels_cnt].label = (char*)malloc((TempStr1.length()+1) * sizeof(char));
		(void)strcpy(go_labels[go_labels_cnt].label, TempStr1.c_str());

		go_labels[go_labels_cnt].line = infiles[infiles_pos-1].line;
		go_labels[go_labels_cnt].file_number = infiles_pos;

		go_labels[go_labels_cnt].procedure = (char*)NULL;

		if (current_proc != (PINFO*)NULL)
		{
			go_labels[go_labels_cnt].procedure = (char*)malloc((strlen(current_proc->proc_name)+1) * sizeof(char));
			(void)strcpy(go_labels[go_labels_cnt].procedure, current_proc->proc_name);
		}
		else
		{
			go_labels[go_labels_cnt].procedure = (char*)malloc(sizeof(char));
			(void)strcpy(go_labels[go_labels_cnt].procedure, "");
		}

		go_labels_cnt++;

		fprintf(cocfile, "R005%s", TempStr1.c_str());
		return RB_OK;
	}
	// RESUME --------------------------------------------------------------
	if (cmd == "RESUME")
	{
		fprintf(cocfile, "R006");
		return RB_OK;
	}
	// IF ------------------------------------------------------------------
	if (cmd == "IF")
	{
		TempStr1 = rbcGetValue(0);

		if (rbcScanChar("\n", false) != '\n')
		{
			CHARACTER_BACK();

			//Standard IF
			fprintf(cocfile, "R0IF%s", TempStr1.c_str());
			return rbcCompile();
		}

		//Extended IF
		fprintf(cocfile, "RXIF%s", TempStr1.c_str());
		extIf++;

		return RB_OK;
	}
	// ENDIF ---------------------------------------------------------------
	if (cmd == "ENDIF")
	{
		if (extIf < 1)
		{
			rbcError(10);
			return 1;
		}
		fprintf(cocfile, "RXEI");
		extIf--;
		return RB_OK;
	}
	// ELSE ----------------------------------------------------------------
	if (cmd == "ELSE")
	{
		if (extIf < 1)
		{
			rbcError(10);
			return 1;
		}
		fprintf(cocfile, "RXEL");
		return RB_OK;
	}
	// ELSEIF --------------------------------------------------------------
	if (cmd == "ELSEIF")
	{
		if (extIf < 1)
		{
			rbcError(10);
			return 1;
		}

		//Extended IF
		fprintf(cocfile, "RXIE%s", rbcGetValue(0).c_str());
		return RB_OK;
	}
	// REPEAT --------------------------------------------------------------
	if (cmd == "REPEAT")
	{
		fprintf(cocfile, "RRPT");

		loop_cnt++;

		return RB_OK;
	}
	// BREAK ---------------------------------------------------------------
	if (cmd == "BREAK")
	{
		fprintf(cocfile, "RBRK");
		return RB_OK;
	}
	// CONT ----------------------------------------------------------------
	if (cmd == "CONT")
	{
		fprintf(cocfile, "RCNT");
		return RB_OK;
	}
	// UNTIL ---------------------------------------------------------------
	if (cmd == "UNTIL")
	{
		fprintf(cocfile, "RUNT%s",  rbcGetValue(0).c_str());

		if (loop_cnt == 0) rbcError(8);
		loop_cnt--;
		return RB_OK;
	}
	// MSGBOX --------------------------------------------------------------
	if (cmd == "MSGBOX" || cmd == "ECHOBOX")
	{
		unsigned long start = ftell(infiles[infiles_pos-1].file);

		TempStr1 = rbcGetValue(0);
		if (rbcScanChar(",=", true) == '=')
		{
			//msgbox as a function
			fseek(infiles[infiles_pos-1].file, start, SEEK_SET);
			TempStr1 = rbcGetVar(rbcScanVar(), false);
			(void)rbcScanChar("=", true);
			TempStr2 = rbcGetValue(0);
			(void)rbcScanChar(",", true);
			TempStr3 = rbcGetValue(0);
			(void)rbcScanChar(",", true);
			fprintf(cocfile, "R063%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(),TempStr3.c_str(), rbcGetValue(0).c_str());
		}
		else
		{
			//msgbox as instruction
			TempStr2 = rbcGetValue(0);
			(void)rbcScanChar(",", true);
			TempStr3 = rbcGetValue(0);
			fprintf(cocfile, "R007%s%s%s", TempStr1.c_str(), TempStr2.c_str(),TempStr3.c_str());
		}

		(void)rbcScanChar("\n", true);
		return RB_OK;
	}
	// INPUTBOX ------------------------------------------------------------
	if (cmd == "INPUTBOX")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr4 = rbcGetValue(0);
		rbcScanChar("\n", true);

		fprintf(cocfile, "R008%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(),TempStr3.c_str(),TempStr4.c_str());
		return RB_OK;
	}
	// PWDBOX --------------------------------------------------------------
	if (cmd == "PWDBOX")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr4 = rbcGetValue(0);
		rbcScanChar("\n", true);

		fprintf(cocfile, "R041%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(),TempStr3.c_str(),TempStr4.c_str());
		return RB_OK;
	}
	// EDITBOX -------------------------------------------------------------
	if (cmd == "EDITBOX")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr4 = rbcGetValue(0);
		rbcScanChar("\n", true);

		fprintf(cocfile, "R032%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(),TempStr3.c_str(),TempStr4.c_str());
		return RB_OK;
	}
	// LISTBOX -------------------------------------------------------------
	if (cmd == "LISTBOX")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar("\n", true);

		fprintf(cocfile, "R031%s%s%s", TempStr1.c_str(), TempStr2.c_str(),TempStr3.c_str());
		return RB_OK;
	}
	// TRAYMENU ------------------------------------------------------------
	if (cmd == "TRAYMENU")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar("\n", true);

		fprintf(cocfile, "R058%s%s%s", TempStr1.c_str(), TempStr2.c_str(),TempStr3.c_str());
		return RB_OK;
	}
	// HALT ----------------------------------------------------------------
	if (cmd == "HALT")
	{
		fprintf(cocfile, "RHLT");

		/*
		if( rbcScanChar("\n", false) != '\n' )
		{
			CHARACTER_BACK();
			fprintf(cocfile, "%s", rbcGetValue(1).c_str() );
		}
		*/

		return RB_OK;
	}
	// LINKVAR -------------------------------------------------------------
	if (cmd == "LINKVAR")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);

		rbcScanChar("=", true);

		TempStr2 = "";
		while (rbcScanChar("+,", true) != '\n')
		{
			TempStr2 += rbcGetValue(0);
			switch (rbcScanChar("+,", false))
			{
				case '+':
				case ',':
					TempStr2 += "#";
			}
			CHARACTER_BACK();
		}

		fprintf(cocfile, "R001%s%s", TempStr1.c_str(), TempStr2.c_str());
		return RB_OK;
	}
	// CALCVAR -------------------------------------------------------------
	if (cmd == "CALCVAR")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);

		rbcScanChar("=", true);

		fprintf(cocfile, "R001%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// SHELL ---------------------------------------------------------------
	if (cmd == "SHELL")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar("\n", true);

		fprintf(cocfile, "R010%s%s", TempStr1.c_str(), TempStr2.c_str());
		return RB_OK;
	}
	// CHDIR ---------------------------------------------------------------
	if (cmd == "CHDIR")
	{
		fprintf(cocfile, "R011%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// MKDIR ---------------------------------------------------------------
	if (cmd == "MKDIR")
	{
		fprintf(cocfile, "R012%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// RMDIR ---------------------------------------------------------------
	if (cmd == "RMDIR")
	{
		fprintf(cocfile, "R013%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// CALL ----------------------------------------------------------------
	if (cmd == "CALL")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar("\n", true);

		fprintf(cocfile, "R030%s%s", TempStr1.c_str(), TempStr2.c_str());
		return RB_OK;
	}
	// SHORTCUT ------------------------------------------------------------
	if (cmd == "SHORTCUT")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(":,", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr4 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr5 = rbcGetValue(0);
		rbcScanChar(",", true);


		fprintf(cocfile, "R065%s%s%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(),
		        TempStr3.c_str(), TempStr4.c_str(), TempStr5.c_str(), rbcGetValue(0).c_str());

		rbcScanChar("\n", true);
		return RB_OK;
	}
	// SYSTEM --------------------------------------------------------------
	if (cmd == "SYSTEM")
	{
		fprintf(cocfile, "R014%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// UPVAR ---------------------------------------------------------------
	if (cmd == "UPVAR")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "RF03%s%s", TempStr1.c_str(), rbcGetValue(1).c_str());
		return RB_OK;
	}
	// LOWVAR --------------------------------------------------------------
	if (cmd == "LOWVAR")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "RF04%s%s", TempStr1.c_str(), rbcGetValue(1).c_str());
		return RB_OK;
	}
	// TRIMVAR -------------------------------------------------------------
	if (cmd == "TRIMVAR")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "RF05%s%s", TempStr1.c_str(), rbcGetValue(1).c_str());
		return RB_OK;
	}
	// GETCHR --------------------------------------------------------------
	if (cmd == "GETCHR")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "RF06%s%s", TempStr1.c_str(), rbcGetValue(1).c_str());
		return RB_OK;
	}
	// GETASC --------------------------------------------------------------
	if (cmd == "GETASC")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "RF12%s%s", TempStr1.c_str(), rbcGetValue(1).c_str());
		return RB_OK;
	}
	// GETLEN --------------------------------------------------------------
	if (cmd == "GETLEN")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "RF07%s%s", TempStr1.c_str(), rbcGetValue(1).c_str());
		return RB_OK;
	}
	// GETENV --------------------------------------------------------------
	if (cmd == "GETENV")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "R015%s%s", TempStr1.c_str(), rbcGetValue(1).c_str());
		return RB_OK;
	}
	// LETENV --------------------------------------------------------------
	if (cmd == "LETENV")
	{
		fprintf(cocfile, "R016%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// COPYVAR -------------------------------------------------------------
	if (cmd == "COPYVAR")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(":,", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr4 = rbcGetValue(0);
		fprintf(cocfile, "RF08%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(), TempStr3.c_str(), TempStr4.c_str());
		return RB_OK;
	}
	// GETCHARAT -----------------------------------------------------------
	if (cmd == "GETCHARAT")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(":,", true);

		fprintf(cocfile, "RF13%s%s%s", TempStr1.c_str(), TempStr2.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// RANDVAR -------------------------------------------------------------
	if (cmd == "RANDVAR")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "RF09%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// DELFILE -------------------------------------------------------------
	if (cmd == "DELFILE")
	{
		fprintf(cocfile, "R017%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// RENAMEFILE ----------------------------------------------------------
	if (cmd == "RENAMEFILE")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(":,", true);
		fprintf(cocfile, "R018%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// FILEEXISTS ----------------------------------------------------------
	if (cmd == "FILEEXISTS")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);

		fprintf(cocfile, "R019%s%s", TempStr1.c_str(),  rbcGetValue(1).c_str());
		return RB_OK;
	}
	// GETPOS --------------------------------------------------------------
	if (cmd == "GETPOS")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(":,", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);

		fprintf(cocfile, "RF02%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(), TempStr3.c_str(), rbcGetValue(0).c_str());
		return RB_OK;

	}
	// GETTOK --------------------------------------------------------------
	if (cmd == "GETTOK")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(":,", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);

		fprintf(cocfile, "RF10%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(), TempStr3.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// REPLACEVAR ----------------------------------------------------------
	if (cmd == "REPLACEVAR")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(":,", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);

		fprintf(cocfile, "RF11%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(), TempStr3.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// CNTVAR --------------------------------------------------------------
	if (cmd == "CNTVAR")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(":,", true);

		fprintf(cocfile, "RF14%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// EXTVAR --------------------------------------------------------------
	if (cmd == "EXTVAR")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(":,", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr4 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr5 = rbcGetValue(0);
		rbcScanChar(",", true);

		fprintf(cocfile, "RF15%s%s%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(), TempStr3.c_str(), TempStr4.c_str(), TempStr5.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// OPEN ----------------------------------------------------------------
	if (cmd == "OPEN")
	{
		fprintf(cocfile, "R020%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// NEWFILE -------------------------------------------------------------
	if (cmd == "NEWFILE")
	{
		fprintf(cocfile, "R021%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// OPENBOX/SAVEBOX -----------------------------------------------------
	if (cmd == "OPENBOX" || cmd == "SAVEBOX")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);

		if (cmd == "OPENBOX")
		{
			fprintf(cocfile, "R022%s%s%s", TempStr1.c_str(), TempStr2.c_str(), rbcGetValue(0).c_str());
		}
		else
		{
			fprintf(cocfile, "R023%s%s%s", TempStr1.c_str(), TempStr2.c_str(), rbcGetValue(0).c_str());
		}
		return RB_OK;
	}
	// READFILE ------------------------------------------------------------
	if (cmd == "READFILE")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(":,", true);
		fprintf(cocfile, "R024%s%s%s", TempStr1.c_str(), TempStr2.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// WRITEFILE -----------------------------------------------------------
	if (cmd == "WRITEFILE")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(":,", true);
		fprintf(cocfile, "R025%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// GETFILE -------------------------------------------------------------
	if (cmd == "GETFILE")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(":,", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);
		fprintf(cocfile, "R051%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(), TempStr3.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// PUTFILE -------------------------------------------------------------
	if (cmd == "PUTFILE")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(":,", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);
		fprintf(cocfile, "R050%s%s%s", TempStr1.c_str(), TempStr2.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// COPYFILE ------------------------------------------------------------
	if (cmd == "COPYFILE")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(":,", true);
		fprintf(cocfile, "R026%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// END -----------------------------------------------------------------
	if (cmd == "END")
	{
		if (infiles_pos-1 > 0)
		{
			rbcCloseSourceFile();
			return RB_OK_IGNORE;
		}
		else
		{
			return RB_TERMINATE;
		}
	}
	// STDOUT/STDIN/STDERR -------------------------------------------------
	if (cmd == "STDOUT" || cmd == "STDIN" || cmd == "STDERR")
	{
		if (cmd == "STDOUT") fprintf(cocfile, "R060");
		if (cmd == "STDIN")  fprintf(cocfile, "R061");
		if (cmd == "STDERR") fprintf(cocfile, "R062");

		fprintf(cocfile, "%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// ECHO/ECHOLN ---------------------------------------------------------
	if (cmd == "ECHO" || cmd == "ECHOLN")
	{
		fprintf(cocfile, "R027%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// CONFIRM -------------------------------------------------------------
	if (cmd == "CONFIRM")
	{

		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "R028%s%s", TempStr1.c_str(), rbcGetValue(1).c_str());
		return RB_OK;
	}
	// MCI -----------------------------------------------------------------
	if (cmd == "MCI")
	{
		fprintf(cocfile, "R029%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// LETREGVAL -----------------------------------------------------------
	if (cmd == "LETREGVAL")
	{
		TempStr4 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",:", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);

		fprintf(cocfile, "R053%s%s%s%s%s", TempStr4.c_str(), TempStr1.c_str(), TempStr2.c_str(),TempStr3.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// GETREGVAL -----------------------------------------------------------
	if (cmd == "GETREGVAL")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr4 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",:", true);
		TempStr3 = rbcGetValue(0);
		rbcScanChar(",", true);


		fprintf(cocfile, "R064%s%s%s%s%s", TempStr1.c_str(), TempStr4.c_str(), TempStr2.c_str(),TempStr3.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// DELREGVAL -----------------------------------------------------------
	if (cmd == "DELREGVAL")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",:", true);

		fprintf(cocfile, "R054%s%s%s%s", TempStr1.c_str(), TempStr2.c_str(),TempStr3.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// NEWREGKEY -----------------------------------------------------------
	if (cmd == "NEWREGKEY")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",:", true);

		fprintf(cocfile, "R055%s%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// DELREGKEY -----------------------------------------------------------
	if (cmd == "DELREGKEY")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",:", true);

		fprintf(cocfile, "R056%s%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// SENDKEYS ------------------------------------------------------------
	if (cmd == "SENDKEYS")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",:", true);
		fprintf(cocfile, "R033%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// SENDKEYS ------------------------------------------------------------
	if (cmd == "MOUSEEVENT")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",", true);

		fprintf(cocfile, "R059%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// FINDFILES -----------------------------------------------------------
	if (cmd == "FINDFILES")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",:", true);
		fprintf(cocfile, "R034%s%s%s", TempStr1.c_str(), TempStr2.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// DISKFREE ------------------------------------------------------------
	if (cmd == "DISKFREE")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "R042%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// DISKSIZE ------------------------------------------------------------
	if (cmd == "DISKSIZE")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "R057%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// DISKTYPE ------------------------------------------------------------
	if (cmd == "DISKTYPE")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "R043%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// SHUTDOWN ------------------------------------------------------------
	if (cmd == "SHUTDOWN")
	{
		fprintf(cocfile, "R044%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// LOCKWORKSTATION -----------------------------------------------------
	if (cmd == "LOCKWORKSTATION")
	{
		fprintf(cocfile, "R045");
		return RB_OK;
	}
	// GETFILEDATE ---------------------------------------------------------
	if (cmd == "GETFILEDATE")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "R046%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// LETFILEDATE ---------------------------------------------------------
	if (cmd == "LETFILEDATE")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",:", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar("\n", true);

		fprintf(cocfile, "R047%s%s", TempStr1.c_str(),TempStr2.c_str());
		return RB_OK;
	}
	// GETFILETIME ---------------------------------------------------------
	if (cmd == "GETFILETIME")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "R048%s%s", TempStr1.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// LETFILEDATE ---------------------------------------------------------
	if (cmd == "LETFILETIME")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",:", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar("\n", true);

		fprintf(cocfile, "R049%s%s", TempStr1.c_str(),TempStr2.c_str());
		return RB_OK;
	}
	// FILESIZE ------------------------------------------------------------
	if (cmd == "FILESIZE")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		fprintf(cocfile, "R035%s%s", TempStr1.c_str(), rbcGetValue(1).c_str());
		return RB_OK;
	}
	// WAIT ----------------------------------------------------------------
	if (cmd == "WAIT")
	{
		fprintf(cocfile, "R036%s", rbcGetValue(1).c_str());
		return RB_OK;
	}
	// FOLDERBOX -----------------------------------------------------------
	if (cmd == "FOLDERBOX")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr3 = rbcGetValue(0);

		fprintf(cocfile, "R037%s%s%s", TempStr1.c_str(), TempStr2.c_str(), TempStr3.c_str());
		return RB_OK;
	}
	// INFOBOX -------------------------------------------------------------
	if (cmd == "INFOBOX")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar("\n", true);

		fprintf(cocfile, "R038%s%s", TempStr1.c_str(), TempStr2.c_str());
		return RB_OK;
	}
	// GETFILEATT ----------------------------------------------------------
	if (cmd == "GETFILEATT")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",:", true);
		TempStr3 = rbcGetValue(0);

		fprintf(cocfile, "R039%s%s%s", TempStr1.c_str(), TempStr2.c_str(), TempStr3.c_str());
		return RB_OK;
	}
	// LETFILEATT ----------------------------------------------------------
	if (cmd == "LETFILEATT")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",:", true);
		TempStr2 = rbcGetValue(0);

		fprintf(cocfile, "R040%s%s", TempStr1.c_str(), TempStr2.c_str());
		return RB_OK;
	}

	// NEWDIALOG -----------------------------------------------------------
	if (cmd == "NEWDIALOG")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",:", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);


		fprintf(cocfile, "RD00%s%s%s", TempStr1.c_str(), TempStr2.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// LETDIALOG -----------------------------------------------------------
	if (cmd == "LETDIALOG")
	{
		TempStr1 = rbcGetValue(0);
		rbcScanChar(",:", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);


		fprintf(cocfile, "RD01%s%s%s", TempStr1.c_str(), TempStr2.c_str(), rbcGetValue(0).c_str());
		return RB_OK;
	}
	// GETDIALOG -----------------------------------------------------------
	if (cmd == "GETDIALOG")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",:", true);
		TempStr3 = rbcGetValue(0);

		fprintf(cocfile, "RD02%s%s%s", TempStr1.c_str(), TempStr2.c_str(), TempStr3.c_str());
		return RB_OK;
	}
	// RUNDIALOG -----------------------------------------------------------
	if (cmd == "RUNDIALOG")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);

		fprintf(cocfile, "RD03%s%s", TempStr1.c_str(), rbcGetValue(1).c_str());
		return RB_OK;
	}
	// DELDIALOG -----------------------------------------------------------
	if (cmd == "DELDIALOG")
	{
		fprintf(cocfile, "RD04%s", rbcGetValue(1).c_str());
		return RB_OK;
	}


	// REM / DEBUGVARS / DEBUGMODE -----------------------------------------
	if (cmd == "REM" || cmd == "DEBUGVARS" || cmd == "DEBUGMODE")
	{
		CHARACTER_BACK();
		if (rbcScanChar("\n", false) != '\n') rbcScan('\n');
		return 1;
	}

	// EXT -----------------------------------------------------------------
	if (cmd == "EXT")
	{
		TempStr1 = rbcGetVar(rbcScanVar(), false);
		rbcScanChar("=", true);
		TempStr2 = rbcGetValue(0);
		rbcScanChar(",", true);
		TempStr3 = rbcGetValue(0);

		fprintf(cocfile, "REXT%s%s%s", TempStr1.c_str(),
		        TempStr2.c_str(), TempStr3.c_str() );

		while ( rbcScanChar(",", false ) == ',')
		{
			fprintf(cocfile, "%s", rbcGetValue(0).c_str() );
		}

		return RB_OK;
	}

	// Procedure/Function call ---------------------------------------------
	if (procinfo_cnt > 0)
	{
		for (int a = 0; a < procinfo_cnt; a++)
		{
			if (strcmp(procinfo[a].proc_name, cmd.c_str()) == 0)
			{
				fprintf(cocfile, "CALL'%s'", procinfo[a].proc_name);

				if (procinfo[a].hasReturnValue)
				{
					fprintf(cocfile, "%s", rbcGetVar(rbcScanVar(), false).c_str());
					if (procinfo[a].parm_cnt > 0) rbcScanChar("=", true);
				}

				if (procinfo[a].parm_cnt > 0)
				{
					for (int b = 0; b < procinfo[a].parm_cnt; b++)
					{
						//Test if the 2nd character is a pointer identifier (because here, the
						//first character is the [ of the variable name
						if (procinfo[a].parms[b][1] == '*')
						{
							fprintf(cocfile, "%s", rbcGetVar(rbcScanVar(), false).c_str());
						}
						else
						{
							fprintf(cocfile, "%s", rbcGetValue(0).c_str());
						}
						rbcScanChar(",", true);
					}
				}
				return RB_OK;
			}
		}
	}

	return RB_ERROR;
}

//------------------------------------------------------------------------------
// execProg()
// executes a program
//------------------------------------------------------------------------------
bool execProg(char prog[STDLINE+1], char parms[STDLINE+1])
{
	char tmp[STDLINE+1];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	if (strcmp(parms, "") != 0)
		sprintf(tmp, "\"%s\" \"%s\"", prog, parms);
	else
		sprintf(tmp, "\"%s\"", prog);

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	if (!CreateProcess(NULL, tmp, NULL, NULL,FALSE, 0, NULL, NULL, &si, &pi ))
	{
		return false;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}


int rbcPreCompile()
{
	char chr;

	cmd = toUpper(rbcGetSymbol());

	// PROC/FUNC -----------------------------------------------------------
	if (cmd == "PROC" || cmd == "FUNC")
	{
		string procName;

		while (!CURRENT_FILEEND)
		{
			chr = rbcScanChar(":\n", false);
			if (chr != ':' && chr != '\n')
			{
				procName += chr;
			}
			else
			{
				break;
			}
		}

		procName = toUpper(Trim(procName));

		if (procinfo_cnt > 0)
		{
			for (int a = 0; a < procinfo_cnt; a++)
			{
				if (strcmp(procinfo[a].proc_name, procName.c_str()) == 0)
				{
					/*
					TempStr1 = procName;
					rbcError(4);
					*/
					if (procinfo[a].parm_cnt > 0)
						return RB_OK_IGNORE;

					return RB_OK;
				}
			}
		}

		if (procinfo_cnt == 0)
		{
			procinfo = (PINFO*)malloc(sizeof(PINFO));
		}
		else
		{
			procinfo = (PINFO*)realloc((PINFO*) procinfo, (procinfo_cnt+1) * sizeof(PINFO));
		}

		procinfo[procinfo_cnt].proc_name = (char*) malloc((procName.length()+1) * sizeof(char));
		strcpy(procinfo[procinfo_cnt].proc_name, procName.c_str());

		procinfo[procinfo_cnt].parm_cnt = 0;
		if (chr == ':')
		{
			do
			{
				if (procinfo[procinfo_cnt].parm_cnt == 0)
				{
					procinfo[procinfo_cnt].parms = (char**) malloc(sizeof(char*));
				}
				else
				{
					procinfo[procinfo_cnt].parms = (char**) realloc((char**)procinfo[procinfo_cnt].parms, (procinfo[procinfo_cnt].parm_cnt+1) * sizeof(char*));
				}

				string varname = rbcScanVar();

				procinfo[procinfo_cnt].parms[procinfo[procinfo_cnt].parm_cnt] = (char*) malloc(varname.length()+1);
				strcpy(procinfo[procinfo_cnt].parms[procinfo[procinfo_cnt].parm_cnt], varname.c_str());

				procinfo[procinfo_cnt].parm_cnt++;
			}
			while (rbcScanChar(",", true) != '\n');
		}

		procinfo[procinfo_cnt].hasReturnValue = false;
		if (cmd == "FUNC") procinfo[procinfo_cnt].hasReturnValue = true;

		procinfo_cnt++;
		return RB_OK_IGNORE;
	}
	// INCLUDE -------------------------------------------------------------
	if (cmd == "INCLUDE")
	{
		if (rbcScanChar("'", false) != '\'')
		{
			CHARACTER_BACK();
			TempStr1 = rbcScan('\n');
		}
		else
		{
			TempStr1 = rbcScan('\'');
		}

		if (!rbcOpenSourceFile(TempStr1))
		{
			if (TempStr1[0] != '\\')
			{
				/*
				string bla = compilerpath + "\\" + LIBDIR + "\\" + TempStr1;
				printf("My File: %s\n", bla.c_str());
				*/
				if (!rbcOpenSourceFile(compilerpath + "\\" + LIBDIR + "\\" + TempStr1))
					rbcError(5);
			}
			else
			{
				rbcError(5);
			}
		}
		return RB_OK_IGNORE;
	}
	// INCLUDE_ONCE --------------------------------------------------------
	if (cmd == "INCLUDE_ONCE")
	{
		if (rbcScanChar("'", false) != '\'')
		{
			CHARACTER_BACK();
			TempStr1 = rbcScan('\n');
		}
		else
		{
			TempStr1 = rbcScan('\'');
		}


		for (int i=0; i<once_included_cnt;i++)
		{
			if (strcmp(once_included[i], TempStr1.c_str()) == 0)
			{
				RB_OK_IGNORE;
			}
		}

		if (once_included_cnt == 0)
		{
			once_included = (char**)malloc(sizeof(char**));
		}
		else
		{
			once_included = (char**)realloc((char**)once_included,
			                                (once_included_cnt+1) * sizeof(char));
		}
		once_included[once_included_cnt] = strdup(TempStr1.c_str());
		once_included_cnt++;

		if (!rbcOpenSourceFile(TempStr1))
		{
			if (TempStr1[0] != '\\')
			{
				if (!rbcOpenSourceFile(compilerpath + "\\" + LIBDIR + "\\" + TempStr1)) rbcError(5);
			}
			else
			{
				rbcError(5);
			}
		}
		return RB_OK_IGNORE;
	}
	// END -----------------------------------------------------------------
	if (cmd == "END")
	{
		if (infiles_pos-1 > 0)
		{
			rbcCloseSourceFile();
			return RB_OK_IGNORE;
		}
		else
		{
			return RB_TERMINATE;
		}
	}

	if ( symbreak == '\n' )
		return RB_OK_IGNORE;

	return RB_OK;
}


#pragma argsused
int main(int argc, char* argv[])
{
	string cloption;
	ifstream iconInfo;
	int res;

	printf("RapidBATCH v%s Script-Compiler v%s%s [build: %s %s]\n", 
		RB_VERSION, 
		RBC_VERSION, 
#ifdef BETA_COMPILE
		"[Beta]",
#else
		"",
#endif
		__DATE__, __TIME__ );
	printf("Copyright (C) 2000-2007 by J.M.K S.F., Jan Max Meyer\n");
	printf("http://www.rapidbatch.com/    mail@jmksf.com\n\n\n");
	
#ifdef START_FILE
	openfile = START_FILE;
	compilerpath = "C:\\jmksf\\rb5";
#else
	if (argv[1] == NULL)
	{
		printf("  Usage:   RBC32 filename [options]\n\n");

		printf("    /L                  Perform automatic link on compiled script\n");
		printf("    /R                  Run after succeeded compile\n");
		printf("    /I:filename         Link icon <filename> to executable\n");
		printf("	/C					Encrypt compiled script\n");

		printf( "\n\n\n" );
		return 0;
	}


	openfile = argv[1];

	compilerpath = argv[0];
	for (int a = compilerpath.length(); a >= 0; a--)
	{
		if (compilerpath[a] == '\\')
		{
			compilerpath = compilerpath.substr(0, a);
			break;
		}
	}
#endif


	/*
	printf( "Cpath: %s\n", compilerpath.c_str() );
	exit(0);
	*/
	//compilerpath = compilerpath.substr(0,compilerpath.length()-9);

	int z = openfile.length() - 1;
	while (openfile.substr(z, 1) != "\\" && z > 0)
	{
		z-- ;
	}

	//string chpath = openfile.substr(0,z);
	//chdir(chpath.c_str());

	for (int c=2;c < argc;c++)
	{
		if (argv[c] != NULL)
		{
			cloption = argv[c];
			if (strcmp(strupr(argv[c]), "/L") == 0)
			{
				autolink = true;
			}
			else if (strcmp(strupr(argv[c]), "/R") == 0)
			{
				runafterhalt = true;
			}
			else if (strcmp(strupr(argv[c]), "/E") == 0)
			{
				editorcall = true;
			}
			else if (strcmp(strupr(argv[c]), "/C") == 0)
			{
				encrypt = true;
			}
			else if (toUpper(cloption.substr(0,3)) == "/I:")
			{
				progicon = cloption.substr(3, cloption.length());
			}
			else if (strcmp(strupr(argv[c]), "/ORCA") == 0)
			{
				//printf("                           _.\n                          ,OO.\n                          (OOT\n                          (OOOT\n                          OOOOOT\n                          OOOOOO)\n                         (OOOOOOO\n                         JOOOOOOO.\n                       _JOOOOOOOOOOT______.\n                ____OOOOOOOOOOOOOO''''OOOOOOOOOOT___\n         ___JOOOOOOOOOOOOOOOOOOO''''OOOOOOOOOOOOOOOOOOT__.\n   ,JOOOOOOOOOOOOOOOOOOOOOOOOOO'OOOOOOOOOOOOOOOOOOOOOOOOOOT__\n ,JOOT_.,_OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO''OOOOOOOOOOOOOOOOOOO__\n ''''''4OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO'`       ,OOOOOOOOOOOOOOOOOOO__.\n  ',     '4OOOOOOOOOOOOOOOOOOOOOOOOOOT     __JOOOOOOOOOOOOOOOOOOO)´''OOOO_\n    '',_,_OOOOOOOOOOOOOOOOOOOOOOOOT`___,'´OT'''''`                     ´''O.\n           OOOOOOOOOOOOOOOOOOOOOO''''\n           (OOOOOOOOOOO)\n            4OOOOOOOOOO)\n             4OOOOOOOOO)\n              4OOOOO)''\n               ´'OO'\n");
				printf("                             \"$$oo \n");
				printf("  ORCAS FOREVER!              \"$o\"o \n");
				printf("                                $$$o\" \n");
				printf("       www.orcaweb.de            $$$$$o\" \n");
				printf("                             ooo$$$$$$$$o o \n");
				printf("                        oo$$$$$$$$$$$$$$$$$o$ o \n");
				printf("                     oo$$$$$$$$$$$$$$$$$$$$$$$o\"o o \n");
				printf("                   o$$$$$$$$$$$$$$$$$$$$$$$$$$$$$oo\" o \n");
				printf("                 o$$$$$\"\"\"\"$$$$$$$$$$$$$$$$$$$$$$$$$$o $ \n");
				printf("                o$$$\"       $$$$$$$$$$$$$$$$$$$$\"\"$\"$$$o \"o \n");
				printf("               $$$$\"         $$$$$$$$$$$$$$$$$$$$      \"$$o\"o \n");
				printf("              $$$$     oooo    \"$\"$$$$$$$$$$$$$$$$ooo   \"$$oo\" \n");
				printf("              $$$   o$$$$$\"\"          \"o$\"\"\"\"$$$\"$\"\"$\"oo $$$$o$o \n");
				printf("             $$$o o$$\"\" o \" \" \" o ooo\" oo$$$$$$      \"$$$$$$$$$$ \n");
				printf("             $$$$$\"o \"        o o\" o\"o$$$$$$$$$        \"\"\"$$$$$$o \n");
				printf("            \"$$$$\"\"          \" $o$$$$\"$\"\"\" \" \"ooo          \" \"$\"$  \n");
				printf("           o$$$$                             $$$$\"$ooo         $ \n");
				printf("      ooo$$$$$$o o                          o$$$$$ $\"  \" \" \" \" \n");
				printf("  oo$$\"$\"$\"\"\"$\"$\"ooo                       \"$$$\"$\" \n");

				printf("\nPlease press any key to continue the compiling process...");
				getch();

				printf("\n\n");
			}
			else
			{
				printf("Warning: Unknown command line option: %s\n", argv[c]);
			}
		}
		else
		{
			break;
		}
	}


	outfile = openfile.substr(0, openfile.length() - 2) + "coc";
	tempfile = openfile.substr(0, openfile.length() - 2) + "tmp";

	// PRE COMPILING PROCESS -----------------------------------------------
	printf( "Pre-compiling..." );

	//Open file
	if (!rbcOpenSourceFile(openfile))
	{
		printf( "ERROR: File '%s' does not exist.\n", openfile.c_str());
		getch();
		return 1;
	}
	//Default Include File
	rbcOpenSourceFile(compilerpath + '\\' + DEFAULT_INCLUDE);

	do
	{
		while (!CURRENT_FILEEND)
		{
			res = rbcPreCompile();


			if (res == RB_TERMINATE) break;

			if (res == RB_OK)
				rbcScan('\n');
		}
	}
	while (infiles_pos-1 > 0 && res != RB_TERMINATE);

	//Close and free the source files
	if (infiles_cnt > 0)
	{
		free(filenames);
	}

	infiles_pos = 0;
	infiles_cnt = 0;


	printf( "Done\n" );
	// COMPILING PROCESS ---------------------------------------------------
	printf( "Compiling..." );

	remove(outfile.c_str());

	if ((cocfile = fopen(outfile.c_str(), "ab")) == NULL)
	{
		printf("Error while creating output file %s\n\nCompiling process aborted.\n", outfile.c_str());
		getch();
		return 1;
	}


	//Create a COC3 formatted compiler output file?
	if (createCOC3_Header)
	{
		fprintf(cocfile, "COC3;%s;rbrun32.rtl;RB5RTL (compiled %s), www.rapidbatch.com\r\n", progicon.c_str(),__DATE__ );
	}
	else
	{
		fprintf(cocfile, "COC2;000000;%s;rbrun32.rtl;RB5RTL (compiled %s), www.rapidbatch.com\r\n", progicon.c_str(), __DATE__);
	}
	
	//If encryption is on, write to another, temporary file
	if( encrypt )
	{
		remove(tempfile.c_str());
		fclose( cocfile );
		cocfile = fopen( tempfile.c_str(), "ab" );
	}
	
	fprintf(cocfile, "RRLR'%s'%c", RBRL_VERSION, (char)01);

	(void)rbcGetVar("errorcode", true); // 0
	(void)rbcGetVar("command", true); // 1
	(void)rbcGetVar("current", true); // 2
	(void)rbcGetVar("ownname", true); // 3
	(void)rbcGetVar("Echo_Title", true); // 4
	(void)rbcGetVar("Confirm_Title", true); // 5
	(void)rbcGetVar("windir", true); // 6
	(void)rbcGetVar("time", true); // 7
	(void)rbcGetVar("winvers", true); // 8
	(void)rbcGetVar("curuser", true); // 9
	(void)rbcGetVar("day", true); //10
	(void)rbcGetVar("month", true); //11
	(void)rbcGetVar("year", true); //12
	(void)rbcGetVar("InputBox_Ok", true); //13
	(void)rbcGetVar("InputBox_Cancel", true); //14
	(void)rbcGetVar("OpenBox_MultiSel", true); //15
	(void)rbcGetVar("SaveBox_MultiSel", true); //16
	(void)rbcGetVar("EditBox_Ok", true); //17
	(void)rbcGetVar("EditBox_Cancel", true); //18
	(void)rbcGetVar("EditBox_Width", true); //19
	(void)rbcGetVar("EditBox_Height", true); //20
	(void)rbcGetVar("ListBox_Width", true); //21
	(void)rbcGetVar("ListBox_Height", true); //22
	(void)rbcGetVar("ListBox_Ok", true); //23
	(void)rbcGetVar("new_line", true); //24
	(void)rbcGetVar("clipboard", true); //25
	(void)rbcGetVar("InfoBox_Width", true); //26
	(void)rbcGetVar("InfoBox_Height", true); //27
	(void)rbcGetVar("InfoBox_Enabled", true); //28
	(void)rbcGetVar("RB_Version", true); //29
	(void)rbcGetVar("RB_Pro", true); //30
	(void)rbcGetVar("ListBox_MultiSel", true); //31
	(void)rbcGetVar("ScreenWidth", true); //32
	(void)rbcGetVar("ScreenHeight", true); //33
	(void)rbcGetVar("active_window", true); //34
	(void)rbcGetVar("crlf", true); //35
	(void)rbcGetVar("tab", true); //36
	(void)rbcGetVar("pipe", true); //37
	(void)rbcGetVar("quot", true); //38
	(void)rbcGetVar("app_icon", true); //39
	(void)rbcGetVar("mouse_x", true); //40
	(void)rbcGetVar("mouse_y", true); //41
	(void)rbcGetVar("true", true); //42
	(void)rbcGetVar("false", true); //43
	(void)rbcGetVar("std_sep", true); //44

	(void)rbcGetVar("case_sensitivity", true); //45

	(void)rbcGetVar("InfoBox_X", true); //46
	(void)rbcGetVar("InfoBox_Y", true); //47
	(void)rbcGetVar("ListBox_X", true); //48
	(void)rbcGetVar("ListBox_Y", true); //49
	(void)rbcGetVar("InputBox_X", true); //50
	(void)rbcGetVar("InputBox_Y", true); //51
	(void)rbcGetVar("EditBox_X", true); //52
	(void)rbcGetVar("EditBox_Y", true); //53

	(void)rbcGetVar("OpenBox_DefFileExt", true); //54
	(void)rbcGetVar("SaveBox_DefFileExt", true); //55

	(void)rbcGetVar("XPStyleActive", true); //56

	(void)rbcGetVar("Metric_Edge_Width", true); //57
	(void)rbcGetVar("Metric_Edge_Height", true); //58
	(void)rbcGetVar("Metric_Caption_Height", true); //59
	(void)rbcGetVar("Metric_Menu_Height", true); //60

	(void)rbcGetVar("eol", true); //61

	//Open file
	if (!rbcOpenSourceFile(openfile))
	{
		printf( "ERROR: File '%s' does not exist.\n", openfile.c_str());
		getch();
		return 1;
	}

	//Default Include File
	rbcOpenSourceFile(compilerpath + '\\' + DEFAULT_INCLUDE);
	do
	{
		while (!CURRENT_FILEEND)
		{
			res = rbcCompile();

			if (res == RB_TERMINATE) break;
			if (res == RB_OK) fprintf(cocfile, "%c", 01);
			if (res == RB_ERROR) rbcError(1);
		}
	}
	while (infiles_pos-1 > 0 && res != RB_TERMINATE);

	if (loop_cnt > 0) rbcError(6);

	fprintf(cocfile, "RHLT%c", 01);

	//Check out all labels if they're accessable
	for (int a=0;a<labels_cnt;a++)
	{
		for (int b=0;b<go_labels_cnt;b++)
		{
			if (strcmp(labels[a].label, go_labels[b].label) == 0)
			{
				if (strcmp(labels[a].procedure, go_labels[b].procedure) != 0)
				{
					TempStr1 = labels[a].label;

					infiles_pos = go_labels[b].file_number;
					infiles[infiles_pos-1].line = go_labels[b].line;

					rbcError(2);
					break;
				}
			}
		}
	}

	
	
	if( encrypt )
	{
		fclose(cocfile);
		printf( "Done\nEncrypting..." );
		
		char* strcont = rbcMapFile( tempfile.c_str() );
		
		if( strcont )
		{
			long len = strlen( strcont );
		
			rbcEncrypt( strcont );
			cocfile = fopen( outfile.c_str(), "ab" );
			for( char* sptr = strcont; sptr < (strcont + len); sptr++ )
				fputc( *sptr, cocfile );
			
			fputc( '1', cocfile );

			free( strcont );
		}
		
		remove(tempfile.c_str());
	}
	else
	{
		fputc( '0', cocfile );
	}
	
	fclose(cocfile);
	

	printf("Done\nLines compiled: %d\n", total_lines+1);


	/*
	        Denk dran: Wenn error passiert, dann compiler sowiso bricht ab,
	        daher hier keine prüfungen mehr...
	*/

	if (autolink == true) //Automatic link?
	{
		printf("Performing automatic link on %s...\n", outfile.c_str());
		/*
		TempStr1 = "\"" + compilerpath + "plink.exe\" \"" + outfile + "\"";
		system(TempStr1.c_str());
		*/
		TempStr1 = compilerpath + "\\plink.exe";
		execProg((char*) TempStr1.c_str(), (char*) outfile.c_str());
		remove(outfile.c_str());

		if (runafterhalt == true)
		{
			TempStr1 = openfile.substr(0, openfile.length() - 2) + "exe";
			execProg((char*) TempStr1.c_str(), "");
		}
	}

	rbcFreeMemory();
	return 0;
}


