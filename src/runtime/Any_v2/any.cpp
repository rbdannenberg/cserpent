#include <cstring>
#include <string>
#include <cstdio>
#include <iostream>
#include "any.h"

uint64_t TAG_MASK     = 0xFFFF000000000000uLL;
uint64_t BIAS         =    0x1000000000000uLL;
uint64_t INT_TAG      = 0xFFFC000000000000uLL;
uint64_t DOUBLE_MASK  = 0x7FFF000000000000uLL;
uint64_t DOUBLE_UP    = 0x7FF0000000000000uLL;
uint64_t DOUBLE_LW    = 0x0001000000000000uLL;
uint64_t PTR_TAG      = 0x0000000000000000uLL;
uint64_t SHORTSTR_TAG = 0xFFFA000000000000uLL;
size_t num_chars = 5; // 8 bytes - 2 (tag) - 1 (nul terminator)

/*
Hexadecimal to Binary Conversion Table:

Hex | Binary   | Hex | Binary   | Hex | Binary   | Hex | Binary
----|----------|-----|----------|-----|----------|-----|--------
 0  | 0000     | 4   | 0100     | 8   | 1000     | C   | 1100
 1  | 0001     | 5   | 0101     | 9   | 1001     | D   | 1101
 2  | 0010     | 6   | 0110     | A   | 1010     | E   | 1110
 3  | 0011     | 7   | 0111     | B   | 1011     | F   | 1111
*/

// Note: integer promotion rules - int64_t & uint64_t, int64_t gets cast to uint64_t

AnyC constructor(int64_t x) {
    #ifdef DEBUG
    int64_t tmp = x & 0xFFFE000000000000;
    if (tmp != 0 && tmp != 0xFFFE000000000000) {
        throw std::runtime_error("Precondition failed: integer value corrupted:");
    }
    #endif
    AnyC result;
    result.integer = x | INT_TAG;
    return result;
}

AnyC constructor(double x) {
    AnyC result;
    result.doub = x;
    #ifdef DEBUG
    if ((result.integer & 0x7FF0000000000000) == DOUBLE_UP) {
        throw std::runtime_error("Precondition failed: Nan or Inf value.");
        return {};
    }
    #endif
    result.integer += BIAS;
    return result;
}

AnyC constructor(void* x) {
    AnyC result;
    // not result.integer = reinterpret_cast<uint64_t>(x);?
    // also shouldn't it be int64_t*
    result.integer = *reinterpret_cast<uint64_t*>(&x);
    #ifdef DEBUG
    if (result.integer & TAG_MASK) {
        std::cerr << "Precondition failed: pointer corrupted" << std::endl;
        return {};
    }
    #endif
    return result;
}

AnyC constructor(std::string x) {
    // 2 possible ways:
    // 1: get 8 bytes starting from x, discard the last 2 bytes, shift right and add tag
    // 2: insert tag bytes in front of string, get 8 bytes starting from front.
    // The following is way no. 1:
#ifdef DEBUG
    if (x.length() > num_chars) {
        std::cerr << "Precondition failed: string too long" << std::endl;
        return {};
    }
#endif
    AnyC result;
    // reads 8 bytes
    uint64_t tmp = *((uint64_t *) (x.c_str()));
    result.integer = (tmp >> 16) | SHORTSTR_TAG;
    // warning: this copies a lot of garbage after the nul terminator into the AnyC
    return result;
}

bool is_int(AnyC x) {
    // x.integer gets converted to uint64_t
    return (x.integer & INT_TAG) == INT_TAG;
}

bool is_double(AnyC x) {
    return x.integer - BIAS < 0xFFF9000000000000uLL;
//    uint64_t tmp = x.integer & DOUBLE_MASK;
//    return tmp >= DOUBLE_LW && tmp <= DOUBLE_UP;
}

bool is_ptr(AnyC x) {
    return (x.integer & TAG_MASK) == PTR_TAG;
}

bool is_shortstr(AnyC x) {
    return (x.integer & TAG_MASK) == SHORTSTR_TAG;
}

int64_t to_int(AnyC x) {
    return (x.integer << 14) >> 14;
}

double to_double(AnyC x) {
    x.integer -= BIAS;
    return x.doub;
}

void* to_ptr(AnyC x) {
    return reinterpret_cast<void*>(x.integer);
}

std::string to_shortstr(AnyC x) {
    // copies from 3rd byte onwards (skips tag) until nul terminator is encountered
    return std::string {&((char *) &(x.integer))[2]};
}

