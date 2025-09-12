# CppTaggedUnion

A modern C++ library that provides **Rust-style tagged unions** (also known as sum types or variant types) through a powerful macro-based API. This library allows you to create type-safe unions with pattern matching capabilities, similar to Rust's `enum` types.

## Features

- 🦀 **Rust-inspired syntax** - Clean, declarative union definitions
- 🔒 **Type-safe** - Compile-time guarantees with no runtime overhead
- 🎯 **Pattern matching** - Powerful `MATCH` macro for handling different cases
- ⚡ **Zero-cost abstractions** - No virtual functions or dynamic allocation
- 🔧 **Modern C++** - Requires C++20 for advanced template features
- 📦 **Header-only** - Just include and use, no compilation required

## Requirements

- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- No external dependencies

> **⚠️ Note**: This library is currently in development. Some compilation issues may exist with certain compiler versions. The API documentation below represents the intended interface.

## Building and Testing

Since this is a header-only library, no build step is required. Simply include the header:

```cpp
#include "tagged_union.hpp"
```

To test the examples, compile with C++20 support:

```bash
g++ -std=c++20 -Wall -Wextra your_file.cpp -o your_program
```

### Running the Example

The repository includes an `example.cpp` file demonstrating basic usage:

```bash
g++ -std=c++20 -Wall -Wextra example.cpp -o example && ./example
```

### Basic Example

Define a tagged union using the `UNION` macro:

```cpp
#include "tagged_union.hpp"
#include <string>
#include <iostream>

// Define a union type with multiple variants
UNION(Result
    , (int, success)
    , (std::string, error)
);

int main() {
    // Create instances using static factory methods
    auto good_result = Result::create_success(42);
    auto bad_result = Result::create_error("Something went wrong");
    
    // Type-safe checking and access
    if (good_result.holds_success()) {
        std::cout << "Success: " << good_result.get_success_ref() << std::endl;
    }
    
    return 0;
}
```

### Pattern Matching Example

```cpp
auto result = MATCH(value, std::string
    , CASE(success, val, { 
        return "Got value: " + std::to_string(val); 
    })
    , CASE(error, err, { 
        return "Error: " + err; 
    })
);
```

## API Reference

### Defining Unions

Use the `UNION` macro to define a new tagged union type:

```cpp
UNION(TypeName
    , (Type1, variant1_name)
    , (Type2, variant2_name)
    , (Type3, variant3_name)
    // ... more variants
);
```

### Generated Methods

For each union with variants, the following methods are automatically generated:

#### Factory Methods
```cpp
// Static factory methods for each variant
static TypeName create_variant_name(Args&&... args);

// In-place construction
TypeName(in_place_tag_t<tag_t::variant_name>, Args&&... args);
```

#### Access Methods
```cpp
// Check which variant is active
tag_t get_tag() const;
bool holds_variant_name() const;

// Safe pointer access (returns nullptr if wrong variant)
VariantType* get_variant_name_ptr();
const VariantType* get_variant_name_ptr() const;

// Reference access (undefined behavior if wrong variant)
VariantType& get_variant_name_ref() &;
const VariantType& get_variant_name_ref() const &;
VariantType&& get_variant_name_ref() &&;

// Emplacement (destroys current value and constructs new one)
VariantType& emplace_variant_name(Args&&... args);
```

### Pattern Matching

Use the `MATCH` macro for exhaustive pattern matching:

```cpp
auto result = MATCH(union_value, ReturnType
    , CASE(variant1, var, { /* handle variant1 with variable 'var' */ })
    , CASE(variant2, var, { /* handle variant2 with variable 'var' */ })
    , OTHERWISE(var, { /* default case */ })
);
```

## Advanced Examples

### Complex Data Types

```cpp
#include "tagged_union.hpp"
#include <vector>
#include <memory>

// Union with complex types
UNION(JsonValue
    , (std::nullptr_t, null)
    , (bool, boolean)
    , (double, number)
    , (std::string, string)
    , (std::vector<JsonValue>, array)
    , (std::map<std::string, JsonValue>, object)
);

// Custom structs as variants
struct Point { int x, y; };
struct Circle { Point center; int radius; };
struct Rectangle { Point top_left, bottom_right; };

UNION(Shape
    , (Point, point)
    , (Circle, circle)
    , (Rectangle, rectangle)
);

int main() {
    auto shape = Shape::create_circle({{0, 0}, 5});
    
    auto area = MATCH(shape, double
        , CASE(point, p, { return 0.0; })
        , CASE(circle, c, { return 3.14159 * c.radius * c.radius; })
        , CASE(rectangle, r, { 
            int width = r.bottom_right.x - r.top_left.x;
            int height = r.bottom_right.y - r.top_left.y;
            return static_cast<double>(width * height);
        })
    );
    
    std::cout << "Area: " << area << std::endl;
    return 0;
}
```

### Error Handling Pattern

```cpp
#include "tagged_union.hpp"
#include <string>
#include <fstream>

UNION(FileResult
    , (std::string, contents)
    , (std::string, io_error)
    , (std::string, permission_error)
);

FileResult read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return FileResult::create_io_error("Could not open file: " + filename);
    }
    
    if (!file.good()) {
        return FileResult::create_permission_error("Permission denied: " + filename);
    }
    
    std::string contents((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    return FileResult::create_contents(contents);
}

int main() {
    auto result = read_file("example.txt");
    
    MATCH(result, void
        , CASE(contents, data, {
            std::cout << "File contents: " << data << std::endl;
        })
        , CASE(io_error, err, {
            std::cerr << "IO Error: " << err << std::endl;
        })
        , CASE(permission_error, err, {
            std::cerr << "Permission Error: " << err << std::endl;
        })
    );
    
    return 0;
}
```

## Comparison with std::variant

| Feature | CppTaggedUnion | std::variant |
|---------|----------------|--------------|
| Syntax | Declarative macro | Template instantiation |
| Pattern Matching | Built-in `MATCH` macro | Requires `std::visit` |
| Named Access | `get_variant_name_ref()` | `std::get<Index>()` |
| Factory Methods | `create_variant_name()` | Constructor overloads |
| Type Safety | Compile-time + runtime checks | Compile-time + runtime checks |
| Performance | Zero-cost abstractions | Zero-cost abstractions |

## Troubleshooting

### Compilation Issues

If you encounter compilation errors:

1. **Ensure C++20 support**: Use `-std=c++20` flag with GCC/Clang
2. **Check compiler version**: Ensure you have a recent compiler (GCC 10+, Clang 10+)
3. **Template errors**: The macro system generates complex templates; check your variant definitions syntax

### Common Issues

- **Syntax Error in UNION**: Ensure each variant follows the pattern `(Type, name)`
- **Missing includes**: Make sure all types used in variants are properly included
- **Pattern matching**: Ensure all cases are covered in `MATCH` statements

## Design Philosophy

This library follows several key principles:

- **Zero-cost abstractions**: No runtime overhead compared to hand-written unions
- **Type safety**: Compile-time checking prevents accessing wrong variants
- **Rust-inspired**: Familiar syntax for developers coming from Rust
- **Modern C++**: Leverages C++20 features for cleaner implementation

## Contributing

Contributions are welcome! Please feel free to:

- Report bugs and compilation issues
- Suggest API improvements
- Submit fixes and enhancements
- Improve documentation

## License

This project is distributed under the terms specified by the repository owner.
