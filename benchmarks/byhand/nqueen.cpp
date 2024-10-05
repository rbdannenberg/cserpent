#include "any.h"
#include "data_structures/array.h"
#include "op_overload.h"
#include <iostream>
#include "benchmarking_utils.h"

Any helper (Any i, Any y, Any n) {
    while (i < n) {
        if ((y & 1 << i) == 0) {
            return i;
        }
        i = i + 1;
    }
}

std::string nq_solve(Any n) {
    Any m = 0;
    Any a = *(new Array(n, -1));
    Any l = *(new Array(n, 0));
    Any c = *(new Array(n, 0));
    Any r = *(new Array(n, 0));
    Any y0 = (1 << n) - 1;
    Any k = 0;
    Any loops = 0;
    while (k >= 0) {
        Any y = (l[k] | c[k] | r[k]) & y0;
        if (((y ^ y0) >> (a[k] + 1)) != 0) {
            Any i = a[k] + 1;
            while ((i < n) && ((y & (1 << i)) != 0)) {
                i = i + 1;
            }
            if (k < n - 1) {
                Any z = 1 << i;
                a.set(k, i);
//                a[k] = i;
                k = k + 1;
                l.set(k, (l[k - 1] | z) << 1);
                c.set(k, c[k - 1] | z);
                r.set(k, (r[k - 1] | z) >> 1);
//                l[k] = (l[k - 1] | z) << 1;
//                c[k] = c[k - 1] | z;
//                r[k] = (r[k - 1] | z) >> 1;
            } else {
                m = m + 1;
                k = k - 1;
            }
        } else {
            a.set(k, -1);
//            a[k] = -1;
            k = k - 1;
        }
        loops = loops + 1;
    }
    //do_not_optimize(m);
    std::string result = std::to_string(to_int(m));
#ifdef FREE
    delete reinterpret_cast<Array*>(to_heap_obj(a));
    delete reinterpret_cast<Array*>(to_heap_obj(l));
    delete reinterpret_cast<Array*>(to_heap_obj(c));
    delete reinterpret_cast<Array*>(to_heap_obj(r));
#endif
    return result;
}
