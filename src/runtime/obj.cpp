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


Cs_class::Cs_class(Symbol *name, int64_t slot_count, int64_t any_slots,
                   MemberTable *table, Cs_class *parent)  :
            Obj {cs_class_class} {
    slots[1] = name;  // name is a symbol, so it is safe to just assign
    *(name->value()) = this;  // store this class as value of it's name
    SLOT(2).integer = slot_count;
    SLOT(3).integer = any_slots;
    if (parent != nullptr) {
        MemberTable parent_table_copy = *(parent->get_member_table());
        // since merge alters the argument, we use a temporary copy
        table->merge(std::move(parent_table_copy));
    }
    SLOT(4).integer = reinterpret_cast<int64_t>(table);
    SLOT(5).integer = reinterpret_cast<int64_t>(parent);
    // A: we could potentially copy the table wholesale, but that mucks
    // around with memory a bit too much for my liking. Get it right
    // first then attempt to refactor.
}

Obj::Obj() {
    // default tag is tag_object
}

Obj::Obj(Cs_class * class_ptr) {
    // default tag is tag_object
    slots[0] = class_ptr;
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
    check_dispatch(to_symbol(css_get_name), args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_name();
}

inline Any cs_class_get_inst_slot_count(Obj* self, Array *args,
                                        Dict *kwargs) {
    check_dispatch(to_symbol(css_get_inst_slot_count), args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_inst_slot_count();
}

inline Any cs_class_get_inst_any_slots(Obj* self, Array *args,
                                       Dict *kwargs) {
    check_dispatch(to_symbol(css_get_name), args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_inst_any_slots();
}

inline Any cs_class_get_member_table(Obj* self, Array *args,
                                     Dict *kwargs) {
    check_dispatch(to_symbol(css_get_name), args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_member_table();
}

MemberTable cs_class_table {
        {to_symbol(css_get_name), cs_class_get_name},
        {to_symbol(css_get_inst_slot_count), cs_class_get_inst_slot_count},
        {to_symbol(css_get_inst_any_slots), cs_class_get_inst_any_slots},
        {to_symbol(css_get_member_table), cs_class_get_member_table}
};

MemberTable cs_obj_table {
        // objects have no methods
};
