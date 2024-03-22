#include <string>
#pragma once

union AnyC{
    int64_t integer;
    double doub;
};

// PRECONDITION: 50-bits complement <=> Top 15 bits are all 0's or 1's
// OCCUPY: 0xFFFC - 0xFFFF
AnyC constructor(int64_t x);

// PRECONDITION: Not NaN or Inf
// OCCUPY: 0x0001 - 0x7FFF; 0x8001 - 0xFFF0
AnyC constructor(double x);

// PRECONDITION: Top 16 bits are 0
AnyC constructor(void* x);

// NOT IMPLEMENTED
AnyC constructor(std::string x);

bool is_int(AnyC x);
bool is_double(AnyC x);
bool is_ptr(AnyC x);
bool is_string(AnyC x);

int64_t to_int(AnyC x);
double to_double(AnyC x);
void* to_ptr(AnyC x);
std::string to_string(AnyC x);

// Operator overload