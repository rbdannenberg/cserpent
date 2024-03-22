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
    std::cerr << "Not implemented" << std::endl;
    return {};
}

bool is_int(AnyC x) {
    return (x.integer & INT_TAG) == INT_TAG;
}

bool is_double(AnyC x) {
    uint64_t tmp = x.integer & DOUBLE_MASK;
    return tmp >= DOUBLE_LW && tmp <= DOUBLE_UP;
}

bool is_ptr(AnyC x) {
    return (x.integer & TAG_MASK) == PTR_TAG;
}

bool is_string(AnyC x) {
    std::cerr << "Not implemented" << std::endl;
    return {};
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

std::string to_string(AnyC x) {
    std::cerr << "Not implemented" << std::endl;
    return {};
}

