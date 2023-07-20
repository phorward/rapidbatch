/* -HEADER----------------------------------------------------------------------
RapidBATCH 5
Copyright (C) 2005-2007 by J.M.K S.F. Software Technologies, Jan Max Meyer
http://www.jmksf.com ++ mail@jmksf.com

File:	rb5_tokens.h (created on 11.05.2007)
Author:	Jan Max Meyer
Usage:	Token definitions
----------------------------------------------------------------------------- */

#ifndef RB5_TOKENS_H
#define RB5_TOKENS_H

enum token_id
{
	STM_USER_DEFINED			= 0,
	
	/* Keywords/Statements */
	STM_DEC,
	STM_RESET,
	STM_LET,
	STM_IF,
	STM_ELSEIF,
	STM_ELSE,
	STM_ENDIF,
	STM_END,
	STM_HALT,
	STM_REPEAT,
	STM_UNTIL,
	STM_CONT,
	STM_BREAK,
	STM_GOTO,
	STM_GOSUB,
	STM_RESUME,
	STM_EXT,
	STM_PROC,
	STM_ENDPROC,
	STM_FUNC,
	STM_ENDFUNC,
	STM_RET,
	
	/* Build-in value operation functions */
	FCT_LINKVAR,
	FCT_CALCVAR,
	FCT_UPVAR,
	FCT_LOWVAR,
	FCT_TRIMVAR,
	FCT_GETCHAR,
	FCT_GETASC,
	FCT_GETLEN,
	FCT_COPYVAR,
	FCT_GETPOS,
	FCT_GETTOK,
	FCT_REPLACEVAR,
	FCT_CNTVAR,	
	FCT_GETCHARAT,
	FCT_RANDVAR,
	
	/* Build-in system-related functions */
	FCT_SHELL,
	FCT_SYSTEM,
	FCT_CALL,
	FCT_CHDIR,
	FCT_MKDIR,
	FCT_RMDIR,
	FCT_SHORTCUT,
	FCT_RENAMEFILE,
	FCT_DELFILE,
	FCT_COPYFILE,
	FCT_OPEN,
	FCT_NEWFILE,
	FCT_GETENV,
	FCT_LETENV,
	FCT_FILEEXISTS,
	FCT_FILESIZE,
	FCT_READFILE,
	FCT_WRITEFILE,
	FCT_PUTFILE,
	FCT_GETFILE,
	FCT_GETFILEDATE,
	FCT_LETFILEDATE,
	FCT_GETFILETIME,
	FCT_LETFILETIME,
	FCT_STDOUT,
	FCT_STDIN,
	FCT_STDERR,
	FCT_MCI,
	FCT_WAIT,
	FCT_FINDFILES,
	FCT_DISKFREE,
	FCT_DISKSIZE,
	FCT_DISKTTYPE,
	FCT_SHUTDOWN,
	FCT_LOCKWORKSTATION,
	FCT_LETREGVAL,
	FCT_GETREGVAL,
	FCT_DELREGVAL,
	FCT_NEWREGKEY,
	FCT_DELREGKEY,
	FCT_SENDKEYS,
	FCT_MOUSEEVENT,
	FCT_GETFILEATT,
	FCT_LETFILEATT,
	
	/* Build-in dialog-related functions */
	FCT_ECHO,
	FCT_ECHOLN,
	FCT_MSGBOX,
	FCT_ECHOBOX,
	FCT_CONFIRM,
	FCT_INPUTBOX,
	FCT_PWDBOX,
	FCT_EDITBOX,
	FCT_LISTBOX,
	FCT_TRAYMENU,
	FCT_OPENBOX,
	FCT_SAVEBOX,
	FCT_FOLDERBOX,
	FCT_INFOBOX,
	
	/* Build-in dialog related functions for individual dialogs */
	FCT_NEWDIALOG,
	FCT_DELDIALOG,
	FCT_LETDIALOG,
	FCT_GETDIALOG,
	FCT_RUNDIALOG
};

#endif