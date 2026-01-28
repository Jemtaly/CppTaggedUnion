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
auto u1 = MyUnion::create_index(0);    // Obviously an index
auto u2 = MyUnion::create_value(42);   // Obviously a value

// Clear, safe access
if (u.holds_index()) {
    int idx = u.get_index_ref();  // Unambiguous intent
}

// Readable pattern matching
auto result = MATCH(std::string, u,
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
auto u1 = MyUnion::create_index(0);
auto u2 = MyUnion::create_value(42);
auto u3 = MyUnion::create_point({10, 20});
```

### Template

```cpp
template<typename T>
UNION(Option
    , (T, some)
    , (struct {}, none)
);

template<typename T, typename E = std::string>
UNION(Result
    , (T, success)
    , (E, error)
);

// Usage
auto o1 = Option<double>::create_some(3.14);
auto r1 = Result<int>::create_success(100);
```

### Special Cases

The library always generates copy/move constructors and assignment operators. Therefore, if any variant type is non-copyable or non-movable, you need to use a workaround:

```cpp
// This will NOT compile because std::unique_ptr is non-copyable, but the UNION macro
// will always attempt to generate copy constructors and assignment operators.
// UNION(NonCopyableUnion
//     , (std::unique_ptr<int>, ptr)
//     , (std::string, name)
// );

// To work around this, we define a template with a dummy parameter, so the compiler
// will only complain about the missing copy operations when you call them explicitly.
template<typename Dummy = void>
UNION(NonCopyableUnionTemplate
    , (std::unique_ptr<int>, ptr)
    , (std::string, name)
);
using NonCopyableUnion = NonCopyableUnionTemplate<>;
```

## Interface

The library generates both template and non-template versions of all methods:

### Construction

#### In-place Construction

```cpp
// Direct in-place construction using tag
MyUnion u1(tu::in_place_tag<MyUnion::tags::name>, "hello");

// Factory methods (recommended)
MyUnion u2 = MyUnion::create<MyUnion::tags::name>("hello");  // Template version
MyUnion u3 = MyUnion::create_name("hello");                   // Non-template version 
```

#### Emplace Methods

```cpp
// Template version - destroys current content and constructs new
u.emplace<MyUnion::tags::value>(42);

// Non-template version
u.emplace_value(42);
```

### Access Methods

```cpp
// Template versions
bool has_name = u.holds<MyUnion::tags::name>();
std::string* ptr = u.get_ptr<MyUnion::tags::name>();
std::string& ref = u.get_ref<MyUnion::tags::name>();

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
auto result = MATCH(std::string, u
    , CASE(index, idx, { return std::format("Index: {}", idx); })
    , CASE(value, val, { return std::format("Value: {}", val); })
    , CASE(name, name, { return std::format("Name: {}", name); })
    , CASE(point, point, { return std::format("Point({}, {})", point.x, point.y); })
    , OTHERWISE(x, { return "Unknown"; })
);
```

### Direct Method Calls

#### Using match()

```cpp
struct Matcher {
    std::string case_name(const std::string& s) { return "Name: " + s; }
    std::string case_index(int i) { return "Index: " + std::to_string(i); }
    std::string otherwise(const auto&) { return "Other"; }
};

std::string result = u.match<std::string>(Matcher{});
```

#### Using visit()

```cpp
std::string result = u.visit<std::string>([](auto tag, auto&& value) {
    using tag_type = decltype(tag);
    if constexpr (tag_type::value == MyUnion::tags::name) {
        return std::format("Name: {}", value);
    } else if constexpr (tag_type::value == MyUnion::tags::index) {
        return std::format("Index: {}", value);
    } else {
        return std::string("Other");
    }
});
```

### Exhaustiveness Checking

The pattern matching constructs enforce exhaustiveness at compile-time. Therefore, at least one of the following conditions must be met:

- All of the union's variants are covered by `CASE(...)` / `case_<tag>(...)` / specific visitor methods;
- An `OTHERWISE(...)` / `otherwise(...)` / default visitor method is provided;
- The return type of the pattern matching expression is `void`.

Otherwise, a compile-time error will occur.

## LSP Type Inference

Modern language servers like clangd automatically infer types in pattern matching:

```cpp
MyUnion const&& u = ...;
auto s = MATCH(std::string, std::move(u)
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
struct JsonValue;

using JsonArray = std::vector<JsonValue>;
using JsonObject = std::map<std::string, JsonValue>;

UNION(JsonValue
    , (std::nullptr_t, null)
    , (bool, boolean)
    , (double, number)
    , (std::string, string)
    , (JsonArray, array)
    , (JsonObject, object)
);

auto json = JsonValue::create_string("hello");
if (json.holds_string()) {
    std::cout << json.get_string_ref() << std::endl;  // Clear intent
}
```
