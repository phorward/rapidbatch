;RapidBATCH 5.0 Beta Edition-4
;English

[Setup]
AppName=RapidBATCH 5.0 Professional Edition
AppVerName=RapidBATCH 5.0 Professional
AppPublisher=J.M.K S.F. Software Technologies
AppPublisherURL=http://www.jmksf.com
AppSupportURL=http://www.jmksf.com
AppUpdatesURL=http://www.jmksf.com
DefaultDirName=c:\jmksf\rb5
DefaultGroupName=RapidBATCH 5.0 Professional Edition
DisableProgramGroupPage=yes
sourcedir=F:\release\rb5_pro_rc1
;LicenseFile=setup.txt
;WizardImageFile=d:\dateien\rel\scripts\rb4logo_se.bmp
outputdir=F:\release


[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"; LicenseFile: "setup.txt"
Name: "ger"; MessagesFile: "compiler:Languages\German.isl"; LicenseFile: "setup_ger.txt"

[Run]
Filename: "{app}\rbb4.exe"; Description: "Run RapidBATCH Builder"; Flags: postinstall shellexec skipifsilent; Languages: en
Filename: "{app}\readme.txt"; Description: "View ReadMe-File"; Flags: postinstall shellexec skipifsilent; Languages: en
Filename: "{app}\doc\manual.html"; Description: "User's manual"; Flags: postinstall shellexec skipifsilent unchecked; Languages: en

Filename: "{app}\rbb4.exe"; Description: "Starte RapidBATCH Builder"; Flags: postinstall shellexec skipifsilent; Languages: ger
Filename: "{app}\readme.txt"; Description: "LiesMich-Datei"; Flags: postinstall shellexec skipifsilent; Languages: ger
Filename: "{app}\doc\manual.html"; Description: "Benutzerhandbuch"; Flags: postinstall shellexec skipifsilent unchecked; Languages: ger


[Files]
;Program files
Source: "rbi32.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "rbrun32.rtl"; DestDir: "{app}"; Flags: ignoreversion
Source: "rbc32.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "plink.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "rbb4.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "riched20.dll"; DestDir: "{app}"; Flags: ignoreversion

;License file
Source: "license.txt"; DestDir: "{app}";   Flags: ignoreversion; Languages: en;
Source: "license_ger.txt"; DestDir: "{app}"; DestName: "license.txt"; Flags: ignoreversion; Languages: ger;

;Readme-File
Source: "readme.txt"; DestDir: "{app}";   Flags: ignoreversion; Languages: en;
Source: "readme_ger.txt"; DestDir: "{app}"; DestName: "readme.txt"; Flags: ignoreversion; Languages: ger;

;Config files
Source: "etc\builder.lng"; DestDir: "{app}\etc"; Flags: ignoreversion; Languages: en;
Source: "etc\builder_ger.lng"; DestDir: "{app}\etc"; DestName: "builder.lng"; Flags: ignoreversion; Languages: ger;
Source: "etc\rbatch.syn"; DestDir: "{app}\etc"; Flags: ignoreversion
Source: "etc\rb5.lng"; DestDir: "{app}\etc"; Flags: ignoreversion; Languages: en;
Source: "etc\rb5_ger.lng"; DestDir: "{app}\etc"; DestName: "rb5.lng"; Flags: ignoreversion; Languages: ger;

;RBB installing tool
Source: "etc\rbb_reg.rb"; DestDir: "{app}\etc"; Flags: ignoreversion
Source: "etc\tools.def"; DestDir: "{app}\etc"; Flags: ignoreversion; Languages: en;
Source: "etc\tools_ger.def"; DestDir: "{app}\etc"; DestName: "tools.def"; Flags: ignoreversion; Languages: ger;

;Documentation
Source: "doc\english\*.html"; DestDir: "{app}\doc";  Flags: ignoreversion; languages: en;
Source: "doc\english\*.gif"; DestDir: "{app}\doc";  Flags: ignoreversion; languages: en;
Source: "doc\english\menu.css"; DestDir: "{app}\doc";  Flags: ignoreversion; languages: en;
Source: "doc\english\contents\*.html"; DestDir: "{app}\doc\contents";  Flags: ignoreversion; languages: en;
Source: "doc\english\contents\*.png"; DestDir: "{app}\doc\contents";  Flags: ignoreversion; languages: en;
Source: "doc\english\contents\jmksf.css"; DestDir: "{app}\doc\contents";  Flags: ignoreversion; languages: en;
Source: "doc\english\contents\img\*.gif"; DestDir: "{app}\doc\contents\img";  Flags: ignoreversion; languages: en;

Source: "doc\german\*.html"; DestDir: "{app}\doc";  Flags: ignoreversion; languages: ger;
Source: "doc\german\*.gif"; DestDir: "{app}\doc";  Flags: ignoreversion; languages: ger;
Source: "doc\german\menu.css"; DestDir: "{app}\doc";  Flags: ignoreversion; languages: ger;
Source: "doc\german\contents\*.html"; DestDir: "{app}\doc\contents";  Flags: ignoreversion; languages: ger;
Source: "doc\german\contents\*.png"; DestDir: "{app}\doc\contents";  Flags: ignoreversion; languages: ger;
Source: "doc\german\contents\jmksf.css"; DestDir: "{app}\doc\contents";  Flags: ignoreversion; languages: ger;
Source: "doc\german\contents\img\*.gif"; DestDir: "{app}\doc\contents\img";  Flags: ignoreversion; languages: ger;

Source: "doc\keycodes.txt"; DestDir: "{app}\doc\contents";  Flags: ignoreversion;
Source: "doc\mci.hlp"; DestDir: "{app}\doc";  Flags: ignoreversion;

;Icons
Source: "icons\32x32\*.ico"; DestDir: "{app}\icons\32x32";  Flags: ignoreversion
Source: "icons\48x48\*.ico"; DestDir: "{app}\icons\48x48";  Flags: ignoreversion

;Library
Source: "lib\*.rb"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "lib\readme.txt"; DestDir: "{app}\lib"; Flags: ignoreversion; languages: en;
Source: "lib\readme_ger.txt"; DestDir: "{app}\lib"; DestName: "readme.txt"; Flags: ignoreversion; languages: ger;

;Samples
Source: "samples\*.rb"; DestDir: "{app}\samples"; Flags: ignoreversion
Source: "samples\*.bmp"; DestDir: "{app}\samples"; Flags: ignoreversion

;Tools
;Visual Dialog Designer
Source: "tools\dd\dd.rb"; DestDir: "{app}\tools\dd"; Flags: ignoreversion

Source: "tools\dd\dd.lng"; DestDir: "{app}\tools\dd"; Flags: ignoreversion; languages: en;
Source: "tools\dd\dd_ger.lng"; DestDir: "{app}\tools\dd"; DestName: "dd.lng"; Flags: ignoreversion; languages: ger;

Source: "tools\dd\dd_license.txt"; DestDir: "{app}\tools\dd"; Flags: ignoreversion; languages: en;
Source: "tools\dd\dd_license_ger.txt"; DestDir: "{app}\tools\dd"; DestName: "dd_license.txt"; Flags: ignoreversion; languages: ger;

Source: "tools\dd\img\*.bmp"; DestDir: "{app}\tools\dd\img"; Flags: ignoreversion;

Source: "tools\dd\doc\english\*.html"; DestDir: "{app}\tools\dd\doc"; Flags: ignoreversion; languages: en;
Source: "tools\dd\doc\english\*.gif"; DestDir: "{app}\tools\dd\doc"; Flags: ignoreversion; languages: en;
Source: "tools\dd\doc\english\menu.css"; DestDir: "{app}\tools\dd\doc"; Flags: ignoreversion; languages: en;
Source: "tools\dd\doc\english\contents\*.html"; DestDir: "{app}\tools\dd\doc\contents"; Flags: ignoreversion; languages: en;
Source: "tools\dd\doc\english\contents\*.png"; DestDir: "{app}\tools\dd\doc\contents"; Flags: ignoreversion; languages: en;
Source: "tools\dd\doc\english\contents\jmksf.css"; DestDir: "{app}\tools\dd\doc\contents"; Flags: ignoreversion; languages: en;
Source: "tools\dd\doc\english\contents\img\*.gif"; DestDir: "{app}\tools\dd\doc\contents\img"; Flags: ignoreversion; languages: en;

Source: "tools\dd\doc\german\*.html"; DestDir: "{app}\tools\dd\doc"; Flags: ignoreversion; languages: ger;
Source: "tools\dd\doc\german\*.gif"; DestDir: "{app}\tools\dd\doc"; Flags: ignoreversion; languages: ger;
Source: "tools\dd\doc\german\menu.css"; DestDir: "{app}\tools\dd\doc"; Flags: ignoreversion; languages: ger;
Source: "tools\dd\doc\german\contents\*.html"; DestDir: "{app}\tools\dd\doc\contents"; Flags: ignoreversion; languages: ger;
Source: "tools\dd\doc\german\contents\*.png"; DestDir: "{app}\tools\dd\doc\contents"; Flags: ignoreversion; languages: ger;
Source: "tools\dd\doc\german\contents\jmksf.css"; DestDir: "{app}\tools\dd\doc\contents"; Flags: ignoreversion; languages: ger;
Source: "tools\dd\doc\german\contents\img\*.gif"; DestDir: "{app}\tools\dd\doc\contents\img"; Flags: ignoreversion; languages: ger;

;Tool Menu Configuration
Source: "tools\tc\tc.rb"; DestDir: "{app}\tools\tc"; Flags: ignoreversion

Source: "tools\tc\tc.lng"; DestDir: "{app}\tools\tc"; Flags: ignoreversion; languages: en;
Source: "tools\tc\tc_ger.lng"; DestDir: "{app}\tools\tc"; DestName: "tc.lng"; Flags: ignoreversion; languages: ger;

;RB2HTML
Source: "tools\rb2html\rb2html.rb"; DestDir: "{app}\tools\rb2html"; Flags: ignoreversion

Source: "tools\rb2html\rb2html.lng"; DestDir: "{app}\tools\rb2html"; Flags: ignoreversion; languages: en;
Source: "tools\rb2html\rb2html_ger.lng"; DestDir: "{app}\tools\rb2html"; DestName: "rb2html.lng"; Flags: ignoreversion; languages: ger;


[Icons]
Name: "{group}\RapidBATCH 5.0 Professional"; Filename: "{app}\rbb4.exe"; WorkingDir: "{app}"

Name: "{group}\User Manual"; Filename: "{app}\doc\manual.html"; Languages: en;
Name: "{group}\Language Reference"; Filename: "{app}\doc\reference.html"; Languages: en;
Name: "{group}\What's New in 5.0"; Filename: "{app}\doc\whatsnew.html"; Languages: en;
Name: "{group}\MCI Reference"; Filename: "{app}\doc\mci.hlp"; Languages: en;
Name: "{group}\License"; Filename: "{app}\license.txt"; Languages: en;
Name: "{group}\ReadMe"; Filename: "{app}\readme.txt"; Languages: en;
Name: "{group}\Uninstall RapidBATCH 5.0"; Filename: "{uninstallexe}"; Languages: en;

Name: "{group}\Visual Dialog Designer"; Filename: "{app}\tools\dd\dd.rb"; WorkingDir: "{app}\tools\dd"; IconFilename: "{app}\rbb4.exe";
Name: "{group}\RB2HTML Script-Code Highlighter"; Filename: "{app}\tools\rb2html\rb2html.rb"; WorkingDir: "{app}\tools\rb2html";  IconFilename: "{app}\rbb4.exe";

Name: "{group}\Benutzerhandbuch"; Filename: "{app}\doc\manual.html"; Languages: ger;
Name: "{group}\Befehlsreferenz"; Filename: "{app}\doc\reference.html"; Languages: ger;
Name: "{group}\Was ist neu in 5.0"; Filename: "{app}\doc\whatsnew.html"; Languages: ger;
Name: "{group}\MCI Referenz"; Filename: "{app}\doc\mci.hlp"; Languages: ger;
Name: "{group}\Lizenz"; Filename: "{app}\license.txt"; Languages: ger;
Name: "{group}\Liesmich"; Filename: "{app}\readme.txt"; Languages: ger;
Name: "{group}\RapidBATCH deinstallieren"; Filename: "{uninstallexe}"; Languages: ger;

Name: "{userdesktop}\RapidBATCH 5.0 Professional"; Filename: "{app}\rbb4.exe"; WorkingDir: "{app}"

[Registry]
Root: HKCR; Subkey: ".rb"; ValueType: string; ValueName: ""; ValueData: "RB5Script"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "RB5Script"; ValueType: string; ValueName: ""; ValueData: "RapidBATCH 5 Script-File"; Flags: uninsdeletekey
Root: HKCR; Subkey: "RB5Script\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\rbb4.exe,1"
Root: HKCR; Subkey: "RB5Script\shell\open\command"; ValueType: string; ValueName: ""; ValueData: "{app}\RBI32.EXE ""%1"" %*"

[UninstallRun]
Filename: "{app}\rbi32.exe"; Parameters: """{app}\etc\rbb_reg.rb"" unreg";

;Root: HKCU; SubKey: "Software\jmksf"; valuetype: none;
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: none;
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "CLASS_BKGND"; valuedata: "FFFFFF"
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "CLASS_COMMENT"; valuedata: "808080"
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "CLASS_KEYWORD"; valuedata: "000000"
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "CLASS_LABEL"; valuedata: "FF0000"
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "CLASS_OPERATOR"; valuedata: "008000"
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "CLASS_SPECIAL_COMMENT"; valuedata: "008000"
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "CLASS_TEXT"; valuedata: "000000"
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "CLASS_VALUE"; valuedata: "0000FF"
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "CLASS_VARIABLE"; valuedata: "000080"
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "FontFace"; valuedata: "Courier New"
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "History0"; valuedata: ""
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "History1"; valuedata: ""
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "History2"; valuedata: ""
;Root: HKCU; SubKey: "Software\jmksf\RapidBATCH"; valuetype: string; valuename: "History3"; valuedata: ""
