
/**
def matmul(a, b):
var n = len(a)
var m = len(b[0])
var p = len(a[0])

var c = []

for i = 0 to n
var ci = [0.0 for z = 0 to m]
for k = 0 to p
var aik = a[i][k]
var bk = b[k]

for j = 0 to m
ci[j] = ci[j] + (aik * bk[j])
c.append(ci)

return c
 */

#include <cstdint>
#include "any.h"
#include "data_structures/array.h"
#include "op_overload.h"
#include "builtin_functions/builtin_functions.h"
#include <iostream>
#include "benchmarking_utils.h"


// a is a (Array *)
Any matmul(Any a, Any b) {
    Any n = len(a);
    Any m = len(b[0]);
    Any p = len(a[0]);
    Any c = *(new Array {});
    for (int64_t i = 0; i < n; i++) {
        // list comprehension
        Any ci = *(new Array (m, 0.0));
        for (int64_t k = 0; k < p; k++) {
            Any aik = a[i][k];
            Any bk = b[k];
            for (int64_t j = 0; j < m; j++) {
                ci[j] = ci[j] + (aik * bk[j]);
            }
        }
        c.append(ci);
    }
    return c;
}

std::string test_matmul(Any n) {
    Any tmp = 1./n/n;
    Any a = *(new Array {});
    Any b = *(new Array {});
    for (int64_t i = 0; i < n; i++) {
        Any _tmp_a = *(new Array {});
        Any _tmp_b = *(new Array {});
        for (int64_t j = 0; j < n; j++) {
            _tmp_a.append(tmp * (i - j) * (i + j));
            _tmp_b.append(tmp * (i + j) * (i - j));
        }
        a.append(_tmp_a);
        b.append(_tmp_b);
    }
    Any d = matmul(a, b);
    //do_not_optimize(d);
    /// This is an optimization blocker, makes it harder to inline and lets us check correctness
    std::string result = std::to_string(to_real(d[static_cast<int64_t>(n) / 2][static_cast<int64_t>(n) / 2]));
#ifdef FREE
    for (int64_t i = 0; i < n; i++) {
        delete reinterpret_cast<Array*>(to_ptr(a[i]));
        delete reinterpret_cast<Array*>(to_ptr(b[i]));
        delete reinterpret_cast<Array*>(to_ptr(d[i]));
    }
    delete reinterpret_cast<Array*>(to_ptr(a));
    delete reinterpret_cast<Array*>(to_ptr(b));
    delete reinterpret_cast<Array*>(to_ptr(d));
#endif
    return result;
}