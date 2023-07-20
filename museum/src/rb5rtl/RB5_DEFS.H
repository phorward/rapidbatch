#ifndef RB5_DEFS_H
#define RB5_DEFS_H

#define STDLINE 2048
#define MAXDIM 10+1

#define RB_OK 0
#define RB_ERROR -1
#define RB_OK_IGNORE 1
#define RB_TERMINATE 99

#define ERRORCODE(errcode) rbrunLetVar(PVAR_ERRORCODE, errcode, procStack_cnt);
#define VARS(var) MessageBox(NULL, var, "RapidBATCH Debug", 16);
#define DEBUG(value) if(debug != (FILE*) NULL) { fprintf(debug, "%s\n", value); fflush(debug); }

#define RB_NEWVAR(var) rbrunNewVar(var, procStack_cnt);
#define RB_LETVAR(var, value) rbrunLetVar(var, value, procStack_cnt);
#define RB_GETVAR(var) rbrunGetVar(var, procStack_cnt);

#define CURRENT_FILEEND (file_offset == file_size)
#define CHARACTER_BACK() file_offset--;
#define NEXT_OFFSET() file_offset++;

#define CHECK_TERMINATOR() FILE* quit = fopen("C:\\rb_halt.txt", "rb"); if(quit != NULL){ fclose(quit); exit(0); }

#define ERRORCODE_0_AND_RB_OK ERRORCODE("0"); return RB_OK;

#define RB_BUILD_IN_DIALOG (hwnd == hEditBox_Win || hwnd == hListBox_Win || hwnd == IB_WIN || hwnd == hDebug_Win)

#define MALLOC_DEBUG(offset, size) fprintf(debug, "MALLOC: %p %ld\n", offset, size ); fflush(debug);

#define REALLOC_DEBUG(offset, size) fprintf(debug, "REALLOC: %p %ld\n", offset, size); \
		fflush(debug);

#define FREE_DEBUG(offset) fprintf(debug, "FREE: %p\n", offset ); fflush(debug);

#define MALLOC(size) malloc(size); //malloc_cnt++; //fprintf(debug, "malloc: malloc_cnt check %d\n", malloc_cnt);
#define FREE(offset) free(offset); //malloc_cnt--; //fprintf(debug, "free: malloc_cnt check %d\n", malloc_cnt);

#define IS_FALSE( val ) ( *(val) == '\0' || strcmp( (val), "-1" ) == 0 )

#endif