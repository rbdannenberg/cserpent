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

"""
def matmul(a, b):

        var n = len(a) # Assignment(Identifier('n'), FunctionCall(Identifier('len'), [Identifier('a')]))
        var m = len(b[0]) # Assignment(Identifier('m'), FunctionCall(Identifier('len'), [Index(Identifier('b'), Constant(0, 'int'))]))
        var p = len(a[0])

        var c = []
"""
class OperatorToken:
    def __init__(self, value):
        self.value = value
        self.token_class = "operator"

class Expression:
    pass

class Literal(Expression):
    def __init__(self, value, type):
        self.value = value
        self.type = type # int, real, string, bool
        self.token_class = "constant"

class BoolOp(Expression):
    def __init__(self, left, op, right):
        self.left = left # an expression
        self.op = op # and, or
        self.right = right # an expression

class BinOp(Expression):
    def __init__(self, left, op, right):
        self.left = left # an expression
        self.op = op # +, -, *, /, %, &, |, ^, <<, >>, **
        self.right = right # an expression

class UnaryOp(Expression):
    def __init__(self, op, value):
        self.op = op # +, -, not, ~
        self.value = value # an expression

class Identifier(Expression):
    def __init__(self, name):
        self.name = name # a string
        self.token_class = "identifier"
        # context?

class Array(Expression):
    def __init__(self, elements):
        self.elements = elements # a list of expressions

class Dictionary(Expression):
    def __init__(self, pairs):
        self.pairs = pairs # a list of key-value pairs

class FunctionCall(Expression):
    def __init__(self, name, parameters):
        self.function_name = name # an identifier
        self.parameters = parameters # a parameter list

class MethodCall(Expression):
    def __init__(self, object, method_name, parameters):
        self.object = object # an expression
        self.method_name = method_name # an identifier
        self.parameters = parameters # a

class Index(Expression):
    def __init__(self, object, index):
        self.object = object # an expression
        self.index = index # an expression

class ParamaterList(Expression):
    def __init__(self, params):
        self.params = params # a list of expressions. can be named or not

class Statement:
    pass

class Assignment(Statement):
    def __init__(self, identifier, expression):
        self.identifier = identifier
        self.expression = expression

class StandaloneExpression(Statement):
    def __init__(self, expression):
        self.expression = expression