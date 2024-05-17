"""
auto matmul(Any a, Any b) {
    int64_t n = len(a);
    int64_t m = len(b[0]);
    int64_t p = len(a[0]);

    auto c = new Array {};
    for (int64_t i = 0; i < n; i++) {
        // list comprehension
        auto ci = new Array {};
        for (int64_t z = 0; z < m; z++) {
            ci->append(0.0);
        }
        for (int64_t k = 0; k < p; k++) {
            Any aik = a[i][k];
            Any bk = b[k];
            for (int64_t j = 0; j < m; j++) {
                (*ci)[j] = (*ci)[j] + (aik * bk[j]);
                c->append(ci);
            }
        }
    }
    return c;
}

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

"""



from semantic_routines import Sem

def test():
    sem = Sem()
    sem.function_begin("matmul", [["Any", "a"], ["Any", "b"]])
    sem.declare_and_assign_local("n", "Any", "len(a)")
    sem.declare_and_assign_local("m", "Any", "len(b[0])")
    sem.declare_and_assign_local("p", "Any", "len(a[0])")
    sem.declare_and_assign_local("c", "Any", sem.convert_expression(["[", "]"]))
    sem.for_begin("i", "0", "n")
    sem.declare_and_assign_local("ci", "Any", sem.convert_expression(["[", "0.0", "for", "z", "=", "0", "to", "m", "]"]))
    sem.for_begin("k", "0", "p")
    sem.declare_and_assign_local("aik", "Any", "a[i][k]")
    sem.declare_and_assign_local("bk", "Any", "b[k]")
    sem.for_begin("j", "0", "m")
    sem.assign_local(sem.convert_expression(["ci", "[", "j", "]"]), sem.convert_expression(["ci", "[", "j", "]", "+", "(", "aik", "*", "bk", "[", "j", "]", ")"]))
    sem.assign_local("void", sem.convert_expression(["c", "->", "append", "(", "ci", ")"]))
    sem.function_end()
    sem.print_file()





