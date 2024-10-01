#pragma once
#include <string>
#include <cstdint>

double force_real(Any x);
int64_t force_int(Any x);
Any force_str(Any x);

Any type_error(Any x);

Any type_error(Any x, const std::string& function);
