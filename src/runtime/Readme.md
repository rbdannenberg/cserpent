The tag of an Any refers to the first 16 bits. We will use the tag to identify the underlying type of an Any.

### Int
Precondition: 49-bit two's complement; in 64-bit ensure top 15 bits are 1's or 0's

Tags (before bias): FFFE - FFFF

### Double
Precondition: not a NaN or canonical, inf, or -inf only

Tags (after bias): 0001 - FFFA

### Pointer
Preconditon: Top 16 bits are 0

Tags: 0000

### Symbols
Tags: FFFA
A Symbol is heap-allocated and contains a string name, function
pointer and global variable value.

### Short Strings
Tags: FFFC
A short string has up to 5 bytes followed by and EOS (zero) byte.

### Strings
Tags: FFFD
A String is heap-allocated and contains a std::string containing
an immutable string. An Any with tag FFFD is a pointer to the String.
