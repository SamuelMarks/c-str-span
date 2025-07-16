c-str-span
==========

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Linux, Windows, macOS](https://github.com/SamuelMarks/c-str-span/actions/workflows/linux-Windows-macOS.yml/badge.svg)](https://github.com/SamuelMarks/c-str-span/actions/workflows/linux-Windows-macOS.yml)
![coverage](reports/test_coverage.svg)
[![C89](https://img.shields.io/badge/C-89-blue)](https://en.wikipedia.org/wiki/C89_(C_version))

UTF-8 replacement for C strings, supporting zero-copy use-cases (non-null-terminated).

Extracted from https://github.com/Azure/azure-sdk-for-c @ [`ac28b8`](https://github.com/Azure/azure-sdk-for-c/tree/ac28b8af9254798c1e891945e896d2ae2bdf745e) with:
  - No dependency on other Azure SDK for C functionalities;
  - C89 (ANSI C) compliance;
  - CTest integration;
  - More `const` correctness;
  - Use `size_t` over `int32_t` for size types;
  - Implement `az_span_printf`;
  - Added support for MSVC 2005, Open Watcom (incl. DOS target), MinGW, and Cygwin

Documentation originally from: https://github.com/Azure/azure-sdk-for-c/tree/main/sdk/docs/core#working-with-spans, now below:

### Working with Spans

An `az_span` is a small data structure (defined in the [c_str_span.h](c_str_span/c_str_span.h) file) wrapping a byte buffer. Specifically, an `az_span` instance contains:

  - a byte pointer
  - an integer size

Use this to create SDKs that pass `az_span` instances to functions to ensure that a buffer's address and size are always passed together; this reduces the chance of bugs. And, since we have the size, operations are fast; for example, we never need to call `strlen` to find the length of a string in order to append to it. Furthermore, when your SDK functions write or copy to an `az_span`, those functions ensure that we never write beyond the size of the buffer; this prevents data corruption. And finally, when reading from an `az_span`, we never read past the `az_span`'s size ensuring that we don't process uninitialized data.

Since your SDK functions require `az_span` parameters, customers must know how to create `az_span` instances so that you can call functions in their SDK. Here are some examples.

Create an empty `az_span`:

```c
az_span empty_span = az_span_empty(); // size = 0
```

Create an `az_span` expression from a byte buffer:

```c
uint8_t buffer[1024];
some_function(AZ_SPAN_FROM_BUFFER(buffer));  // size = 1024
```

Create an `az_span` expression from a string (the span does NOT include the 0-terminating byte):

```c
some_function(AZ_SPAN_FROM_STR("Hello"));  // size = 5
```

As shown above, an `az_span` over a string does not include the 0-terminator. If you need to 0-terminate the string, you can call this function to append a 0 byte (if the span's size is large enough to hold the extra byte):

```c
az_span az_span_copy_u8(az_span destination, uint8_t byte);
```

and then call this function to get the address of the 0-terminated string:

```c
char* str = (char*) az_span_ptr(span); // str points to a 0-terminated string
```

Or, you can call this function to copy the string in the `az_span` to your own `char*` buffer; this function will 0-terminate the string in the `char*` buffer:

```c
void az_span_to_str(char* destination, size_t destination_max_size, az_span source);
```

There are many functions to manipulate `az_span` instances. You can slice (subset an `az_span`), parse an `az_span` containing a string into a number, format a number as a string into an `az_span`, check if two `az_span` instances are equal or the contents of two `az_span` instances are equal, and more.

### Strings

A string is a span of UTF-8 characters. It's not a zero-terminated string. Defined in [c_str_span.h](c_str_span/c_str_span.h).

```c
az_span hello_world = AZ_SPAN_FROM_STR("Hello world!");
```

---

### License

MIT
