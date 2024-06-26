//
// Created by anthony on 6/4/24.
//

#pragma once
#include "obj.h"
#include <any.h>
#include <unordered_map>
#include <functional>

template <>
struct std::hash<Any> {
    std::size_t operator()(Any x) const noexcept {
        return std::hash<uint64_t>{}(x.integer);
    }
};

class Dictionary : public Basic_obj, std::unordered_map<Any, Any> {
};

