
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
#include "basic_obj.h"
#include "obj.h"
#include "runtime.h"
#include "data_structures/array.h"
#include "op_overload.h"
#include "builtin_functions/builtin_functions.h"
#include <iostream>
#include "benchmarking_utils.h"


// a is a (Array *)
Any matmul(Any a_, Any b_) {
    struct Frame {
        uint64_t header;
        Any a;
        Any b;
        Any n;
        Any m;
        Any p;
        Any c;
        Any ci;
        Any aik;
        Any bk;
    } locals;
    memset(&locals, 0, sizeof(locals));
    locals.a = a_;
    locals.b = b_;
    locals.n = len(locals.a);
    locals.m = len(locals.b[0]);
    locals.p = len(locals.a[0]);
    locals.c = *(new Array {});
    STD_FUNCTION_ENTRY(9);

    for (int64_t i = 0; i < locals.n; i++) {
        // list comprehension
        locals.ci = *(new Array(locals.m, 0.0));
        for (int64_t k = 0; k < locals.p; k++) {
            locals.aik = locals.a[i][k];
            locals.bk = locals.b[k];
            for (int64_t j = 0; j < locals.m; j++) {
                locals.ci.set(j, locals.ci[j] + (locals.aik * locals.bk[j]));
            }
        }
        locals.c.append(locals.ci);
    }
    STD_FUNCTION_EXIT(locals.c);
}


std::string test_matmul(Any n_) {
    struct Frame {
        uint64_t header;
        Any n;
        Any tmp;
        Any a;
        Any b;
        Any _tmp_a;
        Any _tmp_b;
        Any d;
    } locals;
    memset(&locals, 0, sizeof(locals));
    locals.n = n_;
    locals.tmp = 1.0 / (locals.n * locals.n);
    locals.a = *(new Array {});
    locals.b = *(new Array {});
    STD_FUNCTION_ENTRY(7);

    for (int64_t i = 0; i < locals.n; i++) {
        locals._tmp_a = *(new Array {});
        locals._tmp_b = *(new Array {});
        for (int64_t j = 0; j < locals.n; j++) {
            locals._tmp_a.append(locals.tmp * (i - j) * (i + j));
            locals._tmp_b.append(locals.tmp * (i + j) * (i - j));
        }
        locals.a.append(locals._tmp_a);
        locals.b.append(locals._tmp_b);
    }
    locals.d = matmul(locals.a, locals.b);
#ifdef IGNORE_CODE
    // do_not_optimize(d);
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
#endif

    STD_FUNCTION_EXIT("");
}

void run_benchmark(int n)
{
    test_matmul(n);
}

std::string benchmark_info()
{
    return "";
}

// mark any global references to heap objects
void gc_mark_roots()
{
   runtime_mark_roots();
   // other globals should be marked here:
}
