//
// Created by anthony on 7/7/24.
//

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <utility>
#include "any.h"
#include "gc.h"
#include "header.h"
#include "heap_obj.h"
#include "obj.h"
#include "runtime.h"
#include "csstring.h"
#include "symbol.h"
#include "array.h"
#include "dict.h"
#include <cstring>

Dict *cs_symbol_table;

Symbol *css_t;

/* This is not used - not complete

Symbol::Symbol(Any name, Any *value, Any func,
               Any_type stype, Cs_class *cs_class)
{
    // precondition: name is not in symbol table
    struct Frame : public Cs_frame {
        Any self;
        Any name;  // this
        Any value;
        Any func;
    } L;
    constexpr int sl_name = 0;
    constexpr int sl_value = 1;
    constexpr int sl_func = 2;
    CS_FUNCTION_ENTRY(1);
    LSET(sl_name, name);
    if (is_in_heap(*value)) {
        LSET(sl_value, *value);
    }
    
    set_tag(tag_symbol);
    set_slot(0, name);
    set_slot(1, value);
    set_slot(2, func);
    slots[3].integer = (uint64_t) stype;  // not a heap pointer
    set_slot(4, Any{cs_class});
    Any symbol(this);  // nan-box this into an Any to place in cs_symbol_table
    cs_symbol_table->insert(name, symbol);
    CS_FUNCTION_EXIT(0);
}
*/

Symbol::Symbol(const char *name_string, uint64_t value, Any func,
               Any_type stype, Cs_class *cs_class)
{
    // precondition: name is not in symbol table
    // This is tricky: since this constructor puts a Heap_obj on the heap,
    // but there is no reference to it yet, it could get GC'd when we convert
    // name to an Any or do a symbol table insert, so we have to store this
    // as a local variable. OTHER PARAMETERS ARE PROTECTED IN THIS SPECIAL
    // CONSTRUCTOR.
    //
    // value is either the address of a global variable (known to C++) with
    // type stype, OR if stype is Any_type::DIRECT, the symbol value is stored
    // directly in slot[1], and value is the nan-boxed Any.integer value.
    //
    // slots are: symbol name, symbol value, function value, stype, cs_class
    struct Frame : public Cs_frame {
        Any result;  // this
    } L;
    constexpr int sl_result = 0;
    CS_FUNCTION_ENTRY(1);
    LSET(sl_result, Any(this));
    set_tag(tag_symbol);
    set_slot(0, Any{name_string});
    slots[1].integer = value;
    set_slot(2, func);
    slots[3].integer = (uint64_t) stype;
    slots[4].integer = (uint64_t) cs_class;
    cs_symbol_table->insert(name(), L.result);
    CS_FUNCTION_EXIT(0);
}


void Symbol::set_symbol_value(Any anyval)
{
    switch (symbol_type()) {
      case Any_type::DIRECT:
        set_any_global(&(slots[1]), anyval);
        break;
      case Any_type::INT:
        *((int64_t *) value()) = as_int(anyval);
        break;
      case Any_type::REAL:
        *((double *) value()) = as_real(anyval);
        break;
      case Any_type::BOOL:
        *((bool *) value()) = to_bool(anyval);
        break;
      case Any_type::SYMBOL: {
        Symbol *sym = as_symbol(anyval);
        HEAP_ITEM_IS_REACHABLE(sym);
        *((Symbol **) value()) = sym;
        break;
      }
      case Any_type::ARRAY: {
        Array *array = as_array(anyval);
        HEAP_ITEM_IS_REACHABLE(array);
        *((Array **) value()) = array;
        break;
      }
      case Any_type::DICT: {
        Dict *dict = as_dict(anyval);
        HEAP_ITEM_IS_REACHABLE(dict);
        *((Dict **) value()) = dict;
        break;
      }
      case Any_type::OBJ: {
        if (is_obj(anyval) &&
            to_obj(anyval)->get_class_ptr() == symbol_class()) {
            Obj *obj = to_obj(anyval);
            HEAP_ITEM_IS_REACHABLE(obj);
            *((Obj **) value()) = obj;
        } else {
            printf("Failure: value type does not match symbol type (Obj)\n");
            exit(1);
        }
        break;
      }
      default:
        printf("Failure: unexpected type\n");
        break;
    }
}


void Symbol::set_symbol_value(int64_t intval)
{
    if (symbol_type() == Any_type::INT) {
        *((int64_t *) value()) = intval;
    } else if (symbol_type() == Any_type::REAL) {
        *((double *) value()) = intval;
    } else {
        printf("Failure: symbol type incompatible with int\n");
        exit(1);
    }
}


void Symbol::set_symbol_value(double realval)
{
    if (symbol_type() == Any_type::INT) {
        *((int64_t *) value()) = realval;
    } else if (symbol_type() == Any_type::REAL) {
        *((double *) value()) = realval;
    } else {
        printf("Failure: symbol type incompatible with real\n");
        exit(1);
    }
}


void Symbol::set_symbol_value(bool boolval)
{
    check(symbol_type() == Any_type::BOOL);
    *((bool *) value()) = boolval;
}


void Symbol::set_symbol_value(Symbol *symval)
{
    check(symbol_type() == Any_type::SYMBOL);
    HEAP_ITEM_IS_REACHABLE(symval);
    *((Symbol **) value()) = symval;
}


void Symbol::set_symbol_value(Array *arrayval)
{
    check(symbol_type() == Any_type::ARRAY);
    HEAP_ITEM_IS_REACHABLE(arrayval);
    *((Array **) value()) = arrayval;
}


void Symbol::set_symbol_value(Dict *dictval)
{
    check(symbol_type() == Any_type::DICT);
    HEAP_ITEM_IS_REACHABLE(dictval);
    *((Dict **) value()) = dictval;
}


std::ostream& operator<<(std::ostream& os, const Symbol *x) {
    os << get_c_str(x->name());
    return os;
}


Any *set_any_global(Any *global_addr, Heap_obj *value) {
/* intended for use in compiled code: assign a heap object to a variable
   declared as Any (either "var" or undeclared)
 */
    HEAP_ITEM_IS_REACHABLE(value);
    // this says "if the GC is in its scan phase and value is
    // non-NULL and value points to a BLACK (unmarked) object,
    // then put value on a list of objects to be marked. (We
    // don't mark it immediately because it might reference
    // many other objects. The list of to-be-marked objects
    // allows us to mark incrementally.)
    *global_addr = Any{value};
    return global_addr;
}


Any *set_any_global(Any *global_addr, Any value)
{
    GLOBAL_WRITE_BLOCK(value);
    *global_addr = value;
    return global_addr;
}


Any *set_any_global(Any *global_addr, const char *value)
{
    return set_any_global(global_addr, Any(value));
}


Any *set_any_global(Any *global_addr, double value)
{
    return set_any_global(global_addr, Any(value));
}

Any *set_any_global(Any *global_addr, int64_t value)
{
    return set_any_global(global_addr, Any(value));
}

Any *set_any_global(Any *global_addr, int value)
{
    return set_any_global(global_addr, Any(int64_t(value)));
}
