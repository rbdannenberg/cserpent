//
// Created by anthony on 6/5/24.
//
#include "obj.h"
#include "data_structures/csstring.h"
#include "data_structures/array.h"
#include "data_structures/symbol.h"
#include "csmem.h"

/* TODO: dynamic method calls
void check_dispatch(const std::string& method, const Array& args, const Dictionary& kwargs, size_t args_len, size_t kwargs_len) {
    if (len(args) != args_len || kwargs_len != 0) { /// change when kwargs are implemented
        std::cout << "Dispatch error by method: " << method << std::endl;
    }
}
*/

Cs_class *Cs_class_class = nullptr;
uint64_t Obj::get_any_slots()
{
    Cs_class *cs_class = (Cs_class *) (slots[0].integer);
    return cs_class->get_inst_any_slots();
}


//Obj::Obj(std::string name) : class_name {std::move(name)} {}

// Cserpent initialization - must be called to create runtime structures
void csobj_init()
{
    Cs_class_class = new Cs_class(new Symbol(new String("Class"), nullptr),
                                  4, 3); // 4 slots, first 2 are type Any
    // the class of Cs_class_class is Cs_class_class!
    Cs_class_class->set_slot(0, Cs_class_class);
}
