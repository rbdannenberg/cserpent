#pragma once

#include "any.h"
#include <string>
#include <cstdint>

double force_real(Any x);

Any type_error(Any x);

Any type_error(Any x, const std::string& function);
