//
// Created by anthony on 6/5/24.
//
#include "obj.h"
#include "data_structures/csstring.h"
#include "data_structures/array.h"
#include "data_structures/symbol.h"
#include "csmem.h"

void check_dispatch(const Symbol& method, const Array& args, const Dictionary& kwargs, size_t args_len, size_t kwargs_len) {
#ifdef DEBUG
    if (len(args) != args_len || kwargs_len != 0) { /// change when kwargs are implemented
        std::cout << "Dispatch error by method: " << method << std::endl;
    }
#endif
}

Obj::Obj() {
    set_tag(tag_object);
}

Obj::Obj(Cs_class * class_ptr) {
    set_slot(0, class_ptr);
    set_tag(tag_object);
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

Any Obj::call(const Symbol& method, const Array &args, const Dictionary &kwargs) {
    Cs_class *cs_class = get_class_ptr();
    return std::invoke(cs_class->find_function(method), this, args, kwargs);
}

//Array *cs_symbols = nullptr;
Cs_class *Cs_class_class = nullptr;
inline Any Cs_class_get_name(Obj* self, const Array& args, const Dictionary& kwargs) {
    check_dispatch("get_name", args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_name();
}
inline Any Cs_class_get_inst_slot_count(Obj* self, const Array& args, const Dictionary& kwargs) {
    check_dispatch("get_inst_slot_count", args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_inst_slot_count();
}
inline Any Cs_class_get_inst_any_slots(Obj* self, const Array& args, const Dictionary& kwargs) {
    check_dispatch("get_name", args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_inst_any_slots();
}
inline Any Cs_class_get_member_table(Obj* self, const Array& args, const Dictionary& kwargs) {
    check_dispatch("get_name", args, kwargs, 0, 0);
    return static_cast<Cs_class*>(self)->get_member_table();
}
MemberTable Cs_class_table {
        {Symbol{"get_name"}, Cs_class_get_name},
        {Symbol{"get_inst_slot_count"}, Cs_class_get_inst_slot_count},
        {Symbol{"get_inst_any_slots"}, Cs_class_get_inst_any_slots},
        {Symbol{"get_member_table"}, Cs_class_get_member_table}
};

// Cserpent initialization - must be called to create runtime structures
void main_init()
{
    Cs_class_class = new Cs_class {Symbol {"Class"}, 5, 0b10001, &Cs_class_table};
}
