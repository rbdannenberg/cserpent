from abstract_syntax import *
from frontend_utils import is_identifier, is_identifier_start
class Lexer:
    def __init__(self, code_line):
        self.code_line = code_line
        self.tokens = []
        self.current_token = ""
        self.token_index = 0
        self.single_char_tokens = {'+', '-', '*', '/', '%', '&', '|', '^', '(', ')', '[', ']', '{', '}', ',', ':', ';', '.', '@', '=', '!', '<', '>', '~'}
        self.user_types = set()
        self.inbuilt_types = {"var", "int", "real", "string"}

    def add_user_type(self, name):
        self.user_types.add(name)

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

