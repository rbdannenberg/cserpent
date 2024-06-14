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
# // post_inner := "." id [ "(" arguments ] post_inner |
# //               "[" expr "]" post_inner | ## indexing
# //               "(" arguments post_inner | empty ## method call
# // arguments := [ [ id "=" ] expr  { "," [ id "=" ] expr }* ] ")"  ## key word arguments

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



# x[5](3, 4) <- when is this a valid expression?
"""
def matmul(a, b):

        var n = len(a) # Assignment(Identifier('n'), FunctionCall(Identifier('len'), [Identifier('a')]))
        var m = len(b[0]) # Assignment(Identifier('m'), FunctionCall(Identifier('len'), [Index(Identifier('b'), Constant(0, 'int'))]))
        var p = len(a[0])

        var c = []
"""
# does Serpent have multiple inheritance? If so, consider setting OperatorToken, Identifier and Literal to inherit from Token
class OperatorToken:
    def __init__(self, value):
        self.value = value
        self.token_class = value

    def __repr__(self):
        return self.value + " : token"

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

class Identifier(Expression):
    def __init__(self, name):
        super().__init__()
        self.name = name # a string
        self.token_class = "identifier"
        # context?

    def __repr__(self):
        return self.name + " : " + self.token_class

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
        self.name = name
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


class Statement:
    def __repr(self):
        return self.__class__.__name__

class Assignment(Statement):
    def __init__(self, identifier, expression):
        self.identifier = identifier
        self.expression = expression
        self.children = [identifier, expression]

class StandaloneExpression(Statement):
    def __init__(self, expression):
        self.expression = expression
        self.children = [expression]

class Declaration(Statement):
    def __init__(self, type, identifier):
        self.type = type
        self.identifier = identifier
        self.children = [type, identifier]

class DeclarationNAssignment(Statement):
    def __init__(self, type, identifier, expression):
        self.type = type
        self.identifier = identifier
        self.expression = expression
        self.children = [type, identifier, expression]