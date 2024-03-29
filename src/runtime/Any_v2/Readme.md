The tag of an AnyC refers to the first 16 bits. We will use the tag to identify the underlying type of an AnyC.

### Int
Precondition: 50-bit two's complement; in 64-bit ensure top 15 bits are 1's or 0's

Tags (after bias): FFFC - FFFF

### Double
Precondition: Not Nan or inf (Fix!)

Tags (after bias): 0001 - 7FF0 and 8001 - FFF0

### Pointer
Preconditon: Top 16 bits address 0

Tags: 0000

### Left unused tags (after bias)
7FF1 - 7FFF and FFF1 - FFFB

equivalently, they are:

0111,1111,1111,0001 - 0111,1111,1111,1111

1111,1111,1111,0001 - 1111,1111,1111,1011
