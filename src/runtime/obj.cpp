//
// Created by anthony on 6/5/24.
//
#include "any.h"
#include "gc.h"
#include "header.h"
#include "heap_obj.h"
#include "obj.h"
#include "csstring.h"
#include "array.h"
#include "dict.h"
#include "symbol.h"
#include "csmem.h"
#include "runtime.h"


Array *pparms;
Dict *kparms;


void *Obj::operator new(size_t size)
{
    size += sizeof(Header);
    Obj *self = (Obj *) ((char *) csmalloc(size) + sizeof(Header));
    // printf("Obj::new allocates %p\n", self);
    return self;
}


void Obj::set_slot(int i, Any x)
{
    check(i >= 0 && i < get_slot_count());
    SLOT_WRITE_BLOCK(this, x);
    slots[i] = x;
}


// check_dispatch - check that there are the right number of args and kwargs
//
void check_dispatch(Symbol *method, size_t max_args_len, bool allow_kw)
{
    check_dispatch(get_c_str(method->name()), max_args_len, allow_kw);
}

void check_dispatch(const char *method, size_t max_args_len, bool allow_kw)
{
    // printf("check_dispatch pparms len %lld max %ld\n", pparms->len(),
    //        max_args_len);
    if (pparms->len() > max_args_len) {
        std::cout << "Dispatch error calling method: " <<
                     "too many (" << pparms->len() << " vs " << max_args_len <<
                     ") positional args in " << method << std::endl;
        assert(false);
        exit(1);
    }
    // printf("check_dispatch kparms allow %d keys %lld\n",
    //        allow_kw, kparms->num_keys());
    if (!allow_kw && kparms->num_keys() > 0) {
        std::cout << "Dispatch error calling method: " <<
                     "expected no keyword args in " << method << std::endl;
        assert(false);
        exit(1);
    }
}

Cs_class::Cs_class(Symbol *name, int64_t slot_count, int64_t any_slots,
                   MemberTable *table, Cs_class *parent) {
    Any name_as_any{name};
    set_slot(1, name_as_any);  // name is a symbol
    slots[2].integer = slot_count;  // unencoded
    slots[3].integer = any_slots;   // unencoded
    if (parent != nullptr) {
        MemberTable parent_table_copy = *(parent->get_member_table());
        // since merge alters the argument, we use a temporary copy
        table->merge(std::move(parent_table_copy));
    }
    slots[4].integer = reinterpret_cast<int64_t>(table);
    slots[5].integer = reinterpret_cast<int64_t>(parent);
    // A: we could potentially copy the table wholesale, but that mucks
    // around with memory a bit too much for my liking. Get it right
    // first then attempt to refactor.
}

Obj::Obj() {
    // default tag is tag_object
}

Obj::Obj(Cs_class * class_ptr) {
    // default tag is tag_object
    slots[0] = Any{class_ptr};
}


uint64_t Obj::get_any_slots()
{
    Cs_class *cs_class = (Cs_class *) (slots[0].integer);
    return cs_class->get_inst_any_slots();
}


Cs_class *Obj::get_class_ptr()
{
    return (Cs_class *) slots[0].integer;
}


bool Obj::isinstance(Cs_class *cs_class)
{
    Cs_class *class_ptr = get_class_ptr();
    while (class_ptr) {
        if (class_ptr == cs_class) {
            return true;
        }
        class_ptr = *(class_ptr->get_superclass());
    }
    return false;
}


void Obj::set_class_ptr(Cs_class * c_ptr) {
    set_slot(0, Any(c_ptr));
}


Any Obj::call(Symbol *method) {
    Cs_class *cs_class = get_class_ptr();
    return std::invoke(cs_class->find_function(method), this);
}


Obj *to_Obj(Any x)
{
    return reinterpret_cast<Obj *>(x.heap_obj);
}


Obj *as_Obj(Any x)
{
    check(is_obj(x));
    return to_Obj(x);
}


//Array *cs_symbols = nullptr;
Any cs_class_get_class_name(Obj* self, const Array &args,
                            const Dict &kwargs) {
    check_dispatch(css_get_name, 0);
    return Any((static_cast<Cs_class*>(self))->get_name());
}


Any cs_class_get_inst_slot_count(Obj* self, const Array &args,
                                        const Dict &kwargs) {
    check_dispatch(css_get_inst_slot_count, 0);
    return Any((static_cast<Cs_class*>(self))->get_inst_slot_count());
}


Any cs_class_get_inst_any_slots(Obj* self, const Array &args,
                                       const Dict &kwargs) {
    check_dispatch(css_get_inst_any_slots, 0);
    return Any((static_cast<Cs_class*>(self))->get_inst_any_slots());
}


Any cs_class_get_member_table(Obj* self, const Array &args,
                                     const Dict &kwargs) {
    check_dispatch(css_get_member_table, 0);
    return Any((static_cast<Cs_class*>(self))->get_member_table());
}


MemberTable cs_class_table;


MemberTable cs_obj_table;
// objects have no methods
