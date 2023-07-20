#define VC_COMPILE
#include "rbb4.h"

char* strtrim(char* str)
{
	int len = 0;
	unsigned int whitespaces = 0;

	if(str == (char*)NULL)
		return (char*)NULL;

	for(whitespaces = 0; whitespaces < strlen(str); whitespaces++)
		if(str[whitespaces] != ' ' && str[whitespaces] != '\t') break;

	if(whitespaces < strlen(str))
	{
		for(len = strlen(str)-1;len > 0; len--)
		{
			if(str[len] != ' '
				&& str[len] != '\t')
					break;
		}

		memcpy(str, str+whitespaces, (len+1) * sizeof(char));
		str[(len-whitespaces)+1] = '\0';
	}
	else
	{
		*str = '\0';
	}

	return str;
}

char* strupper(char* str)
{
	for (unsigned int i = 0; i < strlen(str); i++)
	{
		if (str[i] >= 97 && str[i] <= 122) str[i] = str[i] - 32;
	}

	return str;
}

char* strlower(char* str)
{
	for (unsigned int i = 0; i < strlen(str); i++)
	{
		if (str[i] >= 65 && str[i] <= 90) str[i] = str[i] + 32;
	}

	return str;
}

char* strreplace(char** str, char* sfind, char* sreplace, bool case_sensitivity)
{
	char* retval = (char*)NULL;
	char* tmpval = (char*)NULL;
	char* find = (char*)NULL;
	char* replace = (char*)NULL;
	unsigned long int pos = 0;
	unsigned long int len = 0;

	find = (char*)malloc((strlen(sfind)+1) * sizeof(char));
	strcpy(find, sfind);

	replace = (char*)malloc((strlen(sreplace)+1) * sizeof(char));
	strcpy(replace, sreplace);

	if(case_sensitivity)
	{
		tmpval = (char*)malloc((strlen(*str)+1) * sizeof(char));
		strcpy(tmpval, *str);
		strupr(tmpval);
		strupr(find);
	}

	retval = (char*)malloc(sizeof(char));

	len = strlen(*str);
	for (unsigned int i = 0; i < len; i++)
	{
		if((!case_sensitivity && strncmp(((*str)+i), find, strlen(find)) == 0)
				|| (case_sensitivity && strncmp((tmpval+i), find, strlen(find)) == 0))
		{
			retval = (char*)realloc((char*)retval, (pos + strlen(replace) + 1) * sizeof(char));
			memcpy((retval+pos), replace, strlen(replace) * sizeof(char));
			pos = pos + strlen(replace);
			i += strlen(find)-1;
		}
		else
		{
			retval = (char*)realloc((char*)retval, (pos + 1 + 1) * sizeof(char));
			retval[pos] = *((*str)+i);
			pos++;
		}
	}
	retval[pos] = '\0';

	if(case_sensitivity)
		free(tmpval);

	free(find);
	free(replace);
	free(*str);
	*str = retval;
	return *str;
}

char* strsubstr(char* str, int pos, int len)
{
        char* ret = str + pos;

	if(len > strlen(ret))
                len = strlen(ret);

        ret[len] = '\0';
        return ret;
}

static char* last_str = (char*)NULL;

char* strtoken(char* str, char* token)
{	
	unsigned int token_len = strlen(token);

	if(str == (char*)NULL && last_str != (char*)NULL)
	{
		str = last_str;
	}

	if(strlen(str) == 0)
		return (char*)NULL;

	for(unsigned int len = 0; len < strlen(str); len++)
	{
		if(strncmp(str+len, token, token_len) == 0)
		{
			str[len] = '\0';
			last_str = str+len+token_len;
			return str;
		}
	}

	last_str = str + strlen(str);
	return str;
}
