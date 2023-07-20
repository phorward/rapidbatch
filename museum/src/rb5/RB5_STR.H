#ifndef RB5_STR_H
#define RB5_STR_H

#include "rb5_includes.h"

/*
int alloc_cnt = 0;
void **mem_info;

#define NEWSTR(ptr, len) char* ptr = (char*)malloc((len + 1) * sizeof(char)); NULLSTR(ptr); MEM_ADDPTR(ptr);
#define RESIZESTR(ptr, newlen) ptr = (char*)realloc((char*)ptr, (newlen + 1) * sizeof(char));
#define NULLSTR(ptr) ptr[0] = '\0';
#define DELSTR(ptr) if(ptr != (char*)NULL) free(ptr); ptr = (char*)NULL;

#define STRAPPENDCHR(ptr, chr) ptr[strlen(ptr)+1] = '\0'; ptr[strlen(ptr)] = chr;
#define MEM_ADDPTR(ptr) mem_info = (void**) realloc((void**)mem_info, (alloc_cnt+1) * sizeof(void*));\
				mem_info[alloc_cnt] = ptr;\
				alloc_cnt++;

#define MEM_CLEANUP() for(int i = 0; i < alloc_cnt; i++)\
			{\
				if(mem_info[i] != (void*)NULL) free(mem_info[i]);\
			}\
			if(alloc_cnt > 0) free(mem_info);\
			alloc_cnt = 0;
*/

char* strtrim(char* str);
char* strupper(char* str);
char* strlower(char* str);
char* strreplace(char** str, char* find, char* replace, bool case_sensitivity);
char* strsubstr(char*, int, int);
char* strtoken(char* string, char* token);

#endif
