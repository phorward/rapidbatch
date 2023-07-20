/*
J.M.K S.F. Language Independency Tools v1.0 (LNG)
Copyright (C) 2000-2005 by J.M.K S.F., Jan Max Meyer
*/

#define VC_COMPILE
#include "rb5_lng.h"

LNG_CAT* lng_buildCatalog( char* cat_file, char* prefix )
{
	static LNG_CAT* cat = (LNG_CAT*)NULL;
	FILE* file = (FILE*)NULL;
	char line[ 1024 + 1 ];
	char* sep = (char*)NULL;
	int cat_cnt = 0;

	file = fopen( cat_file, "rt" );

	if (!file)
		return (LNG_CAT*)NULL;

	while (!feof(file))
	{
		fgets( line, 1024, file );
		sep = strchr(line, '=');
		if (*line == ';' || sep != (char*)NULL)
		{
			if (prefix != (char*)NULL
			        && strncmp(line, prefix, strlen(prefix) != 0))
			{
				continue;
			}

			*sep = '\0';

			if (cat_cnt == 0)
			{
				cat = (LNG_CAT*)malloc(sizeof(LNG_CAT));
			}
			else
			{
				cat = (LNG_CAT*)realloc((LNG_CAT*)cat,
				                        (cat_cnt+1) * sizeof(LNG_CAT));
			}

			cat[cat_cnt].lng_label = strdup(line);

			cat[cat_cnt].lng_value = strdup(sep+1);
			cat[cat_cnt].lng_value[strlen(cat[cat_cnt].lng_value)-1] = '\0' ;

			strreplace( &cat[cat_cnt].lng_value, "\\n", "\n", false );

			cat[cat_cnt].last = true;
			if (cat_cnt > 0)
				cat[cat_cnt-1].last = false;

			cat_cnt++;
		}
	}

	fclose(file);

	return cat;
}

char* lng_getLabel( LNG_CAT* cat, char* label )
{
	if (cat != (LNG_CAT*)NULL)
	{
		for (int i=0;; i++)
		{
			if (cat[i].lng_label != (char*)NULL
			        && strcmp(cat[i].lng_label, label) == 0)
			{
				return cat[i].lng_value;
			}

			if (cat[i].last) break;
		}
	}

	return "undefined";
}

void lng_freeCatalog( LNG_CAT* cat )
{
	if (cat == (LNG_CAT*)NULL)
		return;

	for (int i=0;; i++)
	{
		if (cat[i].lng_label != (char*)NULL)
			free(cat[i].lng_label);
		if (cat[i].lng_value != (char*)NULL)
			free(cat[i].lng_value);

		if (cat[i].last) break;
	}

	free(cat);
}
