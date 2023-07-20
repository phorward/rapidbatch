[index] = 'output\index.html'

delfile [index]
findfiles [files] = 'output\*.html', '1'

readfile [template] = 'templates\template.html','0'

replacevar [template] = [template], '#TITLE#', 'RapidBATCH 5.0 User Manual Preview'

repeat
  [cnt] + '1'
  gettok [file] = [files], '|', [cnt]
  if [file] ! ''
    [content] # '<li><a href="' # [file] # '">' # [file] # '</a></li>' # [new_line]
  endif
until [file] = ''

replacevar [template] = [template], '#CONTENT#', '<ul>' # [content] # '</ul>'

replacevar [template] = [template], '#UDMS_PREV#', ''
replacevar [template] = [template], '#UDMS_NEXT#', ''


writefile [index], [template]

echo 'Done'
end
