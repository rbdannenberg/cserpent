from collections import defaultdict

"""Class that provides semantic routines to translate type-annotated Python code into C++ code"""
class Sem():
    def __init__(self, maximum_depth=4):
        self.symbol_table = {}
        self.current_scope = 'global'
        self.current_function = None
        self.current_class = None
        self.current_method = None
        self.current_type = None
        self.current_variable = None
        self.current_parameter = None
        self.current_return = None
        self.current_break = None
        self.current_continue = None

        self.file_code = ""
        self.function_code_lines = []
        self.list_generator = lambda: [None] * maximum_depth
        self.function_variables = defaultdict(self.list_generator)
        self.block_depth = 0

    def add_to_symbol_table(self, name, value):
        self.symbol_table[name] = value

    # Returns the expression with the numbers to C++ format
    # def substitute_numbers(self, expression):

    def function_begin(self, name, parameters):
        self.current_scope = "function"
        if self.function_code_lines:
            raise Exception("Function not ended")
        self.function_code_lines.append("void ")
        line = " " + name + "("
        for parameter in parameters:
            line += parameter[0] + " " + parameter[1] + ","
            self.function_variables[parameter[1]][self.block_depth] = parameter[0]
        line = line[:-1] + ") {\n"
        self.function_code_lines.append(line)

    # this will be recursive
    def infer_type(self, expression):
        # if expression is a literal, return its type
        if expression.isdigit():
            return "int"
        elif expression.replace(".", "", 1).isdigit():
            return "real"
        elif expression[0] == '"' and expression[-1] == '"':
            return "string"
        # if expression is a local variable, return its type
        elif expression in self.function_variables:
            return self.function_variables[expression][self.block_depth]
        # if expression is a global variable, return its type
        elif expression in self.symbol_table:
            return self.symbol_table[expression]
        # if expression is a function call, return its return type
        else:
            return "Any"


    def declare_local(self, variable, type):
        line = type + " " + variable + ";\n"
        self.function_code_lines.append(line)
        self.function_variables[variable][self.block_depth] = type

    # precondition: expression is a valid C++ expression
    def assign_local(self, variable, expression):
        if variable == "":
            self.function_code_lines.append(expression + ";\n")
        else:
            if "." in variable:
                variable_check = variable.split(".")[0]
            elif "[" in variable:
                variable_check = variable.split("[")[0]
            else:
                variable_check = variable
            if variable_check not in self.function_variables:
                raise Exception("Variable " + variable + " not declared")
            type = self.infer_type(expression)
            line = variable + " = " + expression + ";\n"
            self.function_code_lines.append(line)
            self.function_variables[variable][self.block_depth] = type

    def declare_and_assign_local(self, variable, type, expression):
        # if type == "Any":
        #     type = self.infer_type(expression)
        self.function_code_lines.append(type + " ")
        self.function_variables[variable][self.block_depth] = type
        self.assign_local(variable, expression)

    def return_statement(self, expression):
        line = "return " + expression + ";\n"
        self.function_code_lines.append(line)
        self.function_code_lines[0] = self.infer_type(expression)

    def function_end(self):
        self.current_scope = "global"
        self.function_code_lines.append("}\n")
        self.file_code += "".join(self.function_code_lines)
        self.function_code_lines = []
        self.function_variables = defaultdict(self.list_generator)

    def if_begin(self, boolean_expression):
        if self.current_scope == "function":
            line = "if (" + boolean_expression + ") {\n"
            self.function_code_lines.append(line)
            self.block_depth += 1
        else:
            raise Exception("Unimplemented")

    def for_begin(self, variable, start, end, by="1"):
        if self.current_scope == "function":
            line = "for (int64_t " + variable + " = " + start + "; " + variable + " < " + end + "; " + variable + " += " + by + ") {\n"
            self.function_code_lines.append(line)
            self.block_depth += 1
        else:
            raise Exception("Unimplemented")

    def for_each_begin(self, variable, iterable):
        if self.current_scope == "function":
            line = "for (auto " + variable + " : " + iterable + ") {\n"
            self.function_code_lines.append(line)
            self.block_depth += 1
        else:
            raise Exception("Unimplemented")

    def block_end(self):
        if self.current_scope == "function":
            self.block_depth -= 1
            self.function_code_lines.append("}\n")
        else:
            raise Exception("Unimplemented")

    # ensures an identifier (a string) is valid, else throws a compiler error
    def as_identifier(self, identifier):
        if not identifier[0].isalpha():
            raise Exception("Invalid identifier")
        return identifier


    def array_comprehension_old(self, token_list):
        line = ""
        if "for" in token_list:
            for_idx = token_list.index("for")
            if "to" in token_list:
                to_idx = token_list.index("to")
                if not token_list[for_idx + 2] == "=":
                    raise Exception("Comprehension error")
                variable = self.as_identifier(token_list[for_idx + 1])
                start = "".join(token_list[for_idx + 3:to_idx])
                end = "".join(token_list[to_idx + 1:])
                by = "1"
                if "by" in token_list:
                    raise Exception("Unimplemented")
            elif "in" in token_list:
                raise Exception("Unimplemented")
        return line

    def array_comprehension(self, token_list):
        if "for" in token_list:
            for_idx = token_list.index("for")
            if "to" in token_list:
                to_idx = token_list.index("to")
                if not token_list[for_idx + 2] == "=":
                    raise Exception("Comprehension error")
                value = "".join(token_list[1:for_idx])
                start = "".join(token_list[for_idx + 3:to_idx])
                end = "".join(token_list[to_idx + 1:-1])
                if "by" in token_list:
                    raise Exception("Unimplemented")
                return "Array {.value = " + value + ", .start = " + start + ", .end = " + end + "}"
            elif "in" in token_list:
                raise Exception("Unimplemented")

    # converts a parsed serpent expression to a C++ expression
    # probably recursive
    def create_expression(self, token_list):
        # if token_list is a literal, return it
        if len(token_list) == 1:
            return token_list[0]
        if token_list[0] == "[" and token_list[-1] == "]":
            if len(token_list) == 2:
                return "Array {}"
            elif "for" in token_list:
                return self.array_comprehension(token_list)
            # what if it's a list of function return values?
            return "Array {" + ", ".join([token for token in token_list[1:-1]]) + "}"
        # the rest

    def write_to_file(self, filename):
        with open(filename, "w") as file:
            file.write(self.file_code)

    def print_file(self):
        print(self.file_code)

