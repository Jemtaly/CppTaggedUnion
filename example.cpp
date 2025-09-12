#include "tagged_union.hpp"

#include <string>
#include <format>

UNION(MyUnion
    , (int, index)
    , (int, value)
    , (std::string, name)
    , (struct { int x; int y; }, point)
);

int main() {
    MyUnion u = MyUnion::create_name("Hello, World!");
    assert(u.get_tag() == MyUnion::tag_t::name);
    assert(u.holds_name());
    assert(u.get_name_ref() == "Hello, World!");
    assert(u.get_name_ptr() != nullptr);
    assert(u.get_index_ptr() == nullptr);

    u = MyUnion::create_index(42);
    assert(u.get_tag() == MyUnion::tag_t::index);
    assert(u.holds_index());
    assert(u.get_index_ref() == 42);
    assert(u.get_index_ptr() != nullptr);
    assert(u.get_name_ptr() == nullptr);

    u.emplace_value(100);
    assert(u.get_tag() == MyUnion::tag_t::value);
    assert(u.holds_value());
    assert(u.get_value_ref() == 100);
    assert(u.get_value_ptr() != nullptr);
    assert(u.get_index_ptr() == nullptr);

    u = MyUnion::create_point(10, 20);
    assert(u.get_tag() == MyUnion::tag_t::point);
    assert(u.holds_point());
    assert(u.get_point_ref().x == 10 && u.get_point_ref().y == 20);
    assert(u.get_point_ptr() != nullptr);
    assert(u.get_value_ptr() == nullptr);

    int a = MATCH(u, int
        , CASE(index, idx, { return idx; })
        , CASE(value, val, { return val; })
        , CASE(name, name, { return name.size(); })
        , CASE(point, point, { return point.x + point.y; })
        , OTHERWISE(_, { return -1; })
    );
    assert(a == 30);

    std::string s = MATCH(std::move(u), std::string
        , CASE(index, idx, { return std::format("Index: {}", idx); })
        , CASE(value, val, { return std::format("Value: {}", val); })
        , CASE(name, name, { return std::format("Name: {}", name); })
        , CASE(point, point, { return std::format("Point({}, {})", point.x, point.y); })
        , OTHERWISE(_, { return "Unknown"; })
    );
    assert(s == "Point(10, 20)");

    return 0;
}
