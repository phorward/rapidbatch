#ifndef STR_H
#define STR_H

char* strtrim(char* str);
char* strsubstr(char* str, int pos, int len);
char* strtoken(char* str, char* token);
char* strreplace(char** str, char* sfind, char* sreplace, bool case_sensitivity);


#endif