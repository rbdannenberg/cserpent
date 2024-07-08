// array.cpp - implementation of cserpent Array type

#include "any.h"
#include "gc.h"
#include "obj.h"
#include "array.h"
#include "csmem.h"

static std::vector<Any> *to_vector(const Array& x) {
    return (std::vector<Any> *) x.slots;
}

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


int64_t len(const Array& x) {
    std::vector<Any> *data = (std::vector<Any> *) x.slots;
    return data->size();
}

Array subseq(const Array& arr, int64_t start, int64_t end) {
    int64_t arr_len = len(arr);
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
    Array result;
    for (int64_t i = start; i < end; i++) {
        result.append(arr[i]);
    }
    return result;
}
