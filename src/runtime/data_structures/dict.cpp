 /* Dict -- dictionary */

// A Dict inherits from Array and looks like an Array, but it stored key/value
// pairs at alternating even and odd indexes. The entire array is used and is
// initially filled with key values of DICT_EMPTY which is a special signal
// that does not match any actual value that a user could create. This is a
// hash table. Open addressing is used with new probes based on the original
// hash value in an attempt to avoid too much clustering. Deletions are
// accomplished by setting a key to DICT_DELETED so that lookups do not stop
// at the first deleted element (which would happen if instead of DICT_DELETED
// we marked the location as DICT_EMPTY). The Dict is rehashed with 3x the
// space when it gets to be 2/3 full, so we go from 2/3 full to 2/9 full.
// We can now add twice as many new entries before we hash again.

#include <iostream>
#include <sstream>
#include "any.h"
#include "gc.h"
#include "heap_obj.h"
#include "obj.h"
#include "runtime.h"
#include "array.h"
#include "csmem.h"
#include "op_overload.h"
#include "dict.h"

Any DICT_EMPTY;
Any DICT_DELETED;


Dict::Dict(Dict *d)
{
    set_tag(tag_dict);
    get_vector()->resize(len(), DICT_EMPTY);
    used = d->used;
    non_empty = d->non_empty;
    for (int i = 0; i < len(); i++) {
        Array::set(i, (*d)[i]);
    }
}


void Dict::show()
{
    std::vector<Any> *array = get_vector();
    long showlen = array->size();
    for (int i = 0; i < showlen; i += 2) {
        if ((*array)[i].integer == DICT_EMPTY.integer ||
            (*array)[i].integer == DICT_DELETED.integer) continue;
        std::cout << i / 2;
        std::cout << (*array)[i] << ": " << (*array)[i + 1] << std::endl;
    }
}


Array *Dict::get_keys()
{
    struct Frame : public Cs_frame {
        Any result;
    } L;
    constexpr int sl_result = 0;
    memset(&L, 0, sizeof(L));
    STD_FUNCTION_ENTRY(L, 1);
    std::vector<Any> *array = get_vector();
    // len is number of entries * 2
    int64_t slots = array->size() & ~1;  // make it even, round down
    // slots is now how many slots to scan for keys
    Array *result = new Array(used / 2, nil);
    assert(result);
    L.set(sl_result, result);  // save as local to avoid GC
    int j = 0; // where to put key
    for (int i = 0; i < slots; i += 2) {
        Any key = (*array)[i];
        if ((key.integer != DICT_EMPTY.integer) &&
            (key.integer != DICT_DELETED.integer)) {
            assert(j < used / 2);
            result->set(j, key);
            j++;
        }
    }
    assert(j * 2 == used);
    STD_FUNCTION_EXIT(L, result);
}

#ifdef DO_WE_NEED_THIS_FUNCTION
// assuming Dict is the global symbol table, find the variable
// containing val.
Any Dict::find_var_holding(SVal val, Machine_ptr m)
{
    // len is number of entries * 2
    int64_t slots = get_array_room() & ~1;  // make it even, round down
    if (len <= DICT_LINEAR_MAX * 2) {
        slots = len;
    }
    // slots is now how many slots to scan for keys
    for (int i = 0; i < slots; i += 2) {
        SVal key = fastref(i);
        if ((key != m->str_empty) && (key != m->str_deleted)) {
            if (((Symbol_ptr) fastref(i + 1))->value == val) {
                if (key != m->s_previous_value) {
                    return key;
                }
            }
        }
    }
    return NULL;
}
#endif


Array *Dict::get_values()
{
    struct Frame : public Cs_frame {
        Any result;
    } L;
    constexpr int sl_result = 0;
    memset(&L, 0, sizeof(L));
    STD_FUNCTION_ENTRY(L, 1);
    std::vector<Any> *array = get_vector();
    int64_t slots = array->size() & ~1;  // make it even, round down
    Array *result = new Array(used / 2, nil);
    assert(result);
    L.set(sl_result, result);  // save as local to avoid GC
    int j = 0; // where to put the value
    for (int i = 0; i < slots; i += 2) {
        Any key = (*array)[i];
        if ((key.integer != DICT_EMPTY.integer) &&
            (key.integer != DICT_DELETED.integer)) {
            assert(j < used / 2);
            Any value = (*array)[i + 1];
            result->set(j + 1, value);
            j++;
        }
    }
    assert(j == used / 2);
    STD_FUNCTION_EXIT(L, result);
}


void Dict::remove(Any key)
{
    std::vector<Any> *array = get_vector();
    int64_t index = find(key, false);
    if (index >= 0) {
        (*array)[index].integer = DICT_DELETED.integer;
        (*array)[index + 1].integer = 0;  // nullptr
        used -= 2;
        // consider shrinking the dictionary and rehashing
    }
}


void Dict::clear()
{
    used = 0;
    non_empty = 0;
    std::vector<Any> *array = get_vector();
    // slots is now how many slots to scan for keys
    std::fill(array->begin(), array->end(), DICT_EMPTY);
}


int64_t Dict::num_keys()
{
	return used >> 1;
}


// expand -- make dictionary bigger,
//   newlen is number of slots needed (#entries * 2)
//   linear means the dictionary is currently a linear list,
//      otherwise, it is a hash table
//   returns number of slots allocated
int64_t Dict::expand(int64_t newlen)
{
    int64_t oldlen = len();
    std::vector<Any> *myarray = get_vector();
    std::vector<Any> oldarray = std::move(*myarray);
    int64_t nslots = oldlen & ~1; // make it even, round down
    // nslots is now how many slots of data do we scan and insert into newslots?

    // make the new storage for dictionary:
    std::vector<Any> newslots(newlen, DICT_EMPTY);
    *myarray = std::move(newslots);
    used = 0;
    non_empty = 0;
    for (int64_t i = 0; i < nslots; i += 2) {
        Any key = oldarray[i];
        if ((key.integer != DICT_EMPTY.integer) &&
            (key.integer != DICT_DELETED.integer)) {
            int64_t j = find(key, true);
            Array::set(j, key);
            Array::set(j + 1, oldarray[i + 1]);
            used += 2;
            non_empty += 2;
        }
    }
    // what if GC is currently marking *array?
    if (gc_state == GC_MARK_ARRAY && gc_array == this) {
        gc_array_index = 0;  // restart the scan
    }
    
    return newlen;
}

// (5 << 8) + ((5 * 5) << 16) + ((5 * 5 * 5) << 24) + ((5 * 5 * 5 * 5) << 32)
#define SCRAMBLE 2686453351680


uint64_t Dict::compute_string_hash(Any s, const char **str, int64_t *len)
{
    int64_t slen;
    const char *sstr = get_c_str(&s, &slen);
    uint64_t hash = 0;
    int i = 0;
    for (i = 0; i < slen - 4; i += 4) {
        hash = ((hash + *((int32_t *) (sstr + i))) * SCRAMBLE) >> 32;
    }
    if (i < slen) {
        hash = hash * 33 + sstr[i++];
        if (i < slen) {
            hash = hash * 33 + sstr[i++];
            if (i < slen) {
                hash = hash * 33 + sstr[i++];
            }
        }
    }
    if (str) {
        *str = sstr;
        *len = slen;
    }
    return hash;
}

#define I64_HASH(i) \
    ((((i) & ~TAG_MASK) * SCRAMBLE) >> 32);


uint64_t Dict::compute_hash(Any key)
{
    if (is_str(key)) {
        return compute_string_hash(key, nullptr, nullptr);
    } else {
        // pretend it is an integer
        // use SCRAMBLE to randomize high-order bits. Add 1 so we
        return I64_HASH(key.integer);
    }
}

    
std::string debug_str(Dict *x)
{
    std::vector<Any> &data = *(x->get_vector());
    std::stringstream ss;
    ss << "{";
    int64_t slots = data.size() & ~1;
    bool need_comma = false;
    for (int64_t i = 0; i < slots; i += 2) {
        Any key = data[i];  // key is referenced by x, so no need for L.key
        if (need_comma) {
            ss << ", ";
        }
        if (!key.is(DICT_EMPTY) && !key.is(DICT_DELETED)) {
            Any value = data[i + 1];
            ss << key << ":" << value;
        }
        need_comma = true;
    }
    ss << "}";
    return ss.str();
}


#ifdef GCDEBUG
int64_t Dict::find_string(const char *key, Machine_ptr m)
{
    int64_t len1 = strlen(key);
    if (len1 <= MAX_STR_LEN) {  // short string
        SValue s1(key, len1);
        SVal sval1 = s1.enc_str();
        for (int64_t i = 0; i < len; i += 2) {
            if (fastref(i) == sval1) {
                return i;
            }
        }
    } else { // regular string
        for (int64_t i = 0; i < len; i += 2) {
            SVal entry_key = fastref(i);
            SValue s2(entry_key);
            SString_ptr ss2;
            if (GET_STRING(ss2, s2, entry_key)) {
                int64_t len2 = ss2->get_string_bytelen();
                char *str2 = ss2->get_string();
                if ((len1 == len2) &&
                    (memcmp(key, str2, len1) == 0)) {
                    return i;
                }
            }
        }
    }
    return -len;
}
#endif


// return location for key as if we would store something there
//    if inserting is true, put the key into the dictionary if
//       it is not found, otherwise return -1 if not found
//    if linear is true, the dictionary lookup method is linear search
//       otherwise, do a hash table lookup
//    (Normally, the method is linear when there are DICT_LINEAR_MAX or
//    fewer entries, but when the dictionary is reallocated, we may need
//    to build it up from empty using hash inserts.)
//
int64_t Dict::find(Any key, bool inserting)
{
    std::vector<Any> &data = *get_vector();
    uint64_t hash;
    Any entry_key;
    int64_t size = data.size();
    // there is room for size/2 entries, but we want only 2/3 full,
    // so we should allow size/2 * 2/3 entries = size * 2 / 3 slots.
    // non_empty is how many slots we are using. Rehash to 3x size,
    // so if non_empty was 2/3 size, it is now 2/9 size. Now we can
    // add twice as many new entries before we rehash again.
    if (inserting && (non_empty * 3 >= size * 2)) {
        // expand hash table
        size = expand(3 * size);
    }
    size >>= 1; // now size is number of hash table entries
    // hash lookup:
    int64_t index;
    int64_t perturb;
    int64_t deleted_index = -1;
    if (is_str(key)) { // string
        int64_t len1;
        const char *str1;
        hash = compute_string_hash(key, &str1, &len1);
        index = hash % size;
        perturb = hash / size;
        while ((entry_key = data[index * 2]).integer != DICT_EMPTY.integer) {
            if (entry_key.integer == DICT_DELETED.integer) {
                if (deleted_index < 0) deleted_index = index;
            } else if (is_str(entry_key)) {
                int64_t len2;
                const char *str2 = get_c_str(&entry_key);
                if (((len1 == len2) &&
                     (memcmp(str1, str2, (len1 + 3) & (~3))) == 0)) {
                    return index * 2;
                }
            }
            index += 1 + perturb;  // perturb eventually goes to zero, after
                // which we are simply overflowing to the next bin. This
                // ensures that we cannot get into a cycle that does not
                // eventually touch all bins.
            index %= size;
            perturb >>= 5;
            // printf("find: new index %lld\n", index);
        }
    } else { // pointer key, int or float
        index = hash % size;
        perturb = hash / size;
        while ((entry_key = data[index * 2]).integer != DICT_EMPTY.integer) {
            if (entry_key.is(key)) {
                return index * 2;
            }
            if ((deleted_index < 0) &&
                (entry_key.integer == DICT_DELETED.integer)) {
                deleted_index = index;
            }
            index += 1 + perturb;
            index %= size;
            perturb >>= 5;
        }
    }
    if (inserting) {
        if (deleted_index >= 0) {
            index = deleted_index;
        } 
        used += 2;
        non_empty += 2;
        Array::set(index * 2, key);
        return index * 2;
    } else {
        return -1;
    }
}


// insert -- put key/value in dictionary
void Dict::insert(Any key, Any value)
{
    int64_t index = find(key, true);
    assert(index >= 0);
    if (index < 0) return; // fail, returns with error_flag
    Array::set(index + 1, value);
}

/* CSerpent classes are not in symbol table - should they be?
// inverse -- find symbol with a given value
//
// this is used to find names of classes, which are values of symbols
// this method uses knowledge of the layout of Dicts, so if Dicts go
// to a more efficient implementation, this method will break.
//
// Note that this is not the inverse of the dictionary mapping, but 
// the inverse of the machine's global symbols mapping
//
Any Dict::inverse(Any val)
{
    std::vector<Any> &data = *get_vector();
    int64_t dlen = len();
    for (int i = 0; i < dlen; i += 2) {
        Any key = data[i];
        if ((key.integer != DICT_EMPTY.integer) &&
            (key.integer != DICT_DELETED.integer)) {
            // symbols will have an exact match
            if (data[i + 1].integer == val.integer) {
                return key;
            }
        }
    }
    return nil;
}
*/

Dict *Dict::copy()
{
    return new Dict(this);
}
