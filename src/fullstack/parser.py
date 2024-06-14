from abstract_syntax import *
from lexer import Lexer
from pptree import print_tree

class Parser:
    def __init__(self, lexer):
        self.token_list = []
        self.current_token = None
        self.accepted_token = None # for when we need to obtain the identifier/constant
        self.token_index = 0
        self.lexer = lexer
        self.indentation = 0
        self.parsing_completed = False

        # helper sets
        self.constructs = lexer.constructs
        self.control_flow = {"if", "while", "for"}


    """First call sets self.current_token to the first token, subsequent calls does the same on the following tokens."""
    """Should only be called by lex_new_line and accept."""
    def get_token(self):
        if self.token_index > len(self.token_list):
            print("Error: no more tokens.")
            return
        if self.token_index == len(self.token_list):
            self.current_token = None
            # print("Line finished.")
            return
        self.current_token = self.token_list[self.token_index]
        self.token_index += 1

    # result: self.token_list is the next line of tokens,
    # self.current_token is the first (non-indentation) token of the next line.
    def lex_new_line(self):
        self.token_list = self.lexer.tokenize()
        if not self.token_list:
            print("END OF FIIE") # need to decide what to do with this when parsing program
            return False
        # print("New set of tokens: " + str(self.token_list))
        self.current_token = None
        self.accepted_token = None
        self.token_index = 0
        self.get_token()
        assert self.current_token.token_class == "indentation"
        self.indentation = self.current_token.value
        self.get_token()
        return True

    # token classes are "identifier", "literal" and the operators e.g. +, (, =, etc.
    # returns a boolean on whether the token was accepted.
    def accept(self, token_class):
        if self.current_token and self.current_token.token_class == token_class:
            self.accepted_token = self.current_token
            self.get_token()
            return True
        return False

    def accept_multiple(self, set):
        if self.current_token and self.current_token.token_class in set:
            self.accepted_token = self.current_token
            self.get_token()
            return True
        return False

    def expect(self, token_class):
        if self.accept(token_class):
            return True
        print(f"Error: expected {token_class} but got {self.current_token.token_class}")
        return False

    def expect_end(self):
        # should be equivalent to self.current_token is None.
        assert self.token_index == len(self.token_list), "Error: extraneous tokens at the end."

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
        elif self.accept("type"):
            type = self.accepted_token
            self.expect("(")
            arguments = self.arguments()
            return Constructor(type, arguments)
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

    def statement(self):
        res = None
        if self.accept("type"):
            type = self.accepted_token
            self.expect("identifier")
            name = self.accepted_token
            if self.accept("="):
                res = DeclarationNAssignment(type, name, self.expression())
            else:
                res = Declaration(type, name)
        else:
            if self.accept("return"):
                res = ReturnStatement(self.expression())
            else:
                expression = self.expression()
                if self.accept("="):
                    res = Assignment(expression, self.expression())
                else:
                    res = StandaloneExpression(expression)
        self.expect_end()
        if not self.lex_new_line():
            self.parsing_completed = False
        return res

# Starts from first line of the block, returns control at the last line of the block. The alternative doesn't work.
    def block(self, outer_indentation):
        elements = []
        while self.indentation != outer_indentation and not self.parsing_completed:
            assert(self.indentation > outer_indentation), "Error: indentation error in block."
            if self.accept("if"):
                elements.append(self.parse_if())
            elif self.accept("while"):
                elements.append(self.parse_while())
            elif self.accept("for"):
                elements.append(self.parse_for())
            else:
                elements.append(self.statement())
        if not elements:
            print("Error: block cannot be empty.")
            return None
        return Block(elements)

    # returns an If. Should consume every line of the if-else block, leaving token_list to be the fresh line after the block.
    def parse_if(self):
        condition = self.expression()
        self.expect(":")
        outer_indentation = self.indentation
        self.lex_new_line()
        if_block = self.block(outer_indentation)
        print(self.token_list)
        if not self.lex_new_line():
            self.parsing_completed = True
            return If(condition, if_block)
        if self.accept("else"):
            self.expect(":")
            outer_indentation = self.indentation
            self.lex_new_line()
            else_block = self.block(outer_indentation)
            if not self.lex_new_line():
                self.parsing_completed = True
            return If(condition, if_block, else_block)
        return If(condition, if_block)

    def parse_while(self):
        condition = self.expression()
        self.expect(":")
        outer_indentation = self.indentation
        self.lex_new_line()
        block = self.block(outer_indentation)
        if not self.lex_new_line():
            self.parsing_completed = True
        return While(condition, block)

    def parse_for(self):
        self.expect("identifier")
        name = self.accepted_token
        self.expect("=")
        start = self.expression()
        self.expect("to")
        end = self.expression()
        self.expect(":")
        outer_indentation = self.indentation
        self.lex_new_line()
        block = self.block(outer_indentation)
        if not self.lex_new_line():
            self.parsing_completed = True
        return For(name, start, end, block)

    def parse_function(self):
        self.expect("identifier")
        name = self.accepted_token
        self.expect("(")
        arguments = self.arguments()
        self.expect(":")
        outer_indentation = self.indentation
        self.lex_new_line()
        block = self.block(outer_indentation)
        if not self.lex_new_line():
            self.parsing_completed = True
        return Function(name, arguments, block)

    def parse_class(self):
        self.expect("identifier")
        class_name = self.accepted_token
        self.expect(":")
        outer_indentation = self.indentation
        self.lex_new_line()
        # this is different because we can only have member variables and functions, not control flow or assignments
        member_variables = []
        member_functions = []
        while self.indentation != outer_indentation and not self.parsing_completed:
            assert(self.indentation > outer_indentation), "Error: indentation error in class."
            if self.accept("type"):
                type = self.accepted_token
                self.expect("identifier")
                name = self.accepted_token
                member_variables.append(Declaration(type, name))
                if not self.lex_new_line():
                    self.parsing_completed = True
            elif self.accept("def"):
                member_functions.append(self.parse_function())
            else:
                print("Error: class can only have member variables and functions.")
                return None
        if not member_variables and not member_functions:
            print("Error: empty class block")
            return None
        self.lexer.add_user_type(class_name.value)
        return Class(class_name, MemberVariables(member_variables), MemberFunctions(member_functions))

    def program(self):
        elements = []
        self.lex_new_line()
        # this is multiline
        while not self.parsing_completed:
            if self.accept("if"):
                elements.append(self.parse_if())
            elif self.accept("while"):
                elements.append(self.parse_while())
            elif self.accept("for"):
                elements.append(self.parse_for())
            elif self.accept("class"):
                elements.append(self.parse_class())
            elif self.accept("def"):
                elements.append(self.parse_function())
            else:
                elements.append(self.statement())
        return Program(elements)


    # TODO: check why I don't need to assert the token list is empty at the end of parsing.
    # TODO: multiple post_inners
    # TODO: list comprehension
    # TODO: int()
    # TODO: differentiating constructors with function calls
    # TODO: fix mutual recursion newline shenanigans

if __name__ == "__main__":
    lexer = Lexer("test_files/test1.srp")
    parser = Parser(lexer)
    final_ast = parser.program()
    print_tree(final_ast, nameattr='n', last="leftright")















