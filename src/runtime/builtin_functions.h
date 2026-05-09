//
// Created by anthony on 5/17/24.
//
#include <limits>


int64_t len(Any x);
//int64_t len(String x);


// int64_t len(Any x);
// int64_t len(CSString x);
Any max(Any lhs, Any rhs);
int64_t pow(int base, Any exp);

int64_t idiv(Any lhs, int rhs);

int64_t find(Any s, Any pattern,  Any start, Any end);
int64_t find(Any s, Any pattern,  Any start,
             int64_t end = std::numeric_limits<int64_t>::max());
int64_t find(Any s, Any pattern,  int64_t start = 0,
             int64_t end = std::numeric_limits<int64_t>::max());

int64_t find(Any s, StringPtr pattern, Any start, Any end);
int64_t find(Any s, StringPtr pattern, Any start,
             int64_t end = std::numeric_limits<int64_t>::max());
int64_t find(Any s, StringPtr pattern, int64_t start = 0,
             int64_t end = std::numeric_limits<int64_t>::max());

int64_t find(Any s, const char *pattern, Any start, Any end);
int64_t find(Any s, const char *pattern, Any start,
             int64_t end = std::numeric_limits<int64_t>::max());
int64_t find(Any s, const char *pattern, int64_t start = 0,
             int64_t end = std::numeric_limits<int64_t>::max());

int64_t find(StringPtr s, Any pattern, Any start, Any end);
int64_t find(StringPtr s, Any pattern, Any start,
             int64_t end = std::numeric_limits<int64_t>::max());
int64_t find(StringPtr s, Any pattern, int64_t start = 0,
             int64_t end = std::numeric_limits<int64_t>::max());

int64_t find(StringPtr s, const char *pattern, Any start, Any end);
int64_t find(StringPtr s, const char *pattern, Any start,
             int64_t end = std::numeric_limits<int64_t>::max());

int64_t find(const char *s, const char *pattern, Any start, Any end);
int64_t find(const char *s, const char *pattern, Any start,
             int64_t end = std::numeric_limits<int64_t>::max());


Any subseq(Any s, Any start, Any end);
Any subseq(Any s, int64_t start, Any end);
Any subseq(Any s, Any start,
           int64_t end = std::numeric_limits<int64_t>::max());
Any subseq(Any s, int64_t start,
           int64_t end = std::numeric_limits<int64_t>::max());
StringPtr toupper(Any s);
Any is_equal(Any lhs, Any rhs);

Any apply(Symbol function, Array *argarray);
Any sendapply(Obj& obj, Symbol method, Array *argarray);
Any sendapply(Any obj, Symbol method, Array *argarray);
