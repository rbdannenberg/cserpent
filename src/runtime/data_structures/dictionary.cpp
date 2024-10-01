//
// Created by anthony on 6/4/24.
//
#include "any.h"
#include "gc.h"
#include "obj.h"
#include "op_overload.h"
#include "dict.h"
#include "op_overload.h"


std::unordered_map<Any, Any> *to_map(const Dictionary& x) {
    return (std::unordered_map<Any, Any> *) x.slots;
}



Dictionary::Dictionary() {
    set_tag(tag_dict);
    new(slots) map_type {};
}

Dictionary::Dictionary(std::initializer_list<std::pair<const Any, Any>> l) {
    set_tag(tag_dict);
    new(slots) map_type {l};
}

std::string debug_str(const Dictionary& x) {
    return "unimplemented";
}
