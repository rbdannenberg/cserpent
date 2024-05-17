from semantic_routines import Sem

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


