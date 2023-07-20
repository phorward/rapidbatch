#ifndef RB5_BASE_H
#define RB5_BASE_H

#include "rb5_includes.h"
#include "rb5_varsys.h"
#include "rb5_str.h"
#include "rb5_lng.h"
#include "rb5_sendkeys.h"
#include "rb5_helpfct.h"
#include "rb5_tokens.h"

//Runtime / Interpreter difference
#ifdef RB5RTL_CPP
#include "../rb5rtl/rb5_pvar.h"
#include "../rb5rtl/rb5_defs.h"
#else
#include "rb5_pvar.h"
#include "rb5_defs.h"
#include "rb5_lngdef.h"
#endif


LIST Var;

//Declarations and defines

// PROCEDURES ADDRESSING--------------------------------------------------------
struct PROCINFO
{
	char* proc_name;
	char** parms;
	int parm_cnt;
	unsigned long int proc_pos;
	bool hasReturnValue;

#ifndef RB5RTL_CPP
	char* filename; //only in interpreter
#endif
};

PROCINFO* procedures;
int proc_cnt = 0;

//Procedure stack
struct PSTACK
{
	unsigned long int call_pos;
	char* proc_name;
	char** vars;
	char* ret_var;
	int var_cnt;

#ifndef RB5RTL_CPP
	char* filename; //only in interpreter
#endif
};

PSTACK* procStack;
int procStack_cnt = 0;


// LOOPS------------------------------------------------------------------------
struct LOOPINFO
{
	char* proc_name;
	unsigned long int loop_begin;
};

LOOPINFO *loops;
int loop_cnt = 0;

#define CLEAR_LOOPS() 	if(loop_cnt > 0)\
	{\
		for(int i=0; i < loop_cnt; i++)\
		{\
			if(loops[i].proc_name != (char*)NULL)\
				free(loops[i].proc_name);\
		}\
		free(loops);\
		loop_cnt = 0;\
	}


// SUBS-------------------------------------------------------------------------
struct SUBINFO
{
#ifndef RB5RTL_CPP
	char* filename; //only in interpreter
#endif
	unsigned long int call_pos;
};

SUBINFO *subs;
int sub_cnt = 0;

// LABELS-----------------------------------------------------------------------
struct LINFO
{
	char* label_name;
	char* proc_name;
#ifndef RB5RTL_CPP
	char* filename; //only in interpreter
#endif
	unsigned long int label_pos;
};

LINFO *labels;
int label_cnt = 0;


struct WFILEINFO
{
#ifndef RB5RTL_CPP
	char* filename; //only in interpreter
#endif
	FILE* file;
	int last_pos;
};

WFILEINFO* wfiles = (WFILEINFO*) NULL;
int wfiles_cnt = 0;

HINSTANCE hInstance; //Global Application Instance
WNDCLASSEX wincl = {0}; //Global application class
char szClassName[] = "rb5";

int ifcount = 0;

HWND IB_WIN, IB_OK, IB_CANCEL, IB_EDIT, IB_PASSWORD, IB_TEXT; //IBox Information
#define IB_OKBUTTON 1
#define IB_CANCELBUTTON 2

HWND hListBox_Win, hListBox_List, hListBox_ListMulti, hListBox_OK, hListBox_MultiList;
#define LISTBOX_WIN 3
#define LISTBOX_LIST 4
#define LISTBOX_OK 5
#define LISTBOX_MULTILIST 6

HWND hEditBox_Win, hEditBox_Edit, hEditBox_OK, hEditBox_Cancel;
#define EDITBOX_WIN 7
#define EDITBOX_EDIT 8
#define EDITBOX_OK 9
#define EDITBOX_CANCEL 10

HWND hInfoBox_Win, hInfoBox_Info;
#define INFOBOX_WIN 11
#define INFOBOX_INFO 12

#define OKBUTTON_CLICK 0
#define CANCELBUTTON_CLICK 1

HWND hInvisibleWindow;
HMENU hTrayMenu;
static int s_uTaskbarRestart;
bool onTrayMenu = false;
int trayMenuSelect = -1;
HICON hTrayIcon = (HICON)NULL;
char* trayIconToolTip = (char*)NULL;
#define WM_ICONCLICK (WM_APP+0)


long DlgClick = -1;

bool IsEvent = false;

#ifndef RB5RTL_CPP
struct INFILES
{
	char* fname;
	char* file;
	unsigned long int offset;
	unsigned long int include_offset; //for goto's
	unsigned long int filesize;
};

INFILES* infile = (INFILES*) NULL;
int infile_cnt = 0;
int* infileStack = (int*) NULL;
int infileStack_cnt = -1;

#define INFILE_STATUS()\
	DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ HAVING THE FOLLOWING FILES IN MEMORY:");\
	for(int x = 0; x < infile_cnt; x++)\
	{\
		fprintf(debug, "Filename: %s\noffset: %ld\ninclude_offset: %ld\nFilesize: %ld\n---------\n",\
			infile[x].fname, infile[x].offset, infile[x].include_offset, infile[x].filesize);\
		fflush(debug);\
	}\
	DEBUG("FILE STACK:");\
	for(int x = 0; x <= infileStack_cnt; x++)\
	{\
		fprintf(debug, "%d = %d\n", x, infileStack[x]);\
	}\
	fflush(debug);

unsigned long line_begin = 0;
#endif

FILE *workingfile = (FILE*)NULL;

#define XPDEVPIX 10
typedef bool (_stdcall* WinXPStyle)(void); //for Windows XP theme compatibility
typedef bool (_stdcall* LockStation)(void); //for locking Win2000/XP workstations

bool xpThemeActive = false;

char text[STDLINE];
char symbreak;

char* baseDir = (char*)NULL;
char openfile[STDLINE+1];

char* cmd = (char*)NULL;
//Temporary Strings
#define TEMPVARS 10
char* TempStr[TEMPVARS];

//Temporary Floats
float TempNum1;
float TempNum2;

int TempInt1;
int TempInt2;
int last_rand = 0;
int devpix;

//faster file access for READFILE
int _READFILE_line_cnt;
char* _READFILE_file = (char*)NULL;
unsigned long int _READFILE_offset = 0;

//BUILD-IN DEBUGGER
HWND hDebug_Win, hDebug_List, hDebug_CodeLine, hDebug_Step, hDebug_Halt, hDebug_Run, hDebug_ValLine, hDebug_Set, hDebug_TraceInto, hDebug_LineInfo;
#define DEBUG_WIN 21
#define DEBUG_LIST 22
#define DEBUG_CODELINE 23
#define DEBUG_STEP 24
#define DEBUG_HALT 25
#define DEBUG_RUN 26
#define DEBUG_SET 27
#define DEBUG_TRACEINTO 28
#define DEBUG_LINEINFO 29


bool onTrace = false;
bool onDebug = false;
bool autoVarDebug = false;
char** debugvar;
int debugvar_cnt = 0;
bool rb_final_halt = false;
int rb_return_code = 0;
int debug_returnProcStack = -1;

//DIALOG EXTENSIONS ------------------------------------------------------------
#define DLG_MAXEVENTS 12

struct DIALOG_EXTINFO
{
	short ext_type;
	void* ext_handle;
	void* ext_data;
};

#define DIALOG_EXTINFO_VOID             0
#define DIALOG_EXTINFO_TOOLTIP          1
#define DIALOG_EXTINFO_STATUSBAR        2
#define DIALOG_EXTINFO_MENU             3
#define DIALOG_EXTINFO_IMAGE            4
#define DIALOG_EXTINFO_IMAGE_TRANS      5
#define DIALOG_EXTINFO_ICON             6
#define DIALOG_EXTINFO_FONT             7
#define DIALOG_EXTINFO_STYLE            8
#define DIALOG_EXTINFO_MOUSE_X          9
#define DIALOG_EXTINFO_MOUSE_Y          10
#define DIALOG_EXTINFO_ORDER            11
#define DIALOG_EXTINFO_ANIMATION        12


struct DIALOG_INFO
{
	HWND handle;
	char* label;
	bool isParent;
	bool events[DLG_MAXEVENTS];

	DIALOG_EXTINFO* extInfo;
	int extInfo_cnt;

	short window_type;

	HBRUSH hBrush;
	/*COLORREF forecolor;
	COLORREF backcolor;
	bool has_own_colors;*/
};

DIALOG_INFO* dialog_info = (DIALOG_INFO*)NULL;
int dialog_info_cnt = 0;
#define DIALOG_ID 2000

#define RUNDIALOG_TIMER_ID 1999
int dialog_event;

#define DLGEVENT_NOTHING -1
#define DLGEVENT_TIMEOUT 0

#define DLGEVENT_CLICK 1
#define DLGEVENT_DBLCLICK 2
#define DLGEVENT_WINDOWSIZE 3
#define DLGEVENT_WINDOWCLOSE 4
#define DLGEVENT_CHANGE 5
#define DLGEVENT_OK 6
#define DLGEVENT_CANCEL 7
#define DLGEVENT_MOVE 8
#define DLGEVENT_GETFOCUS 9
#define DLGEVENT_LEAVEFOCUS 10
#define DLGEVENT_MOUSEMOVE 11

#define SPECIALEVENT_HOTKEY 99

#define WIN_DIALOG 0
#define WIN_BUTTON 1
#define WIN_INPUT 2
#define WIN_EDIT 3
#define WIN_LABEL 4
#define WIN_OPTION 5
#define WIN_RADIO 6
#define WIN_LIST 7
#define WIN_COMBO 8
#define WIN_GROUP 9
#define WIN_IMAGE 10
#define WIN_PWD 11
#define WIN_MENU 12
#define WIN_MULTILIST 13
#define WIN_ANIMATION 14
#define WIN_RICHEDIT 15
#define WIN_PROGRESS 16
#define WIN_STATIC_COMBO 17

bool new_group          = true; //for RADIO-widgets
HMODULE richedit_dll    = (HMODULE)NULL;

//Dialog help macros
#define RB_DLG_GETEVENT(dlg_cnt, event) dialog_info[dlg_cnt].events[event]
#define RB_DLG_LETEVENT(dlg_cnt, event, state) dialog_info[dlg_cnt].events[event] = state


#define RB_WINDOW_TYPE(pos, wtype) dialog_info[pos].window_type == wtype
#define RB_DLG_NOMENU(pos) if(RB_WINDOW_TYPE(pos, WIN_MENU)) { ERRORCODE("-1"); return RB_OK; }

//Shortcut-Command
typedef IShellLink *LPSHELLLINK;
typedef IPersistFile *LPPERSISTFILE;

//HotKey Information System
char** hotkey_info = (char**)NULL;
int hotkey_info_cnt = 0;


//Environment variable pointers
char** env_values = (char**)NULL;
int env_values_cnt = 0;


//XP Themes Active function
WinXPStyle IsThemeActive = 0;

//External function calls
typedef int EXTERNFUNC ( char**, char**, int );

struct DLL_INFO
{
	char*           dll_name;
	HINSTANCE       dll_handle;
};

DLL_INFO*       dll             = (DLL_INFO*)NULL;
int             dll_cnt         = 0;

//SPECIAL VARIABLES FOR RAPIDBATCH RUNTIME ONLY!!!
#ifdef RB5RTL_CPP
char* filecode = (char*)NULL;
unsigned long file_offset = 0;
unsigned long file_size = 0;
#endif

//Language Independency
#ifndef RB5RTL_CPP
LNG_CAT* lng = (LNG_CAT*)NULL;
#endif

//Runtime / Interpreter differencies
#ifdef RB5RTL_CPP
#include "../rb5rtl/rb5_proto.h"
#else
#include "rb5_proto.h"
#include "rb5_error.h"
#endif


#endif
