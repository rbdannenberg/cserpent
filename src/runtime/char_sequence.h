//
// Created by anthony on 7/7/24.
//

#pragma once
#include <string>
#include <cstdint>
#include <limits>

/// Base class for both Strings and Symbols.
/// Make sure no virtual functions else we have vtables occupying memory
class CharSequence {
public:
    union {
        struct {
            char chars[6];
            uint16_t tag;
        };
        uint64_t data; // long
    };
    CharSequence(); // default constructor: the empty string
    CharSequence(const char * literal); // from string literals, consider making explicit and wrap every literal in CharSequence()
    explicit CharSequence(const std::string& s); // this should only be called internally, hence explicit

//    static const int16_t short_tag;
//    static const uint64_t long_tag;
//    static const uint64_t empty_seq;
    int16_t get_short_tag();
    uint64_t get_long_tag();
    uint64_t get_empty_seq();
    static constexpr int64_t max_short_size = 5;
    static bool is_short_literal(const char *literal);
    static bool is_short_seq(const CharSequence& s);
    static std::string * get_seq_ptr(uint64_t s);
    static uint64_t get_seq_data(std::string * s);
    static std::string temp_str(const CharSequence& s);
};

int64_t len(const CharSequence& s);
