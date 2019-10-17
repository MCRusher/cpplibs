# cpplibs

1. simpleio
  - Implements io operations in a style similar to Java/C#/Pascal using templates and overloads.
  - Includes wide versions of each function family specified by "w" prefix.
  - input functions have two modes:
    * io::mode::RETRY, which is the default and requests a newline after invalid input
    * io::mode::THROW, which is passed via template parameter and throws io::ioex on invalid input

2. cxmem
- wraps C allocation functions in a type safe and class safe set of functions.
- like new, they are not compatible with other allocation strategies (new and malloc, delete and free, etc)
- similar functionality as new, but with added benefit of being resizable.
- mem::alloc has a basic void* overload which can be used as a replacement for std::malloc, and guarantees sizeof(void)==1
  * accepts a mem::rawset template parameter 
    - mem::rawset::NONE, is the default and calls malloc
    - mem::rawset::ZERO zeroes the memory
- mem::alloc<T> calls constructors 
  * Can construct from an initializer list, which accepts a mem::mode template argument
    - mem::mode::COPY, is the default and copy constructs from the list
    - mem::mode::MOVE, attempts a const move construct, same as COPY if T does not implement a const move constructor
    - mem::mode::UB_MOVE. const_casts and attempts move construct via undefined behavior, should not be used and exists for testing purposes only.
  * Can emplace construct N elements from variadic args, all to the same values
- mem::dealloc null sets argument unlike free and delete
- mem::dealloc<void> has basic void* specialization which can be used as a replacement for std::free
- mem::dealloc<T> calls destructors
- mem::size<T> gets the element count of an array allocated with mem::alloc. undefined behavior if array was allocated otherwise.
- mem::grow has basic void* overload which can be used to increase arr by N elements (similar to upwards realloc)
  * accepts a mem::rawset template parameter 
    - mem::rawset::NONE, is the default and does not zero new bytes
    - mem::rawset::ZERO zeroes new bytes
- mem::grow<T> calls constructors on new elements
  * Can construct from an initializer list, which accepts a mem::mode template argument
    - mem::mode::COPY, is the default and copy constructs from the list
    - mem::mode::MOVE, attempts a const move construct, same as COPY if T does not implement a const move constructor
    - mem::mode::UB_MOVE. const_casts and attempts move construct via undefined behavior, should not be used and exists for testing purposes only.
  * Can emplace construct N elements from variadic args, all to the same values
- mem::shrink<void> has basic void* specialization which shrinks an array by N elements (similar to downwards realloc)
- mem::shrink<T> calls destructors for removed elements
- mem::copy(arr,src) has basic void* overload that copies mem::size(arr) bytes from src to arr
- mem::copy(arr) has basic void* overload that allocates, sets, and returns a complete copy of arr
- mem::copy<T>(arr,src) destructs original elements and copy constructs mem::size(arr) elements from src to arr
- mem::copy<T>(arr) allocates, copy constructs, and returns a complete copy of arr
- mem::move<T>(arr,src) destructs original elements and move constructs mem::size(arr) elements from src to arr
- mem::set<T> destructs original elements and sets them to new values
  * Can emplace construct N elements from variadic args, all to the same values
    - requires start position and number of elements to replace before variadic args
  * Can construct from an initializer list, which accepts a mem::mode template argument, and optionally accepts a start pos arument
    - mem::mode::COPY, is the default and copy constructs from the list
    - mem::mode::MOVE, attempts a const move construct, same as COPY if T does not implement a const move constructor
    - mem::mode::UB_MOVE. const_casts and attempts move construct via undefined behavior, should not be used and exists for testing purposes only
