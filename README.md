# CppTaggedUnion

A C++ library providing algebraic data types through macro-generated tagged unions with named variants and compile-time type safety.

## Motivation

While C++ provides `std::variant` for sum types, it functions as a type union with positional access, leading to several limitations when building algebraic data types:

### Problems with std::variant

```cpp
// std::variant approach - unclear and error-prone
using MyVariant = std::variant<int, int, std::string>;
MyVariant v1 = 0;        // Which int is this?
MyVariant v2 = 42;       // Index or value?

// Accessing requires knowing positions
int first_int = std::get<0>(v);   // What does position 0 represent?
int second_int = std::get<1>(v);  // What does position 1 represent?

// Pattern matching is verbose
std::visit([](auto&& value) {
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<T, int>) {
        // But which int variant is this?
    }
}, v);

// Anonymous structs not supported
// std::variant<struct { int x, y; }, int> // Error!
```

### CppTaggedUnion Solution

```cpp
// Clear semantic meaning with named variants
UNION(MyUnion 
    , (int, index)     // Same type, different meaning
    , (int, value)     // Clear semantic distinction
    , (std::string, name)
    , (struct { int x; int y; }, point)  // Anonymous structs supported
);

// Self-documenting construction and access
MyUnion u1 = MyUnion::create_index(0);    // Obviously an index
MyUnion u2 = MyUnion::create_value(42);   // Obviously a value

// Clear, safe access
if (u.holds_index()) {
    int idx = u.get_index_ref();  // Unambiguous intent
}

// Readable pattern matching
std::string result = MATCH(u, std::string 
    , CASE(index, idx, { return std::format("Index: {}", idx); })
    , CASE(value, val, { return std::format("Value: {}", val); })
    , CASE(name, n, { return std::format("Name: {}", n); })
    , CASE(point, p, { return std::format("Point({}, {})", p.x, p.y); })
);
```

### Key Advantages

- **Algebraic data types** with named variants rather than positional type unions
- **Type reuse support** - same type can appear multiple times with clear semantic distinctions
- **Self-documenting code** - method names indicate variant meaning
- **Anonymous struct support** - temporary structures can be defined inline
- **LSP type inference** - modern language servers automatically infer variable types in pattern matching
- **Zero-cost abstractions** with compile-time type safety
- **Header-only** implementation requiring only C++20

## Requirements

- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- No external dependencies

## Usage

### Basic Definition

```cpp
#include "tagged_union.hpp"
#include <string>

UNION(Result
    , (int, success)
    , (std::string, error)
);
```

### Basic Example

```cpp
UNION(MyUnion 
    , (int, index) 
    , (int, value) 
    , (std::string, name) 
    , (struct { int x; int y; }, point)  // Anonymous struct support
);

// Clear semantic construction
MyUnion u1 = MyUnion::create_index(0);
MyUnion u2 = MyUnion::create_value(42);
MyUnion u3 = MyUnion::create_point({10, 20});
```

## Interface

The library generates both template and non-template versions of all methods:

### Construction

#### In-place Construction

```cpp
// Direct in-place construction using tag
MyUnion u1(MyUnion::in_place_tag<MyUnion::tag_t::name>, "hello");

// Factory methods (recommended)
MyUnion u2 = MyUnion::create<MyUnion::tag_t::name>("hello");  // Template version
MyUnion u3 = MyUnion::create_name("hello");                   // Non-template version  
```

#### Emplace Methods

```cpp
// Template version - destroys current content and constructs new
u.emplace<MyUnion::tag_t::value>(42);

// Non-template version
u.emplace_value(42);
```

### Access Methods

```cpp
// Template versions
bool has_name = u.holds<MyUnion::tag_t::name>();
std::string* ptr = u.get_ptr<MyUnion::tag_t::name>();
std::string& ref = u.get_ref<MyUnion::tag_t::name>();

// Non-template versions
bool has_name = u.holds_name();
std::string* ptr = u.get_name_ptr();
std::string& ref = u.get_name_ref();
```

### Safety Considerations

**Safe access methods:** `get_ptr()` and `get_*_ptr()` return `nullptr` if the union doesn't hold the requested type.

**Unsafe access methods:** `get_ref()` and `get_*_ref()` provide direct access without safety checks. Using these with the wrong type results in undefined behavior.

```cpp
MyUnion u = MyUnion::create_name("hello");

// Safe - returns nullptr if wrong type
int* safe_ptr = u.get_index_ptr();  // Returns nullptr

// Unsafe - undefined behavior if wrong type  
int& unsafe_ref = u.get_index_ref();  // Undefined behavior!

// Always check first when using get_ref
if (u.holds_index()) {
    int& safe_ref = u.get_index_ref();  // Safe
}
```

## Pattern Matching

### Macro-based Pattern Matching

```cpp
std::string result = MATCH(u, std::string 
    , CASE(index, idx, { return std::format("Index: {}", idx); }) 
    , CASE(value, val, { return std::format("Value: {}", val); }) 
    , CASE(name, name, { return std::format("Name: {}", name); }) 
    , CASE(point, point, { return std::format("Point({}, {})", point.x, point.y); }) 
    , OTHERWISE(x, { return "Unknown"; })
);
```

### Direct Method Calls

#### Using visit()

```cpp
std::string result = u.visit<std::string>([](auto tag, auto&& value) {
    using tag_type = decltype(tag);
    if constexpr (tag_type::value == MyUnion::tag_t::name) {
        return std::format("Name: {}", value);
    } else if constexpr (tag_type::value == MyUnion::tag_t::index) {
        return std::format("Index: {}", value);
    } else {
        return std::string("Other");
    }
});
```

#### Using match()

```cpp
struct Matcher {
    std::string case_name(const std::string& s) { return "Name: " + s; }
    std::string case_index(int i) { return "Index: " + std::to_string(i); }
    std::string otherwise(const auto&) { return "Other"; }
};

std::string result = u.match<std::string>(Matcher{});
```

## LSP Type Inference

Modern language servers like clangd automatically infer types in pattern matching:

```cpp
MyUnion const&& u = ...;
std::string s = MATCH(std::move(u), std::string 
    , CASE(index, idx, { return std::format("Index: {}", idx); })  // idx inferred as int const&&
    , CASE(value, val, { return std::format("Value: {}", val); })  // val inferred as int const&&
    , CASE(name, name, { return std::format("Name: {}", name); })  // name inferred as std::string const&&
    , CASE(point, point, { return std::format("Point({}, {})", point.x, point.y); })  // point inferred as <anonymous type> const&&
    , OTHERWISE(x, { return "Unknown"; })  // x inferred as MyUnion const&&
);
```

## Example: JSON Representation

This example demonstrates the advantages of algebraic data types:

```cpp
UNION(JsonValue
    , (std::nullptr_t, null)
    , (bool, boolean) 
    , (double, number)
    , (std::string, string)
    , (std::vector<JsonValue>, array)
    , (std::map<std::string, JsonValue>, object)
);

auto json = JsonValue::create_string("hello");
if (json.holds_string()) {
    std::cout << json.get_string_ref() << std::endl;  // Clear intent
}
```
