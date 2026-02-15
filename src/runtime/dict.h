/* Dict -- dictionary */

// dictionary based on dict.cpp and dict.h in serpent64.
// stores data in an array where even entries are keys, odd ones are values

// These defines turn on some debugging, allows you to trace a key
//   through inserts, finds, and removes
//#define DDB 1
//#define DDB_KEY ((Node_ptr) 0x0000000100400040)

// This is the min size of array_data: 2 dict entries
#define MIN_DICT_SLOTS 4

// Hashing: See dict.cpp for hash function and overflow details.
// We store data in pairs (even/odd index values). 2n has the key
// and 2n+1 has the data. nil is an acceptable dictionary key.
// We need special values for DICT_EMPTY and DICT_DELETED
// DICT_EMPTY as a key marks an unused hash table location,
// DICT_DELETED as a key marks a location where the value has been
// removed from the hash table. Deleted locations are necessary
// because of the way we handle overflow -- when doing a lookup, we
// keep probing until we find an "empty" location. If deleted
// elements became "empty", it might look like an overflow chain
// has ended, and entries in the chain might be lost.
//
// DICT_EMPTY and DICT_DELETED are be represented by two invalid
// string pointers (1 and 2) because GC will not try to mark them
// (they point to strings).
//
// Deletion is a special problem: Since we replace deleted entries
// special markers, the empty spaces in the hash table can all 
// become filled. Then, insertion or lookup will fail if the key is
// missing because we detect that a key is absent by finding an empty
// slot. To solve this problem, we keep a count (non_empty) in addition
// to len which contains the number of keys + deleted slots. When we
// insert and the keys+deleted > 2/3 of the slots, we rehash, eliminating
// the deleted slots. The cost of a deletion is therefore the cost of
// rehashing everything, but we can do N/3 inserts and deletes before
// rehashing, which costs about N/3 inserts and initializes N slots.
// Therefore the amortized insertion and deletion costs are still
// constant.

typedef class Dict *Dict_ptr;

extern Any DICT_EMPTY;
extern Any DICT_DELETED;

class Dict : public Array {
public:
    int64_t used;  // how many keys + values are there in the dict?
    int64_t non_empty;  // how many keys + values are there if you
                        // count deleted keys and values too?

    static uint64_t compute_hash(Any key);
    static uint64_t compute_string_hash(Any s, const char **str, int64_t *len);
    int64_t expand(int64_t newlen);

    Dict() {
        set_tag(tag_dict);
        get_vector()->resize(4, DICT_EMPTY);
        used = 0;
        non_empty = 0;
    }

    Dict(Dict *);

    int64_t find(Any key, bool inserting);
    
    void set(int64_t index, Any value) {
        assert(false);  // never call set on Dict - maybe you mean insert()?
    }

    void insert(Any key, Any value);

    bool has_key(Any key) {
        return find(key, false) >= 0; }

    Any &lookup(Any key) {
        int64_t index = find(key, false);
        assert(index >= 0);
        return (*get_vector())[index + 1]; }

    bool bounds_check(int64_t index) {
        return (index >= 0 && index < get_vector()->size()); }

    Array *get_keys();

    // used only for class lookup in symbol table:
    Symbol *find_var_holding(Symbol *val);

    Array *get_values();

    void clear();

    void remove(Any key);

    int64_t num_keys();

    Any inverse(Any val);

    Dict_ptr copy();

    void show_val(int64_t i);

    void show();

    friend std::string debug_str(Dict *x);

#ifdef GCDEBUG
    // special function used in debugging only
    int64 find_string(const char *key);
#endif
};
