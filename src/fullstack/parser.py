from abstract_syntax import *
from pptree import print_tree

"""
A function that takes a string of Python code and returns an AST. Involves lexing and parsing. Define helper functions
if needed.
"""
def is_identifier_start (char):
    return char.isalpha() or char == '_'

def is_identifier(char):
    return char.isalnum() or char == '_'

class Lexer:
    def __init__(self, code_line):
        self.code_line = code_line
        self.tokens = []
        self.current_token = ""
        self.token_index = 0
        self.single_char_tokens = {'+', '-', '*', '/', '%', '&', '|', '^', '(', ')', '[', ']', '{', '}', ',', ':', ';', '.', '@', '=', '!', '<', '>', '~'}


    def tokenize(self):
        in_identifier = False
        in_string = False
        in_number = False
        while self.token_index < len(self.code_line):
            char = self.code_line[self.token_index]
            if in_identifier:
                if is_identifier(char):
                    self.current_token += char
                    self.token_index += 1
                else:
                    self.tokens.append(Identifier(self.current_token))
                    self.current_token = ""
                    in_identifier = False
            elif in_string:
                if char == '"':
                    self.tokens.append(Literal(self.current_token, "string"))
                    self.current_token = ""
                    in_string = False
                else:
                    self.current_token += char
                self.token_index += 1
            elif in_number:
                if char.isdigit() or char == '.':
                    self.current_token += char
                    self.token_index += 1
                else:
                    if "." in self.current_token:
                        type = "real"
                    else:
                        type = "int"
                    self.tokens.append(Literal(self.current_token, type))
                    self.current_token = ""
                    in_number = False
            else:
                if char == '"':
                    in_string = True
                    self.token_index += 1
                elif char.isdigit():
                    in_number = True
                    self.current_token += char
                    self.token_index += 1
                elif is_identifier_start(char):
                    in_identifier = True
                    self.current_token += char
                    self.token_index += 1
                elif char in self.single_char_tokens:
                    self.tokens.append(OperatorToken(char))
                    self.token_index += 1
                elif char == ' ':
                    self.token_index += 1
                else:
                    print("unknown character identified")
                    return self.tokens
        if in_identifier:
            self.tokens.append(Identifier(self.current_token))
        elif in_number:
            if "." in self.current_token:
                type = "real"
            else:
                type = "int"
            self.tokens.append(Literal(self.current_token, type))
        elif in_string:
            print("Error: expected closing quote.")
        return self.tokens


# // expression := sum [ relop sum ]
# // sum := product { (+,-,|,^,<<,>>) product }*
# // product := term { "*,/,%,&" term }*
# // term := "-" term | "+" term | "not" term | "~" term | inner
# // inner := long | double | string | "(" expression ")" | id post_inner | symbol |
# //               array post_inner | dictionary post_inner
# // array := "[" [ expression { "," expression }* ] "]"
# // dictionary := "{" [ expression ":" expression { "," expression }* ] "}"
# // post_inner = empty

class Parser:
    def __init__(self, token_list):
        self.token_list = token_list
        self.current_token = None
        self.prev_token = None # for when we need to obtain the identifier/constant
        self.token_index = 0

    """First call sets self.current_token to the first token, subsequent calls does the same on the following tokens."""
    def get_token(self):
        if self.token_index > len(self.token_list):
            print("Error: no more tokens.")
            return
        if self.token_index == len(self.token_list):
            self.current_token = None
            return
        self.current_token = self.token_list[self.token_index]
        self.token_index += 1

    #
    def accept(self, token_class):
        if self.current_token and self.current_token.token_class == token_class:
            self.prev_token = self.current_token
            self.get_token()
            return True
        return False

    def expect(self, token_class):
        if self.accept(token_class):
            return True
        print(f"Error: expected {token_class} but got {self.current_token.token_class}")
        return False

    def array(self):
        elements = []
        if self.accept("]"):
            return Array(elements)
        elements.append(self.expression())
        while not self.accept("]"):
            self.expect(",")
            elements.append(self.expression())
        return Array(elements)

    def dictionary(self):
        elements = []
        if self.accept("}"):
            return Dictionary(elements)
        key = self.expression()
        self.expect(":")
        value = self.expression()
        elements.append((key, value))
        while not self.accept("}"):
            self.expect(",")
            key = self.expression()
            self.expect(":")
            value = self.expression()
            elements.append((key, value))
        return Dictionary(elements)

    def inner(self):
        if self.accept("literal") or self.accept("identifier"):
            return self.prev_token
        elif self.accept("("):
            expression = self.expression()
            self.expect(")")
            return expression
        elif self.accept("["):
            return self.array()
        elif self.accept("{"):
            return self.dictionary()
        else:
            print("Error: expected inner expression.")
            return None

    def term(self):
        if self.accept("-") or self.accept("+") or self.accept("not") or self.accept("~"):
            return UnaryOp(self.prev_token, self.term())
        else:
            return self.inner()

    def product(self):
        left = self.term()
        while self.accept("*") or self.accept("/") or self.accept("%") or self.accept("&"):
            left = BinOp(left, self.prev_token, self.term())
        return left

    def sum(self):
        left = self.product()
        while self.accept("+") or self.accept("-") or self.accept("|") or self.accept("^") or self.accept("<<") or self.accept(">>"):
            left = BinOp(left, self.prev_token, self.product())
        return left

    def expression(self):
        left = self.sum()
        if self.accept("<") or self.accept(">") or self.accept("==") or self.accept("!=") or self.accept("<=") or self.accept(">="):
            return BoolOp(left, self.prev_token, self.sum())
        return left

    def parse(self):
        self.get_token()
        return self.expression()

if __name__ == "__main__":
    code_line = '[3, 5] + "hello" + 2.5'
    token_list = Lexer(code_line).tokenize()
    print(token_list)
    ast = Parser(token_list).parse()
    print_tree(ast)

