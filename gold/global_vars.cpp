#include "cserpent.h"
#include "symbol.h"
#include "easyglobal.h"
#include "global_vars.h"

Any global_b;
Any global_c;
Any global_a;
int cs_global_init() {
    // Initialize global variables here
    global_a = new Symbol("a");
    global_b = new Symbol("b");
    global_c = new Symbol("c");
    return 0;
}
