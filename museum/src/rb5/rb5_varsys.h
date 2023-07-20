#ifndef RB5_VARSYS_H
#define RB5_VARSYS_H

#include "rb5_includes.h"

//Variable system specific
struct NODE
{
	NODE* prev;
	NODE* next;
	void* varValue;
	char* varName;
	int   varArea;
	NODE* ptr_parent;
	void* child_list;
	NODE* ptr_pointer;
};

struct LIST
{
	NODE* ptr_begin;
	NODE* ptr_end;
	short varType;
};

#define VAR_STRING 0
#define VAR_INT 1
#define VAR_FLOAT 2
#define VAR_DOUBLE 3

#define VARSYS_OKAY 0
#define VARSYS_ERR_VARNOTFOUND -1
#define VARSYS_ERR_INDEXOUTOFRANGE -2
#define VARSYS_ERR_WRONGAREA -3
#define VARSYS_ERR_CHILDLISTEMPTY -3

int newVar(LIST*, char*, void*, int, int*);
int letVar(LIST*, char*, void*, int, int*);
int getVar(LIST*, char*, int, int*, char**);
int resizeVar(LIST* , char* , void* , int , int* );
int ptrVar(LIST*, char*, int, int*, LIST*, char*, int);
long varSize(LIST*, char*, int, int*);
bool varExists(LIST*, char*, int, int*);
int delVar(LIST*, char*, int*, bool);
void printVar(LIST*, char*);
void shutdownVarSys(LIST*);

#endif
