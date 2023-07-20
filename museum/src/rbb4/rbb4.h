#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#include <shellapi.h>
#include <condefs.h>
#include <string.h>
#include <shlobj.h>
#include <dir.h>
#include <io.h>
#include <vector.h>
#include <richole.h>
#include <process.h> 
#include <tom.h>
#include <unknwn.h>

#define DEFINE_GUIDXXX(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID CDECL name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

DEFINE_GUIDXXX(IID_ITextDocument,0x8CC497C0,0xA1DF,0x11CE,0x80,0x98,
                0x00,0xAA,0x00,0x47,0xBE,0x5D);

#include "rbb4.rh"
#include "language.h"
#include "str.h"

#ifndef RBB4_H
#define RBB4_H
    
#define INFO_ERROR(msg) MessageBox(NULL, msg, "RapidBATCH Builder - Error", 16 | MB_SETFOREGROUND);
#define INFO_MSG(msg) MessageBox(NULL, msg, "RapidBATCH Builder - Info", 64 | MB_SETFOREGROUND);
#define NOT_IMPLEMENTED MessageBox(NULL, "BETA EDITION: Feature not implemented yet.", "RapidBATCH Builder - Unimplemented", 16 | MB_SETFOREGROUND);

#define STDLINE 1024
#define STDFIELD 30
#define RGB_LEN 6+1
HWND hMain;
HWND hEdit;
HWND hTmpEdit;
HWND hToolbar;
HWND hStatus;
#define MAIN_MSG 0
#define EDIT_MSG 1
#define TOOLBAR_MSG 2
#define STATUS_MSG 3

HINSTANCE gInstance = (HINSTANCE) NULL;

bool firstPaint = true;

//Syntax highlighting ----------------------------------------------------------
#define CLASS_KEYWORD 0
#define CLASS_OPERATOR 1
#define CLASS_VALUE 2
#define CLASS_VARIABLE 3
#define CLASS_LABEL 4
#define CLASS_COMMENT 5
#define CLASS_SPECIAL_COMMENT 6

#define SYNTAX_CLASSES 7

struct SYNINFO
{
        char *ident;
        short identClass;
        bool otherCharMayFollow;
};

struct LNG_ELEM
{
        int id;
        char* data;
};

int lng_elem = 0;
LNG_ELEM* language_info = (LNG_ELEM*)NULL;

#define SYNTAXFILE "rbatch.syn"
int synInfo_cnt = 0;
SYNINFO *synInfo;

int keyword_cnt[SYNTAX_CLASSES];
int lastCodeLine = -1;
DWORD keyword_color[SYNTAX_CLASSES];
bool keyword_bold[SYNTAX_CLASSES];
bool keyword_italic[SYNTAX_CLASSES];
DWORD base_color;


//MENU ENTRY DEFINES & DECLARATIONS --------------------------------------------


HBITMAP hBitmap;
HMENU hmMain, hmFile, hmEdit, hmScript, hmTools, hmHelp, hmRecent;
MENUITEMINFO menuInfo;
//#define MODIFY_MENUTEXT(menu, id, value) ModifyMenu(menu, id, MF_BYCOMMAND | MF_STRING, id, value);
//#define MODIFY_MENUBITMAP(menu, id, value) ModifyMenu(menu, id, MF_BYCOMMAND | MF_BITMAP, id, value);
//#define MODIFY_MENUBITMAP(menu, id, picture) SetMenuItemBitmaps(menu, id, MF_BYCOMMAND, picture, picture);

char fontFace[STDLINE+1];

#define REGISTRY_HOME "Software\\jmksf\\RapidBATCH"

#define MAX_LEN 1024+1

typedef struct tagTPageOffset {
	long int Start;
	long int End;
	RECT rendRect;
	} TPageOffsets;

/*
struct EDIT_HISTORY
{
        int prev_start;
        int prev_end;
        char* prev_data;

        int new_start;
        int new_end;
        char* new_data;

        int operation;
};

int history_cnt = 0;
EDIT_HISTORY* hist = (EDIT_HISTORY*)NULL;

#define EH_INSERT 0
#define EH_DELETE 1
#define EH_PASTE 2
#define EH_SETPOS 3
*/
#define DISABLE_UNDOREDO() td_code->Undo(tomSuspend, NULL); td_code->Redo(tomSuspend, NULL);
#define ENABLE_UNDOREDO() td_code->Undo(tomResume, NULL); td_code->Redo(tomResume, NULL);

#endif