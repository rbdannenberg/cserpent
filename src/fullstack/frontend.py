from pptree import print_tree
from lexer import Lexer
from parser import Parser

class Frontend:
    def __init__(self):
        self.lexer = Lexer()
        self.parser = Parser()

    def add_user_type(self, name):
        self.lexer.add_user_type(name)

    def parse(self, code_line):
        token_list = Lexer(code_line).tokenize()
        return Parser(token_list).parse_line()

if __name__ == "__main__":
    code_line = 'x.attr(5)'
    token_list = Lexer(code_line).tokenize()
    print(token_list)
    ast = Parser(token_list).parse_line()
    print(ast)
    print_tree(ast)

