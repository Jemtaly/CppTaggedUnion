# CppTaggedUnion

A C++ library providing algebraic data types through macro-generated tagged unions. Unlike std::variant which functions as a type union, this library creates true algebraic data types with named variants, offering superior readability and ease of use when the same type appears multiple times.

## Key Advantages

- **Algebraic data types** rather than simple type unions
- **Named variants** for better code readability and self-documentation  
- **Type reuse support** - same type can appear multiple times with different semantic meanings
- **Anonymous struct support** - temporary structures can be defined inline
- **LSP type inference** - modern language servers like clangd automatically infer variable types in pattern matching
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

### Type Reuse Example

The same type can appear multiple times with different semantic meanings:

```cpp
UNION(MyUnion 
    , (int, index) 
    , (int, value) 
    , (std::string, name) 
    , (struct { int x; int y; }, point)  // Anonymous struct support
);

// Clear semantic access
MyUnion u1 = MyUnion::create_index(0);
MyUnion u2 = MyUnion::create_value(42);

// With std::variant, same types require positional access
using MyVariant = std::variant<int, int, std::string, /* anonymous struct not supported */>;
// Problem: Which int is which? Must use indices std::get<0>() vs std::get<1>()
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

## Comparison with std::variant

| Feature | CppTaggedUnion | std::variant |
|---------|----------------|--------------|
| Data model | Algebraic data type with named variants | Type union with positional access |
| Type reuse | Supports same type multiple times with clear semantics | Same type multiple times requires positional indices |
| Access syntax | Named methods: `get_name_ref()` | Positional access: `std::get<0>()` or `std::get<std::string>()` |
| Pattern matching | Built-in `MATCH` macro + direct methods | Requires `std::visit` with complex syntax |
| Anonymous structs | Supported inline | Not supported |
| LSP integration | Automatic type inference in pattern matching | Manual type annotations required |
| Readability | Self-documenting with semantic names | Positional indices obscure meaning |

## Example: JSON Representation

This example demonstrates the advantages of algebraic data types:

```cpp
// With CppTaggedUnion - clear semantic meaning
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

// With std::variant - positional access obscures meaning
using JsonVariant = std::variant<std::nullptr_t, bool, double, std::string, 
                                std::vector<JsonVariant>, std::map<std::string, JsonVariant>>;

auto json = JsonVariant{std::string("hello")};
if (std::holds_alternative<std::string>(json)) {
    std::cout << std::get<std::string>(json) << std::endl;  // Which std::string variant?
}
```
