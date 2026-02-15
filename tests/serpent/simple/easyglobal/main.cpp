#include "cserpent.h"
#include "global_vars.h"

int easyglobal_main();


int main()
{
    runtime_init();
    cs_global_init();

    easyglobal_main();

    return 0;
}

