
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
#include "gc.h"
#include "heap_obj.h"
#include "obj.h"
#include "runtime.h"
#include "data_structures/array.h"
#include "op_overload.h"
#include "builtin_functions/builtin_functions.h"
#include <iostream>
#include "benchmarking_utils.h"


// a is a (Array *)
Any matmul(Any a_, Any b_) {
    struct Frame : public Cs_frame {
        Any a;
        Any b;
        Any n;
        Any m;
        Any p;
        Any c;
        Any ci;
        Any aik;
        Any bk;
    } L;
    constexpr int SL_a = 0;
    constexpr int SL_b = 1;
    constexpr int SL_n = 2;
    constexpr int SL_m = 3;
    constexpr int SL_p = 4;
    constexpr int SL_c = 5;
    constexpr int SL_ci = 6;
    constexpr int SL_aik = 7;
    constexpr int SL_bk = 8;
    memset(&L, 0, sizeof(L));
    STD_FUNCTION_ENTRY(L, 9);
    L.set(SL_a, a_);
    L.set(SL_b, b_);
    L.set(SL_n, len(L.a));
    L.set(SL_m, len(L.b[0]));
    L.set(SL_p, len(L.a[0]));
    L.set(SL_c, new Array {});

    for (int64_t i = 0; i < L.n; i++) {
        // list comprehension
        L.set(SL_ci, new Array(as_int(L.m), 0.0));
        for (int64_t k = 0; k < L.p; k++) {
            L.set(SL_aik, L.a[i][k]);
            L.set(SL_bk, L.b[k]);
            for (int64_t j = 0; j < L.m; j++) {
                L.ci.set(j, L.ci[j] + (L.aik * L.bk[j]));
            }
        }
        L.c.append(L.ci);
    }
    STD_FUNCTION_EXIT(L, L.c);
}


std::string test_matmul(Any n_) {
    struct Frame : public Cs_frame {
        Any n;
        Any tmp;
        Any a;
        Any b;
        Any _tmp_a;
        Any _tmp_b;
        Any d;
    } L;
    constexpr int sl_n = 0;
    constexpr int sl_tmp = 1;
    constexpr int sl_a = 2;
    constexpr int sl_b = 3;
    constexpr int sl__tmp_a = 4;
    constexpr int sl__tmp_b = 5;
    constexpr int sl_d = 6;
    memset(&L, 0, sizeof(L));
    STD_FUNCTION_ENTRY(L, 7);
    L.set(sl_n, n_);
    L.set(sl_tmp, 1.0 / (L.n * L.n));
    L.set(sl_a, new Array);
    L.set(sl_b, new Array);

    for (int64_t i = 0; i < L.n; i++) {
        L.set(sl__tmp_a, new Array);
        L.set(sl__tmp_b, new Array);
        for (int64_t j = 0; j < L.n; j++) {
            L._tmp_a.append(L.tmp * (i - j) * (i + j));
            L._tmp_b.append(L.tmp * (i + j) * (i - j));
        }
        L.a.append(L._tmp_a);
        L.b.append(L._tmp_b);
    }
    Any test = L.a[0][0];
    // std::cout << "matmul " << L.a << " " << L.b << std::endl;
    L.set(sl_d, matmul(L.a, L.b));
#ifdef IGNORE_CODE
    // do_not_optimize(d);
    /// This is an optimization blocker, makes it harder to inline and lets us check correctness
    std::string result = std::to_string(to_real(d[static_cast<int64_t>(n) / 2][static_cast<int64_t>(n) / 2]));
    return result;
#endif

    STD_FUNCTION_EXIT(L, "");
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
