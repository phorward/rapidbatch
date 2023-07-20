#ifndef RB5_DEFS_H
#define RB5_DEFS_H

#define STDLINE 2048
#define MAXDIM 10+1

#define RB_OK 0
#define RB_ERROR -1
#define RB_TERMINATE 99

#define ERRORCODE(errcode) rbLetVar("errorcode", errcode, procStack_cnt);
#define VARS(var) MessageBox(NULL, var, "RapidBATCH Debug", 16);
#define DEBUG(value) if(debug != (FILE*) NULL) { fprintf(debug, "%s\n", value); fflush(debug); }

#define RB_NEWVAR(var) rbNewVar(var, procStack_cnt);
#define RB_LETVAR(var, value) rbLetVar(var, value, procStack_cnt);
#define RB_GETVAR(var) rbGetVar(var, procStack_cnt);

#define CURRENT_FILEEND (infile[infileStack[infileStack_cnt]].offset >= infile[infileStack[infileStack_cnt]].filesize)
#define CHARACTER_BACK() infile[infileStack[infileStack_cnt]].offset--;
#define NEXT_OFFSET() infile[infileStack[infileStack_cnt]].offset++;
#define CURRENT_CHAR infile[infileStack[infileStack_cnt]].file[infile[infileStack[infileStack_cnt]].offset]

#define CHECK_TERMINATOR() FILE* quit = fopen("C:\\rb_halt.txt", "rb"); if(quit != NULL){ fclose(quit); exit(0); }

#define ERRORCODE_0_AND_RB_OK ERRORCODE("0"); return RB_OK;

#define RB_BUILD_IN_DIALOG (hwnd == (HWND)NULL || hwnd == hEditBox_Win || hwnd == hListBox_Win || hwnd == IB_WIN || hwnd == hDebug_Win)

#define MALLOC(size) malloc(size); //malloc_cnt++; //fprintf(debug, "malloc: malloc_cnt check %d\n", malloc_cnt);
#define FREE(offset) free(offset); //malloc_cnt--; //fprintf(debug, "free: malloc_cnt check %d\n", malloc_cnt);

#define IS_FALSE( val ) ( *(val) == '\0' || strcmp( (val), "-1" ) == 0 )

#endif