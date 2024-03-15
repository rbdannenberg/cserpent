#include "any.h"
#include <cmath>
#ifdef DEBUG
    #include <cassert>
    #define ASSERT(x) assert(x)
#else
    #define ASSERT(x) (void)0
#endif

/*
Hexadecimal to Binary Conversion Table:

Hex | Binary   | Hex | Binary   | Hex | Binary   | Hex | Binary
----|----------|-----|----------|-----|----------|-----|--------
 0  | 0000     | 4   | 0100     | 8   | 1000     | C   | 1100
 1  | 0001     | 5   | 0101     | 9   | 1001     | D   | 1101
 2  | 0010     | 6   | 0110     | A   | 1010     | E   | 1110
 3  | 0011     | 7   | 0111     | B   | 1011     | F   | 1111
*/

/** @brief mask for first 15 bits */
static constexpr int64_t input_int_mask = 0xFFFE000000000000;
static constexpr uint64_t output_int_mask = 0xFFFC000000000000;
static constexpr uint64_t NaN_proper = 0x7ff8000000000000;
static constexpr uint64_t input_ptr_mask = 0xFFFF000000000007;
static constexpr uint64_t output_ptr_mask = 0x7FFC000000000000;
static constexpr uint64_t output_str_mask = 0x7FFE000000000000;

AnyC constructor(int64_t x) {
    int64_t x_masked = x & input_int_mask;
    // Precondition: Top 15 bits are all 0's or 1's
    ASSERT(x_masked == 0 || x_masked == input_int_mask);
    AnyC result;
    result.integer = static_cast<uint64_t>(x);
    result.integer |= output_int_mask;
    return result;
}

AnyC constructor(double x) {
    AnyC result;
    if (isnan(x)) {
        result.integer = NaN_proper;
    }
    else {
        result.real = x;
    }
    return result;
}

AnyC constructor(void *x) {
    AnyC result;
    uint64_t input_bit_pattern = reinterpret_cast<uint64_t>(x);
    // Precondition: first 16 bits and last 3 bits are 0
    ASSERT(input_bit_pattern & input_ptr_mask == 0);
    result.integer = input_bit_pattern | output_ptr_mask;
    return result;
}

AnyC constructor(std::string x) {
    AnyC result;
    void *raw_ptr = static_cast<void*>(&r);
    uint64_t input_bit_pattern = reinterpret_cast<uint64_t>(raw_ptr);
    // Precondition: first 16 bits and last 3 bits are 0
    ASSERT(input_bit_pattern & input_ptr_mask == 0);
    result.integer = input_bit_pattern | output_str_mask;
    return result;
}






