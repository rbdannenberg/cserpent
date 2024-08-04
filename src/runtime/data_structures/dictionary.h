//
// Created by anthony on 6/4/24.
//

#pragma once
#include <any.h>
#include "obj.h"
#include <op_overload.h>
#include <unordered_map>
#include <functional>

template <>
struct std::hash<Any> {
    size_t operator()(Any x) const noexcept {
        return std::hash<uint64_t>{}(x.integer);
    }
};

struct KeyEqual {
    bool operator()(Any a,Any b) const {
        return a == b;
    }
};



class Dictionary : public Basic_obj {
public:
    using value_type = std::pair<const Any, Any>;
    using map_type = std::unordered_map<Any, Any, std::hash<Any>, KeyEqual>;
    int64_t more_slots[((sizeof(map_type) + sizeof(int64_t) - 1) /
                        sizeof(int64_t)) - 1];
    Dictionary();
    // TODO: what happens here? Adding elements to a dictionary requires
    // that you check write_block and possible add new values to the
    // gray list for GC. See Basic_obj::set_slot().
    Dictionary(std::initializer_list<std::pair<const Any, Any>> l);

    friend std::string debug_str(const Dictionary& x);

};

// TODO: This is probably wrong. If there is ever a reference from the
// heap to this object, we will crash in GC. If this were placed on the
// heap and not reported by gc_mark_roots(), it will be freed, overwritten,
// and we will crash.
inline const Dictionary empty_dict {};

std::unordered_map<Any, Any> *to_map(const Dictionary& x);
