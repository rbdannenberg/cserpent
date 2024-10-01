//
// Created by anthony on 7/2/24.
//

#pragma once
#include <string>
#include <cstdint>
#include <limits>

class String : public Basic_obj {
public:
    String(const std::string &s);
    String(const char *s = NULL);
    std::string *get_string() {
        return reinterpret_cast<std::string *>(slots[0].integer); }
    const char *get_c_str() {
        return reinterpret_cast<std::string *>(slots[0].integer)->c_str(); }
    int64_t len() {
        return reinterpret_cast<std::string *>(slots[0].integer)->size(); }
};

int64_t len(String *x);
