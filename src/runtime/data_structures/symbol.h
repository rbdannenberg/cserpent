//
// Created by anthony on 7/7/24.
//


#pragma once
#include <string>
#include <cstdint>
#include <limits>

/**
 * @brief Symbols implemented with value semantics. The reason we can (and should) do this is because
 * strings are immutable, unlike other data structures on the heap.
 * In short, define Symbol to be a class that fits within 64 bits, specifically as a union between a
 * fixed-length char array (for short strings) and a pointer to a heap-allocated std::string (for long strings).
 * Symbol will manage the memory of its member std::string exclusively, so we remove it from the
 * purview of the garbage collector.
 * Reference counting is not needed because every allocated std::string should only have 1 reference.
 */


union Symbol {
    struct {
        // short string
        // "wrong" way round because I hate little endian
        // tag must be last so it lines up with the most significant bytes of data/the pointer
        // nul terminator (chars[5]) is bits 44-47, tag is bits 48-63
        char chars[6];
        uint16_t tag;
    };
    uint64_t data; // long string

    //TODO: upon construction of a symbol, we should check if it's already in the symbol table
    Symbol(); // default constructor: the empty string
    Symbol(const char * literal); // from string literals, consider making explicit and wrap every literal in Symbol()
    explicit Symbol(const std::string& s); // this should only be called internally, hence explicit

    // rule of 3 goes here
    friend void swap(Symbol& first, Symbol& second) noexcept;
    ~Symbol();
    Symbol(const Symbol& other);
    Symbol(Symbol&& other);
    Symbol& operator=(Symbol other); // covers both copy and move assignment
};

std::string temp_str(const Symbol& s);
bool operator==(const Symbol& a, const Symbol& b);
Symbol operator+(const Symbol& a, const Symbol& b);
std::ostream& operator<<(std::ostream& os, const Symbol& x);

template <>
struct std::hash<Symbol> {
    std::size_t operator()(Symbol x) const noexcept {
        return std::hash<std::string>{}(temp_str(x));
    }
};

