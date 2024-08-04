//
// Created by anthony on 7/7/24.
//

#include <stdexcept>
#include <cassert>
#include <utility>
#include "csstring.h"
#include "symbol.h"
#include "array.h"

// symbols are the same as strings except for one tag bit, so use
// string implementation to do all the work.

// A symbol named "" seems like a really bad idea...
static constexpr uint64_t empty_symbol = SYMBOL_TAG;
static constexpr uint64_t string_to_symbol = SYMBOL_TAG - STR_TAG;

/// @brief temporary conversion to exploit the std::string library
/// static so we don't expose it outside of this file
/// I believe std::string has optimizations for short strings as well,
/// so this shouldn't be too wasteful
const std::string temp_str(const Symbol& s) {
    return temp_str(reinterpret_cast<const String &>(s));
}

// TODO: unicode requires wide chars or UTF-8.  I used UTF-8 in Serpent, but
// it is difficult. wide chars are good for our prototype.

Symbol::Symbol() : data {empty_symbol} {}

Symbol::Symbol(const char* literal) :
        data { literal_string_to_data(literal) }
{
    data |= string_to_symbol;  // fix type tag
}


Symbol::Symbol(const std::string& s) : Symbol {s.c_str()} {}

// Symbol strings are referenced permanently through the symbol table
// so they should never be freed.
Symbol::~Symbol() { }


// Symbols are all represented by the same std::string, and the string
// is permanent because of the symbol table, so new Symbol is just a
// copy of the bits.
Symbol::Symbol(const Symbol& other) : data { other.data } { }


void swap(Symbol& first, Symbol& second) noexcept {
    using std::swap;
    swap(first.data, second.data);
}


/* WHAT IS THIS?
Symbol::Symbol(Symbol&& other) : data {} {
    swap(*this, other);
}
*/

/* Overloading assignment to do swap?
Symbol& Symbol::operator=(Symbol other) {
    swap(*this, other);
    return *this;
}
*/

    
bool operator==(const Symbol& a, const Symbol& b) {
    return temp_str(a) == temp_str(b);
}

Symbol operator+(const Symbol& a, const Symbol& b) {
    return Symbol {temp_str(a) + temp_str(b)};
}

std::ostream& operator<<(std::ostream& os, const Symbol& x) {
    os << temp_str(x);
    return os;
}

const Any t {Symbol {"t"}};
const Any True {t};

