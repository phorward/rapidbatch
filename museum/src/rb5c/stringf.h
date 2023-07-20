/* stringf.h - little string librarie
 * Copyright (c) 2001 Fabian Holzer <fabianholzer@t-online.de>
 */

#include <string>
#include <string.h>

#ifndef STRINGF_H
#define STRINGF_H

string Replace(string, string, string);
string Mid(string, int, int);
string Left(string, int);
string Right(string, int);
string toUpper(string);
string toLower(string);
string LTrim(string);
string RTrim(string);
string Trim(string);

/*****************************************************************************/

string Replace(string sString, string sLookFor, string sReplaceWith) {
 int iIndex;

 iIndex = sString.find(sLookFor);
 while (iIndex != -1) {
  sString.replace(iIndex, sLookFor.size(), sReplaceWith);
  iIndex = sString.find(sLookFor);
 }
 return sString;
}

string Mid(string sString, int iStart, int iLen) {
 string sTemp;
 if (iLen <= sString.size()) {
  for (int i = iStart; i < iLen; i++) {
   sTemp += sString[i];
  }
 }
 return sTemp;
}

string Left(string sString, int iLen) {
 string sTemp;
 if (iLen <= sString.size()) {
  for (int i = 0; i < iLen; i++) {
   sTemp += sString[i];
  }
 }
 return sTemp;
}

string Right(string sString, int iLen) {
 string sTemp;
 if (iLen <= sString.size()) {
  for (int i = iLen; i > 0; i--) {
   sTemp += sString[sString.size() - i];
  }
 }
 return sTemp;
}

string toUpper(string sString) {
 string sTemp;
 char ch;

 for (int i = 0; i < sString.size(); i++) {
  ch = sString[i];
  if (ch >= 97 && ch <= 122)
   ch = ch - 32;
  sTemp += ch;
 }
 return sTemp;
}

string toLower(string sString) {
 string sTemp;
 char ch;

 for (int i = 0; i < sString.size(); i++) {
  ch = sString[i];
  if (ch >= 65 && ch <= 90)
   ch = ch + 32;
  sTemp += ch;
 }
 return sTemp;
}

string LTrim(string sString)
{
 string sTemp;
 char ch;
 bool bRead = false;
 if(sString != "")
 {
   for (int i = 0; i < sString.size(); i++)
   {
     if (i == -1) return "";
     ch = sString[i];
     if (ch != ' ')
      bRead = true;
     if (bRead)
      sTemp += ch;
   }
   return sTemp;
 }
}

string RTrim(string sString)
{
 string sTemp;
 char ch;
 int iEnd, i;
 bool bRead = true;
 
 if(sString != "")
 {
   for (i = sString.size() - 1; i >= 0; i--)
   {
     if (i == -1) return "";
     ch = sString[i];
     if (ch != ' ' && bRead)
     {
       iEnd = i;
       bRead = false;
     }
   }
   for (i = 0; i < iEnd + 1; i++)
   {
     ch = sString[i];
     sTemp += ch;
   }
 }
 return sTemp;
}

string Trim(string sString)
{
	if (sString != "")
	{
		return LTrim(RTrim(sString));
	}
	else
	{
		return "";
	}
}

#endif
