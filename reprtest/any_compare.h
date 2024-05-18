// any_compare.h - shared declarations

typedef uint64_t Any;

#define INT_TAG 0xFFFC000000000000uLL
#define ANY_IS_INT(a) (((uint64_t) (a) & INT_TAG) == INT_TAG)
#define ANY_AS_INT(a) ((int64_t) (((a) << 14) >> 14))
#define INT2ANY(a) ((a) | INT_TAG)

typedef union { int64_t si; uint64_t ui; double d; } Anyu;
extern Anyu anyu_temp;
#define ANYU_IS_INT(a) (((a).ui & INT_TAG) == INT_TAG)
#define ANYU_AS_INT(a) (((a).si << 14) >> 14)
// This was my first try:
#define INT2ANYU(a) (anyu_temp.si = (int64_t) ((a) | INT_TAG), anyu_temp)

// This would be better if we could treat integer a as an Anyu:
// #define INT2ANYU(a) ((Anyu) ((a) | INT_TAG))

// This implementation uses inline constructor for union (a little faster),
// but you cannot execute "x = INT2ANYU(0)" because you can only use this
// kind of literal union constructor if you are declaring x:
// #define INT2ANYU(a) { (int64_t) ((a) | INT_TAG) }

// But in this version, we create a constructor for Anyu2 that can do the
// work without a global anyu_temp:
union Anyu2 {
    int64_t si;
    uint64_t ui;
    double d;
    Anyu2(int64_t a) : si(a | INT_TAG) { }
};

#define ANYU2_IS_INT(a) (((a).ui & INT_TAG) == INT_TAG)
#define ANYU2_AS_INT(a) (((a).si << 14) >> 14)
#define INT2ANYU2(a) Anyu2(a)

int64_t addone_baseline(int64_t a);
Any addone_uint(Any a);
Anyu addone_union(Anyu a);
Anyu2 addone_union2(Anyu2 a);
