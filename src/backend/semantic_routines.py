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
        if variable not in self.function_variables:
            raise Exception("Variable " + variable + " not declared")
        type = self.infer_type(expression)
        line = variable + " = " + expression + ";\n"
        self.function_code_lines.append(line)
        self.function_variables[variable][self.block_depth] = type

    def declare_and_assign_local(self, variable, type, expression):
        if type == "Any":
            type = self.infer_type(expression)
        self.declare_local(variable, type)
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

    def write_to_file(self, filename):
        with open(filename, "w") as file:
            file.write(self.file_code)

    def print_file(self):
        print(self.file_code)

"""
def add(int x):  // typespec ("int") is optional
  int y = 6
  var z = x + y
  return z + 1.5
"""

def test():
    sem = Sem()
    sem.function_begin("add", [["int", "x"]])
    sem.declare_and_assign_local("y", "int", "6")
    sem.declare_and_assign_local("z", "Any", "x + y")
    sem.return_statement("z + 1.5")
    sem.function_end()
    sem.print_file()

if __name__ == "__main__":
    test()


