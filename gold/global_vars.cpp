#include "cserpent.h"

#include "easyglobal.h"
#include "global_vars.h"
Any global_b;
Any global_c;
Any global_a;
int cs_global_init() {
    // Initialize global variables here
    define_global("a", &global_a, CS_ANY);
    define_global("b", &global_b, CS_ANY);
    define_global("c", &global_c, CS_ANY);
    return 0;
}
