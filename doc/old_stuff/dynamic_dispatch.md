Because of garbage collection reasons, C++ objects will not have member variables. Instead,
these will be stored in a slots[] array, which store Any's. The backend will compile getter 
and setter functions for each member variable, which will access the slot[] array.

Thus, the only outward-facing members for classes will be functions. This will be accomplished
by the backend walking the AST and recursively transforming member variable access to 
member function calls.

If the object in question is an Any, there will be one further transforming - which is
transforming every member function call into a .call() function call that takes in 
args and kwargs as an Array and Dictionary.