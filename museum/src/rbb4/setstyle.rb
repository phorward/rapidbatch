proc setClassColor: [class_name], [color]
  letregval 'String', 'CURRENT_USER', 'Software\jmksf\RapidBATCH', [class_name], [color]
endproc

func getClassColor: [class_name]
  getregval [val] = 'CURRENT_USER', 'Software\jmksf\RapidBATCH', [class_name]
  ret [val]
endfunc

[ListBox_Width] = '180'
listbox [style] = 'Please choose a style:', 'Standard|Grey|Ocean|Moonlight|Fire||Individual||Exit'
if [style] = '' halt

if [style] = 'Standard'
  setclasscolor 'CLASS_KEYWORD', '000000'
  setclasscolor 'CLASS_VALUE', '0000ff'
  setclasscolor 'CLASS_VARIABLE', '000080'
  setclasscolor 'CLASS_COMMENT', '808080'
  setclasscolor 'CLASS_SPECIAL_COMMENT', '008000'
  setclasscolor 'CLASS_LABEL', 'ff0000'
  setclasscolor 'CLASS_OPERATOR', '008000'
  setclasscolor 'CLASS_TEXT', '000000'
  setclasscolor 'CLASS_BKGND', 'ffffff'
elseif [style] = 'Grey'
  setclasscolor 'CLASS_KEYWORD', 'ffffff'
  setclasscolor 'CLASS_VALUE', 'f3f3f3'
  setclasscolor 'CLASS_VARIABLE', 'e0e0e0'
  setclasscolor 'CLASS_COMMENT', '8B8B8B'
  setclasscolor 'CLASS_SPECIAL_COMMENT', 'ffff00'
  setclasscolor 'CLASS_LABEL', '00ff00'
  setclasscolor 'CLASS_OPERATOR', 'adadad'
  setclasscolor 'CLASS_TEXT', 'ffffff'
  setclasscolor 'CLASS_BKGND', '2f2f2f'
elseif [style] = 'Ocean'
  setclasscolor 'CLASS_KEYWORD', 'ffffff'
  setclasscolor 'CLASS_VALUE', '7D8DFF'
  setclasscolor 'CLASS_VARIABLE', '00b0ff'
  setclasscolor 'CLASS_COMMENT', '00a5ff'
  setclasscolor 'CLASS_SPECIAL_COMMENT', '00ff00'
  setclasscolor 'CLASS_LABEL', 'ffff00'
  setclasscolor 'CLASS_OPERATOR', '00bd00'
  setclasscolor 'CLASS_TEXT', '008ED2'
  setclasscolor 'CLASS_BKGND', '000000'
elseif [style] = 'Moonlight'
  setclasscolor 'CLASS_KEYWORD', 'e5e5e5'
  setclasscolor 'CLASS_VALUE', '00C6FF'
  setclasscolor 'CLASS_VARIABLE', '0084FF'
  setclasscolor 'CLASS_COMMENT', '00ff00'
  setclasscolor 'CLASS_SPECIAL_COMMENT', '00ff00'
  setclasscolor 'CLASS_LABEL', 'ffff00'
  setclasscolor 'CLASS_OPERATOR', '00bd00'
  setclasscolor 'CLASS_TEXT', 'ffffff'
  setclasscolor 'CLASS_BKGND', '000000'
elseif [style] = 'Fire'
  setclasscolor 'CLASS_KEYWORD', 'ff3900'
  setclasscolor 'CLASS_VALUE', 'ffff00'
  setclasscolor 'CLASS_VARIABLE', 'ff7b00'
  setclasscolor 'CLASS_COMMENT', '00ff00'
  setclasscolor 'CLASS_SPECIAL_COMMENT', '00ff00'
  setclasscolor 'CLASS_LABEL', 'ffff00'
  setclasscolor 'CLASS_OPERATOR', 'bd0000'
  setclasscolor 'CLASS_TEXT', 'A52121'
  setclasscolor 'CLASS_BKGND', '000000'
elseif [style] = 'Individual'
  getclasscolor [pre] = 'CLASS_KEYWORD'
  inputbox [new_value] = 'CLASS_KEYWORD', 'Enter hexadecimal color-value for CLASS_KEYWORD', [pre]
  setclasscolor 'CLASS_KEYWORD', [pre]

  getclasscolor [pre] = 'CLASS_VALUE'
  inputbox [new_value] = 'CLASS_VALUE', 'Enter hexadecimal color-value for CLASS_VALUE', [pre]
  setclasscolor 'CLASS_VALUE', [new_value]

  getclasscolor [pre] = 'CLASS_VARIABLE'
  inputbox [new_value] = 'CLASS_VARIABLE', 'Enter hexadecimal color-value for CLASS_VARIABLE', [pre]
  setclasscolor 'CLASS_VARIABLE', [new_value]

  getclasscolor [pre] = 'CLASS_COMMENT'
  inputbox [new_value] = 'CLASS_COMMENT', 'Enter hexadecimal color-value for CLASS_COMMENT', [pre]
  setclasscolor 'CLASS_COMMENT', [new_value]

  getclasscolor [pre] = 'CLASS_SPECIAL_COMMENT'
  inputbox [new_value] = 'CLASS_SPECIAL_COMMENT', 'Enter hexadecimal color-value for CLASS_SPECIAL_COMMENT', [pre]
  setclasscolor 'CLASS_SPECIAL_COMMENT', [new_value]

  getclasscolor [pre] = 'CLASS_LABEL'
  inputbox [new_value] = 'CLASS_LABEL', 'Enter hexadecimal color-value for CLASS_LABEL', [pre]
  setclasscolor 'CLASS_LABEL', [new_value]

  getclasscolor [pre] = 'CLASS_OPERATOR'
  inputbox [new_value] = 'CLASS_OPERATOR', 'Enter hexadecimal color-value for CLASS_OPERATOR', [pre]
  setclasscolor 'CLASS_OPERATOR', [new_value]

  getclasscolor [pre] = 'CLASS_TEXT'
  inputbox [new_value] = 'CLASS_TEXT', 'Enter hexadecimal color-value for CLASS_TEXT', [pre]
  setclasscolor 'CLASS_TEXT', [new_value]

  getclasscolor [pre] = 'CLASS_BKGND'
  inputbox [new_value] = 'CLASS_BKGND', 'Enter hexadecimal color-value for CLASS_BKGND', [pre]
  setclasscolor 'CLASS_BKGND', [new_value]
else
  halt
endif


echo 'Done. Information has been submitted. Please restart the RapidBATCH Builder.'
end
