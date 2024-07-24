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
    std::size_t operator()(Any x) const noexcept {
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
    Dictionary(std::initializer_list<std::pair<const Any, Any>> l);

    friend std::string debug_str(const Dictionary& x);

};

inline const Dictionary empty_dict {};