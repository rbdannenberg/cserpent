//
// Created by anthony on 6/5/24.
//
#include "any.h"
#include "gc.h"
#include "heap_obj.h"
#include "obj.h"
#include "csstring.h"
#include "array.h"
#include "dict.h"
#include "symbol.h"
#include "csmem.h"
#include "runtime.h"

void check_dispatch(Symbol *method, Array *args,
                    Dict *kwargs, size_t args_len, size_t kwargs_len) {
#ifdef DEBUG
    if (len(args) != args_len || kwargs_len != 0) { /// change when kwargs are implemented
        std::cout << "Dispatch error by method: " << method << std::endl;
    }
#endif
}

Obj::Obj() {
    // default tag is tag_object
}

Obj::Obj(Cs_class * class_ptr) {
    // default tag is tag_object
    set_slot(0, class_ptr);
}


uint64_t Obj::get_any_slots()
{
    Cs_class *cs_class = (Cs_class *) (slots[0].integer);
    return cs_class->get_inst_any_slots();
}


Cs_class *Obj::get_class_ptr() {
    return (Cs_class *) slots[0].integer;
}

void Obj::set_class_ptr(Cs_class * c_ptr) {
    set_slot(0, c_ptr);
}


Any Obj::call(Symbol *method, Array *args, Dict *kwargs) {
    Cs_class *cs_class = get_class_ptr();
    return std::invoke(cs_class->find_function(method), this, args, kwargs);
}


//Array *cs_symbols = nullptr;
Cs_class *cs_class_class = nullptr;
inline Any cs_class_get_name(Obj* self, Array *args,
                            Dict *kwargs) {
    check_dispatch(to_symbol(symbol_get_name), args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_name();
}

inline Any cs_class_get_inst_slot_count(Obj* self, Array *args,
                                        Dict *kwargs) {
    check_dispatch(to_symbol(symbol_get_inst_slot_count), args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_inst_slot_count();
}

inline Any cs_class_get_inst_any_slots(Obj* self, Array *args,
                                       Dict *kwargs) {
    check_dispatch(to_symbol(symbol_get_name), args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_inst_any_slots();
}

inline Any cs_class_get_member_table(Obj* self, Array *args,
                                     Dict *kwargs) {
    check_dispatch(to_symbol(symbol_get_name), args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_member_table();
}

MemberTable cs_class_table {
        {to_symbol(symbol_get_name), cs_class_get_name},
        {to_symbol(symbol_get_inst_slot_count), cs_class_get_inst_slot_count},
        {to_symbol(symbol_get_inst_any_slots), cs_class_get_inst_any_slots},
        {to_symbol(symbol_get_member_table), cs_class_get_member_table}
};

// Cserpent initialization - must be called to create runtime structures
void main_init()
{
    cs_class_class = new Cs_class { to_symbol(symbol_Class), 5, 0b1,
                                    &cs_class_table };
    // special case: fill in the class slot now to form a cycle:
    cs_class_class->set_slot(0, cs_class_class);
    cs_obj_class = new Cs_class { to_symbol(symbol_Obj), 1, 0b1,
                                  &cs_class_table};
}
