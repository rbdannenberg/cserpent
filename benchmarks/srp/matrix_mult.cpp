
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

/**
 * Things to do:
 * Overload the [] operator
 * Implement len
 */

#include <cstdint>
#include <any.h>
#include <data_structures/array.h>
#include <op_overload.h>
#include <builtin_functions/builtin_functions.h>
#include <csmem.h>


// a is a (Array *)
Any matmul(Any a, Any b) {
    int64_t n = len(a);
    int64_t m = len(b[0]);
    int64_t p = len(a[0]);

    Any c = Array {};
    for (int64_t i = 0; i < n; i++) {
        // list comprehension
        Any ci = Array {};
        for (int64_t z = 0; z < m; z++) {
            ci.append(0.0);
        }
        for (int64_t k = 0; k < p; k++) {
            Any aik = a[i][k];
            Any bk = b[k];
            for (int64_t j = 0; j < m; j++) {
                ci[j] = ci[j] + (aik * bk[j]);
                c.append(ci);
            }
        }
    }
    return c;
}


