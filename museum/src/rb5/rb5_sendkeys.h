#ifndef RB5_SENDKEYS_H
#define RB5_SENDKEYS_H

#include "rb5_includes.h"

typedef struct
{
	short	keyCode;
	BYTE	keyFlag;
	short	hold;
}
KS_TOKEN;

typedef struct
{
	int     keyCode;
	char*	keyName;
}
KS_PREDEF_KEYS;


int ks_scantoken( char* s, KS_TOKEN* t );
void ks_press( KS_TOKEN* t );
void ks_release( KS_TOKEN* t );
int sendkeys( char* s );

#endif