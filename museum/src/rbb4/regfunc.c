	// LETREGVAL -----------------------------------------------------------
        if(cmd == "LETREGVAL")
        {
	        TempStr5 = Trim(toUpper(rbGetValue(0)));
                rbScanChar(",", true);
	        TempStr1 = Trim(toUpper(rbGetValue(0)));
                rbScanChar(",", true);
	        TempStr2 = Trim(rbGetValue(0));
                rbScanChar(",:", true);
	        TempStr3 = Trim(rbGetValue(0));
                rbScanChar(",", true);
	        TempStr4 = Trim(rbGetValue(0));

                HKEY regKey, rootKey;

                if(TempStr1 == "CLASSES_ROOT") rootKey = HKEY_CLASSES_ROOT;
                if(TempStr1 == "CURRENT_CONFIG") rootKey = HKEY_CURRENT_CONFIG;
                if(TempStr1 == "CURRENT_USER") rootKey = HKEY_CURRENT_USER;
                if(TempStr1 == "LOCAL_MACHINE") rootKey = HKEY_LOCAL_MACHINE;
                if(TempStr1 == "USERS" || TempStr1 == "DYN_DATA") rootKey = HKEY_USERS;


                if(TempStr2 != "")
                {
                        if(RegOpenKey(rootKey, TempStr2.c_str(), &regKey) != ERROR_SUCCESS)
                        {
                                ERRORCODE("-1");
                                return RB_OK;
                        }
                }
                else
                {
                        regKey = rootKey;
                }

                long value_type = REG_SZ;

                if (TempStr5 == "DWORD" || TempStr5 == "1")
                        value_type = REG_DWORD;
                else if (TempStr5 == "BINARY" || TempStr5 == "2")
                        value_type = REG_BINARY;

                if(RegSetValueEx(regKey, TempStr3.c_str(), 0, value_type, TempStr4.c_str(), TempStr4.length()) == ERROR_SUCCESS)
                {
                        ERRORCODE("0");
                }
                else
                {
                        ERRORCODE("-1");
                }

                RegCloseKey(regKey);

         	return RB_OK;
	}
	// GETREGVAL -----------------------------------------------------------
        if(cmd == "GETREGVAL")
        {
	        TempStr1 = rbScanVar();
                rbScanChar("=", true);
	        TempStr2 = toUpper(Trim(rbGetValue(0)));
                rbScanChar(",:", true);
	        TempStr3 = Trim(rbGetValue(0));
                rbScanChar(",", true);
	        TempStr4 = Trim(rbGetValue(0));

                HKEY regKey, rootKey;

                if(TempStr2 == "CLASSES_ROOT") rootKey = HKEY_CLASSES_ROOT;
                if(TempStr2 == "CURRENT_CONFIG") rootKey = HKEY_CURRENT_CONFIG;
                if(TempStr2 == "CURRENT_USER") rootKey = HKEY_CURRENT_USER;
                if(TempStr2 == "LOCAL_MACHINE") rootKey = HKEY_LOCAL_MACHINE;
                if(TempStr2 == "USERS" || TempStr2 == "DYN_DATA") rootKey = HKEY_USERS;

                if(TempStr3 != "")
                {
                        if(RegOpenKey(rootKey, TempStr3.c_str(), &regKey) != ERROR_SUCCESS)
                        {
                                ERRORCODE("-1");
                                return RB_OK;
                        }
                }
                else
                {
                        regKey = rootKey;
                }

                char* regvalue = (char*)malloc(STDLINE * sizeof(char));
                DWORD maxsize = STDLINE;
                long retval;

                DWORD strsize = STDLINE;

                if(TempStr4 != "")
                {
                        while((retval = RegQueryValueEx( regKey, TempStr4.c_str(), NULL, NULL, (LPBYTE) regvalue, &strsize))
                                        == ERROR_MORE_DATA)
                        {
                                strsize += STDLINE;
                                regvalue = (char*)realloc(regvalue, strsize);
                        }
                }
                else
                {
                        while((retval = RegQueryValueEx( regKey, NULL, NULL, NULL, (LPBYTE) regvalue, &strsize))
                                        == ERROR_MORE_DATA)
                        {
                                strsize += STDLINE;
                                regvalue = (char*)realloc(regvalue, strsize);
                        }
                }

                if(retval == ERROR_SUCCESS)
                {
                        ERRORCODE("0");
                        RB_LETVAR(TempStr1, regvalue);
                }
                else
                {
                        ERRORCODE("-1");
                }
                RegCloseKey(regKey);
                if(regvalue != (char*)NULL) free(regvalue);

         	return RB_OK;
	}
