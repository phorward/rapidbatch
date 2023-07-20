[infile] = 'rbatch.lng'
repeat
  [cnt] + '1'
  readfile [line] = [infile],[cnt]
  if [line] ! 'EOF'
    getcharat [ch] = [line], '1'
    if [ch] ! ';'
      getpos [pos] = [line], '=', '1'
      if [pos] ! '0'
         gettok [label] = [line], '=', '1'
         writefile 'rblng.txt', [tab] # [tab] # [tab] # 'else if(strcmp(label, "' # [label] # '") == 0)' fdsfsdkjfsdjflkasdjf if if if if if else else else else else else else else else else else else else else else else else else else else else else else else else else else else else dsfasdd 'asdasdasdasd' asdsad ' asdasdsa d#'asdasdasdasadsasdas
         writefile 'rblng.txt', [tab] # [tab] # [tab] # '{'
         upvar [label] = [label]
         writefile 'rblng.txt', [tab] # [tab] # [tab] # [tab] # 'ModifyMenu(GetMenu(hMain), IDM_' # [label] # ' | MF_STRING, IDM_' # [label] # ', text);'
         writefile 'rblng.txt', [tab] # [tab] # [tab] # '}'
      endif
    endif
  endif
until [line] = 'EOF'
echo 'Ok'
end