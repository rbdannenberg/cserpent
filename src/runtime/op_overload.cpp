#include "any.h"
#include "any_utils.h"
#include "op_overload.h"
#include <data_structures/array.h>
#include <iostream>


// Right now, we are ignoring the fact that strings and lists can be added to
Any operator+ (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return { to_int(lhs) + rhs };
    }
    else if (is_real(lhs)) {
        return { to_real(lhs) + static_cast<double>(rhs) };
    }
    else type_error(lhs);
}

Any operator+ (Any lhs, int rhs) {
    return lhs + static_cast<int64_t>(rhs);
}

Any operator+ (Any lhs, double rhs) {
    return { force_real(lhs) + rhs };
}

Any operator+ (Any lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs + to_int(rhs);
    }
    else if (is_real(rhs)) {
        return lhs + to_real(rhs);
    }
    else type_error(rhs);
}

// Symmetrical + operators
Any operator+ (int64_t lhs, Any rhs) {
    return rhs + lhs;
}

Any operator+ (int lhs, Any rhs) {
    return rhs + lhs;
}

Any operator+ (double lhs, Any rhs) {
    return rhs + lhs;
}

Any operator* (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return { to_int(lhs) * rhs };
    }
    else if (is_real(lhs)) {
        return { to_real(lhs) * static_cast<double>(rhs) };
    }
    else type_error(lhs);
}

Any operator* (Any lhs, int rhs) {
    return lhs * static_cast<int64_t>(rhs);
}

Any operator* (Any lhs, double rhs) {
    return { force_real(lhs) * rhs };
}

Any operator* (Any lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs * to_int(rhs);
    }
    else if (is_real(rhs)) {
        return lhs * to_real(rhs);
    }
    else type_error(rhs);
}

// Symmetrical * operators
Any operator* (int64_t lhs, Any rhs) {
    return rhs * lhs;
}

Any operator* (int lhs, Any rhs) {
    return rhs * lhs;
}

Any operator* (double lhs, Any rhs) {
    return rhs * lhs;
}

// < operator
bool operator< (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs < to_int(rhs);
    }
    else if (is_real(rhs)) {
        return static_cast<double>(lhs) < to_real(rhs);
    }
    else type_error(rhs);
}

bool operator< (double lhs, Any rhs) {
    return lhs < force_real(rhs);
}

bool operator< (Any lhs, Any rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) < rhs;
    }
    else if (is_real(lhs)) {
        return to_real(lhs) < rhs;
    }
    else type_error(lhs);
}

double operator/ (double lhs, Any rhs) {
    return lhs / force_real(rhs);
}

double operator/ (int64_t lhs, Any rhs) {
    return static_cast<double>(lhs) / force_real(rhs);
}

double operator/ (Any lhs, Any rhs) {
    return force_real(lhs) / force_real(rhs);
}

int64_t operator&= (int64_t& lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs &= to_int(rhs);
    }
    else type_error(rhs);
}

int64_t operator&= (Any& lhs, int64_t rhs) {
    if (is_int(lhs)) {
        int64_t _tmp = to_int(lhs) & rhs;
        lhs = _tmp;
        return _tmp;
    }
    else {
        type_error(lhs);
    }
}

int64_t operator&= (Any& lhs, Any rhs) {
    if (is_int(rhs) && is_int(lhs)) {
        int64_t _tmp = to_int(lhs) & to_int(rhs);
        lhs = _tmp;
        return _tmp;
    }
    else {
        type_error(lhs);
    }
}

int64_t operator& (Any lhs, int rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) & static_cast<int64_t>(rhs);
    }
    else type_error(lhs);
}

int64_t operator<< (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs << to_int(rhs);
    }
    else type_error(rhs);
}

/** Define the rest of the operators here:
 *
int64_t operator<< (int lhs, Any rhs);
bool operator>= (Any lhs, int rhs);
int64_t operator| (Any lhs, Any rhs);
int64_t operator| (int64_t lhs, Any rhs);
int64_t operator& (int64_t lhs, Any rhs);
int64_t operator^ (Any lhs, Any rhs);
int64_t operator>> (int64_t lhs, Any rhs);
int64_t operator& (Any lhs, int64_t rhs);
Any operator- (Any lhs, int rhs);
int64_t operator>> (Any lhs, int rhs);
 */

int64_t operator<< (int lhs, Any rhs) {
    if (is_int(rhs)) {
        return static_cast<int64_t>(lhs) << to_int(rhs);
    } else type_error(rhs);
}

bool operator>= (Any lhs, int rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) >= rhs;
    } else type_error(lhs);
}

int64_t operator| (Any lhs, Any rhs) {
    if (is_int(lhs) && is_int(rhs)) {
        return to_int(lhs) | to_int(rhs);
    } else type_error(lhs);
}

int64_t operator| (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs | to_int(rhs);
    } else type_error(rhs);
}

int64_t operator& (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs & to_int(rhs);
    } else type_error(rhs);
}

int64_t operator^ (Any lhs, Any rhs) {
    if (is_int(lhs) && is_int(rhs)) {
        return to_int(lhs) ^ to_int(rhs);
    } else type_error(lhs);
}

int64_t operator>> (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs >> to_int(rhs);
    } else type_error(rhs);
}

int64_t operator& (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) & rhs;
    } else type_error(lhs);
}

Any operator- (Any lhs, int rhs) {
    if (is_int(lhs)) {
        return { to_int(lhs) - rhs };
    } else type_error(lhs);
}

int64_t operator>> (Any lhs, int rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) >> rhs;
    } else type_error(lhs);
}

std::ostream& operator<<(std::ostream& os, Any x) {
    if (is_int(x)) {
        return os << to_int(x);
    }
    else if (is_real(x)) {
        return os << to_real(x);
    }
    else type_error(x, __func__);
}

bool operator==(Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) == rhs;
    }
    else type_error(lhs, __func__);
}

bool operator==(Any lhs, int rhs) {
    return lhs == static_cast<int64_t>(rhs);
}

bool operator==(Any lhs, Any rhs) {
    return lhs.integer == rhs.integer;
}

Any& Any::operator[](int64_t i) {
    if (is_ptr(*this)) {
        Basic_obj *basic_ptr= to_ptr(*this);
        if (basic_ptr->get_tag() == tag_array) {
            // consider using dynamic_cast instead of static_cast
            return (static_cast<Array*>(basic_ptr))->data.at(i);
//            Array arr (static_cast<Array_heap*>(basic_ptr));
//            return arr[i];
        }
    }
//    else if (is_shortstr(*this)) {
//        std::string str = to_shortstr(*this);
//        if (i < str.length()) {
//            // doesn't this defeat the purpose
//            // return a char?
//            return Any {str[i]};
//        }
//    }
// Note: This is because strings should be immutable
    else {
        type_error(*this);
    }
}

Any Any::operator[](int64_t i) const {
    if (is_ptr(*this)) {
        Basic_obj *basic_ptr= to_ptr(*this);
        if (basic_ptr->get_tag() == tag_array) {
            // consider using dynamic_cast instead of static_cast
            return (static_cast<Array*>(basic_ptr))->data.at(i);
        }
    }
    else if (is_shortstr(*this)) {
        std::string str = to_shortstr(*this);
        if (i < str.length()) {
            // doesn't this defeat the purpose
            // return a char?
            return Any {str[i]};
        }
    }
    else {
        type_error(*this);
    }
}

