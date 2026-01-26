#include "cserpent.h"

#include "easyglobal.h"
#include "global_vars.h"
int main() {
    runtime_init();  // Initialize runtime (needed for symbol_t and boolean support)

    cs_global_init();

    global_a = 1;
    global_b = 2;
    std::cout << (global_a + global_b) << std::endl;
    return 0;
}
// mark any global references to heap objects
void gc_mark_roots()
{
runtime_mark_roots();
}
// other globals should be marked here:
