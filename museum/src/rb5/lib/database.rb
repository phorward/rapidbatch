rem +-------------------------------------------------------------------+
rem | RapidBATCH v5.1                                                   |
rem +-------------------------------------------------------------------+
rem | Library: Database Engine                                          |
rem | Version: 1.3                                                      |
rem | Authors: Jan Max Meyer                                            |
rem +-------------------------------------------------------------------+
rem | Copyright (C) 2000-2006 by J.M.K S.F. Software Technologies       |
rem +-------------------------------------------------------------------+
rem | This library is part of the RapidBATCH 5 software package.        |
rem | It may be used within any RapidBATCH script and for free of       |
rem | charge. Changes in this library are allowed for personal purpose. |
rem |                                                                   |
rem | If you find any bugs or wrote own, extending functions, please    |
rem | send us a mail.                                                   |
rem +-------------------------------------------------------------------+

rem *************
rem ** GLOBALS **
rem *************

rem WARNING: The following variables should not be changed or modified!
dec [db_fields], [db_ftypes], [db_fvalues], [db_opened], [db_dir]
dec [db_database], [db_dscext], [db_dbext], [db_delimiter], [db_content], [db_report_content]

[db_opened] = [false]
[db_database] = ''
[case_sensitivity] = [false]

rem NOTE: The following variables can be changed from your implementing
rem       script-file.

rem Database file directory
[db_dir] = [current]

rem Database descriptor file extension
[db_dscext] = '.dsc'

rem Database data file extension
[db_dbext] = '.db'

rem Database lockfile extension
[db_lockext] = '.lck'

rem Database field delimiter
[db_delimiter] = '#'

rem line break identification character (will be replace with [new_line])
getchr [db_newline] = '123'

rem *********************
rem ** IMPLEMENTATIONS **
rem *********************

rem ===================================================================
rem PROCEDURE:
rem db_dataset_to_array
rem
rem DESCRIPTION:
rem Converts a dataset to an array
rem
rem PARAMETERS:
rem + [line]
rem   Dataset-line from the database file
rem + [*dataset]
rem   Pointer to a variable where the data from the set is stored
rem
rem ===================================================================
proc db_dataset_to_array: [line], [*dataset]
	if [db_opened] = [false]
		[errorcode] = [false]
		ret
	endif

	dec [j]
	[j] = '0'
	repeat
		[j] + '1'
		gettok [dataset[j]] = [line], [db_delimiter], [j]
	until [j] = [db_fields:'0']
endproc


rem ===================================================================
rem FUNCTION:
rem db_array_to_dataset
rem
rem DESCRIPTION:
rem Converts an to a dataset
rem
rem PARAMETERS:
rem + [*dataset]
rem   Pointer to a variable where the data from the set is stored
rem
rem RETURN VALUE:
rem On error, the return value an empty line, else the dataset to be
rem written to the database.
rem ===================================================================
func db_array_to_dataset: [*dataset]
	if [db_opened] = [false]
		[errorcode] = [false]
		ret ''
	endif

	dec [line], [j]
	[j] = '0'
	repeat
		if [j] > '0' [line] # [db_delimiter]
		[j] + '1'
		[line] # [dataset:[j]]
	until [j] = [db_fields:'0']

	ret [line]
endfunc


rem ===================================================================
rem FUNCTION:
rem db_dataset_count
rem
rem DESCRIPTION:
rem Returns the number of datasets in the currently opened
rem database file.
rem
rem PARAMETERS:
rem none
rem
rem RETURN VALUE:
rem On error, the return value -1, else the number of items
rem ===================================================================
func db_dataset_count
	if [db_opened] = [false]
		[errorcode] = [false]
		ret '0'
	endif

	ret [db_content:'0']
endfunc


rem ===================================================================
rem PROCEDURE:
rem db_open
rem
rem DESCRIPTION:
rem Opens a database based on a Database description file.
rem
rem PARAMETERS:
rem + [db]
rem   The name of the database to be opened (without .DB extension!)
rem
rem RETURN VALUE:
rem [errorcode] gets '0' on success, else '-1'.
rem ===================================================================
proc db_open: [db]
	dec [i], [j], [line], [ch]

	rem reset database files
	reset [db_fields], [db_ftypes]

	rem check if database files exist
	fileexists [errorcode] = [db_dir] # '\' # [db] # [db_dscext]
	if [errorcode] = [false] ret
	fileexists [errorcode] = [db_dir] # '\' # [db] # [db_dbext]
	if [errorcode] = [false]
		newfile [db_dir] # '\' # [db] # [db_dbext]
	endif
	
	rem lock file exists?
	fileexists [errorcode] = [db_dir] # '\' # [db] # [db_lockext]
	if [errorcode] = [true]
		[errorcode] = [false]
		ret
	endif
	newfile [db_dir] # '\' # [db] # [db_lockext]


	rem build field configuration
	[i] = '0'
	[j] = '0'
	repeat
		[i] + '1'
		readfile [line] = [db_dir] # '\' # [db] # [db_dscext], [i]
		trimvar [line] = [line]
		getcharat [ch] = [line], '1'

		if [ch] ! ';' & [line] ! '' & [line] ! 'EOF'
			[j] + '1'
			gettok [db_fields[j]] = [line], ':', '1'
			gettok [db_ftypes[j]] = [line], ':', '2'

			lowvar [db_ftypes[j]] = [db_ftypes[j]]
			if [db_ftypes[j]] = 'single' [db_ftypes[j]] = '0'
			if [db_ftypes[j]] = 'multi' [db_ftypes[j]] = '1'
			if [db_ftypes[j]] = 'select' [db_ftypes[j]] = '2'
			if [db_ftypes[j]] = 'check' [db_ftypes[j]] = '3'
			if [db_ftypes[j]] = 'list' [db_ftypes[j]] = '4'
			if [db_ftypes[j]] = 'multilist' [db_ftypes[j]] = '5'

			gettok [db_fvalues[j]] = [line], ':', '3'
		endif
	until [line] = 'EOF'

	[db_opened] = [true]
	[db_database] = [db]

	rem Load all sets of the current database into the memory!
	[i] = '0'
	readfile [db_content] = [db_dir] # '\' # [db_database] # [db_dbext], '0'
	repeat
		[i]+'1'
		gettok [line] = [db_content], [crlf], [i]
		if [line] ! [null]
			replacevar [line] = [line], [db_newline], [new_line]

			[db_content[i]] = [line]
			db_dataset_to_array [line], [db_content[i]]
		endif
	until [line] = [null]

	[errorcode] = [true]
endproc


rem ===================================================================
rem PROCEDURE:
rem db_close
rem
rem DESCRIPTION:
rem Closes the currently opened database.
rem
rem PARAMETERS:
rem
rem RETURN VALUE:
rem [errorcode] gets '0' on success, else '-1'.
rem ===================================================================
proc db_close
	if [db_opened] = [false]
		[errorcode] = [false]
		ret
	endif

	rem reset database fields and content
	reset [db_fields], [db_ftypes], [db_content]

	rem unlock data
	delfile [db_dir] # '\' # [db_database] # [db_lockext]

	[db_opened] = [false]
	[db_database] = ''

	[errorcode] = [true]
endproc


rem ===================================================================
rem PROCEDURE:
rem db_store
rem
rem DESCRIPTION:
rem Store the cached content of the currently loaded database to the
rem database file
rem
rem PARAMETERS:
rem
rem RETURN VALUE:
rem [errorcode] gets '0' on success, else '-1'.
rem ===================================================================
proc db_store
	if [db_opened] = [false]
		[errorcode] = [false]
		ret
	endif

	renamefile [db_dir] # '\' # [db_database] # [db_dbext], [db_dir] # '\' # [db_database] # '.tmp'
	delfile [db_dir] # '\' # [db_database] # [db_dbext]

	dec [i]
	[i] = '0'
	repeat
		[i] + '1'
		writefile [db_dir] # '\' # [db_database] # [db_dbext], [db_content:[i]]
	until [i] = [db_content:'0'] | [errorcode] = [false]

	if [errorcode] = [true]
		delfile [db_dir] # '\' # [db_database] # '.tmp'
	endif
endproc

rem ===================================================================
rem FUNCTION:
rem db_find
rem
rem DESCRIPTION:
rem Finds a record.
rem
rem PARAMETERS:
rem + [*dataset]
rem   Pointer to an array structure containing field information to be
rem   searched for. This structure should have the same number of
rem   fields as specified in the database's field description.
rem
rem RETURN VALUE:
rem On error, the return value is '-1', else the id of the dataset
rem that was found.
rem ===================================================================
func db_find: [*dataset]
	if [db_opened] = [false]
		[errorcode] = [false]
		ret '-1'
	endif

	dec [i], [j], [k]
	repeat
		[i] + '1'

		[j] = '0'
		repeat
			[j] + '1'
			if [dataset:[j]] ! ''
				getpos [errorcode] = [db_content[i][j]], [dataset:[j]], '1'

				rem if a value matches, copy the dataset!
				if [errorcode] > '0'
					[k] = '0'
					repeat
						[k] + '1'
						[dataset:[k]] = [db_content[i][k]]
					until [k] = [db_content[i]'0']
					ret [i]
				endif
			endif
		until [j] = [db_fields:'0']
	until [i] = [db_content:'0']
	ret '-1'
endfunc

rem ===================================================================
rem FUNCTION:
rem db_insert
rem
rem DESCRIPTION:
rem Insers a new record into the database. The dataset will be appended
rem to the database file.
rem
rem PARAMETERS:
rem + [*dataset]
rem   Pointer to an array structure containing the data that should
rem   be written. This structure should have the same number of
rem   fields as specified in the database's field description.
rem
rem RETURN VALUE:
rem The record ID of the inserted dataset
rem ===================================================================
func db_insert: [*dataset]
	dec [id], [line]

	if [db_opened] = [false]
		[errorcode] = [false]
		ret [null]
	endif

	[id] = [db_content:'0']
	[id] + '1'

	[db_content:[id]] = [null]
	
	db_update [id], [dataset]
	ret [id]
endfunc

rem ===================================================================
rem PROCEDURE:
rem db_delete
rem
rem DESCRIPTION:
rem Deletes an existing record.
rem
rem PARAMETERS:
rem + [id]
rem   Dataset ID that should be deleted.
rem
rem RETURN VALUE:
rem [errorcode] gets '0' on success, else '-1'.
rem ===================================================================
proc db_delete: [id]
	if [db_opened] = [false]
		[errorcode] = [false]
		ret
	endif

	dec [tmp], [i], [j], [k]

	[i] = '0'
	[k] = '0'
	repeat
		[i] + '1'
		if [i] ! [id]
			[k] + '1'
			[tmp[k]] = [db_content[i]]
			[j] = '0'
			repeat
				[j] + '1'
				[tmp[k][j]] = [db_content[i][j]]
			until [j] = [db_content[i]'0']
		endif
	until [i] = [db_content'0']

	reset [db_content]

	[i] = '0'
	repeat
		[i] + '1'

		[db_content[i]] = [tmp[i]]
		[j] = '0'

		repeat
			[j] + '1'
			[db_content[i][j]] = [tmp[i][j]]
		until [j] = [tmp[i]'0']
	until [i] = [tmp'0']

	db_store

	[errorcode] = [true]
endproc

rem ===================================================================
rem PROCEDURE:
rem db_fetch
rem
rem DESCRIPTION:
rem Fetches the record with the specified ID from the database to a
rem pointed array structure.
rem
rem PARAMETERS:
rem + [id]
rem   The ID of the dataset that should be fetched. If this parameter
rem   is set to 0, db_fetch returns the last record of the table.
rem + [*dataset]
rem   Pointer to an array structure that is filled by the function
rem   when the dataset is fetched. This structure should have the same
rem   number of fields as specified in the database's field descrip-
rem   tion.
rem
rem RETURN VALUE:
rem [errorcode] gets '0' on success, else '-1'.
rem ===================================================================
proc db_fetch: [id], [*dataset]
	if [db_opened] = [false]
		[errorcode] = [false]
		ret
	endif

	dec [i]

	if [id] < '1'
		[id] = [db_content'0']
	endif
	if [id] > [db_content'0']
		[errorcode] = [false]
		ret
	endif

	if [id] > '0'
		[i] = '0'
		repeat
			[i] + '1'
			[dataset:[i]] = [db_content[id][i]]
		until [i] = [db_content[id]'0']

		[errorcode] = [true]
	else
		[errorcode] = [false]
	endif
endproc

rem ===================================================================
rem PROCEDURE:
rem db_update
rem
rem DESCRIPTION:
rem Updates the record with the specified ID in the database to the
rem data of the assigned array structure.
rem
rem PARAMETERS:
rem + [id]
rem   The ID of the dataset that should be updated.
rem + [*dataset]
rem   Pointer to an array structure that contains the information that
rem   should be updated to the database.
rem   This structure should have the same number of fields as speci-
rem   fied in the database's field description.
rem
rem RETURN VALUE:
rem [errorcode] gets '0' on success, else '-1'.
rem ===================================================================
proc db_update: [id], [*dataset]
	if [db_opened] = [false]
		[errorcode] = [false]
		ret
	endif

	if [id] < '1' | [id] > [db_content'0']
		[errorcode] = [false]
		ret
	endif

	dec [i]
	[i] = '0'
	repeat
		[i] + '1'
		[db_content[id][i]] = [dataset:[i]]
	until [i] = [db_fields:'0']

	db_array_to_dataset [db_content[id]] = [dataset]
	db_store
	[errorcode] = [true]
endproc


rem ===================================================================
rem FUNCTION:
rem db_editmask
rem
rem DESCRIPTION:
rem This function generates a dialog window based on the database's
rem description. This function is very useful to quickly create a
rem basic user interface for database applications. The dialog that is
rem generated can be modified after the function was executed, and must
rem be shown manually by the application that generates it.
rem
rem PARAMETERS:
rem + [usedefaults]
rem   If this is set to '0', default-values from the database descrip-
rem   tion are automatically inserted into the fields, else, all
rem   fields are left blank.
rem
rem RETURN VALUE:
rem On success, the label of the generated dialog is returned.
rem ===================================================================

rem NOTE: The following variables are default values to position and
rem       size the generated input-fields and controls for the
rem       db_editmask-routine. These values can be changed from the
rem       particular implementing script.
[db_dialog_single_width] = '200'
[db_dialog_multi_width] = '200'
[db_dialog_select_width] = '80'
[db_dialog_check_width] = '80'
[db_dialog_list_width] = '200'

[db_dialog_single_height] = '25'
[db_dialog_multi_height] = '120'
[db_dialog_select_height] = '100'
[db_dialog_check_height] = '25'
[db_dialog_list_height] = '120'

[db_dialog_label_width] = '150'
[db_dialog_label_height] = '25'

[db_dialog_field_spacing] = '5'

func db_editmask: [usedefaults]
	dec [dlgname],[y],[i], [value], [maxheight], [maxwidth]

	if [db_opened] = [false] ret ''
	if [db_fields:'0'] = '0' ret ''

	repeat
		randvar [dlgname] = '1024'
		[dlgname] = 'dbdialog' # [dlgname]
		newdialog [dlgname], 'dialog', '1|1|800|600'
	until [errorcode] = [true]

	letdialog [dlgname], 'style', 'single'
	letdialog [dlgname], 'visible', [false]

	[y] = [db_dialog_field_spacing]
	[i] = '0'
	repeat
		[i] + '1'

		newdialog [dlgname] # ':lbl_' # [db_fields:[i]], 'label', '5' # [std_sep] # [y] # [std_sep] # [db_dialog_label_width] # [std_sep] # [db_dialog_label_height]
		letdialog [dlgname] # ':lbl_' # [db_fields:[i]], 'caption', [db_fields:[i]]

		[value] = ''
		if [usedefaults] ! [false] [value] = [db_fvalues[i]]

		if [db_ftypes:[i]] = '0'
			newdialog [dlgname] # ':val_' # [db_fields:[i]], 'input', [db_dialog_label_width] # [std_sep] # ([y] - [db_dialog_field_spacing]) # [std_sep] # [db_dialog_single_width] # [std_sep] # [db_dialog_single_height]
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'text', [value]
			[y] + [db_dialog_single_height]
		elseif [db_ftypes:[i]] = '1'
			newdialog [dlgname] # ':val_' # [db_fields:[i]], 'edit', [db_dialog_label_width] # [std_sep] # ([y] - [db_dialog_field_spacing]) # [std_sep] # [db_dialog_multi_width] # [std_sep] # [db_dialog_multi_height]
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'text', [value]
			[y] + [db_dialog_multi_height]
		elseif [db_ftypes:[i]] = '2'
			newdialog [dlgname] # ':val_' # [db_fields:[i]], 'combo', ([db_dialog_label_width] + '1') # [std_sep] # ([y] - [db_dialog_field_spacing]) # [std_sep] # [db_dialog_select_width] # [std_sep] # [db_dialog_select_height]
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'items', [value]

			rem We use the height for a single field because this is a combo box!
			[y] + [db_dialog_single_height]
		elseif [db_ftypes:[i]] = '3'
			newdialog [dlgname] # ':val_' # [db_fields:[i]], 'option', [db_dialog_label_width] # [std_sep] # ([y] - [db_dialog_field_spacing]) # [std_sep] # [db_dialog_check_width] # [std_sep] # [db_dialog_check_height]
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'event_click', [false]
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'checked', [value]

			[y] + [db_dialog_check_height]
		elseif [db_ftypes:[i]] = '4'
			newdialog [dlgname] # ':val_' # [db_fields:[i]], 'list', [db_dialog_label_width] # [std_sep] # ([y] - [db_dialog_field_spacing]) # [std_sep] # [db_dialog_list_width] # [std_sep] # [db_dialog_list_height]
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'items', [value]

			[y] + [db_dialog_list_height]
		elseif [db_ftypes:[i]] = '5'
			newdialog [dlgname] # ':val_' # [db_fields:[i]], 'list_sorted', [db_dialog_label_width] # [std_sep] # ([y] - [db_dialog_field_spacing]) # [std_sep] # [db_dialog_list_width] # [std_sep] # [db_dialog_list_height]
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'text', [value]
			[y] + [db_dialog_list_height]
		endif

		if [i] = '1' letdialog [dlgname] # ':val_' # [db_fields:[i]], 'focus', [true]
	until [i] = [db_fields:'0']


	[maxwidth] = [db_dialog_single_width]
	if [maxwidth] < [db_dialog_multi_width] [maxwidth] = [db_dialog_multi_width]
	if [maxwidth] < [db_dialog_select_width] [maxwidth] = [db_dialog_select_width]

	letdialog [dlgname], 'position', '1' # [std_sep] # '1' # [std_sep] # ([maxwidth] + [db_dialog_label_width] + '10') # [std_sep] # ([y] + '20')
	ret [dlgname]
endfunc

rem ===================================================================
rem PROCEDURE:
rem db_mask_put_dataset
rem
rem DESCRIPTION:
rem This procedure fills a by db_editmask generated dataset input
rem mask with the values assigned over the dataset.
rem
rem PARAMETERS:
rem + [dlgname]
rem   Name of the dialog where the dataset should be put to.
rem + [*dataset]
rem   Pointer to an array structure that contains the information that
rem   should be displayed in the mask.
rem   This structure should have the same number of fields as speci-
rem   fied in the database's field description.
rem
rem RETURN VALUE:
rem On success, [errorcode] gets '0', else '-1'.
rem ===================================================================
proc db_mask_put_dataset: [dlgname], [*dataset]
	dec [i], [value]

	if [db_opened] = [false]
		[errorcode] = [false]
		ret
	endif
	if [db_fields:'0'] = '0'
		[errorcode] = [false]
		ret
	endif

	[i] = '0'
	repeat
		[i] + '1'

		[value] = [dataset:[i]]

		if [db_ftypes:[i]] = '0'
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'text', [value]
		elseif [db_ftypes:[i]] = '1'
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'text', [value]
		elseif [db_ftypes:[i]] = '2'
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'text', [dataset:[i]]
		elseif [db_ftypes:[i]] = '3'
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'checked', [value]
		elseif [db_ftypes:[i]] = '4'
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'selection', [value]
		elseif [db_ftypes:[i]] = '5'
			letdialog [dlgname] # ':val_' # [db_fields:[i]], 'selection', [value]
		endif
	until [i] = [db_fields:'0']

	[errorcode] = [true]
endproc

rem ===================================================================
rem PROCEDURE:
rem db_mask_get_dataset
rem
rem DESCRIPTION:
rem This procedure does the reverse operation as db_mask_put_dataset
rem performs it. It reads all fields of a generated database mask and
rem stores the values to a corresponding array structure.
rem
rem PARAMETERS:
rem + [dlgname]
rem   Name of the dialog where the dataset should be put to.
rem + [*dataset]
rem   Pointer to an array structure where the content of the input
rem   mask are copied to.
rem
rem RETURN VALUE:
rem On success, [errorcode] gets '0', else '-1'.
rem ===================================================================
proc db_mask_get_dataset: [dlgname], [*dataset]
	dec [i], [value]

	reset [dataset]

	if [db_opened] = [false]
		[errorcode] = [false]
		ret
	endif
	if [db_fields:'0'] = '0'
		[errorcode] = [false]
		ret
	endif

	[i] = '0'
	repeat
		[i] + '1'

		if [db_ftypes:[i]] = '0'
			getdialog [value] = [dlgname] # ':val_' # [db_fields:[i]], 'text'
		elseif [db_ftypes:[i]] = '1'
			getdialog [value] = [dlgname] # ':val_' # [db_fields:[i]], 'text'
		elseif [db_ftypes:[i]] = '2'
			getdialog [value] = [dlgname] # ':val_' # [db_fields:[i]], 'text'
		elseif [db_ftypes:[i]] = '3'
			getdialog [value] = [dlgname] # ':val_' # [db_fields:[i]], 'checked'
		elseif [db_ftypes:[i]] = '4'
			getdialog [value] = [dlgname] # ':val_' # [db_fields:[i]], 'selection'
		elseif [db_ftypes:[i]] = '5'
			getdialog [value] = [dlgname] # ':val_' # [db_fields:[i]], 'selection'
		endif

		[dataset:[i]] = [value]

	until [i] = [db_fields:'0']

	[errorcode] = [true]
endproc

rem ===================================================================
rem PROCEDURE:
rem db_fillup_defauls
rem
rem DESCRIPTION:
rem This procedure fills a dataset array structure with the default
rem values from the database field definitions.
rem
rem PARAMETERS:
rem + [*dataset]
rem   Pointer to an array structure where the default data is stored
rem   to.
rem
rem RETURN VALUE:
rem On success, [errorcode] gets '0', else '-1'.
rem ===================================================================
proc db_fillup_defaults: [*dataset]
	dec [i], [value]

	if [db_opened] = [false]
		[errorcode] = [false]
		ret
	endif
	if [db_fields:'0'] = '0'
		[errorcode] = [false]
		ret
	endif

	[dataset] = ''
	reset [dataset]

	[i] = '0'
	repeat
		[i] + '1'
		if [db_ftypes:[i]] ! '2' [dataset:[i]] = [db_fvalues:[i]]
	until [i] = [db_fields:'0']

	[errorcode] = [true]
endproc



dec [db_report_content]

rem ===================================================================
rem PROCEDURE:
rem db_report
rem
rem DESCRIPTION:
rem Executes a report from a report definition file.
rem A report is a ASCII-text file containing commands written in a
rem simple scripting language for creating reports, which is inter-
rem pretered by db_report and db_report_section.
rem
rem PARAMETERS:
rem + [report_desc]
rem   Path to the report description file
rem
rem RETURN VALUE:
rem On success, [errorcode] gets '0', else '-1'.
rem ===================================================================
proc db_report: [report_desc]
	dec [i], [old_linebr], [variables], [outfile]

	[old_linebr] = [eol]
	[eol] = [null]
	[i] = '0'

	db_report_section [report_desc], [i], [outfile], [null], [null], [null], [null], [variables]

	reset [db_report_content]

	[eol] = [old_linebr]
endproc

rem ===================================================================
rem PROCEDURE:
rem db_report_section
rem
rem DESCRIPTION:
rem Internal function for executing a report.
rem
rem PARAMETERS:
rem + [report_desc]
rem   Path to the report description file
rem + [*i]
rem	Line in the current report that is executed
rem + [*outfile]
rem 	Output file
rem + [db]
rem 	Database to run on
rem + [join_fld]
rem 	Number of the join-field
rem + [join_vol]
rem 	Value to verify with the join-field
rem + [*join]
rem 	Dataset of the previous level
rem + [*vars]
rem 	Report variables to be accessed globally in the report script
rem
rem RETURN VALUE:
rem On success, [errorcode] gets '0', else '-1'.
rem ===================================================================
proc db_report_section: [report_desc], [*i], [*outfile], [db], [join_fld], [join_val], [*join], [*vars]
	dec [db_line], [line], [start], [cmd], [join], [dbnext], [join_fld_next]
	dec [join_val_next], [id], [text],[pos],[len],[source],[target],[fld], [j], [k], [l], [p], [var]

	if [db] ! [null]
		[l] = '1'
		if [db_report_content:'0'] ! '0'
			repeat
				if [db_report_content:[l]] = [db] break
				[l] + '1'
			until [l] = [db_report_content:'0']+'1'
		endif

		if [l] = [db_report_content:'0']+'1'
			db_open [db]
			if [errorcode] = [false] ret

			[db_report_content:[l]] = [db]
			[db] = [l]

			[j] = '0'
			repeat
				[j] + '1'
				[db_report_content[db][j]] = [db_content:[j]]
				[k] = '0'
				repeat
					[k] + '1'
					[db_report_content[db][j][k]] = [db_content:[j][k]]
				until [k] = [db_content:[j]'0']
			until [j] = [db_content:'0']
			db_close
		endif

		[db] = [l]
	endif

	[id] = '0'

	[start] = [i]
	repeat
		if [db] ! ''
			if [join_fld] ! '' & [join_fld] > '0'
				repeat
					[id] + '1'
				until [id] = [db_report_content[db]'0'] | [db_report_content[db][id][join_fld]] = [join_val]
				
				if [id] = [db_report_content[db]'0'] & [db_report_content[db][id][join_fld]] ! [join_val] break
			else
				[id] + '1'
			endif
		endif

		[i] = [start]
		repeat
			[i] + '1'
			readfile [line] = [report_desc], [i]
			
			if [errorcode] = [true] & [line] ! [null]
				[p] = '1'
				gettok [cmd] = [line], ':', [p]
				[p]+'1'

				trimvar [cmd] = [cmd]
				if [cmd] = [null] cont
				
				upvar [cmd] = [cmd]
				
				rem these are the commands of the report scripting language
				if [cmd] = 'LOOP'
					gettok [dbnext] = [line], ':', [p]

					rem echo '>' # [dbnext]# '< >'#[db]#'< >' # [id]#'< >' #[join_val_next] # '< = >' # [db_report_content[db][id][join_val_next]] # '<'

					[p]+'1'
					gettok [join_fld_next] = [line], ':', [p]
					if [join_fld_next] ! ''
						[p]+'1'
						gosub 'get_source'
					endif

					if [join_fld_next] ! [null]
						db_report_section [report_desc], [i], [outfile], [dbnext], [join_fld_next], [source], [db_report_content[db][id]], [vars]
					else
						db_report_section [report_desc], [i], [outfile], [dbnext], [null], [null], [db_report_content[db][id]], [vars]
					endif

					if [errorcode] = [false] ret
				elseif [cmd] = 'FILE'
					gosub 'get_source'
					
					[outfile] = [source]
					delfile [outfile]
					[errorcode] = [true]
				elseif [cmd] = 'OPEN'
					open [outfile]
				elseif [cmd] = 'ASK'
					gosub 'get_target'

					getpos [pos] = [line], ':', [p]-'1'
					[text] = [null]
					if [pos] > '0'
						getlen [len] = [line]
						copyvar [text] = [line], [pos]+'1', [len]
					endif

					inputbox [vars:[target]] = [report_desc], [text], [vars:[target]]

				elseif [cmd] = 'FREE'
					gosub 'get_target'
					[vars:[target]] = [null]
				elseif [cmd] = 'SET'
					gosub 'get_target'
					gosub 'get_source'
					
					[vars:[target]] = [source]
				elseif [cmd] = 'ADD'
					gosub 'get_target'
					gosub 'get_source'

					[vars:[target]] + [source]
				elseif [cmd] = 'SUB'
					gosub 'get_target'
					gosub 'get_source'

					[vars:[target]] - [source]
				elseif [cmd] = 'MUL'
					gosub 'get_target'
					gosub 'get_source'

					[vars:[target]] * [source]
				elseif [cmd] = 'DIV'
					gosub 'get_target'
					gosub 'get_source'

					[vars:[target]] / [source]
				elseif [cmd] = 'CAT'
					gosub 'get_target'
					gosub 'get_source'

					[vars:[target]] # [source]
				elseif [cmd] = 'PRINT'
					gosub 'get_source'

					writefile [outfile], [source]
				elseif [cmd] = 'ENDLOOP' | [cmd] = 'COMMENT'
					rem continue...
				else
					msgbox [null] = [report_desc] # ' (line ' # [i] # ')', 'Error: Unknown report command "' # [cmd] # '"', '16'
					[errorcode] = [false]
					ret
				endif
			endif
		until [errorcode] = [false] | [cmd] = 'ENDLOOP'

		if [db] = '' break
	until [id] = [db_report_content[db]'0'] | [errorcode] = [false]
	
	[errorcode] = [true]
	ret

	rem two utility routines
	%get_target
	gettok [target] = [line],':',[p]
	trimvar [target] = [target]
	[p]+'1'

	if [target] < '1'
		msgbox [report_desc] # ' (line ' # [i] # ')', 'Error: Unable to access variable number ' # [target], '16'
		[errorcode] = [false]
		ret
	endif
	[errorcode] = [true]
	resume

	%get_source
	gettok [source] = [line], ':', [p]
	trimvar [source] = [source]
	upvar [source] = [source]

	getpos [pos] = [line], ':', [p]
	[text] = [null]
	if [pos] > '0'
		getlen [len] = [line]
		copyvar [text] = [line], [pos]+'1', [len]
	endif
	
	if [source] = 'THIS'
		trimvar [fld] = [text]
		[text] = [db_report_content[db][id][fld]]
	elseif [source] = 'PREV'
		trimvar [fld] = [text]
		[text] = [join:[fld]]
	elseif [source] = 'VAR'
		trimvar [fld] = [text]
		if [fld] < '1'
			msgbox [report_desc] # ' (line ' # [i] # ')', 'Error: Unable to access variable number ' # [fld], '16'
			[errorcode] = [false]
			ret
		endif
		[text] = [vars:[fld]]
	elseif [source] = 'NEWLINE'
		[text] = [crlf]	
	endif
	[source] = [text]
	[errorcode] = [true]
	resume

endproc

end