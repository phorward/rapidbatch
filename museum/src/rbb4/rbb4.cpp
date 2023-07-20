/*
        J.M.K S.F. RapidBATCH v5.0 Builder v4.0 Debug
	(C) 2004, 2005 by J.M.K S.F. Software Technologies, Jan Max Meyer
*/
#pragma argsused

#include "rbb4.h"

#define RBB4_PROGRAM_INFO "J.M.K S.F. RapidBATCH v5.0 Builder v4.0 Beta 4"
#define RBB4_COPYRIGHT_INFO "Copyright (C) 2000-2005 by J.M.K S.F., Jan Max Meyer"

#define RBATCH_REGISTRY_PATH "Software\\jmksf\\RapidBATCH"
#define MAX_HISTORY 8

//#define DEBUG_MODE ON

//---------------------------------------------------------------------------
USERC("rbb4.rc");
USEUNIT("str.cpp");
//---------------------------------------------------------------------------
#ifdef DEBUG_MODE
FILE* debug;
#endif                                                                                                                                  

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GotoLine_DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK SetCommand_DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Find_DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Compile_DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Debug_DlgProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK About_DlgProc (HWND, UINT, WPARAM, LPARAM);

bool ReadConsoleApplication(LPTSTR lpszApplication, VOID (*lpfnHandler)(LPSTR lpszBuffer));
void ReadConsoleApplicationHandler(LPSTR lpszBuffer);
HWND compiler_outputs;

void readLngCatalog(void);
bool addLngText(int, char*);
static char* getLngText(int);
void cleanLngCatalog(void);

bool highlightCode(int);
char* nextLineIndent(void);
bool canAvoidHighlight(int);
void update_history_menu(char*);
void setCursorColor();
void readSyntaxDef(void);
bool onHighlight = false;
bool multilineHighlight = false;
bool exec_program(char* cmdline, bool wait);

HWND CreateToolbar(HWND);
HMENU CreateContextMenu(void);
bool loadFile(char*, bool);
bool saveFile(char*, bool);
bool run_script(char*, char*, bool);
void set_caption(char*);
void set_lineinfo(void);
void PrintFile(void);

bool setRegEntry(HKEY, char*, char*, char*);
bool getRegEntry(HKEY, char*, char*, char*, long);
bool getRGB(char*, int*, int*, int*);
DWORD getClassColor(char*);
//bool getClassBoldState(char*);

void loadToolsMenu(void);
char** tools_exec = (char**)NULL;
int tools_exec_cnt = 0;

char** history = (char**)NULL;
int history_cnt = 0;
HMENU hist_menu = (HMENU)NULL;

HMENU hcMenu = (HMENU)NULL;

bool egg = false;

//BOOL PopPrntPrintFile (HINSTANCE, HWND, HWND, PTSTR);
//BOOL PrintFile (HINSTANCE, HWND, HWND, PTSTR);

char current_file[STDLINE+1];
char rb_path [STDLINE+1];
char *findstring = (char*)NULL;
bool file_saved = true;
bool behind_dialog = false;

bool file_changed = false;
CHARRANGE last_pos;
FINDTEXT find_text;
char* cmdline_parms = (char*)NULL;
HWND hDlgPrint;
IUnknown 		*tr_code = NULL;
ITextDocument		*td_code;

char icon_file[STDLINE+1]; //Icon file in compiler dialog

//DEBUG SETTINGS DIALOG
bool rb_debug_session = false;
int debug_mode = 0;
#define DM_STEP 0
#define DM_TRACE 1

int var_mode = 0;
#define VM_NONE 0
#define VM_AUTO 1
#define VM_MANUAL 2
char* debug_vars = (char*)NULL;

//---------------------------------------------------------------------------
#pragma argsused
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
        HACCEL hAccel;
        MSG  msg;
        WNDCLASS wc;
        const char szAppName[] = "rbb4";

        gInstance = hInstance;
	
        wc.style                = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc		= WndProc;
        wc.cbClsExtra		= 0;
        wc.cbWndExtra		= 0;
        wc.hInstance		= hInstance;
        wc.hIcon                = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BASEICON));
        //wc.hIconsm              = LoadIcon(hInstance, IDI_BASEICON);
        wc.hCursor              = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetSysColorBrush(COLOR_MENU);
	wc.lpszMenuName		= szAppName;
        wc.lpszClassName	= szAppName;

        RegisterClass(&wc);
        InitCommonControls();

        #ifdef DEBUG_MODE
	debug = fopen("C:\\RBB4_DEBUG.TXT", "wt");
	#endif


	hMain = CreateWindowEx (0, szAppName, "", WS_POPUP | WS_OVERLAPPEDWINDOW | WS_VISIBLE, (GetSystemMetrics(SM_CXSCREEN)/2) - 300, (GetSystemMetrics(SM_CYSCREEN)/2) - 250, 700, 600, NULL, (HMENU)0, hInstance, NULL);
        (void)strcpy(current_file, "");
        set_lineinfo();

        hAccel = LoadAccelerators (hInstance, szAppName);

        //Get RapidBATCH Builder directory
	GetModuleFileName(gInstance, rb_path, STDLINE);
	for(int i = strlen(rb_path); i >= 0; i--)
	{
		if(rb_path[i] == '\\')
		{
			rb_path[i] = '\0';
			break;
		}
	}

	//Load syntax definitions
	readSyntaxDef();
	readLngCatalog();

	hcMenu = CreateContextMenu();

	//Load file from commandline
	if(strcmp(szCmdLine, "") != 0)
	{
		char* file_name = szCmdLine;
		if(szCmdLine[0] == '\"')
		{
			file_name++;
			for(unsigned int i=1; i < strlen(szCmdLine); i++)
			{
				if(szCmdLine[i] == '\"')
				{
					szCmdLine[i] = '\0';
					break;
				}
			}
		}
		(void)strcpy(current_file, file_name);
		loadFile(current_file, false);
		set_caption(current_file);

        	for(int i = strlen(current_file); i >= 0; i--)
        	{
        		if(current_file[i] == '\\')
        		{
        			current_file[i] = '\0';

                                chdir(current_file);

                                current_file[i] = '\\';
        			break;
        		}
	        }
	}
	else
	{
                //SetFocus(hEdit);
		set_caption(getLngText(LNG_DEFAULTFILE));
        }

        ShowWindow(hMain, iCmdShow);
        UpdateWindow(hMain);

	loadToolsMenu();

        update_history_menu((char*)NULL);
	//INFO_MSG("Welcome!\n\nThis is a beta edition!\nSome menu items are not working yet,\nbut they will do soon!\n\nEnjoy your session ;)\n-- jan max");

       	while(GetMessage(&msg, NULL, 0, 0))
	{
                if(!TranslateAccelerator (hMain, hAccel, &msg)) //!IsDialogMessage(hMain, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


        #ifdef DEBUG_MODE
        fclose(debug);
        #endif

        //free the rest of allocated memory
	if(findstring != (char*)NULL) free(findstring);
	if(debug_vars != (char*)NULL) free(debug_vars);
	cleanLngCatalog();

        td_code->Release();
        tr_code->Release();

        return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(onHighlight)
	{
		return 1;
	}

	switch(message)
	{
		case WM_CREATE:
			//Create the editor field ------------------------------
			if (!LoadLibrary("RICHED20.DLL"))
                        {
                                INFO_ERROR("Error when loading RICHED20.DLL");
                                return 0;
                        }

                        RECT rc;
                        GetClientRect(hWnd, &rc);
                        //hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "RICHEDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN, 0, 27, rc.right, rc.bottom - 21, hWnd, (HMENU)EDIT_MSG, gInstance, NULL);
                        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN, 0, 27, rc.right, rc.bottom - 21, hWnd, (HMENU)EDIT_MSG, gInstance, NULL);

			SendMessage(hEdit, EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_KEYEVENTS | ENM_SELCHANGE | ENM_MOUSEEVENTS);

			SendMessage(hEdit, EM_GETOLEINTERFACE, 0, (LPARAM)&tr_code);

			if(tr_code == (IRichEditOle*)NULL)
			{
                                INFO_ERROR("Error when trying to get RichEdit OLE Object");
				return 0;
			}

			tr_code->QueryInterface(IID_ITextDocument,(void**)&td_code);



			//TempEdit field
			hTmpEdit = CreateWindowEx(0, "EDIT", "", WS_CHILD | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN, 0, 0, 100, 100, hWnd,(HMENU)NULL, gInstance, NULL);

                        //Create the Statusbar ---------------------------------
                        hStatus = CreateWindowEx (0, STATUSCLASSNAME, "", WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP, 0, 0, 0, 0, hWnd, (HMENU)STATUS_MSG, gInstance, NULL);
                        SendMessage(hStatus, SB_SIMPLE, 0, 0);

                        //Create the Toolbar -----------------------------------
			hToolbar = CreateToolbar(hWnd);
			return 0;

		case WM_CONTEXTMENU:
			POINT point;
			int menusel;
			
			POINTSTOPOINT (point, lParam);
			if ((point.x == -1) && (point.y == -1))
			{
				point.x = point.y = GetSystemMetrics (SM_CXEDGE);
				ClientToScreen (hMain, &point);
			}

			if(WindowFromPoint(point) == hEdit)
			{
				menusel = (int)TrackPopupMenu (hcMenu, TPM_LEFTBUTTON | TPM_RETURNCMD, point.x, point.y, 0, hWnd, NULL);

				/*
				iAuswahl = (int) TrackPopupMenu (hcMenu, TPM_CENTERALIGN | TPM_LEFTBUTTON |
					       TPM_RETURNCMD, point.x, point.y, 0, hWnd, 0);

				*/

				switch(menusel)
				{
					case LNG_CONTEXTMENU_COPY:
						SendMessage(hEdit, WM_COPY, 0, 0);
						break;
					case LNG_CONTEXTMENU_CUT:
						SendMessage(hEdit, WM_CUT, 0, 0);
						break;
					case LNG_CONTEXTMENU_PASTE:
						SendMessage(hTmpEdit, WM_PASTE, 0, 0);

						long line, col;
						SendMessage(hEdit, EM_GETSEL, (WPARAM)&col, NULL);
						line = (int)SendMessage(hEdit, EM_LINEFROMCHAR, col, 0);

						SendMessage(hEdit, WM_PASTE, 0, 0);

						for(int i = line; i < (line + SendMessage(hTmpEdit, EM_GETLINECOUNT,NULL,NULL));i++)
						{
							highlightCode(i);
						}
						SendMessage(hTmpEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)"");

						break;
					case LNG_CONTEXTMENU_SELECT_ALL:
						SendMessage(hEdit, EM_SETSEL, 0, -1);
						break;
					case LNG_CONTEXTMENU_DELETE:
						SendMessage(hEdit, EM_REPLACESEL, TRUE, (LPARAM)"");
						break;
						
					default:
						break;
				}
			}
			return 0;

		//Resizing the element
		case WM_SIZE:
			RECT sizeInfo, statusInfo;

			GetWindowRect(hToolbar, &sizeInfo);
			MoveWindow(hToolbar, 0, 0, LOWORD(lParam), 25, TRUE);

			GetWindowRect(hStatus, &statusInfo);
			MoveWindow(hStatus, 0, HIWORD(lParam) - 10, LOWORD(lParam), HIWORD(lParam), TRUE);
			MoveWindow(hStatus, 0, statusInfo.bottom - statusInfo.top, LOWORD(lParam), HIWORD(lParam) - (statusInfo.bottom - statusInfo.top) + 1, TRUE );

			GetWindowRect(hEdit, &sizeInfo);


                        /*char test[100];
                        sprintf(test, "%d %d %d", statusInfo.bottom, statusInfo.top);
                        INFO_MSG(test);*/

			MoveWindow(hEdit, 0, 27, LOWORD(lParam), HIWORD(lParam) - (statusInfo.bottom - statusInfo.top + 25), TRUE);

			InvalidateRect(hEdit, NULL, TRUE);
			return 1;
                case WM_CLOSE:
                        if(!file_saved)
                        {
				switch(MessageBox(hMain, getLngText(LNG_MSG_SAVEANYCHANGES), "RapidBATCH Builder", 32 + 3))
                                {
					case 6:
						if(!saveFile(current_file, false)) return 0;
                                                file_saved = true;
                                                break;
                                        case 2:
                                                return 0;
                                }
                        }
                        break;
                case WM_ERASEBKGND:
                        return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_CHAR:
                        break;
                case WM_ACTIVATE:
                        SetFocus(hEdit);
			return 0;
                case WM_NOTIFY:
                {
			NMHDR* pnmhdr = reinterpret_cast<NMHDR*>(lParam);

                        switch(pnmhdr->code)
                        {
                                case TTN_GETDISPINFO:
                                {
					static LPTOOLTIPTEXT lpttt;

					lpttt = (LPTOOLTIPTEXT) lParam;
                                        lpttt->hinst = gInstance;
					//lpttt->uFlags = TTF_DI_SETITEM;
					int idButton = lpttt->hdr.idFrom;

                                        static char tooltip_info[STDLINE+1];
                                        (void)strcpy(tooltip_info, "");

                                        switch(idButton)
		                	{
                                                case IDM_FILE_NEW:
                                                        (void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_NEWFILE));
                                                        break;
                                                case IDM_FILE_OPEN:
							(void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_OPENFILE));
                                                        break;
                                                case IDM_FILE_SAVE:
							(void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_SAVEFILE));
							break;
                                                case IDM_EDIT_UNDO:
							(void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_UNDO));
							break;
                                                case IDM_EDIT_REDO:
                                                        (void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_REDO));
							break;
                                                case IDM_EDIT_CUT:
							(void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_CUT));
                                                        break;
						case IDM_EDIT_COPY:
                                                        (void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_COPY));
                                                        break;
						case IDM_EDIT_PASTE:
                                                        (void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_PASTE));
							break;
                                                case IDM_EDIT_DELETE:
                                                        (void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_DELETE));
                                                        break;
                                                case IDM_EDIT_FIND:
                                                        (void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_FIND));
                                                        break;
                                                case IDM_SCRIPT_RUN:
                                                        (void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_RUN));
							break;
						case IDM_HELP_REFERENCE:
							(void)strcpy(tooltip_info, getLngText(LNG_TOOLTIP_HELP));
							break;
					}

					lpttt->lpszText = tooltip_info;
					break;
				}
				case EN_MSGFILTER:
				{
					MSGFILTER* pFilter = reinterpret_cast<MSGFILTER*>(lParam);

					if(pFilter->msg == WM_KEYUP)
					{
						int selStart, selEnd;
						SendMessage(hEdit, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);

						if(selStart != selEnd)
							break;

						switch(pFilter->wParam)
						{
							case VK_RETURN:
							{
								if(!behind_dialog)
								{
									int EventMask = SendMessage(hEdit, EM_SETEVENTMASK, 0, ENM_NONE);
									DISABLE_UNDOREDO();
									SendMessage(hEdit, EM_REPLACESEL, (WPARAM)true, (LPARAM)nextLineIndent());
									ENABLE_UNDOREDO();
									SendMessage(hEdit, EM_SETEVENTMASK, 0, EventMask);
								}
								break;
							}
                                                }

						SendMessage(hEdit, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);
						if(selStart == selEnd)
						{
							set_lineinfo();
							file_saved = false;
							setCursorColor();
						}

						/*
						if((GetAsyncKeyState(VK_TAB) & 0x80000000)? 1 : 0)
						{
							keybd_event( VK_CONTROL, 0, 0, 0);
							keybd_event( VK_TAB, 0, 0, 0);
							keybd_event( VK_TAB, 0, KEYEVENTF_KEYUP, 0);
							keybd_event( VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
						}
						*/
					}
					/*
					else if(pFilter->msg == WM_KEYDOWN && pFilter->wParam == VK_RETURN)
					{
						SetFocus(hTmpEdit);
					}
					else if(pFilter->msg == WM_KEYUP && pFilter->wParam == VK_RETURN)
					{
						SetFocus(hEdit);
					}
					*/
					else if(pFilter->msg == WM_LBUTTONUP)
					{
						set_lineinfo();
					}

					break;
				}
				default:
					break;
			}
			break;
		}

                case WM_COMMAND:
                        if((HWND)lParam == hEdit && HIWORD(wParam) == EN_CHANGE)
                        {
                                switch(LOWORD(wParam))
				{
                                        case EDIT_MSG:
                                                int pos;
						SendMessage(hEdit, EM_GETSEL, (WPARAM) &pos, NULL);

						pos = (int)SendMessage(hEdit, EM_LINEFROMCHAR, pos, 0);


                                                if(!canAvoidHighlight(pos))
						{
							highlightCode(pos);
							//SendMessage(hEdit, EM_EMPTYUNDOBUFFER, 0, 0);
                                                }
                                                break;
                                }
                        }
                        else
                        {
                                switch(LOWORD(wParam))
                                {
                                        case IDM_FILE_NEW:
                                                if(!file_saved)
						{
                                                        switch(MessageBox(hMain, getLngText(LNG_MSG_SAVEANYCHANGES), "RapidBATCH Builder", 32 + 3))
                                                        {
                                                                case 6:
                                                                        if(!saveFile(current_file, false)) return 0;
                                                                        file_saved = true;
									break;
                                                                case 2:
                                                                        return 0;
							}
                                                }
						set_caption(getLngText(LNG_DEFAULTFILE));
						(void)strcpy(current_file, "");
						SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)"");
                                                file_saved = true;
                                                set_lineinfo();
                                                break;
					case IDM_FILE_OPEN:
						if(!file_saved)
                                                {
                                                        switch(MessageBox(hMain, getLngText(LNG_MSG_SAVEANYCHANGES), "RapidBATCH Builder", 32 + 3))
                                                        {
                                                                case 6:
                                                                        if(!saveFile(current_file, false)) return 0;
                                                                        file_saved = true;
                                                                        break;
                                                                case 2:
                                                                        return 0;
                                                        }
                                                }

						if(loadFile(current_file, true))
                                                {
                                                        file_saved = true;
							set_caption(current_file);
                                                }
						break;
					case IDM_FILE_SAVE:
                                                if(saveFile(current_file, false))
                                                {
                                                        file_saved = true;
                                                        set_caption(current_file);
                                                }
						break;
                                        case IDM_FILE_SAVE_AS:
						if(saveFile(current_file, true))
                                                {
                                                        file_saved = true;
							set_caption(current_file);
                                                }
                                                break;
                                        case IDM_FILE_PRINT:

                                                //PrintFile(gInstance, hMain, hEdit, "Test");
                                                PrintFile();
                                                break;

                                        case IDM_FILE_QUIT:
						PostQuitMessage(0);
                                                break;
					case IDM_EDIT_UNDO:
                                                if(SendMessage(hEdit, EM_CANUNDO, 0, 0) != 0)
                                                {
                                                        SendMessage(hEdit, EM_UNDO, 0, 0);
                                                }
						break;
					case IDM_EDIT_REDO:
                                                if(SendMessage(hEdit, EM_CANREDO, 0, 0) != 0)
                                                {
                                                        SendMessage(hEdit, EM_REDO, 0, 0);
                                                }
                                                break;
                                        case IDM_EDIT_CUT:
						SendMessage(hEdit, WM_CUT, 0, 0);
						break;
					case IDM_EDIT_COPY:
						SendMessage(hEdit, WM_COPY, 0, 0);
                                                break;
                                        case IDM_EDIT_PASTE:
                                        {
						SendMessage(hTmpEdit, WM_PASTE, 0, 0);

						long line, col;
						SendMessage(hEdit, EM_GETSEL, (WPARAM)&col, NULL);
						line = (int)SendMessage(hEdit, EM_LINEFROMCHAR, col, 0);

						SendMessage(hEdit, WM_PASTE, 0, 0);

						for(int i = line; i < (line + SendMessage(hTmpEdit, EM_GETLINECOUNT,NULL,NULL));i++)
						{
                                                        highlightCode(i);
                                                }
                                                SendMessage(hTmpEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)"");
						break;
                                        }
					case IDM_EDIT_DELETE:
						SendMessage(hEdit, EM_REPLACESEL, TRUE, (LPARAM)"");
						break;
					case IDM_EDIT_SELECT_ALL:
						SendMessage(hEdit, EM_SETSEL, 0, -1);
                                                break;
					case IDM_EDIT_FIND_NEXT:
                                                if(findstring != (char*)NULL)
                                                {
							find_text.lpstrText = findstring;

							SendMessage(hEdit, EM_GETSEL, (WPARAM)&last_pos.cpMin, (LPARAM)NULL);
							find_text.chrg.cpMin = last_pos.cpMin+1;

							//Excuse me for this bad thing ...;)
                                                        seach_again:
                                                        find_text.chrg.cpMax = (int)SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
                                                        last_pos.cpMin = (int)SendMessage(hEdit, EM_FINDTEXT, FR_DOWN, (LPARAM)&find_text);

							if(last_pos.cpMin > -1)
                                                        {
								SendMessage(hEdit, EM_SETSEL, last_pos.cpMin, last_pos.cpMin + strlen(find_text.lpstrText));
							}
							else
                                                        {
                                                                find_text.chrg.cpMin = 0;
								goto seach_again;
                                                        }
                                                        set_lineinfo();
                                                        break;
						}
                                        case IDM_EDIT_FIND:
                                        {
						int result = DialogBox(gInstance, "IDD_FIND", hWnd, Find_DlgProc);
						behind_dialog = true;

						switch (result)
                                                {
                                                        case -1:
                                                                break;
							case 0:
								find_text.lpstrText = findstring;

								find_text.chrg.cpMin = 0;
								find_text.chrg.cpMax = (int)SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
								last_pos.cpMin = (int)SendMessage(hEdit, EM_FINDTEXT, FR_DOWN, (LPARAM)&find_text);

                                                                if(last_pos.cpMin > -1)
                                                                {
									SendMessage(hEdit, EM_SETSEL, last_pos.cpMin, last_pos.cpMin + strlen(find_text.lpstrText));

									set_lineinfo();
                                                                }
                                                                else
                                                                {
									INFO_MSG(getLngText(LNG_MSG_NORESULTSFOUND));
									if(findstring != (char*)NULL) free(findstring);
									findstring = (char*)NULL;
								}
								break;

						}
						return 0;
					}
					case IDM_EDIT_COMMENT_SEL:
					{
						/* REM-setzen oder entfernen */
						long int start, end;
						long int line_cnt = 0;
						char* selection = (char*)NULL;
						char* newtext = (char*)NULL;

						SendMessage(hEdit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);

						start = SendMessage(hEdit, EM_LINEINDEX, (int)SendMessage(hEdit, EM_LINEFROMCHAR, (WPARAM)start, (LPARAM)0), 0);
						end = SendMessage(hEdit, EM_LINEINDEX, (int)SendMessage(hEdit, EM_LINEFROMCHAR, (WPARAM)end, (LPARAM)0), 0)
							+ SendMessage(hEdit, EM_LINELENGTH, end, 0);

							int last_line = -1;
							int line = -1;

							for(long int i = start; i <= end; i++)
							{
								line = (int)SendMessage(hEdit, EM_LINEFROMCHAR, (WPARAM)i, (LPARAM)0);

								if(line != last_line)
								{
									long int len = SendMessage(hEdit, EM_LINELENGTH, i, 0);

									line_cnt++;

									if(len == 0)
									{
										if(newtext == (char*)NULL)
										{
											newtext = (char*)malloc((2+3) * sizeof(char));
											*newtext = '\0';
										}
										else
										{
											newtext = (char*)realloc((char*)newtext,
														(strlen(newtext)+2+3) * sizeof(char));
										}


										if(start != end)
										{
											(void)strcat(newtext, "\r\n");
											continue;
										}
									}


									selection = (char*) malloc((len + 3) * sizeof(char));

									*(int *) selection = (int)len;

									SendMessage(hEdit, EM_GETLINE, (WPARAM)line, (LPARAM)(char*)selection);

									selection[len] = '\0';

									char* tmp = strdup(selection);
									strupr(tmp);
									//tmp = strltrim(tmp);

									bool freetmp = false;
									if(strcmp(strsubstr(tmp, 0, 4), "REM ") == 0)
									{
										free(tmp);

										tmp = selection;

										int j = 0;
										for(; j<(signed int)strlen(selection);j++)
										{
											if(*tmp != ' ') break;
											tmp += j;
										}

										tmp = strsubstr(tmp, 4, strlen(selection));
									}
									else
									{
										tmp = (char*)realloc((char*)tmp, (strlen(selection)+3+1+1) * sizeof(char));
										sprintf(tmp, "rem %s", selection);
										freetmp = true;
									}

									if(newtext == (char*)NULL)
									{
										newtext = (char*)malloc((strlen(tmp)+3) * sizeof(char));
										*newtext = '\0';
									}
									else
									{
										newtext = (char*)realloc((char*)newtext,
													(strlen(newtext)+strlen(tmp)+3) * sizeof(char));
									}

									if(strlen(newtext) != 0)
										(void)strcat(newtext, "\r\n");
										
									(void)strcat(newtext, tmp);

									if(freetmp)
										free(tmp);

									free(selection);

									last_line = line;
								}
							}
						

						onHighlight = true;
						SendMessage(hEdit, EM_SETSEL, (WPARAM)start, (LPARAM)end);
						SendMessage(hEdit, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)newtext);


						start = (int)SendMessage(hEdit, EM_LINEFROMCHAR, (WPARAM)start, (LPARAM)0);
                                                end = start + line_cnt;

						for(long int j=start; j < end; j++)
						{
							highlightCode(j);
						}

						onHighlight = false;

						if(newtext != (char*)NULL)
						{
							/*char bla[1024];
							sprintf(bla, ">%s<", newtext);
							INFO_MSG(bla);*/
							free(newtext);
						}


						break;
					}
                                        case IDM_SCRIPT_RUN:
                                                if(saveFile(current_file, false))
                                                {
                                                        run_script(current_file, cmdline_parms, false);
							set_caption(current_file);
                                                }
                                                break;
                                        case IDM_SCRIPT_COMPILE:
					{
						int result = DialogBox(gInstance, "IDD_COMPILE", hWnd, Compile_DlgProc);
                                                switch (result)
                                                {
                                                        case -1:
                                                                //INFO_MSG("cancel");
                                                                break;
							default:
								break;
                                                }
                                                break;
                                        }
                                        case IDM_SCRIPT_DEBUG:
					{
						int result = DialogBox(gInstance, "IDD_DEBUG", hWnd, Debug_DlgProc);
                                                behind_dialog = true;

                                                switch (result)
                                                {
							case -1:
                                                                //INFO_MSG("cancel");
                                                                break;
                                                        default:
								break;
                                                }
                                                break;
					}
					case IDM_HELP_ABOUT:
					{
                                                //INFO_MSG("RapidBATCH v5.0 Builder 4.0 Beta III\n(C)2004, 2005 by J.M.K S.F., Jan Max Meyer\n\n+++ UNFINISHED BETA VERSION +++\nThis software can have malfunctions.");
						DialogBox(gInstance, "IDD_ABOUT", hWnd, About_DlgProc);
                                                break;
                                        }
                                        case IDM_EDIT_GOTO_LINE:
					{
						int result = DialogBox(gInstance, "IDD_GOTOLINE", hWnd, GotoLine_DlgProc);
						behind_dialog = true;

                                                switch (result)
                                                {
                                                        case -1:
                                                                //INFO_MSG("cancel");
								break;
                                                        default:
                                                                break;
						}
                                                break;
                                        }
                                        case IDM_SCRIPT_COMMAND:
					{
						int result = DialogBox(gInstance, "IDD_SETCOMMAND", hWnd, SetCommand_DlgProc);
						behind_dialog = true;

						switch (result)
                                                {
                                                        case -1:
                                                                //INFO_MSG("cancel");
                                                                break;
							default:
                                                                break;
                                                }
                                                break;
					}

					case IDM_HELP_MANUAL:
					case IDM_HELP_WHATSNEW:
					case IDM_HELP_REFERENCE:
                                        case IDM_HELP_LICENSE:
                                        case IDM_HELP_WEBSITE:
						NOT_IMPLEMENTED;
						break;
					case IDM_TOOLS_CONFIGURE:
						ShowWindow(hMain, SW_HIDE);

						run_script( "etc\\rbb_tc.rb", "", true);

						loadToolsMenu();
						ShowWindow(hMain, SW_SHOW);
                                                SetActiveWindow(hMain);
						break;

					default:
						if(LOWORD(wParam) > IDM_TOOLS_MENU &&
							LOWORD(wParam) <= IDM_TOOLS_MENU + tools_exec_cnt)
						{
							char* exec = strdup( tools_exec[ LOWORD(wParam) - IDM_TOOLS_MENU - 1] );

							//Path variables
							char temp_val[STDLINE+1];

							(void)strreplace(&exec, "$CURRENT", current_file, true);
							(void)strreplace(&exec, "$RBHOME", rb_path, true);

                                                        sprintf(temp_val, "%s\\RBI32.EXE", rb_path);
							(void)strreplace(&exec, "$RBI", temp_val, true);

                                                        sprintf(temp_val, "%s\\RBC32.EXE", rb_path);
							(void)strreplace(&exec, "$RBC", temp_val, true);

							/* sprintf(nc, "%d", GetCurrentProcessId());
							(void)strreplace(&exec, "$rbb_pid", nc, true); */

							//INFO_MSG(tools_exec[ LOWORD(wParam) - IDM_TOOLS_MENU - 1]);
							exec_program( exec, false );

						}
						/*
						else if(LOWORD(wParam) >  IDM_FILE_HISTORY)
						{
							char xxx[100];
							sprintf(xxx, "%d %d", LOWORD(wParam), LOWORD(wParam) - IDM_FILE_HISTORY - 1);
							INFO_MSG(xxx);
						}
						*/
						else if(LOWORD(wParam) > IDM_FILE_HISTORY &&
							LOWORD(wParam) <= IDM_FILE_HISTORY + history_cnt)
						{
							if(strcmp(current_file, history[LOWORD(wParam) - IDM_FILE_HISTORY - 1]) != 0)
							{
								if(!file_saved)
								{
									switch(MessageBox(hMain, getLngText(LNG_MSG_SAVEANYCHANGES), "RapidBATCH Builder", 32 + 3))
									{
										case 6:
											if(!saveFile(current_file, false)) return 0;
											file_saved = true;
											break;
										case 2:
											return 0;
									}
								}


								if(loadFile( history[LOWORD(wParam) - IDM_FILE_HISTORY - 1] , false ))
								{
									file_saved = true;
									strcpy(current_file, history[LOWORD(wParam) - IDM_FILE_HISTORY - 1]);
									set_caption(current_file);
								}
							}
						}
						break;
				}
			}
			return 0;
		default:
			break;

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


//------------------------------------------------------------------------------
// readSyntaxDef()
//------------------------------------------------------------------------------
void readSyntaxDef(void)
{
	char buffer[STDLINE+1];

        (void)sprintf(buffer, "%s\\etc\\rbatch.syn", rb_path);

	FILE* synFile = (FILE*) NULL;
        synFile = fopen(buffer, "rt");
        if(synFile == (FILE*)NULL)
        {
                INFO_ERROR("File etc\\rbatch.syn not found.\nSyntax-Highlighting will be disabled.");
		return;
	}

	//Try to read CLASS_KEYWORD. If it does not exist, run RapidBATCH builder setup-script
	if(!getRegEntry(HKEY_CURRENT_USER, RBATCH_REGISTRY_PATH, "CLASS_KEYWORD", buffer, STDLINE))
	{
		sprintf(buffer, "%s\\etc\\rbb_setup.rb", rb_path);
		run_script(buffer, "nowarning", true);
	}

        while(!feof(synFile) && fgets(buffer, STDLINE, synFile) != (char*) NULL)
        {
                switch(*buffer)
                {
                        case ';':
                        case '\n':
				break;

                        default:
                                char identifier [STDFIELD+1];
                                char ident_class[STDFIELD+1];
                                int othermayfollow = 0;

                                sscanf(buffer, "%s %s %d", &identifier, &ident_class, &othermayfollow);

                                if(synInfo_cnt == 0)
                                {
                                        synInfo = (SYNINFO*)malloc(sizeof(SYNINFO));
                                }
				else
				{
                                        synInfo = (SYNINFO*)realloc((SYNINFO*)synInfo, (synInfo_cnt+1) * sizeof(SYNINFO));
				}

                                //Identifier
                                synInfo[synInfo_cnt].ident = (char*) malloc((strlen(identifier)+1) * sizeof(char));
				strcpy(synInfo[synInfo_cnt].ident, identifier);

                                if(othermayfollow == 0)
                                        synInfo[synInfo_cnt].otherCharMayFollow = true;
				else
                                        synInfo[synInfo_cnt].otherCharMayFollow = false;


				if(strcmp(ident_class, "KEYWORD") == 0)
				{
					synInfo[synInfo_cnt].identClass = CLASS_KEYWORD;
					strupr(synInfo[synInfo_cnt].ident);
				}
                                else if(strcmp(ident_class, "OPERATOR") == 0)
                                        synInfo[synInfo_cnt].identClass = CLASS_OPERATOR;
                                else if(strcmp(ident_class, "VARIABLE") == 0)
                                        synInfo[synInfo_cnt].identClass = CLASS_VARIABLE;
                                else if(strcmp(ident_class, "VALUE") == 0)
                                        synInfo[synInfo_cnt].identClass = CLASS_VALUE;
				else if(strcmp(ident_class, "SINGLELINE_COMMENT") == 0)
				{
					synInfo[synInfo_cnt].identClass = CLASS_COMMENT;
					strupr(synInfo[synInfo_cnt].ident);
				}
				else if(strcmp(ident_class, "LABEL") == 0)
                                        synInfo[synInfo_cnt].identClass = CLASS_LABEL;
				else if(strcmp(ident_class, "SPECIAL_COMMENT") == 0)
				{
					synInfo[synInfo_cnt].identClass = CLASS_SPECIAL_COMMENT;
					strupr(synInfo[synInfo_cnt].ident);
				}

                                synInfo_cnt++;
		}
        }
        fclose(synFile);

	/*
	char test[100];
        sprintf(test, "%d %d %d", R, G, B);
        INFO_MSG(test);
        */


	keyword_color[CLASS_KEYWORD] = getClassColor("CLASS_KEYWORD"); //RGB(0,0,0);
        //keyword_bold[CLASS_KEYWORD] = getClassBoldState("CLASS_KEYWORD");
        keyword_color[CLASS_OPERATOR] = getClassColor("CLASS_OPERATOR");
        //keyword_bold[CLASS_OPERATOR] = getClassBoldState("CLASS_OPERATOR");
        keyword_color[CLASS_VARIABLE] = getClassColor("CLASS_VARIABLE");
        //keyword_bold[CLASS_VARIABLE] = getClassBoldState("CLASS_VARIABLE");
        keyword_color[CLASS_VALUE] = getClassColor("CLASS_VALUE");
        //keyword_bold[CLASS_VALUE] = getClassBoldState("CLASS_VALUE");
	keyword_color[CLASS_COMMENT] = getClassColor("CLASS_COMMENT");
        //keyword_bold[CLASS_COMMENT] = getClassBoldState("CLASS_COMMENT");
        keyword_color[CLASS_LABEL] = getClassColor("CLASS_LABEL");
        //keyword_bold[CLASS_LABEL] = getClassBoldState("CLASS_LABEL");
        keyword_color[CLASS_SPECIAL_COMMENT] = getClassColor("CLASS_SPECIAL_COMMENT");
        //keyword_bold[CLASS_SPECIAL_COMMENT] = getClassBoldState("CLASS_SPECIAL_COMMENT");

        base_color = getClassColor("CLASS_TEXT");
	if(!getRegEntry(HKEY_CURRENT_USER, RBATCH_REGISTRY_PATH, "FontFace", fontFace, STDLINE))
        {
                (void)strcpy(fontFace, "Courier New");
        }

        SendMessage(hEdit, EM_SETBKGNDCOLOR, 0, getClassColor("CLASS_BKGND"));
}

inline char* nextLineIndent(void)
{
	static char indent[STDLINE+1];
	(void)strcpy(indent, "");

	int codeLine = SendMessage(hEdit,EM_LINEFROMCHAR, -1, 0);
	if(codeLine-1 > -1)
	{
		//Get length and position of the current line
		int lineStart = SendMessage(hEdit, EM_LINEINDEX, codeLine-1, 0);
		int lineEnd = lineStart + SendMessage(hEdit, EM_LINELENGTH, lineStart, 0);

		//Copy previous line into text buffer
		char* line = (char*)NULL;
		line = (char*) malloc((((lineEnd - lineStart) + 3)) * sizeof(char));

		*(int *) line = (lineEnd - lineStart) ;
		SendMessage(hEdit, EM_GETLINE, (WPARAM)codeLine-1, (LPARAM)line);

		line[(lineEnd - lineStart)] = '\0';

		for(unsigned int i=0; i < strlen(line) && i < STDLINE;i++)
		{
			switch(line[i])
			{
				case '\t':
				case ' ':
					indent[i] = line[i];
					indent[i+1] = '\0';
					break;
			}
		}

		free(line);
	}
	return indent;
}


//------------------------------------------------------------------------------
// highlightCode()
//------------------------------------------------------------------------------
inline bool highlightCode(int codeLine)
{
        //td_code->Undo(tomSuspend, NULL);
        DISABLE_UNDOREDO();

	int EventMask = SendMessage(hEdit, EM_SETEVENTMASK, 0, ENM_NONE);

        if(!multilineHighlight) SetFocus(hTmpEdit);
	onHighlight = true;

        SendMessage(hEdit, EM_HIDESELECTION, true, false);
        bool unLockWindow = false;

        lastCodeLine = codeLine;

        //Get current position
	CHARRANGE Original;
	SendMessage(hEdit, EM_EXGETSEL, 0, (LPARAM)&Original);

	//Get length and position of the current line
	long int lineStart = SendMessage(hEdit, EM_LINEINDEX, codeLine, 0);
	long int lineEnd = lineStart + SendMessage(hEdit, EM_LINELENGTH, lineStart, 0);
	//fprintf(debug, "codeLine = %d lineStart = %d lineEnd = %d length: %d\n", codeLine, lineStart, lineEnd, lineEnd - lineStart);

	CHARRANGE LineSelection;
	LineSelection.cpMin = lineStart;
	LineSelection.cpMax = lineEnd;
	SendMessage(hEdit, EM_EXSETSEL, 0, (LPARAM)&LineSelection);

        CHARFORMAT FormatNormal;
        memset(&FormatNormal, 0, sizeof(CHARFORMAT));
        FormatNormal.cbSize = sizeof(CHARFORMAT);
	FormatNormal.dwMask = CFM_COLOR | CFM_BOLD | CFM_FACE | CFM_SIZE;
        FormatNormal.crTextColor = base_color;
	FormatNormal.dwEffects = 0;

	FormatNormal.yHeight = 200;
        FormatNormal.bCharSet = 0xEE;
        strcpy(FormatNormal.szFaceName, fontFace);

	SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&FormatNormal);

        //Copy current line into text buffer
        char* line = (char*)NULL;
        line = (char*) malloc((((lineEnd - lineStart) + 3)) * sizeof(char));

        //fprintf(debug, "Allocating: %d bytes\n", line_size);

	*(int *) line = (lineEnd - lineStart) ;
	SendMessage(hEdit, EM_GETLINE, (WPARAM)codeLine, (LPARAM)line);
        line[(lineEnd - lineStart)] = '\0';

        char keyword    [STDFIELD+1];
        keyword[0] = '\0';

        CHARFORMAT FormatKey;
        memset(&FormatKey, 0, sizeof(FormatKey));
        FormatKey.cbSize = sizeof(FormatKey);
	FormatKey.dwMask = CFM_COLOR | CFM_BOLD;

        int kwPos = 0;
        bool commentLine = false;

        //Free the keyword_cnt informations
        for(int i = 0; i < (sizeof(keyword_cnt)/sizeof(keyword_cnt[0])); i++) keyword_cnt[i] = 0;

        //Highlight the keywords
	for(unsigned int i = 0; i < (strlen(line)+1); i++)
	{
		for(int j = 0; j < synInfo_cnt; j++)
                {
                        if(keyword[0] == synInfo[j].ident[0] && (synInfo[j].identClass == CLASS_VARIABLE
                                || synInfo[j].identClass == CLASS_VALUE))
                        {
                                FormatKey.dwEffects = 0;
                                if(synInfo[j].identClass == CLASS_VARIABLE)
                                {
                                        FormatKey.crTextColor = keyword_color[CLASS_VARIABLE];
					keyword_cnt[CLASS_VARIABLE]++;
                                }
                                else
                                {
                                        FormatKey.crTextColor = keyword_color[CLASS_VALUE];
                                        keyword_cnt[CLASS_VALUE]++;
                                }

                                int vStart = i-1;

                                int varOpen = 0;
				for(;i < ((signed int)strlen(line)+1);i++)
                                {
                                        if(line[i] == synInfo[j].ident[0] && synInfo[j].identClass == CLASS_VARIABLE) varOpen++;
                                        if(line[i] == synInfo[j].ident[1])
					{
                                                if(varOpen > 0)
                                                {
							varOpen--;
                                                        continue;
                                                }
                                                i++;
						break;
                                        }
                                        //fprintf(debug, ">%c< >%s> %d %d vStart %d\n", line[i], synInfo[j].ident, i, varOpen, vStart);
                                }

				if(i >= (signed int)strlen(line))
				{
                                        if(synInfo[j].identClass == CLASS_VARIABLE)
                                        {
						keyword_cnt[CLASS_VARIABLE]++;
                                        }
                                        else
                                        {
                                                keyword_cnt[CLASS_VALUE]++;
                                        }
                                }


                                SendMessage(hEdit, EM_SETSEL, lineStart + vStart, lineStart + i);
                                SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&FormatKey);

				kwPos = 0;
                                break;
                        }
			if(strcmp(keyword, synInfo[j].ident) == 0 && synInfo[j].identClass == CLASS_OPERATOR)
                        {
                                FormatKey.dwEffects = 0;

                                if(synInfo[j].otherCharMayFollow && ((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z'))) break;

                                FormatKey.crTextColor = keyword_color[CLASS_OPERATOR];
                                keyword_cnt[CLASS_OPERATOR]++;

                                SendMessage(hEdit, EM_SETSEL, lineStart + (i - kwPos), lineStart + i);
                                SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&FormatKey);

                                kwPos = 0;
                                break;
			}

			strupr(keyword);
			if(strcmp(keyword, synInfo[j].ident) == 0 && synInfo[j].identClass == CLASS_KEYWORD)
			{
				if(synInfo[j].otherCharMayFollow && ((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z'))) break;

				FormatKey.dwEffects = CFE_BOLD;
				FormatKey.crTextColor = keyword_color[CLASS_KEYWORD];
                                keyword_cnt[CLASS_KEYWORD]++;

                                SendMessage(hEdit, EM_SETSEL, lineStart + (i - kwPos), lineStart + i);
                                SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&FormatKey);

                                kwPos = 0;
				break;
			}
			if(strcmp(keyword, synInfo[j].ident) == 0 &&
				(synInfo[j].identClass == CLASS_COMMENT || synInfo[j].identClass == CLASS_LABEL
					|| synInfo[j].identClass == CLASS_SPECIAL_COMMENT))
                        {
                                if(synInfo[j].otherCharMayFollow && ((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z'))) break;

                                FormatKey.dwEffects = 0;

                                if(synInfo[j].identClass == CLASS_COMMENT)
                                {
                                        keyword_cnt[CLASS_COMMENT]++;
                                        FormatKey.crTextColor = keyword_color[CLASS_COMMENT];
                                }
                                else if(synInfo[j].identClass == CLASS_SPECIAL_COMMENT)
                                {
                                        keyword_cnt[CLASS_SPECIAL_COMMENT]++;
                                        FormatKey.crTextColor = keyword_color[CLASS_SPECIAL_COMMENT];
                                }
                                else
                                {
					keyword_cnt[CLASS_LABEL]++;
                                        FormatKey.crTextColor = keyword_color[CLASS_LABEL];
                                }

                                SendMessage(hEdit, EM_SETSEL, lineStart + (i - kwPos), lineEnd);
                                SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&FormatKey);

                                kwPos = 0;
                                commentLine = true;
				break;
                        }
		}

                if(commentLine) break;

                if(!((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z')))
                {
			kwPos = 0;
                        keyword[0] = '\0';
                }

                if(line[i] != ' ' && line[i] != '\t')
                {
                        keyword[kwPos] = line[i];
                        keyword[kwPos+1] = '\0';

                        kwPos++;
                }
        }
	//memset(&line, 0, line_size * sizeof(char));
        SendMessage(hEdit, EM_SETEVENTMASK, 0, EventMask);
	SendMessage(hEdit, EM_HIDESELECTION, false, false);
        SendMessage(hEdit, EM_EXSETSEL, 0, (LPARAM)&Original);

	onHighlight = false;
	if(!multilineHighlight) SetFocus(hEdit);

        //fprintf(debug, "Freeing line length: %d Data: \"%s\"  bytes should be %d\n", strlen(line), line, line_size);
        //fflush(debug);
        free(line);
        //fprintf(debug, "------------------------------------------------------------------------ Ok\n");

        //td_code->Undo(tomResume, NULL);
        ENABLE_UNDOREDO();
	return true;
}

//------------------------------------------------------------------------------
// canAvoidHighlight()
//------------------------------------------------------------------------------
inline bool canAvoidHighlight(int codeLine)
{
	if(lastCodeLine != codeLine)
	{
                return false;
        }

	CHARFORMAT CurrentFormat;
        memset(&CurrentFormat, 0, sizeof(CHARFORMAT));
        CurrentFormat.cbSize = sizeof(CHARFORMAT);
        CurrentFormat.dwMask = CFM_BOLD | CFM_FACE | CFM_SIZE;
        CurrentFormat.dwEffects = 0;

	//Get length and position of the current line
        int lineStart = SendMessage(hEdit, EM_LINEINDEX, codeLine, 0);
	int lineEnd = lineStart + SendMessage(hEdit, EM_LINELENGTH, lineStart, 0);

        int keywordtmp_cnt[SYNTAX_CLASSES];

	CHARRANGE LineSelection;
        LineSelection.cpMin = lineStart;
        LineSelection.cpMax = lineEnd;

        //Copy current line into text buffer
        char* line = (char*)NULL;
        line = (char*) malloc((((lineEnd - lineStart) + 3)) * sizeof(char));

        *(int *) line = (lineEnd - lineStart) ;
        SendMessage(hEdit, EM_GETLINE, (WPARAM)codeLine, (LPARAM)line);
        line[(lineEnd - lineStart)] = '\0';

        char keyword    [STDFIELD+1];
        keyword[0] = '\0';

        int kwPos = 0;
	bool commentLine = false;

        //Free the keywordtmp_cnt informations
        for(int i = 0; i < (sizeof(keywordtmp_cnt)/sizeof(keywordtmp_cnt[0])); i++) keywordtmp_cnt[i] = 0;

        //Highlight the keywords
        for(unsigned int i = 0; i < (strlen(line)+1); i++)
        {
		for(int j = 0; j < synInfo_cnt; j++)
                {
                        if(keyword[0] == synInfo[j].ident[0] && (synInfo[j].identClass == CLASS_VARIABLE
				|| synInfo[j].identClass == CLASS_VALUE))
                        {
				if(synInfo[j].identClass == CLASS_VARIABLE)
                                {
                                        keywordtmp_cnt[CLASS_VARIABLE]++;
                                }
                                else
                                {
                                        keywordtmp_cnt[CLASS_VALUE]++;
                                }

                                int varOpen = 0;
                                bool endOfValue = false;
				for(;i < (strlen(line)+1);i++)
                                {
                                        if(line[i] == synInfo[j].ident[0] && synInfo[j].identClass == CLASS_VARIABLE) varOpen++;
                                        if(line[i] == synInfo[j].ident[1])
					{
                                                if(varOpen > 0)
                                                {
                                                        varOpen--;
                                                        continue;
                                                }
                                                i++;
                                                endOfValue = true;
                                                break;
                                        }
                                }

                                if(i >= strlen(line))
                                {
					if(synInfo[j].identClass == CLASS_VARIABLE)
                                        {
						CurrentFormat.crTextColor = keyword_color[CLASS_VARIABLE];
                                                keywordtmp_cnt[CLASS_VARIABLE]++;
                                        }
                                        else
                                        {
                                                CurrentFormat.crTextColor = keyword_color[CLASS_VALUE];
                                                keywordtmp_cnt[CLASS_VALUE]++;
                                        }
                                }


                                kwPos = 0;
				break;
                        }
                        if(strcmp(keyword, synInfo[j].ident) == 0 && synInfo[j].identClass == CLASS_OPERATOR)
                        {
                                if(synInfo[j].otherCharMayFollow && ((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z'))) break;
                                keywordtmp_cnt[CLASS_OPERATOR]++;
                                kwPos = 0;
                                break;
			}

			strupr(keyword);
			if(strcmp(keyword, synInfo[j].ident) == 0 && synInfo[j].identClass == CLASS_KEYWORD)
                        {
                                if(synInfo[j].otherCharMayFollow && ((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z'))) break;

                                keywordtmp_cnt[CLASS_KEYWORD]++;
				kwPos = 0;
                                break;
			}

			//Easter egg ;)) XD
			if(strcmp(keyword, "YIFF") == 0 && !egg)
			{
				INFO_MSG("Einen lieben Gruss an das Furry Fandom!\n\n*fieps* Euer ~neo");
				egg = true;
				break;
			}

			if(strcmp(keyword, synInfo[j].ident) == 0 && (synInfo[j].identClass == CLASS_COMMENT || synInfo[j].identClass == CLASS_LABEL || synInfo[j].identClass == CLASS_SPECIAL_COMMENT))
                        {
                                if(synInfo[j].otherCharMayFollow && ((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z'))) break;

                                if(synInfo[j].identClass == CLASS_COMMENT)
                                {
                                        keywordtmp_cnt[CLASS_COMMENT]++;
                                }
                                else if(synInfo[j].identClass == CLASS_SPECIAL_COMMENT)
                                {
                                        keywordtmp_cnt[CLASS_SPECIAL_COMMENT]++;
				}
                                else
                                {
                                        keywordtmp_cnt[CLASS_LABEL]++;
                                }

                                kwPos = 0;
                                commentLine = true;
                                break;
                        }
                }

                if(commentLine) break;

                //fprintf(debug, "An line[%d]: >%c<\n", i, line[i]);
                if(!((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z')))
                {
			kwPos = 0;
                        keyword[0] = '\0';
                }

		if(line[i] != ' ' && line[i] != '\t')
                {
                        keyword[kwPos] = line[i];
                        keyword[kwPos+1] = '\0';
                        kwPos++;
                }
        }
	//Verify keywordtmp_cnt with keyword_cnt informations
        for(int i = 0; i < (sizeof(keywordtmp_cnt)/sizeof(keywordtmp_cnt[0])); i++)
        {
                if(keywordtmp_cnt[i] != keyword_cnt[i]) return false;
        }


        CurrentFormat.yHeight = 200;
        CurrentFormat.bCharSet = 0xEE;
        strcpy(CurrentFormat.szFaceName, fontFace);

        SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&CurrentFormat);

        free(line);

        return true;
}
//------------------------------------------------------------------------------
// setCursorColor()
//------------------------------------------------------------------------------
inline void setCursorColor()
{
        int codeLine, caretPos;
	SendMessage(hEdit, EM_GETSEL, (WPARAM)&caretPos, NULL);
        codeLine = (int)SendMessage(hEdit, EM_LINEFROMCHAR, caretPos, 0);
        caretPos = caretPos - SendMessage(hEdit, EM_LINEINDEX, codeLine, 0);

        bool formatNormal = true;

        //Get length and position of the current line
        int lineStart = SendMessage(hEdit, EM_LINEINDEX, codeLine, 0);
        int lineEnd = lineStart + SendMessage(hEdit, EM_LINELENGTH, lineStart, 0);

        CHARRANGE LineSelection;
        LineSelection.cpMin = lineStart;
        LineSelection.cpMax = lineEnd;

        //Copy current line into text buffer
        char* line = (char*)NULL;
	line = (char*) malloc((((lineEnd - lineStart) + 3)) * sizeof(char));
        //line = new char[(lineEnd - lineStart) + 3];

        *(int *) line = (lineEnd - lineStart) ;
        SendMessage(hEdit, EM_GETLINE, (WPARAM)codeLine, (LPARAM)line);
        line[(lineEnd - lineStart)] = '\0';

        char keyword    [STDFIELD+1];
        keyword[0] = '\0';

        int kwPos = 0;
        bool commentLine  =false;


        for(int i = 0; i < (strlen(line)+1); i++)
	{
                for(int j = 0; j < synInfo_cnt; j++)
		{
                        if(keyword[0] == synInfo[j].ident[0] && (synInfo[j].identClass == CLASS_VARIABLE
                                || synInfo[j].identClass == CLASS_VALUE))
                        {
                                int varOpen = 0;

                                for(;i < (strlen(line)+1);i++)
                                {
                                        if(i == caretPos) formatNormal = false;
                                        if(line[i] == synInfo[j].ident[0] && synInfo[j].identClass == CLASS_VARIABLE) varOpen++;
                                        if(line[i] == synInfo[j].ident[1])
                                        {
                                                if(varOpen > 0)
                                                {
                                                        varOpen--;
                                                        continue;
                                                }
                                                i++;
                                                break;
                                        }
                                }


                                if(i >= strlen(line))
                                {
                                        formatNormal = false;
				}

				kwPos = 0;
				break;
                        }
                        if(strcmp(strupr(keyword), synInfo[j].ident) == 0 && (synInfo[j].identClass == CLASS_COMMENT || synInfo[j].identClass == CLASS_LABEL || synInfo[j].identClass == CLASS_SPECIAL_COMMENT))
                        {
                                if(synInfo[j].otherCharMayFollow && ((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z'))) break;

                                kwPos = 0;
                                commentLine = true;
                                formatNormal = false;
                                break;
                        }
                }

                if(commentLine) break;

                //fprintf(debug, "An line[%d]: >%c<\n", i, line[i]);
                if(!((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z')))
                {
                        kwPos = 0;
                        keyword[0] = '\0';
                }

                if(line[i] != ' ' && line[i] != '\t')
                {
                        keyword[kwPos] = line[i];
                        keyword[kwPos+1] = '\0';
                        kwPos++;
		}
	}
        if(formatNormal)
        {
		CHARFORMAT FormatNormal;
                memset(&FormatNormal, 0, sizeof(CHARFORMAT));
                FormatNormal.cbSize = sizeof(CHARFORMAT);
                FormatNormal.dwMask = CFM_COLOR | CFM_BOLD | CFM_FACE | CFM_SIZE;
                FormatNormal.crTextColor = base_color;
                FormatNormal.dwEffects = 0;

                FormatNormal.yHeight = 200;
                FormatNormal.bCharSet = 0xEE;
                strcpy(FormatNormal.szFaceName, fontFace);

                SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&FormatNormal);
        }

        free(line);

}

HWND CreateToolbar (HWND hWindow)
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	 *	STD_COPY  Specifies the copy image.              *
	 *	STD_CUT  Specifies the cut image.                *
	 *	STD_DELETE  Specifies the delete image.          *
	 *	STD_FILENEW  Specifies the new file image.       *
	 *	STD_FILEOPEN  Specifies the open file image.     *
	 *	STD_FILESAVE  Specifies the save file image.     *
	 *	STD_FIND  Specifies the find image.              *
	 *	STD_HELP  Specifies the help image.              *
	 *	STD_PASTE  Specifies the paste image.            *
	 *	STD_PRINT  Specifies the print image.            *
	 *	STD_PRINTPRE  Specifies the print preview image. *
	 *	STD_PROPERTIES  Specifies the properties image.  *
	 *	STD_REDOW  Specifies the redo image.             *
	 *	STD_REPLACE  Specifies the replace image.        *
	 *	STD_UNDO  Specifies the undo image.              *
	 * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	TBBUTTON mypictures [] =
	{
		 {STD_FILENEW ,IDM_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
                 {STD_FILEOPEN, IDM_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
                 {STD_FILESAVE, IDM_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},

                 {0, 0, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_SEP, 0, 0},

		 {STD_UNDO, IDM_EDIT_UNDO, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
		 {STD_REDOW, IDM_EDIT_REDO, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},

		 {STD_COPY ,IDM_EDIT_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
                 {STD_CUT, IDM_EDIT_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
                 {STD_PASTE, IDM_EDIT_PASTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
		 {STD_DELETE, IDM_EDIT_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
                 {STD_FIND, IDM_EDIT_FIND, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},

                 {0, 0, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_SEP, 0, 0},

                 {0, IDM_SCRIPT_RUN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},

		 {0, 0, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_SEP, 0, 0},

		 {0, IDM_HELP_REFERENCE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
	};


	/* Toolbaar erstellen mit tooltips */
	HWND hTree = CreateWindowEx (NULL,
				   TOOLBARCLASSNAME,
				   "",
				   WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
				   0,0,0,0,
				   hWindow,
				   (HMENU)1,
				   gInstance,
				   NULL);

	SendMessage(hTree, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof (TBBUTTON), 0);

	TBADDBITMAP bit;
	memset (&bit, 0, sizeof (TBADDBITMAP));
	bit.hInst = HINST_COMMCTRL;
	bit.nID = IDB_STD_SMALL_COLOR;
	SendMessage(hTree,TB_ADDBITMAP,0,(LPARAM)&bit);

        //Own icons
        bit.hInst = NULL;
        bit.nID = (unsigned int)LoadImage(gInstance,
                                        MAKEINTRESOURCE(IDI_TB_BITMAPS),
                                        IMAGE_BITMAP,
                                        2 * GetSystemMetrics(SM_CXSMICON),
					GetSystemMetrics(SM_CYSMICON),
                                        LR_DEFAULTCOLOR | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);

	mypictures[12].iBitmap = SendMessage(hTree,TB_ADDBITMAP,1,(LPARAM)&bit);
        mypictures[14].iBitmap = mypictures[12].iBitmap + 1;

        //Add all pictures
        SendMessage (hTree, TB_ADDBUTTONS, (WPARAM)(sizeof(mypictures)/sizeof(mypictures[0])), (LPARAM)(TBBUTTON*)&mypictures);

        //Set tooltips
        //SendMessage (hTree, TB_ADDSTRING, (WPARAM)NULL, (LPARAM)"Hello World\0\0");

	SendMessage (hTree, TB_AUTOSIZE, 0, 0);
	ShowWindow (hTree, SW_SHOW);
	UpdateWindow (hTree);

	return hTree;
}

HMENU CreateContextMenu(void)
{
	HMENU men;

	men = CreatePopupMenu ();

	InsertMenu (men, 0, MF_BYPOSITION, LNG_CONTEXTMENU_COPY, getLngText(LNG_CONTEXTMENU_COPY) );
	InsertMenu (men, 1, MF_BYPOSITION, LNG_CONTEXTMENU_CUT, getLngText(LNG_CONTEXTMENU_CUT));
	InsertMenu (men, 2, MF_BYPOSITION, LNG_CONTEXTMENU_PASTE, getLngText(LNG_CONTEXTMENU_PASTE));
	InsertMenu (men, 3, MF_SEPARATOR, 0, NULL); // Separator
	InsertMenu (men, 4, MF_BYPOSITION, LNG_CONTEXTMENU_DELETE, getLngText(LNG_CONTEXTMENU_DELETE));
	InsertMenu (men, 5, MF_BYPOSITION, LNG_CONTEXTMENU_SELECT_ALL, getLngText(LNG_CONTEXTMENU_SELECT_ALL));

	return men;
}


bool loadFile(char* file, bool showdialog)
{
        char cwd[STDLINE];
        char fileName[STDLINE];

        if(showdialog)
        {
                (void)strcpy(fileName, "");

                OPENFILENAME FileDialog; //Structure for Open/Save-Dialogs
                ZeroMemory(&FileDialog, sizeof(FileDialog));

                FileDialog.lStructSize          = sizeof(FileDialog);

		FileDialog.lpstrFile            = fileName;
                FileDialog.nMaxFile             = STDLINE;
                FileDialog.nFilterIndex         = 0;
		FileDialog.lpstrTitle           = getLngText(LNG_DLG_FILEOPEN);
                FileDialog.nMaxFileTitle        = STDLINE * sizeof(char);
                getcwd(cwd, STDLINE);
                FileDialog.lpstrInitialDir      = cwd;
                FileDialog.hwndOwner            = hMain;
                FileDialog.Flags                = OFN_EXPLORER | OFN_HIDEREADONLY;
                FileDialog.lpstrFilter          = "RapidBATCH Scripts\0*.rb\0All files (*.*)\0*.*\0\0";

                //GetOpenFileName(&FileDialog) == false)
		if(!GetOpenFileName(&FileDialog))
                {
                        /*
			switch(CommDlgExtendedINFO_ERROR())
                        {
                                case CDERR_DIALOGFAILURE: INFO_MSG("CDERR_DIALOGFAILURE"); break;
                                case CDERR_FINDRESFAILURE: INFO_MSG("CDERR_FINDRESFAILURE"); break;
                                case CDERR_NOHINSTANCE: INFO_MSG("CDERR_NOHINSTANCE"); break;
                                case CDERR_INITIALIZATION: INFO_MSG("CDERR_INITIALIZATION"); break;
                                case CDERR_NOHOOK: INFO_MSG("CDERR_NOHOOK"); break;
                                case CDERR_LOCKRESFAILURE: INFO_MSG("CDERR_LOCKRESFAILURE"); break;
                                case CDERR_NOTEMPLATE: INFO_MSG("CDERR_NOTEMPLATE"); break;
                                case CDERR_LOADRESFAILURE: INFO_MSG("CDERR_LOADRESFAILURE"); break;
                                case CDERR_STRUCTSIZE: INFO_MSG("CDERR_STRUCTSIZE"); break;
                                case CDERR_LOADSTRFAILURE: INFO_MSG("CDERR_LOADSTRFAILURE"); break;
                                case FNERR_BUFFERTOOSMALL: INFO_MSG("FNERR_BUFFERTOOSMALL"); break;
				case CDERR_MEMALLOCFAILURE: INFO_MSG("CDERR_MEMALLOCFAILURE"); break;
                                case FNERR_INVALIDFILENAME: INFO_MSG("FNERR_INVALIDFILENAME"); break;
                                case CDERR_MEMLOCKFAILURE: INFO_MSG("CDERR_MEMLOCKFAILURE"); break;
				case FNERR_SUBCLASSFAILURE: INFO_MSG("FNERR_SUBCLASSFAILURE"); break;
                        }
                        */
                        return false;
                }
        }
        else
        {
		(void)strcpy(fileName, file);
	}

	if(strcmp(fileName, getLngText(LNG_DEFAULTFILE)) != 0) update_history_menu(fileName);

        FILE* src_file = (FILE*)NULL;

        src_file = fopen(fileName, "rb");
        if(src_file == (FILE*)NULL) return false;

        fseek(src_file, 0L, SEEK_END);
        char* file_content = (char*) malloc((ftell(src_file)+1) * sizeof(char));
        if(file_content == (char*)NULL)
        {
                INFO_ERROR("Your computer rans out of memory.");
                return false;
        }

        (void)strcpy(file_content, "");

	fseek(src_file, 0L, SEEK_SET);

	unsigned long int pos = 0;
        while(!feof(src_file))
        {
		char ch = fgetc(src_file);
                if(!feof(src_file)) file_content[pos]= ch;
                pos++;
        }
        file_content[pos-1] = '\0';
        fclose(src_file);

        LockWindowUpdate(hEdit);
        multilineHighlight = true;
        SetFocus(hTmpEdit);

        SendMessage(hEdit, WM_SETTEXT, NULL, (LPARAM)file_content);
        free(file_content);

        for(int i = 0; i < SendMessage(hEdit, EM_GETLINECOUNT, NULL, NULL); i++)
        {
                highlightCode(i);
        }
        LockWindowUpdate(NULL);
        multilineHighlight = false;

        SetFocus(hEdit);
        (void)strcpy(file, fileName);
        return true;
}

bool saveFile(char* file, bool showdialog)
{
        char cwd[STDLINE];
        char fileName[STDLINE];

	if(strcmp(file, "") == 0 || showdialog)
        {
                (void)strcpy(fileName, "");

                OPENFILENAME FileDialog; //Structure for Open/Save-Dialogs
                ZeroMemory(&FileDialog, sizeof(FileDialog));

                FileDialog.lStructSize          = sizeof(FileDialog);

                FileDialog.lpstrFile            = fileName;
                FileDialog.nMaxFile             = STDLINE;
                FileDialog.nFilterIndex         = 0;
		FileDialog.lpstrTitle           = getLngText(LNG_DLG_FILESAVE);
                FileDialog.nMaxFileTitle        = STDLINE * sizeof(char);

                getcwd(cwd, STDLINE);
                FileDialog.lpstrInitialDir      = cwd;
                FileDialog.hwndOwner            = hMain;
                FileDialog.Flags                = OFN_EXPLORER | OFN_HIDEREADONLY;
		FileDialog.lpstrFilter          = "RapidBATCH Scripts\0*.rb\0All files (*.*)\0*.*\0\0";
                FileDialog.nFileExtension       = 0;
                FileDialog.lpstrDefExt          = "rb";

                //GetOpenFileName(&FileDialog) == false)
                if(!GetSaveFileName(&FileDialog))
                {
			return false;
                }

                FILE* test_file = (FILE*)NULL;
                test_file = fopen(fileName, "rb");
		if(test_file != (FILE*)NULL)
                {
                        //INFO_MSG("File exists...");
                        fclose(test_file);
                        switch(MessageBox(hMain, getLngText(LNG_MSG_FILEEXISTS), "RapidBATCH Builder", 32 + 4))
                        {
                                case 7:
					return false;
                        }
                }
        }
        else
        {
                (void)strcpy(fileName, file);
        }

        if(strcmp(fileName, getLngText(LNG_DEFAULTFILE)) != 0) update_history_menu(fileName);

        FILE* dest_file = (FILE*)NULL;

        dest_file = fopen(fileName, "wb");
        if(dest_file == (FILE*)NULL) return false;

        int len = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);

	char* file_content = (char*) malloc((len+1) * sizeof(char));
        if(file_content == (char*)NULL)
	{
                INFO_ERROR("Your computer rans out of memory.");
                return false;
	}
        (void)strcpy(file_content, "");

	SendMessage(hEdit, WM_GETTEXT, (WPARAM) len+1, (LPARAM) file_content);
        //file_content[len] = '\0';

        fprintf(dest_file, "%s", file_content);
        fclose(dest_file);

        free(file_content);

        (void)strcpy(file, fileName);

        return true;
}

bool run_script(char* scriptfile, char* parms, bool wait)
{
	char buffer[STDLINE+1];
	char envstr[STDLINE+1];

	if(rb_debug_session)
	{
		switch(debug_mode)
		{
			case DM_STEP:
				(void)strcpy(buffer, "STEP");
				break;
			case DM_TRACE:
				(void)strcpy(buffer, "TRACE");
				break;
		}
		(void)sprintf(envstr, "RB_DEBUGMODE=%s", buffer);
		putenv(envstr);

		switch(var_mode)
		{
			case VM_NONE:
				(void)strcpy(buffer, "NONE");
				break;
			case VM_AUTO:
				(void)strcpy(buffer, "AUTO");
				break;
			case VM_MANUAL:
				(void)strcpy(buffer, debug_vars);
				break;

		}
		sprintf(envstr, "RB_DEBUGVARS=%s", buffer);
		putenv(envstr);
	}
	else
	{
		(void)sprintf(envstr, "RB_DEBUGMODE=");
		putenv(envstr);

		(void)sprintf(envstr, "RB_DEBUGVARS=");
		putenv(envstr);
	}


	if(parms == (char*)NULL)
	{
		(void)sprintf(buffer, "%s\\%s \"%s\"", rb_path, "RBI32.EXE", scriptfile);
	}
	else
	{
                (void)sprintf(buffer, "%s\\%s \"%s\" %s", rb_path, "RBI32.EXE", scriptfile, parms);
        }

	exec_program(buffer, wait);

        return true;
}

void update_history_menu(char* filename)
{
	char	value_name	[STDLINE+1];
	char	history_tmp	[STDLINE+1];

	if(hist_menu != (HMENU)NULL)
	{
		ModifyMenu(GetMenu(hMain), (int)hist_menu, MF_BYCOMMAND | MF_STRING, IDM_FILE_HISTORY, getLngText(LNG_DYNAMICMENU_HISTORY));
		DestroyMenu(hist_menu);
	}

	hist_menu = CreateMenu();

	//Update history in registry
	if(filename != (char*) NULL)
	{
		for(int i=MAX_HISTORY-1;i>=0;i--)
		{
			sprintf(value_name, "History%d", i);

			if(getRegEntry(HKEY_CURRENT_USER, RBATCH_REGISTRY_PATH, value_name, history_tmp, STDLINE))
			{
				if(strcmp(history_tmp, "") == 0)
					continue;
				if(strcmp(history_tmp, filename) == 0)
					break;

				if(i < MAX_HISTORY-1)
				{
					sprintf(value_name, "History%d", i+1);
					if(!setRegEntry(HKEY_CURRENT_USER, RBATCH_REGISTRY_PATH, value_name, history_tmp))
					{
						INFO_ERROR(history_tmp);
					}
				}
			}
                }

		if(strcmp(history_tmp, filename) != 0 &&
			!setRegEntry(HKEY_CURRENT_USER, RBATCH_REGISTRY_PATH, "History0", filename))
		{
                        INFO_ERROR(filename);
		}
	}	

	//Reload history menu
	if(history_cnt > 0)
	{
		for(int i=0; i < history_cnt; i++)
		{
			free(history[i]);
		}
		free(history);
		history_cnt = 0;
	}

	for(int i=0;i<MAX_HISTORY;i++)
	{
		sprintf(value_name, "History%d", i);

		if(getRegEntry(HKEY_CURRENT_USER, RBATCH_REGISTRY_PATH, value_name, history_tmp, STDLINE)
			&& strcmp(history_tmp, "") != 0)
		{
			bool already_exists = false;
			for(int j = 0; j < history_cnt; j++)
			{

				if(strcmp(history_tmp, history[j]) == 0)
				{
					already_exists = true;
					break;
				}
			}

			if(already_exists)
				continue;

			if(history_cnt == 0)
			{
				history = (char**)malloc(sizeof(char*));
			}
			else
			{
				history = (char**)realloc((char**)history, (history_cnt+1) * sizeof(char*));
			}

			history[history_cnt] = strdup(history_tmp);
			history_cnt++;

			sprintf(history_tmp, "&%d: %s", history_cnt, history[history_cnt-1]);

			AppendMenu(hist_menu, MF_STRING, IDM_FILE_HISTORY + history_cnt, history_tmp);
		}
		else
		{
			break;
		}
	}



	if(history_cnt > 0)
	{
		ModifyMenu(GetMenu(hMain), IDM_FILE_HISTORY, MF_BYCOMMAND | MF_POPUP | MF_STRING, (int)hist_menu, getLngText(LNG_DYNAMICMENU_HISTORY));
	}
	else
	{
		ModifyMenu(GetMenu(hMain), IDM_FILE_HISTORY, MF_BYCOMMAND | MF_STRING | MF_GRAYED, IDM_FILE_HISTORY, getLngText(LNG_DYNAMICMENU_HISTORY));
	}


/*
	char hist_file[STDLINE+1];
	char tmp[STDLINE+1];
	int flags = 0;

        for(int i=0; i < MAX_HISTORY; i++)
        {
                (void)sprintf(tmp, "History%d", i);
		if(getRegEntry(HKEY_CURRENT_USER, RBATCH_REGISTRY_PATH, tmp, hist_file, STDLINE)
                        && strcmp(hist_file, "") != 0)
                {
                        (void)sprintf(tmp, "&%d: %s", i+1, hist_file);
                        flags = MF_ENABLED;
                }
                else
                {
                        (void)sprintf(tmp, "&%d: ", i+1);
                        flags = MF_GRAYED | MF_DISABLED;
		}
                if(ModifyMenu(GetMenu(hMain), IDM_FILE_HISTORY_START + i, MF_BYCOMMAND | MF_STRING | flags, IDM_FILE_HISTORY_START + i, tmp) == 0)
                {
                        INFO_ERROR(tmp);
		}
	}
	*/
        DrawMenuBar(hMain);
}


void loadToolsMenu(void)
{
	char value_name		[STDLINE+1];
	char tool_info		[STDLINE+1];
	char* label		= (char*)NULL;
	char tool_delimiter	= '|';
	int sep_line		= 0;

	static HMENU tools;

	tools = GetSubMenu(GetMenu(hMain), 3);
	if(tools != (HMENU)NULL)
	{
		DestroyMenu(tools);
	}

	tools = CreateMenu();

	//AppendMenu(tools, MF_STRING, IDM_TOOLS_MENU + 1, "Hello World");

	if(tools_exec_cnt > 0)
	{
		for(int i=0; i < tools_exec_cnt; i++)
		{
			free(tools_exec[i]);
		}
		free(tools_exec);
		tools_exec_cnt = 0;
	}

	for(int i=0;;i++)
	{
		sprintf(value_name, "Tool%d", i);

		if(getRegEntry(HKEY_CURRENT_USER, RBATCH_REGISTRY_PATH, value_name, tool_info, STDLINE)
			&& strcmp(tool_info, "") != 0)
		{
			if(strcmp(tool_info, "-") != 0)
			{
				label = strchr(tool_info, tool_delimiter);

				if(label == (char*)NULL)
				{
					label = tool_info - 1;
				}
				else
				{
					*label = '\0';
				}

				if(tools_exec_cnt == 0)
				{
					tools_exec = (char**)malloc(sizeof(char*));
				}
				else
				{
					tools_exec = (char**)realloc((char**)tools_exec, (tools_exec_cnt+1) * sizeof(char*));
				}

				tools_exec[tools_exec_cnt] = strdup(tool_info);
				tools_exec_cnt++;

				/*
				char xxx[STDLINE];
				sprintf(xxx, "%d %s", IDM_TOOLS_MENU + (i+1), label);
				INFO_MSG(xxx);
				*/

				AppendMenu(tools, MF_STRING, (IDM_TOOLS_MENU + (i+1)) - sep_line, label+1);
			}
			else
			{
				sep_line++;
				AppendMenu(tools, MF_SEPARATOR, NULL, NULL);
			}
		}
		else
		{
			if(tools_exec_cnt > 0)
				AppendMenu(tools, MF_SEPARATOR, NULL, NULL);

			AppendMenu(tools, MF_STRING, IDM_TOOLS_CONFIGURE, getLngText(LNG_DYNAMICMENU_TOOLS_CONFIGURE));
			break;
		}
	}



	ModifyMenu(GetMenu(hMain), 3, MF_BYPOSITION | MF_POPUP | MF_STRING, (int)tools, getLngText(LNG_DYNAMICMENU_TOOLS));

	/*
	if(tools_exec_cnt > 0)
	{
		ModifyMenu(GetMenu(hMain), IDM_TOOLS_MENU, MF_BYCOMMAND | MF_POPUP | MF_STRING, (int)tools, getLngText(LNG_DYNAMICMENU_TOOLS));
	}
	else
	{
		ModifyMenu(GetMenu(hMain), IDM_TOOLS_MENU, MF_BYCOMMAND | MF_STRING | MF_GRAYED, NULL, getLngText(LNG_DYNAMICMENU_TOOLS));
	}
	*/
	DrawMenuBar(hMain);
}


void set_caption(char* filename)
{
        char title[STDLINE+1];
        (void)sprintf(title, "RapidBATCH 5 - %s", filename);
        SendMessage(hMain, WM_SETTEXT, 0, (WPARAM)title);

        int selStart, selEnd;
        SendMessage(hEdit, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);
}

void set_lineinfo()
{
        char text[STDLINE+1];
        int line, col;
	SendMessage(hEdit, EM_GETSEL, (WPARAM)&col, (LPARAM)NULL);

        line = (int)SendMessage(hEdit, EM_LINEFROMCHAR, col, 0);
	col = col - SendMessage(hEdit, EM_LINEINDEX, line, 0);

        (void)sprintf(text, "%05d:%03d", line+1, col+1);
	SendMessage(hStatus, SB_SETTEXT, (WPARAM)0, (LPARAM)text);

	behind_dialog = false;
}


bool getRegEntry(HKEY basekey, char* subkey, char* valname, char* result, long strsize)
{
        HKEY regkey;
        if(strcmp(subkey, "") != 0)
	{
                if(RegOpenKey(basekey, subkey, &regkey) != ERROR_SUCCESS)
                {
                        return false;
                }
        }
        else
        {
                regkey = basekey;
        }

        DWORD maxsize = (DWORD)strsize;
        DWORD type = NULL;

        if(RegQueryValueEx( regkey, valname, NULL, &type, (LPBYTE)result, &maxsize) != ERROR_SUCCESS)
	{
                return false;
        }

        RegCloseKey(regkey);
        return true;
}

bool setRegEntry(HKEY basekey, char* subkey, char* valname, char* value)
{
        HKEY regkey;

	if(strcmp(subkey, "") != 0)
        {
                if(RegOpenKey(basekey, subkey, &regkey) != ERROR_SUCCESS)
                {
                        return false;
                }
        }
        else
        {
                regkey = basekey;
        }


	if(RegSetValueEx(regkey, valname, 0, REG_SZ, value, strlen(value)+1) != ERROR_SUCCESS)
        {
                return false;
	}
	RegCloseKey(regkey);
        return true;
}


bool getRGB(char* value, int* r, int* g, int* b)
{
        int pos = 0;
        int current_value, res;

	while(pos < strlen(value))
        {
                for(int i=0; i < 2; i++)
		{
                        current_value = 0;

                        switch(value[pos+i])
                        {
                                case '9':
                                        current_value++;
                                case '8':
                                        current_value++;
                                case '7':
                                        current_value++;
                                case '6':
                                        current_value++;
                                case '5':
                                        current_value++;
				case '4':
                                        current_value++;
                                case '3':
                                        current_value++;
                                case '2':
                                        current_value++;
                                case '1':
                                        current_value++;
                                        break;
                                case 'A':
                                case 'a':
                                        current_value = 10;
                                        break;
                                case 'B':
				case 'b':
                                        current_value = 11;
					break;
                                case 'C':
				case 'c':
                                        current_value = 12;
                                        break;
                                case 'D':
                                case 'd':
                                        current_value = 13;
                                        break;
                                case 'E':
                                case 'e':
                                        current_value = 14;
					break;
                                case 'F':
                                case 'f':
                                        current_value = 15;
                                        break;
                        }
                        if(i==0)
                                res = current_value * 16;
                        else
                                res = res + current_value;
                }

                switch(pos)
                {
                        case 0:
                                *r = res;
                                break;
                        case 2:
                                *g = res;
				break;
                        case 4:
				*b = res;
                                break;
                        default:
                                return false;
                }
                pos += 2;
        }
	return true;
}

DWORD getClassColor(char* class_name)
{
        char rgb_value[STDLINE];
	if(!getRegEntry(HKEY_CURRENT_USER, RBATCH_REGISTRY_PATH, class_name, rgb_value, STDLINE))
        {
		char error_msg[STDLINE];
		sprintf(error_msg, "WARNING: Unable to load color for syntax class \"%s\"", class_name);
		INFO_ERROR(error_msg);
	}

	rgb_value[RGB_LEN] = '\0';
        int R,G,B;
        getRGB(rgb_value, &R, &G, &B);

        return RGB(R,G,B);
}

/*
bool getClassBoldState(char* class_name)
{
        char bold_state[STDLINE];

        if(!getRegEntry(HKEY_CURRENT_USER, RBATCH_REGISTRY_PATH, class_name, bold_state, STDLINE))
        {
                INFO_ERROR("error");
	}

        if(bold_state[RGB_LEN+1] == '1') return true;
        return false;
}

bool getClassItalicState(char* class_name)
{
        char italic_state[STDLINE];

        if(!getRegEntry(HKEY_CURRENT_USER, RBATCH_REGISTRY_PATH, class_name, italic_state, STDLINE))
        {
                INFO_ERROR("error");
        }

        if(italic_state[RGB_LEN+3] == '1') return true;
	return false;
}
*/


BOOL CALLBACK GotoLine_DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
        static HWND hInput = (HWND)NULL;

	switch (message)
        {
		case WM_INITDIALOG:
		{
                        //Read labels from language catalog
			SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_GOTOLINE_TITLE));
			SendMessage(GetDlgItem(hDlg, IDOK), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_GOTOLINE_OK));
                        SendMessage(GetDlgItem(hDlg, IDCANCEL), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_GOTOLINE_CANCEL));
                        SendMessage(GetDlgItem(hDlg, IDC_LABEL), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_GOTOLINE_PROMPT));

                        char line_str[STDLINE+1];
                        int sel;
                        hInput = GetDlgItem (hDlg, IDC_LINENUMBER);
                        SendMessage(hEdit, EM_GETSEL, (WPARAM)&sel, NULL);
                        sel = (int)SendMessage(hEdit, EM_LINEFROMCHAR, sel, 0);
                        sprintf(line_str, "%d", sel+1);
                        SendMessage(hInput, WM_SETTEXT, 100, (LPARAM) line_str);
                        return true;
                }
                case WM_PAINT:
                        break;
                case WM_COMMAND:
                        switch (wParam)
                        {
                                case IDOK:
                                {
                                        char* tmp_value = (char*)NULL;

                                        hInput = GetDlgItem (hDlg, IDC_LINENUMBER);
                                        tmp_value = (char*)malloc(((int)SendMessage(hInput, WM_GETTEXTLENGTH,0,0)*sizeof(char))+1);
                                        SendMessage(hInput, WM_GETTEXT, (int)SendMessage(hInput, WM_GETTEXTLENGTH,0,0)+1, (LPARAM)tmp_value);

                                        long int goto_line = atoi(tmp_value) - 1;


                                        SendMessage(hEdit, EM_SETSEL, SendMessage(hEdit, EM_LINEINDEX, goto_line, 0), SendMessage(hEdit, EM_LINEINDEX, goto_line, 0));

                                        free(tmp_value);
                                        EndDialog(hDlg, 0);
                                        break;
                                }
                                case IDCANCEL:
                                        EndDialog(hDlg, -1);
                                        break;
                        }
                        break;
                case WM_QUIT:
                        EndDialog(hDlg, -1);
                        break;
                case WM_DESTROY:
                        EndDialog(hDlg, -1);
                        break;
                case WM_CLOSE:
                        EndDialog(hDlg, -1);
                        break;
                default:
                        break;
        }
        return false;
}

BOOL CALLBACK SetCommand_DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
        static HWND hInput = (HWND)NULL;

        switch (message)
        {
                case WM_INITDIALOG:
                        //Read labels from language catalog
			SendMessage(hDlg, WM_SETTEXT, 0, (WPARAM)getLngText(LNG_DLG_SETCOMMAND_TITLE));
                        SendMessage(GetDlgItem(hDlg, IDOK), WM_SETTEXT, 0, (WPARAM)getLngText(LNG_DLG_SETCOMMAND_OK));
                        SendMessage(GetDlgItem(hDlg, IDCANCEL), WM_SETTEXT, 0, (WPARAM)getLngText(LNG_DLG_SETCOMMAND_CANCEL));

                        if(cmdline_parms != (char*)NULL) SendMessage(GetDlgItem(hDlg, IDC_COMMANDLINE), WM_SETTEXT, 0, (WPARAM)cmdline_parms);
                        return true;
                case WM_PAINT:
                        break;
		case WM_COMMAND:
                        switch (wParam)
                        {
                                case IDOK:
                                        if(cmdline_parms != (char*)NULL) free(cmdline_parms);
                                        hInput = GetDlgItem (hDlg, IDC_COMMANDLINE);

                                        cmdline_parms = (char*)malloc(((int)SendMessage(hInput, WM_GETTEXTLENGTH,0,0)*sizeof(char))+1);
                                        SendMessage(hInput, WM_GETTEXT, (int)SendMessage(hInput, WM_GETTEXTLENGTH,0,0)+1, (LPARAM) cmdline_parms);

                                        EndDialog(hDlg, 0);
                                        break;
				case IDCANCEL:
                                        EndDialog(hDlg, -1);
                                        break;
                        }
			break;
                case WM_QUIT:
                        EndDialog(hDlg, -1);
                        break;
		case WM_DESTROY:
                        EndDialog(hDlg, -1);
                        break;
                case WM_CLOSE:
                        EndDialog(hDlg, -1);
                        break;
                default:
                        break;
        }
        return false;
}

BOOL CALLBACK Find_DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
        static HWND hInput = (HWND)NULL;

        switch (message)
        {
                case WM_INITDIALOG:
                        //Read labels from language catalog
			SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_FIND_TITLE));
			SendMessage(GetDlgItem(hDlg, IDOK), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_FIND_OK));
                        SendMessage(GetDlgItem(hDlg, IDCANCEL), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_FIND_CANCEL));
			return true;
                case WM_PAINT:
			break;
		case WM_KEYUP:
			INFO_MSG("hmm...");
			break;
                case WM_COMMAND:
                        switch (wParam)
                        {
                                case IDOK:
                                        if(findstring != (char*)NULL) free(findstring);
					hInput = GetDlgItem (hDlg, IDC_FINDTEXT);

                                        findstring = (char*)malloc(((int)SendMessage(hInput, WM_GETTEXTLENGTH,0,0)*sizeof(char))+1);
					SendMessage(hInput, WM_GETTEXT, (int)SendMessage(hInput, WM_GETTEXTLENGTH,0,0)+1, (LPARAM) findstring);

                                        EndDialog(hDlg, 0);
					break;
					
                                case IDCANCEL:
                                        EndDialog(hDlg, -1);
                                        break;
                        }
                        return true;
                case WM_QUIT:
                        EndDialog(hDlg, -1);
                        break;
                case WM_DESTROY:
                        EndDialog(hDlg, -1);
			break;
                case WM_CLOSE:
                        EndDialog(hDlg, -1);
                        break;
                default:
                        break;
	}
        return false;
}

BOOL CALLBACK Compile_DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
        char command_line[STDLINE+1];

	switch (message)
        {
                case WM_INITDIALOG:
                        //Read labels from language catalog
			SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_COMPILE_TITLE));
                        SendMessage(GetDlgItem(hDlg, IDC_COMPILER_OK), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_COMPILE_COMPILEBUTTON));
                        SendMessage(GetDlgItem(hDlg, IDCANCEL), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_COMPILE_CANCELBUTTON));

                        SendMessage(GetDlgItem(hDlg, IDC_COMPILER_AUTOLINK), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_COMPILE_AUTOLINK));
                        SendMessage(GetDlgItem(hDlg, IDC_COMPILER_LINKICON), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_COMPILE_LINKICON));
                        SendMessage(GetDlgItem(hDlg, IDC_COMPILER_RUNAFTERHALT), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_COMPILE_RUNAFTERHALT));

                        //Set default states
                        SendMessage(GetDlgItem(hDlg, IDC_COMPILER_AUTOLINK), BM_SETCHECK, BST_CHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_COMPILER_CMDLINE), WM_SETTEXT, (WPARAM)2+1, (LPARAM)"/L");
                        compiler_outputs = GetDlgItem(hDlg, IDC_COMPILER_OUTPUT);
                        return true;
                case WM_PAINT:

                        break;
                case WM_COMMAND:
                        switch (wParam)
                        {
				case IDC_COMPILER_OK:
                                {
                                        char buffer[STDLINE+1];
                                        char exe_file[STDLINE+1];
                                        bool compile_succeeded = false;

                                        (void)strcpy(exe_file, current_file);
                                        for(int i = strlen(exe_file); i > 0 && exe_file[i] != '.'; i--)
						exe_file[i] = '\0';
                                        (void)strcat(exe_file, "exe");


                                        //RUN COMPILER -------------------------

                                        //With or without icon?
                                        if(SendMessage(GetDlgItem(hDlg, IDC_COMPILER_LINKICON), BM_GETCHECK, 0, 0) == BST_CHECKED)
                                        {
						(void)sprintf(buffer, "%s\\RBC32.EXE \"%s\" /I:\"%s\" /E", rb_path, current_file, icon_file);
                                        }
					else
					{
                                                (void)sprintf(buffer, "%s\\RBC32.EXE \"%s\" /E", rb_path, current_file);
                                        }

                                        if(ReadConsoleApplication(buffer, ReadConsoleApplicationHandler) == false)
                                        {
                                                INFO_ERROR("RBC32.EXE not found.\nProgram cannot be compiled.");
                                                return false;
                                        }

                                        //RUN LINKER ---------------------------
					if(SendMessage(GetDlgItem(hDlg, IDC_COMPILER_AUTOLINK), BM_GETCHECK, 0, 0) == BST_CHECKED)
					{
						remove( exe_file );

                                                char coc_file[STDLINE+1];
                                                (void)strcpy(coc_file, current_file);
                                                for(int i = strlen(coc_file); i > 0 && coc_file[i] != '.'; i--)
                                                        coc_file[i] = '\0';

                                                (void)strcat(coc_file, "coc");

                                                (void)sprintf(buffer, "%s\\PLINK.EXE \"%s\"", rb_path, coc_file);

                                                if(ReadConsoleApplication(buffer, ReadConsoleApplicationHandler) == false)
                                                {
                                                        INFO_ERROR("PLINK.EXE not found.\nProgram cannot be linked.");
                                                        return false;
                                                }


                                                FILE* tmpfile = fopen(exe_file, "rb");
                                                if(tmpfile == (FILE*)NULL)
                                                {
							INFO_ERROR(getLngText(LNG_MSG_COMPILEERROR));
                                                        compile_succeeded = false;
                                                }
                                                else
                                                {
                                                        fclose(tmpfile);
                                                        if(SendMessage(GetDlgItem(hDlg, IDC_COMPILER_RUNAFTERHALT), BM_GETCHECK, 0, 0) == BST_UNCHECKED)
                                                        {
                                                                INFO_MSG(getLngText(LNG_MSG_COMPILESUCCEED));
                                                        }
							compile_succeeded = true;
						}
                                        }

                                        if(SendMessage(GetDlgItem(hDlg, IDC_COMPILER_RUNAFTERHALT), BM_GETCHECK, 0, 0) == BST_CHECKED)
                                        {
                                                if(cmdline_parms == (char*)NULL)
                                                {
							(void)sprintf(buffer, "%s", exe_file);
                                                }
                                                else
                                                {
                                                        (void)sprintf(buffer, "%s %s", exe_file, cmdline_parms);
						}

                                                WinExec(buffer, SW_SHOW);
                                        }

                                        if(compile_succeeded)
                                                EndDialog(hDlg, 0);

					break;
				}
				case IDCANCEL:
					EndDialog(hDlg, -1);
					break;
				case IDC_COMPILER_AUTOLINK:
				case IDC_COMPILER_RUNAFTERHALT:
				case IDC_COMPILER_LINKICON:
					(void)strcpy(command_line, "");
					if(SendMessage(GetDlgItem(hDlg, IDC_COMPILER_AUTOLINK), BM_GETCHECK, 0, 0) == BST_CHECKED) (void)strcat(command_line, "/L");
					if(SendMessage(GetDlgItem(hDlg, IDC_COMPILER_RUNAFTERHALT), BM_GETCHECK, 0, 0) == BST_CHECKED) (void)strcat(command_line, " /R");

					if(wParam == IDC_COMPILER_LINKICON &&
					 	SendMessage(GetDlgItem(hDlg, IDC_COMPILER_LINKICON), BM_GETCHECK, 0, 0) == BST_CHECKED)
					{
						char cwd[STDLINE+1];
						(void)strcpy(icon_file, "");

						OPENFILENAME FileDialog; //Structure for Open/Save-Dialogs
						ZeroMemory(&FileDialog, sizeof(FileDialog));

						FileDialog.lStructSize          = sizeof(FileDialog);

						FileDialog.lpstrFile            = icon_file;
						FileDialog.nMaxFile             = STDLINE;
						FileDialog.nFilterIndex         = 0;
						FileDialog.lpstrTitle           = getLngText(LNG_DLG_COMPILE_OPENICON);
						FileDialog.nMaxFileTitle        = STDLINE * sizeof(char);
						getcwd(cwd, STDLINE);
						FileDialog.lpstrInitialDir      = cwd;
						FileDialog.hwndOwner            = hMain;
						FileDialog.Flags                = OFN_EXPLORER | OFN_HIDEREADONLY;
						FileDialog.lpstrFilter          = "Icon-Files (*.ico)\0*.ico\0\0";

						//GetOpenFileName(&FileDialog) == false)
						if(!GetOpenFileName(&FileDialog))
						{
							SendMessage(GetDlgItem(hDlg, IDC_COMPILER_LINKICON), BM_SETCHECK, BST_UNCHECKED, 0);
						}
					}

					if(SendMessage(GetDlgItem(hDlg, IDC_COMPILER_LINKICON), BM_GETCHECK, 0, 0) == BST_CHECKED)
					{
						(void)strcat(command_line, " /I:\"");
						(void)strcat(command_line, icon_file);
						(void)strcat(command_line, "\"");
					}

                                        SendMessage(GetDlgItem(hDlg, IDC_COMPILER_CMDLINE), WM_SETTEXT, (WPARAM)strlen(command_line)+1, (LPARAM)command_line);
                                        break;
                        }
                        break;
                case WM_QUIT:
                        EndDialog(hDlg, -1);
                        break;
		case WM_DESTROY:
                        EndDialog(hDlg, -1);
                        break;
                case WM_CLOSE:
                        EndDialog(hDlg, -1);
                        break;
                default:
                        break;
        }
	return false;
}

BOOL CALLBACK Debug_DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			//Read labels from language catalog
			SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_DEBUG_TITLE));
			SendMessage(GetDlgItem(hDlg, IDOK), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_DEBUG_OKBUTTON));
			SendMessage(GetDlgItem(hDlg, IDCANCEL), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_DEBUG_CANCELBUTTON));
                        SendMessage(GetDlgItem(hDlg, IDC_DEBUG_DEBUGSCRIPT), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_DEBUG_DEBUGSCRIPT));

			SendMessage(GetDlgItem(hDlg, IDC_DEBUG_GROUPDEBUGMODE), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_DEBUG_DEBUGMODE));
			SendMessage(GetDlgItem(hDlg, IDC_DEBUG_STEP), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_DEBUG_STEP));
			SendMessage(GetDlgItem(hDlg, IDC_DEBUG_TRACE), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_DEBUG_TRACE));
			SendMessage(GetDlgItem(hDlg, IDC_DEBUG_GROUPVARMON), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_DEBUG_VARMON));
			SendMessage(GetDlgItem(hDlg, IDC_VARMON_NONE), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_DEBUG_NONE));
			SendMessage(GetDlgItem(hDlg, IDC_VARMON_AUTO), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_DEBUG_AUTO));
			SendMessage(GetDlgItem(hDlg, IDC_VARMON_MANUAL), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_DEBUG_MANUAL));

			if(!rb_debug_session)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_DEBUG_GROUPDEBUGMODE), false);
				EnableWindow(GetDlgItem(hDlg, IDC_DEBUG_STEP), false);
				EnableWindow(GetDlgItem(hDlg, IDC_DEBUG_TRACE), false);
				EnableWindow(GetDlgItem(hDlg, IDC_DEBUG_GROUPVARMON), false);
				EnableWindow(GetDlgItem(hDlg, IDC_VARMON_NONE), false);
				EnableWindow(GetDlgItem(hDlg, IDC_VARMON_AUTO), false);
				EnableWindow(GetDlgItem(hDlg, IDC_VARMON_MANUAL), false);
				EnableWindow(GetDlgItem(hDlg, IDC_VARMON_VARS), false);
				SendMessage(GetDlgItem(hDlg, IDC_COMPILER_AUTOLINK), BM_SETCHECK, BST_CHECKED, 0);
			}
			else
			{
				EnableWindow(GetDlgItem(hDlg, IDC_DEBUG_GROUPDEBUGMODE), true);
				EnableWindow(GetDlgItem(hDlg, IDC_DEBUG_STEP), true);
				EnableWindow(GetDlgItem(hDlg, IDC_DEBUG_TRACE), true);
				EnableWindow(GetDlgItem(hDlg, IDC_DEBUG_GROUPVARMON), true);
				EnableWindow(GetDlgItem(hDlg, IDC_VARMON_NONE), true);
				EnableWindow(GetDlgItem(hDlg, IDC_VARMON_AUTO), true);
				EnableWindow(GetDlgItem(hDlg, IDC_VARMON_MANUAL), true);
				EnableWindow(GetDlgItem(hDlg, IDC_VARMON_VARS), true);
				
				SendMessage(GetDlgItem(hDlg, IDC_DEBUG_DEBUGSCRIPT), BM_SETCHECK, BST_CHECKED, 0);
			}


			switch(debug_mode)
			{
				case DM_STEP:
					SendMessage(GetDlgItem(hDlg, IDC_DEBUG_STEP), BM_SETCHECK, BST_CHECKED, 0);
					SendMessage(GetDlgItem(hDlg, IDC_DEBUG_TRACE), BM_SETCHECK, BST_UNCHECKED, 0);
					break;
				case DM_TRACE:
					SendMessage(GetDlgItem(hDlg, IDC_DEBUG_TRACE), BM_SETCHECK, BST_CHECKED, 0);
					SendMessage(GetDlgItem(hDlg, IDC_DEBUG_STEP), BM_SETCHECK, BST_UNCHECKED, 0);
					break;
			}

			switch(var_mode)
			{
				case VM_NONE:
					SendMessage(GetDlgItem(hDlg, IDC_VARMON_NONE), BM_SETCHECK, BST_CHECKED, 0);
					SendMessage(GetDlgItem(hDlg, IDC_VARMON_AUTO), BM_SETCHECK, BST_UNCHECKED, 0);
					SendMessage(GetDlgItem(hDlg, IDC_VARMON_MANUAL), BM_SETCHECK, BST_UNCHECKED, 0);
					break;
				case VM_AUTO:
					SendMessage(GetDlgItem(hDlg, IDC_VARMON_AUTO), BM_SETCHECK, BST_UNCHECKED, 0);
					SendMessage(GetDlgItem(hDlg, IDC_VARMON_AUTO), BM_SETCHECK, BST_CHECKED, 0);
					SendMessage(GetDlgItem(hDlg, IDC_VARMON_MANUAL), BM_SETCHECK, BST_UNCHECKED, 0);
					break;
				case VM_MANUAL:
					SendMessage(GetDlgItem(hDlg, IDC_VARMON_AUTO), BM_SETCHECK, BST_UNCHECKED, 0);
					SendMessage(GetDlgItem(hDlg, IDC_VARMON_AUTO), BM_SETCHECK, BST_UNCHECKED, 0);
					SendMessage(GetDlgItem(hDlg, IDC_VARMON_MANUAL), BM_SETCHECK, BST_CHECKED, 0);
					break;
			}

			return true;
		case WM_PAINT:
			break;
		case WM_COMMAND:
                        switch (wParam)
			{
				case IDOK:
					if(debug_vars != (char*)NULL) free(debug_vars);
					debug_vars = (char*)malloc(((int)SendMessage(GetDlgItem(hDlg, IDC_VARMON_VARS), WM_GETTEXTLENGTH,0,0)*sizeof(char))+1);
					SendMessage(GetDlgItem(hDlg, IDC_VARMON_VARS), WM_GETTEXT, (int)SendMessage(GetDlgItem(hDlg, IDC_VARMON_VARS), WM_GETTEXTLENGTH,0,0)+1, (LPARAM) debug_vars);

					EndDialog(hDlg, 0);
					break;
				case IDCANCEL:
					EndDialog(hDlg, -1);
					break;
				case IDC_DEBUG_DEBUGSCRIPT:
					if(SendMessage(GetDlgItem(hDlg, IDC_DEBUG_DEBUGSCRIPT), BM_GETCHECK, 0, 0) == BST_CHECKED)
					{
						rb_debug_session = true;
					}
					else
					{
						rb_debug_session = false;
					}

					Debug_DlgProc(hDlg, WM_INITDIALOG, 0, 0);
					break;
				case IDC_VARMON_NONE:
					var_mode = VM_NONE;
					break;
				case IDC_VARMON_AUTO:
					var_mode = VM_AUTO;
					break;
				case IDC_VARMON_MANUAL:
					var_mode = VM_MANUAL;
					SetFocus(GetDlgItem(hDlg, IDC_VARMON_VARS));
					break;
				case IDC_DEBUG_STEP:
					debug_mode = DM_STEP;
					break;
				case IDC_DEBUG_TRACE:
					debug_mode = DM_TRACE;
					break;
			}
			break;
		case WM_QUIT:
			EndDialog(hDlg, -1);
			break;
		case WM_DESTROY:
			EndDialog(hDlg, -1);
			break;
		case WM_CLOSE:
			EndDialog(hDlg, -1);
			break;
		default:
			break;
	}
	return false;
}

BOOL CALLBACK About_DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
        switch (message)
        {
		case WM_INITDIALOG:
		{
			HBITMAP bmp = (HBITMAP) LoadImage(gInstance, MAKEINTRESOURCE(IDI_RB5_LOGO),IMAGE_BITMAP, NULL, NULL, LR_DEFAULTCOLOR | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
			SendMessage(GetDlgItem(hDlg, IDC_LOGO), STM_SETIMAGE,IMAGE_BITMAP,(LPARAM) bmp);

                        SendMessage(GetDlgItem(hDlg, IDC_PROGNAME), WM_SETTEXT, 0, (WPARAM)RBB4_PROGRAM_INFO);
			SendMessage(GetDlgItem(hDlg, IDC_COPYRIGHT), WM_SETTEXT, 0, (WPARAM)RBB4_COPYRIGHT_INFO);
			SendMessage(GetDlgItem(hDlg, IDC_REFERLICENSE), WM_SETTEXT, 0, (WPARAM)getLngText(LNG_DLG_ABOUT_REFERLICENSE));

			HFONT p_font = CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FW_DONTCARE | DEFAULT_CHARSET, "MS Shell Dlg");
			SendMessage(GetDlgItem(hDlg, IDC_PROGNAME), WM_SETFONT, (LPARAM)p_font, 0);
			SendMessage(GetDlgItem(hDlg, IDC_COPYRIGHT), WM_SETFONT, (LPARAM)p_font, 0);

                        HFONT w_font = CreateFont(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FW_DONTCARE | DEFAULT_CHARSET, "MS Shell Dlg");
			SendMessage(GetDlgItem(hDlg, IDC_WEBSITE), WM_SETFONT, (LPARAM)w_font, 0);

			//Read labels from language catalog
			/*SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_GOTOLINE_TITLE));
			SendMessage(GetDlgItem(hDlg, IDOK), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_GOTOLINE_OK));
			SendMessage(GetDlgItem(hDlg, IDCANCEL), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_GOTOLINE_CANCEL));
			SendMessage(GetDlgItem(hDlg, IDC_LABEL), WM_SETTEXT, 0, (LPARAM)getLngText(LNG_DLG_GOTOLINE_PROMPT));
			*/
			break;
		}
		case WM_MOUSEMOVE:
		{
                        /*
			RECT wpos, pos;

			GetWindowRect(GetDlgItem(hDlg, IDC_WEBSITE), &pos);
			GetWindowRect(hDlg, &wpos);

			pos.left = pos.left - wpos.left;
			pos.top = pos.top - wpos.top;

			char info[100];
			sprintf(info, "X: %d Y: %d -> l %d t %d r %d b %d", LOWORD(lParam),
				HIWORD(lParam), pos.left, pos.top, pos.right, pos.bottom);
			SendMessage(GetDlgItem(hDlg, IDC_WEBSITE), WM_SETTEXT, 0, (WPARAM)info);

			SendMessage(GetDlgItem(hDlg, IDC_WEBSITE), WM_SETTEXT, 0, (WPARAM)info);


			if(LOWORD(lParam) >= pos.left && LOWORD(lParam) <= pos.right
				&& HIWORD(lParam) >= pos.top && HIWORD(lParam) <= pos.bottom)
			{
				SendMessage(GetDlgItem(hDlg, IDC_WEBSITE), WM_SETTEXT, 0, (WPARAM)"fuck yeah!");
			}
			*/
			break;
		}

                case WM_PAINT:
                        break;
                case WM_COMMAND:
                        switch (wParam)
                        {
                                case IDOK:
                                {
					EndDialog(hDlg, 0);
					break;
				}
                        }
                        break;
                case WM_QUIT:
                        EndDialog(hDlg, -1);
                        break;
                case WM_DESTROY:
                        EndDialog(hDlg, -1);
                        break;
                case WM_CLOSE:
                        EndDialog(hDlg, -1);
			break;
                default:
                        break;
        }
        return false;
}


/*
BOOL CALLBACK PrintDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
     	{
     		case WM_INITDIALOG :
			EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE, MF_GRAYED);
          		return TRUE;

     		case WM_COMMAND :
          		bUserAbort = TRUE;
          		EnableWindow (GetParent (hDlg), TRUE);
          		DestroyWindow (hDlg);
          		hDlgPrint = NULL;
          		return TRUE;
     	}
     	return FALSE;
}

BOOL CALLBACK AbortProc (HDC hPrinterDC, int iCode)
{
     	MSG msg;

     	while (!bUserAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
     	{
          	if (!hDlgPrint || !IsDialogMessage (hDlgPrint, &msg))
          	{
               		TranslateMessage (&msg);
               		DispatchMessage (&msg);
          	}
     	}
	return !bUserAbort;
}
*/

void PrintFile(void)
{
	static DOCINFO  di = { sizeof (DOCINFO) };
	static PRINTDLG pd;
	BOOL            bSuccess;
	int             yChar, iCharsPerLine, iLinesPerPage, iTotalLines,
	            iTotalPages, iPage, iLine, iLineNum;
	PTSTR           pstrBuffer;
	TCHAR           szJobName [64 + MAX_PATH];
	TEXTMETRIC      tm;
	WORD            iColCopy, iNoiColCopy;

	ZeroMemory(&pd, sizeof(PRINTDLG));
	pd.lStructSize = sizeof(PRINTDLG);
	pd.hwndOwner   = hMain;
	pd.hDevMode    = NULL;
	pd.hDevNames   = NULL;
	pd.Flags       = PD_RETURNDC|PD_PRINTSETUP;

	if (!PrintDlg((LPPRINTDLG)&pd)) return;

	// calculate the size of the printer page in device units.  to be
	// complete, you should actually get the printable size of the page
	// and make sure that the selected margins are within the printable
	// area of the page.  we'll skip that complexity here.
	int wPage = GetDeviceCaps(pd.hDC, PHYSICALWIDTH);
	int hPage = GetDeviceCaps(pd.hDC, PHYSICALHEIGHT);

	//int wPage = GetDeviceCaps(pd.hDC, HORZRES);
	//int hPage = GetDeviceCaps(pd.hDC, VERTRES);

	// get the number of printer pixels per inch.
	int xPPI = GetDeviceCaps(pd.hDC, LOGPIXELSX);
	int yPPI = GetDeviceCaps(pd.hDC, LOGPIXELSY);

        int inch = 1440; //1440
	// convert the page size from device units to twips.
	int wTwips = MulDiv(wPage, inch, xPPI);
	int hTwips = MulDiv(hPage, inch, yPPI);

	// put the page size into a RECT.
	RECT pageRect;
	pageRect.left = pageRect.top = 0;
	pageRect.right = wTwips;
	pageRect.bottom = hTwips;

	// now calculate the size of the rendering rectangle (the area of the
	// page into which the output will be word-wrapped) in twips.  for
	// this example, two-inch margins are hard-coded.
	RECT rendRect;
	rendRect.left = rendRect.top = 0;
	rendRect.right = pageRect.right - inch * 4;
	rendRect.bottom = pageRect.bottom - inch * 4;

	// now calculate the page offsets that correspond to each page.
	// you would normally build the table of offsets just before opening
	// a Print Dialog and put the page count on the dialog so that the
	// user could select a range of pages to print.  although we are not
	// adding the complexity of selecting a range of pages to be printed
	// to this program, we'll go ahead and demonstrate building the table.
	std::vector<TPageOffsets> FPageOffsets;

	// set up the starting page.
	TPageOffsets po;
	po.Start = 0;

	// initialize the formatting data.
	FORMATRANGE fr;
	fr.hdc = pd.hDC;
	fr.hdcTarget = pd.hDC;
	fr.chrg.cpMin = po.Start;
	fr.chrg.cpMax = -1;

	// get the size of the text in the control (use EM_GETTEXTLENEX for RE 2.0).
	int lastOffset = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);

	// clear the formatting buffer.
	SendMessage(hEdit, EM_FORMATRANGE, (WPARAM) 0, (LPARAM) 0);

	// the printer probably cannot print to the edges of the page.  Rich Edit
	// moves the render rectangle relative to the top left printable point.
	// we are, presumbably, already rendering within the printable area
	// so move the printer origin up and left by the offset.
	SaveDC(fr.hdc);
	SetMapMode(fr.hdc, MM_TEXT);
	int xOffset = GetDeviceCaps(pd.hDC, PHYSICALOFFSETX);
	int yOffset = GetDeviceCaps(pd.hDC, PHYSICALOFFSETY);
        /*
	xOffset += MulDiv(1440 + 1440, xPPI, 1440);
	yOffset += MulDiv(1440 + 1440, xPPI, 1440);
        */
	xOffset += MulDiv(1440, xPPI, 1440);
	yOffset += MulDiv(1440, xPPI, 1440);
	SetViewportOrgEx(fr.hdc, xOffset, yOffset, NULL);

	// loop through data to format and build page offset table.
	do {
		// set up remaining format data.
		fr.rc = rendRect;			// RE returns something useful here...
		fr.rcPage = pageRect;
		po.Start = fr.chrg.cpMin;

		// format range with render set to false.
		fr.chrg.cpMin = SendMessage(hEdit, EM_FORMATRANGE,
			(WPARAM) 0, (LPARAM) &fr);

		po.End = fr.chrg.cpMin - 1;
		po.rendRect = fr.rc;		// and where is this documented?
		FPageOffsets.push_back(po);
		} while (fr.chrg.cpMin != -1 && fr.chrg.cpMin < lastOffset);

	// at this point, FPageOffsets.size() is the page count which will
	// always be at least one.
	int pageCount = FPageOffsets.size();

	// but for testing, let's only print the first page
	//pageCount = 1;

	// now print the pages.  first, clear the formatting buffer.
	SendMessage(hEdit, EM_FORMATRANGE, (WPARAM) 0, (LPARAM) 0);

	// begin printing.  note that BeginDoc() changes Printer()->Handle from
	// an IC to a DC, Printer()->Handle has changed so we set a lot of stuff
	// again.
	RestoreDC(fr.hdc, -1);

	//Printer()->BeginDoc();
        di.cbSize = sizeof(DOCINFO);
        di.lpszDocName = current_file ;

        StartDoc (pd.hDC, &di);

	fr.hdc = pd.hDC;
	fr.hdcTarget = pd.hDC;
	SaveDC(fr.hdc);
	SetViewportOrgEx(fr.hdc, xOffset, yOffset, NULL);

	// loop through data to print.
	bool firstPage = true;
	int currPage = 0;
	do
        {
                StartPage (pd.hDC);
                
		// advance the page if not first page.
		//if (firstPage) firstPage = false;
		//else
                //Printer()->NewPage();

		// set up remaining format data.
		fr.rc = FPageOffsets[currPage].rendRect;	// documented where?
		fr.rcPage = pageRect;
		fr.chrg.cpMin = FPageOffsets[currPage].Start;
		fr.chrg.cpMax = FPageOffsets[currPage].End;

		// format range with render set to true.
		fr.chrg.cpMin = SendMessage(hEdit, EM_FORMATRANGE,
			(WPARAM) 1, (LPARAM) &fr);


	        EndPage (pd.hDC);

        }
        while (++currPage < pageCount);

	// restore the DC (before EndDoc()!) and end printing.
	RestoreDC(fr.hdc, -1);

	EndDoc(pd.hDC);

	// clean up.
	SendMessage(hEdit, EM_FORMATRANGE, (WPARAM) 0, (LPARAM) 0);
	FPageOffsets.erase(FPageOffsets.begin(), FPageOffsets.end());
}
/*
BOOL PrintFile (HINSTANCE hInst, HWND hwnd, HWND hwndEdit,
		       PTSTR szTitleName)
{
     DOCINFO  di;
     PRINTDLG pd ;
     BOOL bSuccess ;
     int yChar, iCharsPerLine, iLinesPerPage, iTotalLines,
	 iTotalPages, iPage, iLine, iLineNum ;
     PTSTR pstrBuffer ;
     TCHAR szJobName [64 + MAX_PATH] ;
     TEXTMETRIC tm ;
     WORD iColCopy, iNoiColCopy ;

          // Invoke Print common dialog box
     ZeroMemory(&pd, sizeof(pd));
     pd.lStructSize         = sizeof (PRINTDLG) ;
     pd.hwndOwner           = hwnd ;
     pd.hDevMode            = NULL ;
     pd.hDevNames           = NULL ;
     pd.Flags               = PD_ALLPAGES | PD_COLLATE |
                              PD_RETURNDC | PD_NOSELECTION |
                              PD_PRINTSETUP ;
     pd.nCopies             = 1 ;

     PrintDlg (&pd);

     iTotalLines = SendMessage (hwndEdit, EM_GETLINECOUNT, 0, 0);
     if( iTotalLines == 0 )
        return TRUE;

          // Calculate necessary metrics for file

     GetTextMetrics (pd.hDC, &tm) ;
     yChar = tm.tmHeight + tm.tmExternalLeading ;

     iCharsPerLine = GetDeviceCaps (pd.hDC, HORZRES) / tm.tmAveCharWidth ;
     iLinesPerPage = GetDeviceCaps (pd.hDC, VERTRES) / yChar ;
     iTotalPages   = (iTotalLines + iLinesPerPage - 1) / iLinesPerPage ;

          // Allocate a buffer for each line of text

     pstrBuffer =(PTSTR) malloc (sizeof (TCHAR) * (iCharsPerLine + 1)) ;


     bSuccess   = TRUE ;

          // Start the document
     di.cbSize = sizeof(DOCINFO);
     di.lpszDocName = current_file ;

     if (StartDoc (pd.hDC, &di) > 0)
     {
               // Collation requires this loop and iNoiColCopy

       for (iColCopy = 0 ;
          iColCopy<((WORD) pd.Flags & PD_COLLATE ? pd.nCopies:1);
          iColCopy++)
             {
                for (iPage = 0 ; iPage < iTotalPages ; iPage++)
                  {
                     for (iNoiColCopy = 0 ;
                          iNoiColCopy<( pd.Flags&PD_COLLATE?1
                                        :pd.nCopies );
                          iNoiColCopy++)
                        {
                           if (StartPage (pd.hDC) < 0)
                              {
                                 bSuccess = FALSE ;
                                 break ;
			      }
                        // For each page, print the lines
                        for (iLine=0;iLine<iLinesPerPage;iLine++)
                           {
                             iLineNum=iLinesPerPage*iPage+iLine;

                              if (iLineNum > iTotalLines)
                                   break ;

                            *(int *) pstrBuffer = iCharsPerLine ;
                            TextOut (pd.hDC, 0, yChar * iLine,
                                     pstrBuffer,(int) SendMessage
                                     (hwndEdit, EM_GETLINE,
                                     (WPARAM) iLineNum, (LPARAM)
				     pstrBuffer) );
                           }

                         if (EndPage (pd.hDC) < 0)
                         {
                              bSuccess = FALSE ;
                              break ;
                         }

                    }

                    if (!bSuccess)
                         break ;
               }

               if (!bSuccess)
                    break ;
	  }
     }
     else
          bSuccess = FALSE ;

     if (bSuccess)
          EndDoc (pd.hDC) ;

     free (pstrBuffer) ;
     DeleteDC (pd.hDC) ;
     return bSuccess;
}
*/

/*
BOOL PopPrntPrintFile (HINSTANCE hInst, HWND hwnd, HWND hwndEdit,
                       PTSTR szTitleName)
{
	static DOCINFO  di = { sizeof (DOCINFO) };
	static PRINTDLG pd;
	BOOL            bSuccess;
	int             yChar, iCharsPerLine, iLinesPerPage, iTotalLines,
	            iTotalPages, iPage, iLine, iLineNum;
	PTSTR           pstrBuffer;
	TCHAR           szJobName [64 + MAX_PATH];
	TEXTMETRIC      tm;
	WORD            iColCopy, iNoiColCopy;

	// Aufruf des Druck-Dialogs (COMMDLG)
	pd.lStructSize         = sizeof (PRINTDLG);
	pd.hwndOwner           = hwnd;
	pd.hDevMode            = NULL;
	pd.hDevNames           = NULL;
	pd.hDC                 = NULL;
	pd.Flags               = PD_ALLPAGES | PD_COLLATE |
	                     PD_RETURNDC | PD_NOSELECTION;
	pd.nFromPage           = 0;
	pd.nToPage             = 0;
	pd.nMinPage            = 0;
	pd.nMaxPage            = 0;
	pd.nCopies             = 1;
	pd.hInstance           = NULL;
	pd.lCustData           = 0L;
	pd.lpfnPrintHook       = NULL;
	pd.lpfnSetupHook       = NULL;
	pd.lpPrintTemplateName = NULL;
	pd.lpSetupTemplateName = NULL;
	pd.hPrintTemplate      = NULL;
	pd.hSetupTemplate      = NULL;


	ZeroMemory(&pd, sizeof(PRINTDLG));
	pd.lStructSize = sizeof(PRINTDLG);
	pd.hwndOwner   = hwndEdit;
	pd.hDevMode    = NULL;
	pd.hDevNames   = NULL;
	pd.Flags       = PD_RETURNDC|PD_PRINTSETUP;

	if (!PrintDlg((LPPRINTDLG)&pd)) return TRUE;

	if (0 == (iTotalLines = SendMessage (hwndEdit, EM_GETLINECOUNT, 0, 0))) return TRUE;

	// Schriftgröße ermitteln, darüber Zeilenzahl und Seitenzahl berechnen
	GetTextMetrics (pd.hDC, &tm);
	yChar = tm.tmHeight + tm.tmExternalLeading;

	iCharsPerLine = GetDeviceCaps (pd.hDC, HORZRES) / tm.tmAveCharWidth;
	iLinesPerPage = GetDeviceCaps (pd.hDC, VERTRES) / yChar;
	iTotalPages   = (iTotalLines + iLinesPerPage - 1) / iLinesPerPage;

	// Puffer für eine Textzeile
	pstrBuffer = (char*)malloc (sizeof (TCHAR) * (iCharsPerLine + 1));

	bSuccess   = TRUE;


	// Start des Ausdruckens
	GetWindowText (hwnd, szJobName, sizeof (szJobName));
	di.lpszDocName = szJobName;

	if (StartDoc (pd.hDC, &di) > 0)
	{
	 	for (iColCopy = 0; iColCopy < ((WORD) pd.Flags & PD_COLLATE ? pd.nCopies : 1);iColCopy++)
	 	{
	      		for (iPage = 0; iPage < iTotalPages; iPage++)
	      		{
	           		for (iNoiColCopy = 0; iNoiColCopy < (pd.Flags & PD_COLLATE ? 1 : pd.nCopies); iNoiColCopy++)
	           		{
	                		// Start einer Seite
	                		if (StartPage (pd.hDC) < 0)
	                		{
	                     			bSuccess = FALSE;
	                     			break;
					}

	                		// zeilenweise Ausgabe
	                		for (iLine = 0; iLine < iLinesPerPage; iLine++)
                    			{
	                     			iLineNum = iLinesPerPage * iPage + iLine;

	                     			if (iLineNum > iTotalLines) break;

	                     			*(int *) pstrBuffer = iCharsPerLine;

	                     			TextOut (pd.hDC, 0, yChar * iLine, pstrBuffer, (int)SendMessage (hwndEdit, EM_GETLINE, (WPARAM) iLineNum, (LPARAM) pstrBuffer)-2);
	                		}
	                		if (EndPage (pd.hDC) < 0)
	                		{
	                     			bSuccess = FALSE;
	                     			break;
	                		}
	           		}

	           		if (!bSuccess) break;
	      		}

	      		if (!bSuccess) break;
		}
	}
	else
         {
		bSuccess = FALSE;
         }

	if (bSuccess) EndDoc (pd.hDC);

	free (pstrBuffer);
	DeleteDC (pd.hDC);

	return bSuccess;
}
*/


/*
BOOL PrintFile(HWND hwndEdit)
{
	PRINTDLG printer;
	DOCINFO docinfo;
	HDC hDCprinter;
	HANDLE hFile;
	char* file_content = (char*)NULL;
	long fileSize;
	BOOL bSuccess = FALSE;

	PRINTDLG printer;

	ZeroMemory(&printer, sizeof(PRINTDLG));
	printer.lStructSize = sizeof(PRINTDLG);
	printer.hwndOwner   = hwndEdit;
	printer.hDevMode    = NULL;
	printer.hDevNames   = NULL;
	printer.Flags       = PD_RETURNDC|PD_PRINTSETUP;


	PrintDlg((LPPRINTDLG)&printer);

	ZeroMemory(&docinfo, sizeof(DOCINFO));
	docinfo.cbSize = sizeof(DOCINFO);
	docinfo.lpszDocName = current_file;



	hFile = CreateFile(current_file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != 0xFFFFFFFF)
		{
                 	pszFileText = (char*)GlobalAlloc(GPTR, dwFileSize + 1);
	   		if(pszFileText != NULL)
	  	 	{
	      			DWORD dwRead;
	      			if(ReadFile( hFile, pszFileText, dwFileSize, &dwRead, NULL ))
	         		{
	            			pszFileText[dwFileSize] = '\0';
	         		}
	   		}
		}
	}
	else
        {
                return FALSE;
        }
	CloseHandle(hFile);


        FILE* src_file = fopen(current_file, "rb");
        if(src_file == (FILE*)NULL) return false;

        fseek(src_file, 0L, SEEK_END);
        fileSize = ftell(src_file);
        file_content = (char*) malloc((fileSize+1) * sizeof(char));
        if(file_content == (char*)NULL)
        {
                INFO_ERROR("Your computer ran out of memory.");
                return false;
        }

        (void)strcpy(file_content, "");

        fseek(src_file, 0L, SEEK_SET);

        unsigned long int pos = 0;
        while(!feof(src_file))
        {
                char ch = fgetc(src_file);
                if(!feof(src_file)) file_content[pos]= ch;
                pos++;
        }
        file_content[pos-1] = '\0';
        fclose(src_file);


	if(file_content[0] == '\0') return FALSE;

	hDCprinter = CreateCompatibleDC(printer.hDC);

	PrintDlg((LPPRINTDLG)&printer);

	StartDoc(printer.hDC,&docinfo);
	StartPage(printer.hDC);

        TEXTMETRIC tm;
        GetTextMetrics (printer.hDC, &tm);
        yChar = tm.tmHeight + tm.tmExternalLeading;

        char *file_line = (char*)NULL;


	TextOut(printer.hDC,50,50,file_content, fileSize);

	EndPage(printer.hDC);
	EndDoc(printer.hDC);
	DeleteDC(printer.hDC);
	DeleteDC(hDCprinter);

        free(file_content);

	if (printer.hDevMode  != NULL) GlobalFree(printer.hDevMode);
	if (printer.hDevNames != NULL) GlobalFree(printer.hDevNames);
	return bSuccess;
}    */


//------------------------------------------------------------------------------
// readLngCatalog()
//------------------------------------------------------------------------------
void readLngCatalog(void)
{
        char buffer[STDLINE+1];
        char* label = (char*)NULL;
        char* text = (char*)NULL;

        (void)sprintf(buffer, "%s\\etc\\rbatch.lng", rb_path);

        FILE* lngFile = (FILE*) NULL;
        lngFile = fopen(buffer, "rt");
        if(lngFile == (FILE*)NULL)
        {
                INFO_ERROR("File etc\\rbatch.lng not found or corrupted.\nPlease re-install RapidBATCH.");
                return;
        }

        while(!feof(lngFile) && fgets(buffer, STDLINE, lngFile) != (char*) NULL)
        {
                label = (char*)NULL;
                text = (char*)NULL;
                switch(*buffer)
                {
                        case ';':
                        case '\n':
                                continue;

                        default:
                                int len = strlen(buffer);
                                for(int i = 0; i<len;i++)
                                {
                                        if(buffer[i] == '=')
					{

                                                buffer[i] = '\0';

                                                label = (char*)buffer;
                                                text = (char*)(buffer + i + 1);

                                                i = strlen(buffer);
                                                buffer[len-1] = '\0';
                                                /*
                                                char test[100];
                                                sprintf(test, "%d >%s<", i, buffer);
                                                INFO_MSG(test);
                                                */
                                                break;
					}
                                }
                                break;
                }


                for(int i = 0; i < strlen(text); i++)
                {
                        if(text[i] == '~') text[i] = '\n';
                }


                if(label != (char*)NULL && strcmp(label, "") != 0)
		{
                        // MAIN MENÜS ------------------------------------------
                        if(strcmp(label, "Menu_File") == 0)
			{
                                ModifyMenu(GetMenu(hMain), 0, MF_BYPOSITION | MF_STRING, NULL, text);
                        }
                        else if(strcmp(label, "Menu_Edit") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), 1, MF_BYPOSITION | MF_STRING, NULL, text);
                        }
                        else if(strcmp(label, "Menu_Script") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), 2, MF_BYPOSITION | MF_STRING, NULL, text);
                        }
			else if(strcmp(label, "Menu_Tools") == 0)
			{
				addLngText(LNG_DYNAMICMENU_TOOLS, text); //This is a dynamic menu!!!
			}
                        else if(strcmp(label, "Menu_Help") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), 4, MF_BYPOSITION | MF_STRING, NULL, text);
                        }

                        // FILE MENU -------------------------------------------
                        else if(strcmp(label, "Menu_File_New") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), IDM_FILE_NEW, MF_BYCOMMAND | MF_STRING, IDM_FILE_NEW, text);
                        }
                        else if(strcmp(label, "Menu_File_Open") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), IDM_FILE_OPEN, MF_BYCOMMAND | MF_STRING, IDM_FILE_OPEN, text);
                        }
                        else if(strcmp(label, "Menu_File_Recent") == 0)
			{
				addLngText(LNG_DYNAMICMENU_HISTORY, text); //This is a dynamic menu!!!
				//ModifyMenu(GetSubMenu(GetMenu(hMain), 0), 2, MF_BYPOSITION | MF_STRING, NULL, text);
                        }
                        else if(strcmp(label, "Menu_File_Save") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), IDM_FILE_SAVE, MF_BYCOMMAND | MF_STRING, IDM_FILE_SAVE, text);
                        }
                        else if(strcmp(label, "Menu_File_Save_as") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), IDM_FILE_SAVE_AS, MF_BYCOMMAND | MF_STRING, IDM_FILE_SAVE_AS, text);
			}
                        else if(strcmp(label, "Menu_File_Print") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), IDM_FILE_PRINT, MF_BYCOMMAND | MF_STRING, IDM_FILE_PRINT, text);
                        }
                        else if(strcmp(label, "Menu_File_Quit") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), IDM_FILE_QUIT, MF_BYCOMMAND | MF_STRING, IDM_FILE_QUIT, text);
                        }

                        // EDIT MENU -------------------------------------------
                        else if(strcmp(label, "Menu_Edit_Undo") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), IDM_EDIT_UNDO, MF_BYCOMMAND | MF_STRING, IDM_EDIT_UNDO, text);
                        }
			else if(strcmp(label, "Menu_Edit_Redo") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), IDM_EDIT_REDO, MF_BYCOMMAND | MF_STRING, IDM_EDIT_REDO, text);
                        }
                        else if(strcmp(label, "Menu_Edit_Copy") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_EDIT_COPY, MF_BYCOMMAND | MF_STRING, IDM_EDIT_COPY, text);
				addLngText( LNG_CONTEXTMENU_COPY, text );
			}
			else if(strcmp(label, "Menu_Edit_Cut") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_EDIT_CUT, MF_BYCOMMAND | MF_STRING, IDM_EDIT_CUT, text);
				addLngText( LNG_CONTEXTMENU_CUT, text );
			}
                        else if(strcmp(label, "Menu_Edit_Paste") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), IDM_EDIT_PASTE, MF_BYCOMMAND | MF_STRING, IDM_EDIT_PASTE, text);
				addLngText( LNG_CONTEXTMENU_PASTE, text );
			}
			else if(strcmp(label, "Menu_Edit_Delete") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_EDIT_DELETE, MF_BYCOMMAND | MF_STRING, IDM_EDIT_DELETE, text);
				addLngText( LNG_CONTEXTMENU_DELETE, text );
			}
			else if(strcmp(label, "Menu_Edit_Select_all") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_EDIT_SELECT_ALL, MF_BYCOMMAND | MF_STRING, IDM_EDIT_SELECT_ALL, text);
				addLngText( LNG_CONTEXTMENU_SELECT_ALL, text );
			}
			else if(strcmp(label, "Menu_Edit_Find") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_EDIT_FIND, MF_BYCOMMAND | MF_STRING, IDM_EDIT_FIND, text);
			}
			else if(strcmp(label, "Menu_Edit_Find_next") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_EDIT_FIND_NEXT, MF_BYCOMMAND | MF_STRING, IDM_EDIT_FIND_NEXT, text);
			}
			else if(strcmp(label, "Menu_Edit_Comment_Selection") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_EDIT_COMMENT_SEL, MF_BYCOMMAND | MF_STRING, IDM_EDIT_COMMENT_SEL, text);
			}
			else if(strcmp(label, "Menu_Edit_Goto_Line") == 0)
                        {
                                ModifyMenu(GetMenu(hMain), IDM_EDIT_GOTO_LINE, MF_BYCOMMAND | MF_STRING, IDM_EDIT_GOTO_LINE, text);
                        }
                        // SCRIPT MENU -----------------------------------------
			else if(strcmp(label, "Menu_Script_Run") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_SCRIPT_RUN, MF_BYCOMMAND | MF_STRING, IDM_SCRIPT_RUN, text);
			}
			else if(strcmp(label, "Menu_Script_Compile") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_SCRIPT_COMPILE, MF_BYCOMMAND | MF_STRING, IDM_SCRIPT_COMPILE, text);
			}
			else if(strcmp(label, "Menu_Script_DebuggerProperties") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_SCRIPT_DEBUG, MF_BYCOMMAND | MF_STRING, IDM_SCRIPT_DEBUG, text);
			}
			else if(strcmp(label, "Menu_Script_SetCommand") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_SCRIPT_COMMAND, MF_BYCOMMAND | MF_STRING, IDM_SCRIPT_COMMAND, text);
			}
			// TOOLS MENU ------------------------------------------
                        else if(strcmp(label, "Menu_Tools_Configure") == 0)
			{
				addLngText(LNG_DYNAMICMENU_TOOLS_CONFIGURE, text); //This is a dynamic menu!!!
			}
			// HELP MENU -------------------------------------------
			else if(strcmp(label, "Menu_Help_Manual") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_HELP_MANUAL, MF_BYCOMMAND | MF_STRING, IDM_HELP_MANUAL, text);
			}
			else if(strcmp(label, "Menu_Help_Whatsnew") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_HELP_WHATSNEW, MF_BYCOMMAND | MF_STRING, IDM_HELP_WHATSNEW, text);
			}
			else if(strcmp(label, "Menu_Help_Reference") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_HELP_REFERENCE, MF_BYCOMMAND | MF_STRING, IDM_HELP_REFERENCE, text);
			}
			else if(strcmp(label, "Menu_Help_License") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_HELP_LICENSE, MF_BYCOMMAND | MF_STRING, IDM_HELP_LICENSE, text);
			}
			else if(strcmp(label, "Menu_Help_Website") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_HELP_WEBSITE, MF_BYCOMMAND | MF_STRING, IDM_HELP_WEBSITE, text);
			}
			else if(strcmp(label, "Menu_Help_About") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_HELP_ABOUT, MF_BYCOMMAND | MF_STRING, IDM_HELP_ABOUT, text);
			}
                        // TOOLBAR ---------------------------------------------
			else if(strcmp(label, "Tooltip_NewFile") == 0)
			{
                                addLngText(LNG_TOOLTIP_NEWFILE, text);
			}
			else if(strcmp(label, "Tooltip_OpenFile") == 0)
			{
                                addLngText(LNG_TOOLTIP_OPENFILE, text);
			}
			else if(strcmp(label, "Tooltip_SaveFile") == 0)
			{
				addLngText(LNG_TOOLTIP_SAVEFILE, text);
			}
			else if(strcmp(label, "Tooltip_Find") == 0)
			{
                                addLngText(LNG_TOOLTIP_FIND, text);
			}
			else if(strcmp(label, "Tooltip_Copy") == 0)
			{
                                addLngText(LNG_TOOLTIP_COPY, text);
			}
			else if(strcmp(label, "Tooltip_Cut") == 0)
			{
                                addLngText(LNG_TOOLTIP_CUT, text);
			}
			else if(strcmp(label, "Tooltip_Paste") == 0)
			{
                                addLngText(LNG_TOOLTIP_PASTE, text);
			}
			else if(strcmp(label, "Tooltip_Run") == 0)
			{
                                addLngText(LNG_TOOLTIP_RUN, text);
			}
			else if(strcmp(label, "Tooltip_Help") == 0)
			{
                                addLngText(LNG_TOOLTIP_HELP, text);
			}
			else if(strcmp(label, "Tooltip_Delete") == 0)
			{
                                addLngText(LNG_TOOLTIP_DELETE, text);
			}
			else if(strcmp(label, "Tooltip_Undo") == 0)
			{
                                addLngText(LNG_TOOLTIP_UNDO, text);
			}
			else if(strcmp(label, "Tooltip_Redo") == 0)
			{
                                addLngText(LNG_TOOLTIP_REDO, text);
			}

                        // OPEN/SAVE DIALOGS -----------------------------------
			else if(strcmp(label, "Dialog_FileOpen_Title") == 0)
			{
				addLngText(LNG_DLG_FILEOPEN, text);
			}
			else if(strcmp(label, "Dialog_FileSave_Title") == 0)
			{
				addLngText(LNG_DLG_FILESAVE, text);
			}
                        // DEFAULT FILE ----------------------------------------
			else if(strcmp(label, "default_file") == 0)
			{
                                addLngText(LNG_DEFAULTFILE, text);
			}
                        // FIND DIALOG -----------------------------------------
			else if(strcmp(label, "Dialog_Find_Title") == 0)
			{
                                addLngText(LNG_DLG_FIND_TITLE, text);
			}
			else if(strcmp(label, "Dialog_Find_OkButton") == 0)
			{
				addLngText(LNG_DLG_FIND_OK, text);
			}
			else if(strcmp(label, "Dialog_Find_CancelButton") == 0)
			{
                                addLngText(LNG_DLG_FIND_CANCEL, text);
			}
                        // GOTO LINE DIALOG ------------------------------------
			else if(strcmp(label, "Dialog_GotoLine_Title") == 0)
			{
                                addLngText(LNG_DLG_GOTOLINE_TITLE, text);
			}
			else if(strcmp(label, "Dialog_GotoLine_OkButton") == 0)
			{
				addLngText(LNG_DLG_GOTOLINE_OK, text);
			}
			else if(strcmp(label, "Dialog_GotoLine_CancelButton") == 0)
			{
                                addLngText(LNG_DLG_GOTOLINE_CANCEL, text);
			}
			else if(strcmp(label, "Dialog_GotoLine_Prompt") == 0)
			{
                                addLngText(LNG_DLG_GOTOLINE_PROMPT, text);
			}
                        // SETCOMMAND DIALOG -----------------------------------
			else if(strcmp(label, "Dialog_SetCommand_Title") == 0)
			{
                                addLngText(LNG_DLG_SETCOMMAND_TITLE, text);
			}
			else if(strcmp(label, "Dialog_SetCommand_OkButton") == 0)
			{
                                addLngText(LNG_DLG_SETCOMMAND_OK, text);
			}
			else if(strcmp(label, "Dialog_SetCommand_CloseButton") == 0)
			{
                                addLngText(LNG_DLG_SETCOMMAND_CANCEL, text);
			}
                        // MESSAGEBOXES ----------------------------------------
			else if(strcmp(label, "MessageBox_SaveAnyChanges") == 0)
			{
                                addLngText(LNG_MSG_SAVEANYCHANGES, text);
			}
			else if(strcmp(label, "MessageBox_NoResultsFound") == 0)
			{
                                addLngText(LNG_MSG_NORESULTSFOUND, text);
			}
			else if(strcmp(label, "MessageBox_FileExits") == 0)
			{
                                addLngText(LNG_MSG_FILEEXISTS, text);
			}
			else if(strcmp(label, "MessageBox_CompileError") == 0)
			{
				addLngText(LNG_MSG_COMPILEERROR, text);
			}
                        else if(strcmp(label, "MessageBox_CompileSucceeded") == 0)
			{
                                addLngText(LNG_MSG_COMPILESUCCEED, text);
			}

                        // COMPILE DIALOG --------------------------------------
			else if(strcmp(label, "Dialog_Compile_OpenIcon") == 0)
			{
                                addLngText(LNG_DLG_COMPILE_OPENICON, text);
			}
			else if(strcmp(label, "Dialog_Compile_Title") == 0)
			{
                                addLngText(LNG_DLG_COMPILE_TITLE, text);
			}
			else if(strcmp(label, "Dialog_Compile_CompileButton") == 0)
			{
                                addLngText(LNG_DLG_COMPILE_COMPILEBUTTON, text);
			}
			else if(strcmp(label, "Dialog_Compile_CancelButton") == 0)
			{
                                addLngText(LNG_DLG_COMPILE_CANCELBUTTON, text);
			}
			else if(strcmp(label, "Dialog_Compile_AutoLink") == 0)
			{
                                addLngText(LNG_DLG_COMPILE_AUTOLINK, text);
			}
			else if(strcmp(label, "Dialog_Compile_LinkIcon") == 0)
			{
                                addLngText(LNG_DLG_COMPILE_LINKICON, text);
			}
			else if(strcmp(label, "Dialog_Compile_RunAfterHalt") == 0)
			{
                                addLngText(LNG_DLG_COMPILE_RUNAFTERHALT, text);
			}
                        // DEBUG DIALOG ----------------------------------------
                        else if(strcmp(label, "Dialog_Debug_Title") == 0)
			{
                                addLngText(LNG_DLG_DEBUG_TITLE, text);
			}
			else if(strcmp(label, "Dialog_Debug_OkButton") == 0)
			{
				addLngText(LNG_DLG_DEBUG_OKBUTTON, text);
			}
                        else if(strcmp(label, "Dialog_Debug_CancelButton") == 0)
			{
                                addLngText(LNG_DLG_DEBUG_CANCELBUTTON, text);
			}
                        else if(strcmp(label, "Dialog_Debug_DebugScript") == 0)
			{
				addLngText(LNG_DLG_DEBUG_DEBUGSCRIPT, text);
			}
                        else if(strcmp(label, "Dialog_Debug_StepMode") == 0)
			{
				addLngText(LNG_DLG_DEBUG_STEP, text);
			}
                        else if(strcmp(label, "Dialog_Debug_TraceMode") == 0)
			{
                                addLngText(LNG_DLG_DEBUG_TRACE, text);
			}
                        else if(strcmp(label, "Dialog_Debug_None") == 0)
			{
                                addLngText(LNG_DLG_DEBUG_NONE, text);
			}
                        else if(strcmp(label, "Dialog_Debug_Automatic") == 0)
			{
                                addLngText(LNG_DLG_DEBUG_AUTO, text);
			}
                        else if(strcmp(label, "Dialog_Debug_Manual") == 0)
			{
                                addLngText(LNG_DLG_DEBUG_MANUAL, text);
			}
                        else if(strcmp(label, "Dialog_Debug_DebugMode") == 0)
			{
                                addLngText(LNG_DLG_DEBUG_DEBUGMODE, text);
			}
                        else if(strcmp(label, "Dialog_Debug_VariableMonitoring") == 0)
			{
                                addLngText(LNG_DLG_DEBUG_VARMON, text);
			}
			// ABOUT DIALOG ----------------------------------------
			else if(strcmp(label, "Dialog_About_ReferLicense") == 0)
			{
				addLngText(LNG_DLG_ABOUT_REFERLICENSE, text);
			}


                        /*
			else if(strcmp(label, "Dialog_Find_Title") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_FIND_TITLE | MF_STRING, IDM_DIALOG_FIND_TITLE, text);
			}
			else if(strcmp(label, "Dialog_Find_FindButton") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_FIND_FINDBUTTON | MF_STRING, IDM_DIALOG_FIND_FINDBUTTON, text);
			}
			else if(strcmp(label, "Dialog_Find_CancelButton") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_FIND_CANCELBUTTON | MF_STRING, IDM_DIALOG_FIND_CANCELBUTTON, text);
			}
			else if(strcmp(label, "Dialog_SetCommand_Title") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_SETCOMMAND_TITLE | MF_STRING, IDM_DIALOG_SETCOMMAND_TITLE, text);
			}
			else if(strcmp(label, "Dialog_SetCommand_CloseButton") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_SETCOMMAND_CLOSEBUTTON | MF_STRING, IDM_DIALOG_SETCOMMAND_CLOSEBUTTON, text);
			}
			else if(strcmp(label, "Dialog_Compile_Title") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_COMPILE_TITLE | MF_STRING, IDM_DIALOG_COMPILE_TITLE, text);
			}
			else if(strcmp(label, "Dialog_Compile_CompileButton") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_COMPILE_COMPILEBUTTON | MF_STRING, IDM_DIALOG_COMPILE_COMPILEBUTTON, text);
			}
			else if(strcmp(label, "Dialog_Compile_AutoLink") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_COMPILE_AUTOLINK | MF_STRING, IDM_DIALOG_COMPILE_AUTOLINK, text);
			}
			else if(strcmp(label, "Dialog_Compile_LinkIcon") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_COMPILE_LINKICON | MF_STRING, IDM_DIALOG_COMPILE_LINKICON, text);
			}
			else if(strcmp(label, "Dialog_Compile_RunAfterHalt") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_COMPILE_RUNAFTERHALT | MF_STRING, IDM_DIALOG_COMPILE_RUNAFTERHALT, text);
			}
			else if(strcmp(label, "Dialog_Debug_Title") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_DEBUG_TITLE | MF_STRING, IDM_DIALOG_DEBUG_TITLE, text);
			}
			else if(strcmp(label, "Dialog_Debug_OkButton") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_DEBUG_OKBUTTON | MF_STRING, IDM_DIALOG_DEBUG_OKBUTTON, text);
			}
			else if(strcmp(label, "Dialog_Debug_CancelButton") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_DEBUG_CANCELBUTTON | MF_STRING, IDM_DIALOG_DEBUG_CANCELBUTTON, text);
			}
			else if(strcmp(label, "Dialog_Debug_DebugScript") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_DEBUG_DEBUGSCRIPT | MF_STRING, IDM_DIALOG_DEBUG_DEBUGSCRIPT, text);
			}
			else if(strcmp(label, "Dialog_Debug_StepMode") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_DEBUG_STEPMODE | MF_STRING, IDM_DIALOG_DEBUG_STEPMODE, text);
			}
			else if(strcmp(label, "Dialog_Debug_TraceMode") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_DEBUG_TRACEMODE | MF_STRING, IDM_DIALOG_DEBUG_TRACEMODE, text);
			}
			else if(strcmp(label, "Dialog_Debug_None") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_DEBUG_NONE | MF_STRING, IDM_DIALOG_DEBUG_NONE, text);
			}
			else if(strcmp(label, "Dialog_Debug_Automatic") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_DEBUG_AUTOMATIC | MF_STRING, IDM_DIALOG_DEBUG_AUTOMATIC, text);
			}
			else if(strcmp(label, "Dialog_Debug_Manual") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_DEBUG_MANUAL | MF_STRING, IDM_DIALOG_DEBUG_MANUAL, text);
			}
			else if(strcmp(label, "Dialog_Debug_DebugMode") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_DEBUG_DEBUGMODE | MF_STRING, IDM_DIALOG_DEBUG_DEBUGMODE, text);
			}
			else if(strcmp(label, "Dialog_Debug_VariableMonitoring") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_DIALOG_DEBUG_VARIABLEMONITORING | MF_STRING, IDM_DIALOG_DEBUG_VARIABLEMONITORING, text);
			}
			else if(strcmp(label, "MessageBox_CreateNewFile") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_MESSAGEBOX_CREATENEWFILE | MF_STRING, IDM_MESSAGEBOX_CREATENEWFILE, text);
			}
			else if(strcmp(label, "MessageBox_OpenOtherFile") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_MESSAGEBOX_OPENOTHERFILE | MF_STRING, IDM_MESSAGEBOX_OPENOTHERFILE, text);
			}
			else if(strcmp(label, "MessageBox_QuitWithoutSaving") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_MESSAGEBOX_QUITWITHOUTSAVING | MF_STRING, IDM_MESSAGEBOX_QUITWITHOUTSAVING, text);
			}
			else if(strcmp(label, "MessageBox_StringNotFound") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_MESSAGEBOX_STRINGNOTFOUND | MF_STRING, IDM_MESSAGEBOX_STRINGNOTFOUND, text);
			}
			else if(strcmp(label, "MessageBox_NoMoreResults") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_MESSAGEBOX_NOMORERESULTS | MF_STRING, IDM_MESSAGEBOX_NOMORERESULTS, text);
			}
			else if(strcmp(label, "MessageBox_Overwrite") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_MESSAGEBOX_OVERWRITE | MF_STRING, IDM_MESSAGEBOX_OVERWRITE, text);
			}
			else if(strcmp(label, "Tooltip_NewFile") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_TOOLTIP_NEWFILE | MF_STRING, IDM_TOOLTIP_NEWFILE, text);
			}
			else if(strcmp(label, "Tooltip_OpenFile") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_TOOLTIP_OPENFILE | MF_STRING, IDM_TOOLTIP_OPENFILE, text);
			}
			else if(strcmp(label, "Tooltip_SaveFile") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_TOOLTIP_SAVEFILE | MF_STRING, IDM_TOOLTIP_SAVEFILE, text);
			}
			else if(strcmp(label, "Tooltip_Find") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_TOOLTIP_FIND | MF_STRING, IDM_TOOLTIP_FIND, text);
			}
			else if(strcmp(label, "Tooltip_Copy") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_TOOLTIP_COPY | MF_STRING, IDM_TOOLTIP_COPY, text);
			}
			else if(strcmp(label, "Tooltip_Cut") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_TOOLTIP_CUT | MF_STRING, IDM_TOOLTIP_CUT, text);
			}
			else if(strcmp(label, "Tooltip_Paste") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_TOOLTIP_PASTE | MF_STRING, IDM_TOOLTIP_PASTE, text);
			}
			else if(strcmp(label, "Tooltip_Run") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_TOOLTIP_RUN | MF_STRING, IDM_TOOLTIP_RUN, text);
			}
			else if(strcmp(label, "Tooltip_Help") == 0)
			{
				ModifyMenu(GetMenu(hMain), IDM_TOOLTIP_HELP | MF_STRING, IDM_TOOLTIP_HELP, text);
			}
                        */


                }
        }
        fclose(lngFile);
}


bool addLngText(int id, char* data)
{
        if(lng_elem == 0)
        {
                language_info = (LNG_ELEM*)malloc(sizeof(LNG_ELEM));
        }
        else
        {
                language_info = (LNG_ELEM*)realloc((LNG_ELEM*)language_info, (lng_elem+1) * sizeof(LNG_ELEM));
        }

        language_info[lng_elem].id = id;
        language_info[lng_elem].data = (char*)malloc((strlen(data)+1) * sizeof(char));
        (void)strcpy(language_info[lng_elem].data, data);

        lng_elem++;
        return true;
}

static char* getLngText(int id)
{
	if(lng_elem == 0) return (char*)NULL;

	for(int i = 0;i < lng_elem; i++)
	{
		/*
		fprintf(debug, "%d %d %d %s\n", i, id, language_info[i].id, language_info[i].data);
		fflush(debug);
		*/
		if(language_info[i].id == id)
                {
			return language_info[i].data;
                }
        }
        return (char*)NULL;
}

void cleanLngCatalog(void)
{
        if(lng_elem == 0) return;

        for(int i = 0;i < lng_elem; i++)
        {
                if(language_info[i].data != (char*)NULL) free(language_info[i].data);
	}
        free(language_info);
}

void ReadConsoleApplicationHandler(LPSTR lpszBuffer)
{
        char* current_content;
        current_content = (char*)malloc(((int)SendMessage(compiler_outputs, WM_GETTEXTLENGTH,0,0) + strlen(lpszBuffer) + 1 ) * sizeof(char));
        SendMessage(compiler_outputs, WM_GETTEXT, (int)SendMessage(compiler_outputs, WM_GETTEXTLENGTH,0,0)+1, (LPARAM) current_content);
        strcat(current_content, lpszBuffer);
        SendMessage(compiler_outputs, WM_SETTEXT,(WPARAM)strlen(current_content)+1, (LPARAM)current_content);
        SendMessage(compiler_outputs, EM_SETSEL,strlen(current_content)- 3,0);
        free(current_content);

        //int len = (int)SendMessage(compiler_outputs, WM_GETTEXTLENGTH,0,0);
        /*
        SendMessage(compiler_outputs, EM_SETSEL,0,0);
        SendMessage(compiler_outputs, EM_REPLACESEL, true,(LPARAM)lpszBuffer);
	*/
        UpdateWindow(compiler_outputs);
}

bool ReadConsoleApplication(LPTSTR lpszApplication, VOID (*lpfnHandler)(LPSTR lpszBuffer))
{
	HANDLE hOutputReadTemporary = NULL;
        HANDLE hOutputRead = NULL;
        HANDLE hOutputWrite = NULL;
        HANDLE hInputWriteTemporary = NULL;
        HANDLE hInputRead = NULL;
        HANDLE hInputWrite = NULL;
        HANDLE hErrorWrite = NULL;
        SECURITY_ATTRIBUTES sa = {0};
        PROCESS_INFORMATION pi = {0};
        STARTUPINFO si = {0};
        CHAR lpBuffer[STDLINE+1] = {0};
        DWORD nBytesRead = 0;

        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = true;

	if(CreatePipe(&hOutputReadTemporary, &hOutputWrite, &sa, 0) == false)
		return false;

	if(DuplicateHandle(GetCurrentProcess(), hOutputWrite, GetCurrentProcess(),
		&hErrorWrite, 0, true, DUPLICATE_SAME_ACCESS) == false)
		return false;

	if(CreatePipe(&hInputRead, &hInputWriteTemporary, &sa, 0) == false)
		return false;

	if(DuplicateHandle(GetCurrentProcess(), hOutputReadTemporary, GetCurrentProcess(),
		&hOutputRead, 0, false, DUPLICATE_SAME_ACCESS) == false)
		return false;

	if(DuplicateHandle(GetCurrentProcess(), hInputWriteTemporary, GetCurrentProcess(),
		&hInputWrite, 0, false, DUPLICATE_SAME_ACCESS) == false)
		return false;

	if(CloseHandle(hOutputReadTemporary) == false)
		return false;

	if(CloseHandle(hInputWriteTemporary) == false)
		return false;

	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdOutput = hOutputWrite;
	si.hStdInput = hInputRead;
	si.hStdError = hErrorWrite;
	si.wShowWindow = SW_HIDE;

	if(CreateProcess(NULL, lpszApplication, NULL, NULL, true,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi) == false)
		return false;

	if(CloseHandle(pi.hThread) == false)
		return false;

	if(CloseHandle(hOutputWrite) == false)
		return false;

	if(CloseHandle(hInputRead) == false)
		return false;

	if(CloseHandle(hErrorWrite) == false)
		return false;

	while(true)
	{
		if(ReadFile(hOutputRead, lpBuffer, STDLINE, &nBytesRead, NULL) == false)
                {
			if(GetLastError() == ERROR_BROKEN_PIPE)
                        {
			        break;
                        }
                        else
                        {
				return false;
			}
                }

		lpBuffer[nBytesRead] = '\0';
		lpfnHandler(lpBuffer);
	}

	if(CloseHandle(hOutputRead) == false) return false;
	if(CloseHandle(hInputWrite) == false) return false;

	return true;
}

bool exec_program(char* cmdline, bool wait)
{
	bool ret = true;
        //INFO_MSG(cmdline);
	if(!wait)
	{
		if(WinExec(cmdline, SW_SHOW) < 32)
		{
			ret = false;
		}
	}
	else
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		SECURITY_ATTRIBUTES saProcess;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );
		si.dwFlags = STARTF_USESHOWWINDOW;

		saProcess.nLength = sizeof(saProcess);
		saProcess.lpSecurityDescriptor = NULL;
		saProcess.bInheritHandle = FALSE;

		si.wShowWindow = SW_SHOW;

		if( !CreateProcess( NULL, cmdline, &saProcess, NULL, FALSE, 0, NULL, NULL, &si, &pi) )
		{
			ret = false;
		}
		else
                {
			WaitForSingleObject(pi.hProcess,INFINITE);
                        //GetExitCodeProcess(pi.hProcess,&dwCode);

			/*MSG message;
                        while(WaitForSingleObject(pi.hProcess, 100) == WAIT_TIMEOUT)
                        {
                                if(PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
                                {
                                        TranslateMessage(&message);
                                        DispatchMessage(&message);
                                }
			}
			
                        if(WaitForSingleObject( pi.hProcess, 0) == WAIT_OBJECT_0)
                        {
                                DWORD dwCode;
                                GetExitCodeProcess(pi.hProcess,&dwCode);
			}
			else
			{
				ret = false;
			}
			*/

                }
                CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}


	if(!ret)
	{
		char error_msg[STDLINE+1];
		sprintf(error_msg, "The following command line could not be executed:\n%s", cmdline);
		INFO_ERROR(error_msg);
	}
	
	return ret;
}

