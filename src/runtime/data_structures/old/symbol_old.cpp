//
// Created by anthony on 6/4/24.
//

#include "any.h"
#include "gc.h"
#include "obj.h"
#include "symbol_old.h"
#include "array.h"

Array *cs_symbols = nullptr;

void symbol_init()
{
    cs_symbols = new Array;
}

