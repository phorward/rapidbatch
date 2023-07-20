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

void rbDebug_LoadLine(void);
void rbDebug_AddVar(char*);
void rbDebug_LoadList(char*);
bool rbGetVar(char*, char**, int);
bool rbNewVar(char*, int );
bool rbLetVar(char*, char*, int );
bool rbDelVar(char*, bool);
bool rbPtrVar(char*, char*, int);
bool rbVarName_Extract(char*, char**, int*);
bool rbVarExists(char*, int);
void rbShutdown(void);

char rbScanChar(char*, bool);
bool rbScanVar(char**);
bool rbScan(char, char**);
char rbGetChar();
void rbError(int, char*);
void rbAddLabel(char*);
bool rbCondition(void);
bool rbGetValue(int, char**);
int rbFunction( char* cmd, char** retval, bool from_expr );
inline bool rbGetSymbol(char** symbol);

//Dialog extensions
void rbDlg_letInformation(int, int, string);
string rbDlg_getInformation(int, int);
bool rb_Dlg_GetEventByHwnd(HWND, int);

bool rbDlg_CreateMenu(HWND, char*, char*);
int rbDlg_CreateDialogInfo(char*);
DIALOG_EXTINFO* rbDlg_CreateDialogExtInfo(int dialog_info_pos);
DIALOG_EXTINFO* rbDlg_GetDialogExtInfo(int dialog_info_pos, short type);
void rbDlg_DeleteDialogExtInfo(int dialog_info_pos, short type);

inline bool rbGetBuildInVar(char*, char**);
inline bool rbLetBuildInVar(char*, char*);

bool rbMapScriptFile(char* filename);

/* rb5_helpfct.h */
int rbCalculate( char** result, char* operand1, char* operand2, int op );

#endif