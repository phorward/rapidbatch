rem +-------------------------------------------------------------------+
rem | Web Gallery Generator                                             |
rem +-------------------------------------------------------------------+
rem | This script generates a Web picture gallery as a HTML-file from a |
rem | directory containing JPEG-pictures. You can create a standard or  |
rem | a thumbnail gallery. Primarily, this example is a relict from Ra- |
rem | pidBATCH 4.4, but it was extended with RapidBATCH 5.0 functio-    |
rem | nalities.                                                         |
rem +-------------------------------------------------------------------+

rem if nothing was specified at script start-up, ask the user ;)
if [command] = ''
	folderbox [command] = 'Please select a folder:', ''
	if [command] = ''
		echo 'Invalid directory. Please restart the script.'
		halt
	endif
endif

rem verify if this is a directory
getfileatt [is_dir] = [command], 'directory'
if [is_dir] = '-1'
	msgbox 'Error','You need to specifiy a directory to this script, not a file.','16'
	halt
endif

rem change to the gallery directory to get the pictures...
chdir [command]
if [errorcode] = '-1'
	echo 'Directory does not exists.'
	halt
endif

rem ask for destination filename
[SaveBox_DefFileExt] = 'html'
savebox [dest] = 'Save gallery to HTML-file:','HTML-Files|*.html;*.htm|All files|*.*'
if [dest] = '' halt

rem prompt a gallery title
inputbox [title] = 'Gallery title', 'Please enter a title your gallery should have.','My Gallery'

rem ask if thumbnails should be created...
confirm [thumb] = 'Create thumbnail gallery?'

infobox 'Creating gallery...', 'show'

writefile [dest]:'<html>'
writefile [dest]:'	<head>'
writefile [dest]:'		<title>' # [title] # '</title>'
writefile [dest]:'	</head>'
writefile [dest]:'	<body bgcolor="#000000" text="#ffffff">'
writefile [dest]:'		<center>'
writefile [dest]:'			<h1>' # [title] # '</h1>'
writefile [dest]:'			<hr size="0" color="#0000ff">'

rem retrieve file list from directory
findfiles [files] = '*.jpg', '1'

rem go through the files
[cnt] = '1'
repeat
	rem get filename from list
	gettok [file] = [files], '|', [cnt]
	
	if [file] ! ''
		rem add image-tag for the file
		[tmp] = '				'
		if [thumb] = '0'
			[tmp] # '<a href="' # [file] # '"><img src="' # [file] # '" border="0" width="100" height="80" alt="' # [file] # '"></a>'
		else
			[tmp] # '<img src="' # [file] # '" border="0" alt="' # [file] # '"><br>' # [file2] # '<br><br>'
		endif
		writefile [dest]:[tmp]
	endif
	
	[cnt] + '1'
until [file] = ''

writefile [dest]:'			<hr size="0" color="#0000ff">'
writefile [dest]:'		</center>'
writefile [dest]:'	</body>'
writefile [dest]:'</html>'

wait '1000'

infobox 'Gallery has been created.', 'show'
wait '2000'
end