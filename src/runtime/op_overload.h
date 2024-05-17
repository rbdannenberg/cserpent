#pragma once

#include "any.h"

Any operator+ (Any lhs, int rhs);
Any operator+ (Any lhs, int64_t rhs);
Any operator+ (Any lhs, double rhs);
Any operator+ (Any lhs, Any rhs);
Any operator+ (int lhs, Any rhs);
Any operator+ (int64_t lhs, Any rhs);
Any operator+ (double lhs, Any rhs);

