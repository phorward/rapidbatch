#ifndef includesH
#define includesH

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>

#include <string.h>
#include <stdio.h>
#include <shlobj.h>
#include <dos.h>
#include <io.h>
#include <conio.h>

#ifndef VC_COMPILE
#include <vector.h>
#include <dir.h>
//#include <condefs.h>
#include <fstream.h>
#include <iostream.h>
#else
#include <direct.h>
#endif


/*
void* malloc_debug(size_t len)
{
	FILE* trace = fopen("C:\\rb5_alloc.txt", "at");
	void* block = malloc( len );
	fprintf(trace, "malloc: %p\n", block);
	fclose(trace);
	return block;
}

void* realloc_debug(void* ext, size_t len)
{
	FILE* trace = fopen("C:\\rb5_alloc.txt", "at");
	void* block = realloc(ext, len );
	fprintf(trace, "realloc: %p\n", block);
	fclose(trace);
	return block;
}

void free_debug(void* block)
{
	FILE* trace = fopen("C:\\rb5_alloc.txt", "at");
	fprintf(trace, "free: %p\n", block);
	free(block);
	fclose(trace);
}*/

#endif