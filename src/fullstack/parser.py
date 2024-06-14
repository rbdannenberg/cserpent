from abstract_syntax import *

class Parser:
    def __init__(self, token_list):
        self.token_list = token_list
        self.current_token = None
        self.accepted_token = None # for when we need to obtain the identifier/constant
        self.token_index = 0

    """First call sets self.current_token to the first token, subsequent calls does the same on the following tokens."""
    def get_token(self):
        if self.token_index > len(self.token_list):
            print("Error: no more tokens.")
            return
        if self.token_index == len(self.token_list):
            self.current_token = None
            print("Line finished.")
            return
        self.current_token = self.token_list[self.token_index]
        self.token_index += 1

    # token classes are "identifier", "literal" and the operators e.g. +, (, =, etc.
    # returns a boolean on whether the token was accepted.
    def accept(self, token_class):
        if self.current_token and self.current_token.token_class == token_class:
            self.accepted_token = self.current_token
            self.get_token()
            return True
        return False

    def expect(self, token_class):
        if self.accept(token_class):
            return True
        print(f"Error: expected {token_class} but got {self.current_token.token_class}")
        return False

    # peeks beyond the current token. If the count is 0, it returns the current token (what is next to be accepted).
    def peek(self, count):
        if count == 0:
            return self.current_token
        if self.token_index + count - 1 >= len(self.token_list):
            return None
        return self.token_list[self.token_index + count - 1]

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

    def arguments(self):
        # assumes "(" has been accepted
        if self.accept(")"):
            return ArgumentList([], {})
        args = []
        kwargs = {}
        if (self.peek(0).token_class == "identifier" and self.peek(1).token_class == "="
                and self.peek(2).token_class != "="): # avoid something stupid like (a = b, c == d)
            self.accept("identifier")
            key = self.accepted_token
            self.expect("=")
            kwargs[key] = self.expression()
        else:
            args.append(self.expression())
        while not self.accept(")"):
            self.expect(",")
            if (self.peek(0).token_class == "identifier" and self.peek(1).token_class == "="
                    and self.peek(2).token_class != "="):
                self.accept("identifier")
                key = self.accepted_token
                self.expect("=")
                kwargs[key] = self.expression()
            else:
                args.append(self.expression())
        return ArgumentList(args, kwargs)

    def post_inner(self, expression):
        # TODO: see how this works with multiple post_ineers
        # TODO: inquire about accepting some illegal expressions like {"hello": 5}(5)
        if self.accept("("):
            arguments = self.arguments()
            return FunctionCall(expression, arguments)
        elif self.accept("."):
            self.expect("identifier")
            id = self.accepted_token
            if self.accept("("):
                arguments = self.arguments()
                return MethodCall(expression, id, arguments)
            return GetAttribute(expression, id)
        elif self.accept("["):
            index = self.expression()
            self.expect("]")
            return Index(expression, index)
        else:
            return expression

    def inner(self):
        if self.accept("literal"):
            return self.accepted_token
        elif self.accept("("):
            expression = self.expression()
            self.expect(")")
            return expression
        elif self.accept("identifier"):
            return self.post_inner(self.accepted_token)
        elif self.accept("["):
            array = self.array()
            return self.post_inner(array)
        elif self.accept("{"):
            dict = self.dictionary()
            return self.post_inner(dict)
        else:
            print("Error: expected inner expression.")
            return None


    def term(self):
        if self.accept("-") or self.accept("+") or self.accept("not") or self.accept("~"):
            return UnaryOp(self.accepted_token, self.term())
        else:
            return self.inner()

    def product(self):
        left = self.term()
        while self.accept("*") or self.accept("/") or self.accept("%") or self.accept("&"):
            left = BinOp(left, self.accepted_token, self.term())
        return left

    def sum(self):
        left = self.product()
        while self.accept("+") or self.accept("-") or self.accept("|") or self.accept("^") or self.accept("<<") or self.accept(">>"):
            left = BinOp(left, self.accepted_token, self.product())
        return left

    def expression(self):
        left = self.sum()
        if self.accept("<") or self.accept(">") or self.accept("==") or self.accept("!=") or self.accept("<=") or self.accept(">="):
            return BoolOp(left, self.accepted_token, self.sum())
        return left

    def parse_line(self):
        self.get_token()
        return self.expression()

