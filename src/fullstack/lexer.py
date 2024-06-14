from abstract_syntax import *
from frontend_utils import is_identifier, is_identifier_start
class Lexer:
    # The weird thing is that I'm basically using Lexer as a namespace.
    # code_line, tokens, current_token can all be local variables inside tokenize
    # the type sets are good to have as instance variables though, since that prevents re-initialization for each line.

    def __init__(self, file_path):
        self.code_line = ""
        self.tokens = []
        self.current_token = ""
        self.token_index = 0
        self.single_char_tokens = {'+', '-', '*', '/', '%', '&', '|', '^', '(', ')', '[', ']', '{', '}', ',', ':', ';', '.', '@', '=', '!', '<', '>', '~'}
        self.user_types = set()
        self.inbuilt_types = {"var", "int", "real", "string"}
        self.all_types = self.inbuilt_types.union(self.user_types)
        self.constructs = {"for", "if", "while", "class", "def", "return"}
        with open(file_path, 'r') as file:
            file_content = file.read()
            file_content = file_content.split("\n")
            file_content = [line for line in file_content if line != ""]
            self.lines = file_content
        self.current_line = 0

    def add_user_type(self, name):
        self.user_types.add(name)
        self.all_types.add(name)

    def handle_identifier(self):
        if self.current_token in self.all_types:
            self.tokens.append(Type(self.current_token))
        elif self.current_token in self.constructs:
            self.tokens.append(Construct(self.current_token))
        else:
            self.tokens.append(Identifier(self.current_token))

    def tokenize(self):
        if self.current_line >= len(self.lines):
            return None
        self.code_line = self.lines[self.current_line]
        self.current_line += 1
        self.tokens = []
        self.current_token = ""
        self.token_index = 0

        in_identifier = False
        in_string = False
        in_number = False
        indentation = 0
        while self.code_line[self.token_index] == ' ':
            self.token_index += 1
            indentation += 1
            if self.token_index >= len(self.code_line):
                return self.tokens
        self.tokens.append(Indentation(indentation))
        while self.token_index < len(self.code_line):
            char = self.code_line[self.token_index]
            if char == '#':
                break
            if in_identifier:
                if is_identifier(char):
                    self.current_token += char
                    self.token_index += 1
                else:
                    self.handle_identifier()
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
                    if char == "=":
                        assert self.token_index + 1 < len(self.code_line), "Error: expected another character after '='"
                        if self.code_line[self.token_index + 1] == "=":
                            self.tokens.append(OperatorToken("=="))
                            self.token_index += 2
                            continue
                    self.tokens.append(OperatorToken(char))
                    self.token_index += 1
                elif char == ' ':
                    self.token_index += 1
                else:
                    print("unknown character identified")
                    return self.tokens
        if in_identifier:
            self.handle_identifier()
        elif in_number:
            if "." in self.current_token:
                type = "real"
            else:
                type = "int"
            self.tokens.append(Literal(self.current_token, type))
        elif in_string:
            print("Error: expected closing quote.")

        if len(self.tokens) == 1: # only indentation token
            return self.tokenize()
        return self.tokens

