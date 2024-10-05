// array.cpp - implementation of cserpent Array type

#include "any.h"
#include "gc.h"
#include "basic_obj.h"
#include "obj.h"
#include "array.h"
#include "csmem.h"
#include "op_overload.h"
#include "dict.h"
#include <algorithm>
#include <sstream>
#include "runtime.h"

static std::vector<Any> *to_vector(Array *x) {
    return (std::vector<Any> *) x->slots;
}

int64_t len(Array *x) { return x->len(); }

Array::Array() {
    set_tag(tag_array);
    // use placement new to create vector at &slots[0]:
    new(slots) (std::vector<Any>);
}

Array::Array(std::initializer_list<Any> l) {
    set_tag(tag_array);
    new (slots) std::vector<Any> {l};
}

Array::Array(int64_t size, Any value) {
    set_tag(tag_array);
    std::vector<Any> &data = *new(slots) (std::vector<Any>)(size);
    if (size > 0) {
        set(0, value);  // handles color and gc_write_block for GC
        // for the rest of the array, since we're duplicating value, there
        // is no need to call the more expensive set method and we can just
        // write value directly to array storage:
        for (int64_t i = 1; i < size; i++) {  // fill the rest with value
            data[i] = value;
        }
    }
}

std::vector<Any> * Array::get_vector() const {
    return (std::vector<Any> *) slots;
}

Array::Array(Array *x) {
    set_tag(tag_array);
    new(slots) std::vector<Any> {*(x->get_vector())};
}

static int64_t handle_index(int64_t i, int64_t len) {
    if (i < 0) {
        i = len + i;
    }
    if (i < 0 || i > len) {
        throw std::out_of_range("Array index out of range");
    }
    return i;
}


Array *Array::append(Any x) {
    std::vector<Any> *data = get_vector();
    // push_back does not respect GC requirements to use set(), so first we 
    // create space by pushing y; then we set the space to x:
    Any y;
    data->push_back(y);
    set(data->size() - 1, x);
    return this;
}


Array *Array::append(int64_t x) {
    // slight optimization of append since x does not need to be marked, so we
    // do not need to use set():
    std::vector<Any> *data = get_vector();
    data->push_back(Any {x});
    return this;
}


Array *Array::append(double x) {
    std::vector<Any> *data = get_vector();
    data->push_back(Any {x});
    return this;
}


void Array::set(int64_t index, Any value) {
    std::vector<Any> *data = get_vector();
    Basic_obj *vptr;
    if (gc_write_block && is_basic_obj(value) && get_color() != GC_BLACK) {
        basic_obj_make_gray(to_basic_obj(value));
    }
    data->at(index) = value;
}    


void Array::set(int64_t index, double value) {
    std::vector<Any> *data = get_vector();
    data->at(index) = value;
}


void Array::set(int64_t index, int64_t value) {
    std::vector<Any> *data = get_vector();
    data->at(index) = value;
}

/*
Any& Array::operator[](int64_t i) {
    std::vector<Any> *data = get_vector();
    return data->at(i);
}
*/


Any Array::operator[](int64_t i) const {
    std::vector<Any> *data = get_vector();
    return data->at(i);
}

// Refactor to constexpr function, after validating correctness.
Any Array::call(Any method, Array *args, Dict *kwargs) {
    if (method.is(symbol_append)) { // potential symbol optimization here
        check_dispatch(to_symbol(symbol_append), args, kwargs, 1, 0);
        return append((*args)[0]);
    } else if (method.is(symbol_last)) {
        check_dispatch(to_symbol(symbol_last), args, kwargs, 0, 0);
        return last();
    } else if (method.is(symbol_insert)) {
        check_dispatch(to_symbol(symbol_insert), args, kwargs, 2, 0);
        return insert(to_int((*args)[0]), (*args)[1]);
    } else if (method.is(symbol_unappend)) {
        check_dispatch(to_symbol(symbol_unappend), args, kwargs, 0, 0);
        return unappend();
    } else if (method.is(symbol_uninsert)) {
        check_dispatch(to_symbol(symbol_uninsert), args, kwargs, 2, 0);
        if (args->len() == 1) {
            return uninsert(to_int((*args)[0]));
        }
        return uninsert(to_int((*args)[0]), to_int((*args)[1]));
    } else if (method.is(symbol_reverse)) {
        check_dispatch(to_symbol(symbol_reverse), args, kwargs, 0, 0);
        return reverse();
    } else if (method.is(symbol_copy)) {
        check_dispatch(to_symbol(symbol_copy), args, kwargs, 0, 0);
        return copy();
    } else if (method.is(symbol_set_len)) {
        check_dispatch(to_symbol(symbol_set_len), args, kwargs, 1, 0);
        return set_len(to_int((*args)[0]));
    }
    throw std::runtime_error("Array: no such method");
}


int64_t Array::len() const {
    std::vector<Any> *data = (std::vector<Any> *) slots;
    return data->size();
}

Array *subseq(Array *arr, int64_t start, int64_t end) {
    // precondition: name is not in symbol table
    struct Frame : public Cs_frame {
        Any result;
    } L;
    constexpr int sl_result = 0;
    memset(&L, 0, sizeof(L));
    STD_FUNCTION_ENTRY(L, 1);
    int64_t arr_len = arr->len();
    if (end == std::numeric_limits<int64_t>::max()) {
        end = arr_len;
    }
    if (end < 0) {
        end = arr_len + end;
    }
    if (start < 0) {
        start = arr_len + start;
    }
    // do bounds checking: 0 <= start <= end <= arr_len
    if (start < 0 || start > end || end > arr_len) {
        throw std::out_of_range("subseq: out of range");
    }
    Array *result = new Array {};
    L.set(sl_result, result);
    for (int64_t i = start; i < end; i++) {
        result->append((*arr)[i]);
    }
    STD_FUNCTION_EXIT(L, result);
}

bool is_equal(Array *lhs, Array *rhs) {
    return &lhs == &rhs;
}

std::ostream &operator<<(std::ostream &os, Array *x) {
    std::vector<Any> &data = *(x->get_vector());
    os << "[";
    for (auto i = data.begin(); i != data.end(); i++) {
        os << *i;
        if (i != data.end() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

std::string debug_str(Array *x) {
    std::vector<Any> &data = *(x->get_vector());
    std::stringstream ss;
    ss << "[";
    for (auto i = data.begin(); i != data.end(); i++) {
        ss << *i;
        if (i != data.end() - 1) {
            ss << ", ";
        }
    }
    ss << "]";
    return ss.str();
}


Any Array::last() {
    std::vector<Any> *data = get_vector();
    return data->back();
}

Array *Array::insert(int64_t i, Any x) {
    std::vector<Any> *data = get_vector();
    // do bounds checking
    i = handle_index(i, data->size());
    data->insert(data->begin() + i, x);
    return this;
}

Any Array::unappend() {
    std::vector<Any> *data = get_vector();
    Any last = data->back();
    data->pop_back();
    return last;
}

Array *Array::uninsert(int64_t i, int64_t j) {
    // tricky because we could cause GC to miss marking an element that
    // moves into a previous slot
    std::vector<Any> *data = get_vector();
    i = handle_index(i, data->size());
    if (j == std::numeric_limits<int64_t>::max()) {
        data->erase(data->begin() + i);
    } else {
        j = handle_index(j, data->size());
        data->erase(data->begin() + i, data->begin() + j);
    }
    // we could possibly optimize by checking to see if we actually need
    // to adjust gc_array_index, but it's easier and certainly safe to
    // just re-mark the entire array
    if (this == gc_array) {
        gc_array_index = 0;
    }
    return this;
}

Array *Array::reverse() {
    std::vector<Any> *data = get_vector();
    std::reverse(data->begin(), data->end());
    // watch out! if we are marking this array, restart the marking:
    if (this == gc_array) {
        gc_array_index = 0;
    }
    return this;
}

Array *Array::copy() {
    return new Array (*this);
}

Array *Array::set_len(int64_t new_len) {
    std::vector<Any> *data = get_vector();
    data->resize(new_len);
    return this;
}
