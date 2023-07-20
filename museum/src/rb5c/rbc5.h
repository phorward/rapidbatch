#ifndef rb5H
#define rb5H

#include "includes.h"

#define STDLINE 2048
#define MAXDIM 10+1

#define RB_OK 0
#define RB_ERROR -1
#define RB_TERMINATE 99
#define RB_OK_IGNORE 1

#define CURRENT_FILEEND feof(infiles[infiles_pos-1].file)
#define CHARACTER_BACK() fseek(infiles[infiles_pos-1].file, ftell(infiles[infiles_pos-1].file)-1, SEEK_SET);

char rbcScanChar(char[], bool);
string rbcGetVar(string, bool);
#endif
