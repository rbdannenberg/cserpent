//
// Created by anthony on 6/4/24.
//
#include "any.h"
#include "gc.h"
#include "obj.h"
#include "op_overload.h"
#include "dict.h"
#include "op_overload.h"


std::unordered_map<Any, Any> *to_map(const Dict& x) {
    return (std::unordered_map<Any, Any> *) x.slots;
}



Dict::Dict() {
    set_tag(tag_dict);
    new(slots) map_type {};
}

Dict::Dict(std::initializer_list<std::pair<const Any, Any>> l) {
    set_tag(tag_dict);
    new(slots) map_type {l};
}

std::string debug_str(const Dict& x) {
    return "unimplemented";
}
