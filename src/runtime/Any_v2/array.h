// array.h -- interface for Array class


// Array_data contains the actual array elements. Arrays point to their
// data so that the data can be replaced when the array expands
class Array_data : public Basic_obj {
  public:
    int64_t slots;      // how many elements are allocated?
    AnyC elements[12];  // 12 is arbitrary. Length may be smaller or larger.
};


class Array : public Obj {
  public:
    int64 len;    // array length
    Array_data *data;

    // allocate an array and data:
    static Array *create(int64_t len, int64_t max_len);
    void set_array_len(int64_t alen);  // set the length
    void set_array_data(Array_data *p);  // set the data pointer
    void append(Array *array);  // append array to this Array
    void clear();  // sets length to zero
    Array *copy(); // shallow copy
    int count(AnyC value);  // how many times is value in data?
    Array *flatten(Array *array); // append all elements of array;
            // each array element found is first flattened (so this
            // performs a depth-first traversal of a tree of Arrays)
    int index(AnyC value);  // index of first occurrence of value
    void insert(int64_t index, AnyC value);  // insert value at index
    AnyC last();  // last element
    void revers();   // reverse elements
    // void sort(??);  // sort the array
    void remove(AnyC value);  // remove first occurrence of value
    Array *subseq(int64_t from, int64_t to);
    AnyC unappend();  // pop the last element
    AnyC uninsert(int64_t from, int64_t to);  // remove range
};
