
##### EXPRESSION ######
expression := sum [ relop sum ]
sum := product { (+,-,|,^,<<,>>) product }*
product := term { "*,/,%,&" term }*
term := "-" term | "+" term | "not" term | "~" term | inner
inner := literal | "(" expression ")" | id post_inner |
"[" array post_inner | "{" dictionary post_inner | type "(" arguments post_inner
literal := integer | real | string | symbol
array := [ expression { "," expression }* ] "]"
dictionary := [ expression ":" expression { "," expression }* ] "}"
post_inner := "." id [ "(" arguments ] post_inner |
"[" expr "]" post_inner | ## indexing
"(" arguments post_inner | empty ## method call
arguments := [ [ id "=" ] expr  { "," [ id "=" ] expr }* ] ")"  ## key word arguments

##### STATEMENT ######
statement := [ type ] identifier "=" expression | type identifier | expression
statement := type identfier [ "=" expression ] | expression [ "=" expression ] | "return" expression
d
##### MULTI-LINE ###### 
**Note**: from here, we move on from single-line parsing.
**Note**: need to be an indentation level deeper than the previous line.

class := "class" identifier ":" { type identifier | function }*
function := "def" identifier "(" arguments ")" ":" block
if := "if" expression ":" block [ "else" ":" block ]
while := "while" expression ":" block
for := "for" identifier "=" expression "to" expression block
block := { statement | control_flow }*
control_flow := if | while | for
program := { class | function | statement | control_flow }*


