//
// Created by anthony on 7/7/24.
//


#pragma once
#include <string>
#include <cstdint>
#include <limits>

/**
 * @brief Symbols implemented with value semantics. The reason we can
 * (and should) do this is because strings are immutable, unlike other
 * data structures on the heap.  In short, define Symbol to be a class
 * that fits within 64 bits, specifically as a union between a
 * fixed-length char array (for short strings) and a pointer to a
 * heap-allocated std::string (for long strings).  Reference counting
 * is not needed because every allocated std::string should only have
 * 1 reference.  However, every copy of a string requires a new
 * heap-allocated std::string.
 */

// In a little-endian machine, short strings start at byte 0,
// and the tag bits are bytes 6 & 7 in an 8-byte array.
// In a big-endian machine, short strings start at byte 2,
// and the tag bits are bytes 0 & 1
//
// Strings have an extra type bit at bit 47 to distinguish short
// and long strings, so only 4 bytes + EOS are available for
// short strings.

#ifdef LITTLE_ENDIAN_ORDER
#define STR_BASE 0
#else
#define STR_BASE 2
#endif

union Symbol {
    char chars[8];  // short string data starting at 0 or 2
    uint64_t data;  // long string or TAG

    //TODO: upon construction of a symbol, we should check if it's
    //already in the symbol table
    Symbol(); // default constructor: the empty string

    // from string literals, consider making explicit and wrap every
    // literal in Symbol():
    Symbol(const char * literal);

    // this should only be called internally, hence explicit
    explicit Symbol(const std::string& s);

    // rule of 3 goes here
    friend void swap(Symbol& first, Symbol& second) noexcept;
    ~Symbol();
    Symbol(const Symbol& other);
    // What's this?  Symbol(Symbol&& other);
    Symbol& operator=(Symbol other); // covers both copy and move assignment
};

const std::string temp_str(const Symbol& s);
bool operator==(const Symbol& a, const Symbol& b);
Symbol operator+(const Symbol& a, const Symbol& b);
std::ostream& operator<<(std::ostream& os, const Symbol& x);

template <>
struct std::hash<Symbol> {
    size_t operator()(Symbol x) const noexcept {
        return std::hash<std::string>{}(temp_str(x));
    }
};

