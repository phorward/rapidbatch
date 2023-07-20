#ifndef RB5_LNG_H
#define RB5_LNG_H

#include "rb5_includes.h"
#include "rb5_str.h"

struct LNG_CAT
{
	char* lng_label;
	char* lng_value;
	bool last;
};

LNG_CAT* lng_buildCatalog( char* cat_file, char* prefix );
char* lng_getLabel( LNG_CAT* cat, char* label );
void lng_freeCatalog( LNG_CAT* cat );

#endif
