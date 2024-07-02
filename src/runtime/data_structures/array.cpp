// array.cpp - implementation of cserpent Array type

#include "any.h"
#include "gc.h"
#include "obj.h"
#include "array.h"
#include "csmem.h"

/* create_array_data for Array_data as subclass of Basic_obj
Array_data *create_array_data(int64_t size, Any value) {
    // zero-length array should be represented by null Array_data
    assert(size > 0);
    // number of slots will be size + 1 (first slot is slot count)
    Array_data *data = reinterpret_cast<Array_data *>(
            CSMALLOCTN(char, slot_count_to_size(size + 1)));
    data->header = 0;
    data->set_tag(tag_array_data);
    data->set_color(initial_color);
    // leave #slots = 0, so we always get the size from slots[0]
    data->slot[0] = size + 1;
    data->set_slot(1, value);  // handles color and write_block for GC
    for (int64_t i = 1; i < size; i++) {  // fill the rest with value
        data->slot[i + 1].integer = value.integer;
    }
    return data;
}
*/


Array::Array() {
    this->set_tag(tag_array);
    // use placement new to create vector at &slots[0]:
    new(slots) (std::vector<Any>);
}

// Array::Array(std::initializer_list<Any> l) : data {l} {set_tag(tag_array);}

Array::Array(int64_t size, Any value) {
    set_tag(tag_array);
    std::vector<Any> &data = *new(slots) (std::vector<Any>)(size);
    if (size > 0) {
        set(0, value);  // handles color and write_block for GC
        for (int64_t i = 1; i < size; i++) {  // fill the rest with value
            data[i] = value;
        }
    }
}


void Array::append(Any x) {
    std::vector<Any> *data = (std::vector<Any> *) slots;
    data->push_back(nullptr);
    set(data->size() - 1, x);
}


void Array::append(int64_t x) {
    append(Any {x});
}


void Array::append(double x) {
    append(Any {x});
}


void Array::set(int64_t index, Any value) {
    std::vector<Any> *data = (std::vector<Any> *) slots;
    Basic_obj *vptr;
    if (write_block && is_ptr(value) && get_color() != GC_BLACK) {
        basic_obj_make_gray(to_ptr(value));
    }
    data->at(index) = value;
}    


void Array::set(int64_t index, double value) {
    std::vector<Any> *data = (std::vector<Any> *) slots;
    data->at(index) = value;
}


void Array::set(int64_t index, int64_t value) {
    std::vector<Any> *data = (std::vector<Any> *) slots;
    data->at(index) = value;
}


Any& Array::operator[](int64_t i) {
    std::vector<Any> *data = (std::vector<Any> *) slots;
    return data->at(i);
}


Any Array::operator[](int64_t i) const {
    std::vector<Any> *data = (std::vector<Any> *) slots;
    return data->at(i);
}


int64_t len(Array *x) {
    std::vector<Any> *data = (std::vector<Any> *) x->slots;
    return data->size();
}


/*
int64_t Array::available_len(); {
    if (len(this) > 0)
        return reinterpret_cast<Array_data>(slot[1])->slot[0].integer;
    } else {
        return 0;
    }
}
*/
