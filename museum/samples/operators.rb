rem +-------------------------------------------------------------------+
rem | Operator Demo                                                     |
rem +-------------------------------------------------------------------+
rem | This script is a simple demonstration how the 6 RapidBATCH opera- |
rem | tors operate and behave on values.                                |
rem +-------------------------------------------------------------------+

inputbox [value1] = 'Value 1', 'Please enter the first value:', ''
if [value1] = '' halt

inputbox [value2] = 'Value 2', 'Please enter the second value:', ''
if [value2] = '' halt

[text] = 'With the values ' # [value1] # ' and ' # [value2]
[text] # ' we' # [quot] # 're getting the following results:'
[text] # [crlf] # [crlf]

[text] # [value1] # ' + ' # [value2] # ' = ' # ([value1] + [value2]) # [crlf]
[text] # [value1] # ' - ' # [value2] # ' = ' # ([value1] - [value2]) # [crlf]
[text] # [value1] # ' * ' # [value2] # ' = ' # ([value1] * [value2]) # [crlf]
[text] # [value1] # ' / ' # [value2] # ' = ' # ([value1] / [value2]) # [crlf]
[text] # [value1] # ' ~ ' # [value2] # ' = ' # ([value1] ~ [value2]) # [crlf]
[text] # [value1] # ' # ' # [value2] # ' = ' # ([value1] # [value2])

echo [text]
end
