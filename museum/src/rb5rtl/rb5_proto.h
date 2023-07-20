#ifndef RB5_PROTO_H
#define RB5_PROTO_H

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM , LPARAM );
BOOL BrowseForFolder(char*, char*, char*);
bool readDirectory(char** file_list, char* filter, char* dirname, int depth, char* sep);
void iconToTaskbar (HWND, HICON,char*, bool);
HFONT SetFont(HWND, LPCTSTR, int, WORD);
char* GetOSName();
#define FF_BOLD 2
#define FF_ITALIC 4
#define FF_UNDERLINE 8
#define FF_STRIKEOUT 16

static int CALLBACK BrowseForFolderHook(HWND, UINT, LPARAM, LPARAM);

bool rbrunGetVar(char*, char**, int);
bool rbrunNewVar(char*, int );
bool rbrunLetVar(char*, char*, int );
bool rbrunDelVar(char*, bool);
bool rbrunPtrVar(char*, char*, int);
bool rbrunVarName_Extract(char*, char**, int*);
bool rbrunVarExists(char*, int);
void rbrunShutdown(void);

char rbrunScanChar(char*);
bool rbrunScanVar(char**);
bool rbrunScan(char, char**);
char rbrunGetChar();
void rbrunAddLabel(char*);
bool rbrunCondition(void);
bool rbrunGetValue(char**);

bool rbDlg_CreateMenu(HWND win, char* win_name, char* menudef);
int rbDlg_CreateDialogInfo(char* label);
DIALOG_EXTINFO* rbDlg_CreateDialogExtInfo(int dialog_info_pos);
DIALOG_EXTINFO* rbDlg_GetDialogExtInfo(int dialog_info_pos, short type);
void rbDlg_DeleteDialogExtInfo(int dialog_info_pos, short type);

inline bool rbGetBuildInVar(char*, char**);
inline bool rbLetBuildInVar(char*, char*);

bool rbrunGetFactor( char** retval );
bool rbrunGetTerm( char** retval );
bool rbrunGetExpr( char** retval );
bool rbrunGetValue( char** retval );

/* rb5_helpfct.h */
int rbCalculate( char** result, char* operand1, char* operand2, int op );

#endif