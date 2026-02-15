#include "cserpent.h"
#include "easyglobal.h"
#include "global_vars.h"

int easyglobal_main() {
    set_any_global(&csg_a, 1);
    set_any_global(&csg_b, 2);
    std::cout << (csg_a + csg_b) << std::endl;
    return 0;
}
