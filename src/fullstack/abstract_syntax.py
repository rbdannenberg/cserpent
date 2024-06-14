### Reference grammar from compiler.cpp: ###
# // expr := disj [ conj ]
# // disj := conj [ disjunctions ]
# // disjunctions := "or" [ conj [ disjunctions ]]
# // conj := relation [ conjunctions ]
# // conjunctions := "and" [ relation [ conjunctions ]]
# // relation := sum [ relop sum ]
# // sum := prod [ addends]
# // addends := { (+,-,|,^,<<,>>) prod }*
# // prod := exp [ factors ]
# // factors := { "*,/,%,&" exp }*
# // exp := term [ exponents ]
# // exponents := { "**" term }*
# // term := "-" term | "+" term | "not" term | "~" term | inner # left recursion?
# // inner := "(" expr ")" | id post_inner | long | double | symbol |
# //               "[" literal_array post_inner | "{" literal_dict post_inner |
# //               string
# // literal_array := [ expr { "," expr }* ] "]"
# // literal_dict := [ expr ":" expr { "," expr }* ] "}"
# // post_inner := "." id [ "(" actuals ] post_inner |
# //               "[" expr "]" post_inner | ## indexing
# //               "(" actuals post_inner | empty ## method call
# // actuals := [ [ id "=" ] expr  { "," [ id "=" ] expr }* ] ")"  ## key word arguments

# @note this is the grammar for parsing! it is more complex than the abstract syntax.

###### EXPRESSION ######
# expression := sum [ relop sum ]
# sum := product { (+,-,|,^,<<,>>) product }*
# product := term { "*,/,%,&" term }*
# term := "-" term | "+" term | "not" term | "~" term | inner
# inner := literal | "(" expression ")" | id post_inner |
#               "[" array post_inner | "{" dictionary post_inner
# literal := integer | real | string | symbol
# array := [ expression { "," expression }* ] "]"
# dictionary := [ expression ":" expression { "," expression }* ] "}"
# post_inner := "." id [ "(" arguments ] post_inner |
#                "[" expr "]" post_inner | ## indexing
#                "(" arguments post_inner | empty ## method call
# arguments := [ [ id "=" ] expr  { "," [ id "=" ] expr }* ] ")"  ## key word arguments

###### STATEMENT ######
# statement := [ type ] identifier "=" expression | type identifier | expression
# statement := type identfier [ "=" expression ] | identifier "=" expression | expression

###### MULTI-LINE ###### @note from here, we move on from single-line parsing.
### Blocks need to be an indentation level deeper than the previous line.
# class := "class" identifier ":" block
# function := "def" identifier "(" arguments ")" ":" block
# if := "if" expression ":" block [ "else" ":" block ]
# while := "while" expression ":" block
# for := "for" identifier "=" expression "to" expression block
# block := { statement | control_flow }*
# control_flow := if | while | for
# program := { class | function | statement | control_flow }*

# This is hella messy. I think the best way is to separate the tokens and the abstract syntax (i.e. distinguish between
# the literal token and the literal abstract syntax.
class Token:
    def __init__(self, value):
        self.value = value
        self.token_class = None

# does Serpent have multiple inheritance? If so, consider setting OperatorToken, Identifier and Literal to inherit from Token
class OperatorToken(Token):
    def __init__(self, value):
        super().__init__(value)
        self.token_class = value

    def __repr__(self):
        return self.value + " : token"

# types fulfill the same role no matter the type, but each construct is unique, similiar to operator tokens.
class Type(Token):
    def __init__(self, value):
        self.value = value
        self.token_class = "type"
        self.children = []

    def __repr__(self):
        return self.value + " : type"

class Construct(Token):
    def __init__(self, value):
        super().__init__(value)
        self.token_class = value

class Expression:
    def __init__(self):
        # for printing using pptree
        self.children = []

    def __repr__(self):
        return self.__class__.__name__

class Literal(Expression):
    def __init__(self, value, type):
        super().__init__()
        self.value = value
        self.type = type # int, real, string, bool
        self.token_class = "literal"

    def __repr__(self):
        return self.value + " : " + self.type + " " + self.token_class

class Identifier(Expression):
    def __init__(self, value):
        super().__init__()
        self.value = value # a string
        self.token_class = "identifier"
        # context?

    def __repr__(self):
        return self.value + " : " + self.token_class
class BoolOp(Expression):
    def __init__(self, left, op, right):
        super().__init__()
        self.left = left # an expression
        self.op = op # and, or
        self.right = right # an expression
        self.children = [left, right]

    def __repr__(self):
        return f"BoolOp {self.op.value}"

class BinOp(Expression):
    def __init__(self, left, op, right):
        super().__init__()
        self.left = left # an expression
        self.op = op # +, -, *, /, %, &, |, ^, <<, >>, **
        self.right = right # an expression
        self.children = [left, right]

    def __repr__(self):
        return f"BinOp {self.op.value} "

class UnaryOp(Expression):
    def __init__(self, op, value):
        super().__init__()
        self.op = op # +, -, not, ~
        self.value = value # an expression


class Array(Expression):
    def __init__(self, elements):
        super().__init__()
        self.elements = elements # a list of expressions
        self.children = elements

    def __repr__(self):
        return "Array of " + str(len(self.elements)) + " elements"

class Dictionary(Expression):
    def __init__(self, pairs):
        super().__init__()
        self.pairs = pairs # a list of key-value pairs

    def __repr__(self):
        return "Dictionary of " + str(len(self.pairs)) + " pairs"

class FunctionCall(Expression):
    def __init__(self, name, parameters):
        super().__init__()
        self.function_name = name # an identifier
        self.parameters = parameters # a parameter list
        self.children = [name, parameters]

class MethodCall(Expression):
    def __init__(self, object, method_name, parameters):
        super().__init__()
        self.object = object # an expression
        self.method_name = method_name # an identifier
        self.parameters = parameters # a
        self.children = [object, method_name, parameters]

class GetAttribute(Expression):
    def __init__(self, object, attribute):
        super().__init__()
        self.object = object # an expression
        self.attribute = attribute # an identifier
        self.children = [object, attribute]

class Index(Expression):
    def __init__(self, object, index):
        super().__init__()
        self.object = object # an expression
        self.index = index # an expression
        self.children = [object, index]

class ArgumentList(Expression):
    def __init__(self, args, kwargs):
        super().__init__()
        self.args = args # a list of expressions. can be named or not
        self.kwargs = kwargs # a dictionary of named expressions. Key must either be an identifier.
        self.children = args + list(kwargs.items())


class BlockElement:
    def __repr__(self):
        return self.__class__.__name__

class Statement(BlockElement):

class Assignment(Statement):
    def __init__(self, identifier, expression):
        super().__init__()
        self.identifier = identifier
        self.expression = expression
        self.children = [identifier, expression]

class StandaloneExpression(Statement):
    def __init__(self, expression):
        super().__init__()
        self.expression = expression
        self.children = [expression]

class Declaration(Statement):
    def __init__(self, type, identifier):
        super().__init__()
        self.type = type
        self.identifier = identifier
        print(identifier)
        self.children = [type, identifier]

class DeclarationNAssignment(Statement):
    def __init__(self, type, identifier, expression):
        super().__init__()
        self.type = type
        self.identifier = identifier
        self.expression = expression
        self.children = [type, identifier, expression]

##### MULTI_LINE #####
class ControlFlow(BlockElement):
    pass

class If(ControlFlow):
    def __init__(self, condition, block, else_block=None):
        super().__init__()
        self.condition = condition # must be a boolean expression
        self.block = block
        self.else_block = else_block
        self.children = [condition, block]
        if else_block:
            self.children.append(else_block)

class While(ControlFlow):
    def __init__(self, condition, block):
        super().__init__()
        self.condition = condition # must be a boolean expression
        self.block = block
        self.children = [condition, block]

class For(ControlFlow):
    def __init__(self, identifier, start, end, block):
        super().__init__()
        self.identifier = identifier
        self.start = start
        self.end = end
        self.block = block
        self.children = [identifier, start, end, block]

class Function():
    def __init__(self, name, arguments, block):
        super().__init__()
        self.name = name
        self.arguments = arguments
        self.block = block
        self.children = [name, arguments, block]

class Class():
    def __init__(self, name, member_variables, member_functions):
        super().__init__()
        self.name = name
        self.member_variables = member_variables
        self.member_functions = member_functions
        self.children = [name, member_variables, member_functions]

class Program():
    def __init__(self, elements):
        self.elements = elements
        self.children = elements





