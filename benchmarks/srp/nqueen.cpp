#include <any.h>
#include <data_structures/array.h>

Any helper (Any i, Any y, Any n) {
    while (i < n) {
        if ((y & 1 << i) == 0) {
            return i;
        }
        i = i + 1;
    }
}

Any nq_solve(Any n) {
    Any m = 0;
    Any a = Array (-1, n);
    Any l = Array (0, n);
    Any c = Array (0, n);
    Any r = Array (0, n);
    Any y0 = (1<<n) - 1;
    Any k = 0;
    Any loops = 0;
    while (k >= 0) {
        if (k == n) {
            m = m + 1;
            k = k - 1;
        } else {
            if (k == 0) {
                l = Array (0, n);
                c = Array (0, n);
                r = Array (0, n);
            }
            l[k] = helper(0, y0, n);
            if (l[k] < n) {
                y0 = y0 ^ 1 << l[k];
                c[k] = helper(0, y0, n);
                if (c[k] < n) {
                    y0 = y0 ^ 1 << c[k];
                    r[k] = helper(0, y0, n);
                    if (r[k] < n) {
                        y0 = y0 ^ 1 << r[k];
                        k = k + 1;
                    } else {
                        k = k - 1;
                    }
                } else {
                    k = k - 1;
                }
            } else {
                k = k - 1;
            }
        }
        loops = loops + 1;
    }
}