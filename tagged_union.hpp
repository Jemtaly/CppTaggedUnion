#pragma once

#include <cassert>
#include <memory>
#include <utility>

#define UNPACK(...) __VA_ARGS__

#define VA_NARGS(...) VA_NARGS_IMPL(ignored, ##__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define VA_NARGS_IMPL(ignored, _9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) N

#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define CONCAT_IMPL(a, b) a##b

#define FOR_EACH(action, mems, ...) CONCAT(FOR_EACH_IMPL_, VA_NARGS(__VA_ARGS__))(action, mems, __VA_ARGS__)
#define FOR_EACH_IMPL_9(action, mems, args, ...) action(mems, args) FOR_EACH_IMPL_8(action, mems, __VA_ARGS__)
#define FOR_EACH_IMPL_8(action, mems, args, ...) action(mems, args) FOR_EACH_IMPL_7(action, mems, __VA_ARGS__)
#define FOR_EACH_IMPL_7(action, mems, args, ...) action(mems, args) FOR_EACH_IMPL_6(action, mems, __VA_ARGS__)
#define FOR_EACH_IMPL_6(action, mems, args, ...) action(mems, args) FOR_EACH_IMPL_5(action, mems, __VA_ARGS__)
#define FOR_EACH_IMPL_5(action, mems, args, ...) action(mems, args) FOR_EACH_IMPL_4(action, mems, __VA_ARGS__)
#define FOR_EACH_IMPL_4(action, mems, args, ...) action(mems, args) FOR_EACH_IMPL_3(action, mems, __VA_ARGS__)
#define FOR_EACH_IMPL_3(action, mems, args, ...) action(mems, args) FOR_EACH_IMPL_2(action, mems, __VA_ARGS__)
#define FOR_EACH_IMPL_2(action, mems, args, ...) action(mems, args) FOR_EACH_IMPL_1(action, mems, __VA_ARGS__)
#define FOR_EACH_IMPL_1(action, mems, args, ...) action(mems, args) FOR_EACH_IMPL_0(action, mems, __VA_ARGS__)
#define FOR_EACH_IMPL_0(action, mems, ...)

// Union implementation

#define UNION_TAG_FIELD(mems, args) UNION_TAG_FIELD_CALL((UNPACK mems, UNPACK args))
#define UNION_TAG_FIELD_CALL(sums) UNION_TAG_FIELD_IMPL sums
#define UNION_TAG_FIELD_IMPL(type_name, field_type, field_name) field_name,

#define UNION_ALTERNATIVE_TYPE(mems, args) UNION_ALTERNATIVE_TYPE_CALL((UNPACK mems, UNPACK args))
#define UNION_ALTERNATIVE_TYPE_CALL(sums) UNION_ALTERNATIVE_TYPE_IMPL sums
#define UNION_ALTERNATIVE_TYPE_IMPL(type_name, field_type, field_name) \
    template<>                                                         \
    struct alternative<tag_t::field_name> {                            \
        using type = field_type;                                       \
    };

#define UNION_STORAGE_FIELD(mems, args) UNION_STORAGE_FIELD_CALL((UNPACK mems, UNPACK args))
#define UNION_STORAGE_FIELD_CALL(sums) UNION_STORAGE_FIELD_IMPL sums
#define UNION_STORAGE_FIELD_IMPL(type_name, field_type, field_name) alternative_t<tag_t::field_name> field_name;

#define UNION_COPY_CASE(mems, args) UNION_COPY_CASE_CALL((UNPACK mems, UNPACK args))
#define UNION_COPY_CASE_CALL(sums) UNION_COPY_CASE_IMPL sums
#define UNION_COPY_CASE_IMPL(type_name, field_type, field_name)                                                          \
    case tag_t::field_name:                                                                                              \
        new (std::addressof(this->m_storage.field_name)) alternative_t<tag_t::field_name>((other).m_storage.field_name); \
        break;

#define UNION_MOVE_CASE(mems, args) UNION_MOVE_CASE_CALL((UNPACK mems, UNPACK args))
#define UNION_MOVE_CASE_CALL(sums) UNION_MOVE_CASE_IMPL sums
#define UNION_MOVE_CASE_IMPL(type_name, field_type, field_name)                                                                   \
    case tag_t::field_name:                                                                                                       \
        new (std::addressof(this->m_storage.field_name)) alternative_t<tag_t::field_name>(std::move(other).m_storage.field_name); \
        break;

#define UNION_DESTRUCT_CASE(mems, args) UNION_DESTRUCT_CASE_CALL((UNPACK mems, UNPACK args))
#define UNION_DESTRUCT_CASE_CALL(sums) UNION_DESTRUCT_CASE_IMPL sums
#define UNION_DESTRUCT_CASE_IMPL(type_name, field_type, field_name)  \
    case tag_t::field_name:                                          \
        std::destroy_at(std::addressof(this->m_storage.field_name)); \
        break;

#define UNION_CONSTRUCT_IF_ELSE(mems, args) UNION_CONSTRUCT_IF_ELSE_CALL((UNPACK mems, UNPACK args))
#define UNION_CONSTRUCT_IF_ELSE_CALL(sums) UNION_CONSTRUCT_IF_ELSE_IMPL sums
#define UNION_CONSTRUCT_IF_ELSE_IMPL(type_name, field_type, field_name)                                                 \
    if constexpr (tag == tag_t::field_name) {                                                                           \
        new (std::addressof(this->m_storage.field_name)) alternative_t<tag_t::field_name>(std::forward<Args>(args)...); \
    } else

#define UNION_GET_PTR_IF_ELSE(mems, args) UNION_GET_PTR_IF_ELSE_CALL((UNPACK mems, UNPACK args))
#define UNION_GET_PTR_IF_ELSE_CALL(sums) UNION_GET_PTR_IF_ELSE_IMPL sums
#define UNION_GET_PTR_IF_ELSE_IMPL(type_name, field_type, field_name) \
    if constexpr (tag == tag_t::field_name) {                         \
        return m_tag == tag ? &this->m_storage.field_name : nullptr;  \
    } else

#define UNION_GET_LVALUE_REF_IF_ELSE(mems, args) UNION_GET_LVALUE_REF_IF_ELSE_CALL((UNPACK mems, UNPACK args))
#define UNION_GET_LVALUE_REF_IF_ELSE_CALL(sums) UNION_GET_LVALUE_REF_IF_ELSE_IMPL sums
#define UNION_GET_LVALUE_REF_IF_ELSE_IMPL(type_name, field_type, field_name) \
    if constexpr (tag == tag_t::field_name) {                                \
        return (*this).m_storage.field_name;                                 \
    } else

#define UNION_GET_RVALUE_REF_IF_ELSE(mems, args) UNION_GET_RVALUE_REF_IF_ELSE_CALL((UNPACK mems, UNPACK args))
#define UNION_GET_RVALUE_REF_IF_ELSE_CALL(sums) UNION_GET_RVALUE_REF_IF_ELSE_IMPL sums
#define UNION_GET_RVALUE_REF_IF_ELSE_IMPL(type_name, field_type, field_name) \
    if constexpr (tag == tag_t::field_name) {                                \
        return std::move(*this).m_storage.field_name;                        \
    } else

#define UNION_VISIT_LVALUE_REF_CASE(mems, args) UNION_VISIT_LVALUE_REF_CASE_CALL((UNPACK mems, UNPACK args))
#define UNION_VISIT_LVALUE_REF_CASE_CALL(sums) UNION_VISIT_LVALUE_REF_CASE_IMPL sums
#define UNION_VISIT_LVALUE_REF_CASE_IMPL(type_name, field_type, field_name)                                                              \
    case tag_t::field_name:                                                                                                              \
        if constexpr (requires { std::forward<Visitor>(visitor)(tu::in_place_tag<tag_t::field_name>, (*this).m_storage.field_name); }) { \
            return std::forward<Visitor>(visitor)(tu::in_place_tag<tag_t::field_name>, (*this).m_storage.field_name);                    \
        } else if constexpr (requires { std::forward<Visitor>(visitor)((*this)); }) {                                                    \
            return std::forward<Visitor>(visitor)((*this));                                                                              \
        } else {                                                                                                                         \
            return;                                                                                                                      \
        }

#define UNION_VISIT_RVALUE_REF_CASE(mems, args) UNION_VISIT_RVALUE_REF_CASE_CALL((UNPACK mems, UNPACK args))
#define UNION_VISIT_RVALUE_REF_CASE_CALL(sums) UNION_VISIT_RVALUE_REF_CASE_IMPL sums
#define UNION_VISIT_RVALUE_REF_CASE_IMPL(type_name, field_type, field_name)                                                                       \
    case tag_t::field_name:                                                                                                                       \
        if constexpr (requires { std::forward<Visitor>(visitor)(tu::in_place_tag<tag_t::field_name>, std::move(*this).m_storage.field_name); }) { \
            return std::forward<Visitor>(visitor)(tu::in_place_tag<tag_t::field_name>, std::move(*this).m_storage.field_name);                    \
        } else if constexpr (requires { std::forward<Visitor>(visitor)(std::move(*this)); }) {                                                    \
            return std::forward<Visitor>(visitor)(std::move(*this));                                                                              \
        } else {                                                                                                                                  \
            return;                                                                                                                               \
        }

#define UNION_SPECIFIC_METHOD(mems, args) UNION_SPECIFIC_METHOD_CALL((UNPACK mems, UNPACK args))
#define UNION_SPECIFIC_METHOD_CALL(sums) UNION_SPECIFIC_METHOD_IMPL sums
#define UNION_SPECIFIC_METHOD_IMPL(type_name, field_type, field_name)            \
    template<typename... Args>                                                   \
    static type_name create_##field_name(Args &&...args) {                       \
        return create<tag_t::field_name>(std::forward<Args>(args)...);           \
    }                                                                            \
                                                                                 \
    template<typename... Args>                                                   \
    alternative_t<tag_t::field_name> &emplace_##field_name(Args &&...args) {     \
        return emplace<tag_t::field_name>(std::forward<Args>(args)...);          \
    }                                                                            \
                                                                                 \
    alternative_t<tag_t::field_name> *get_##field_name##_ptr() {                 \
        return this->get_ptr<tag_t::field_name>();                               \
    }                                                                            \
                                                                                 \
    alternative_t<tag_t::field_name> const *get_##field_name##_ptr() const {     \
        return this->get_ptr<tag_t::field_name>();                               \
    }                                                                            \
                                                                                 \
    alternative_t<tag_t::field_name> &get_##field_name##_ref() & {               \
        return (*this).template get_ref<tag_t::field_name>();                    \
    }                                                                            \
                                                                                 \
    alternative_t<tag_t::field_name> const &get_##field_name##_ref() const & {   \
        return (*this).template get_ref<tag_t::field_name>();                    \
    }                                                                            \
                                                                                 \
    alternative_t<tag_t::field_name> &&get_##field_name##_ref() && {             \
        return std::move(*this).template get_ref<tag_t::field_name>();           \
    }                                                                            \
                                                                                 \
    alternative_t<tag_t::field_name> const &&get_##field_name##_ref() const && { \
        return std::move(*this).template get_ref<tag_t::field_name>();           \
    }                                                                            \
                                                                                 \
    bool holds_##field_name() const {                                            \
        return holds<tag_t::field_name>();                                       \
    }

#define UNION_MATCH_LVALUE_REF_CASE(mems, args) UNION_MATCH_LVALUE_REF_CASE_CALL((UNPACK mems, UNPACK args))
#define UNION_MATCH_LVALUE_REF_CASE_CALL(sums) UNION_MATCH_LVALUE_REF_CASE_IMPL sums
#define UNION_MATCH_LVALUE_REF_CASE_IMPL(type_name, field_type, field_name)                                           \
    case tag_t::field_name:                                                                                           \
        if constexpr (requires { std::forward<Matcher>(matcher).case_##field_name((*this).m_storage.field_name); }) { \
            return std::forward<Matcher>(matcher).case_##field_name((*this).m_storage.field_name);                    \
        } else if constexpr (requires { std::forward<Matcher>(matcher).otherwise((*this)); }) {                       \
            return std::forward<Matcher>(matcher).otherwise((*this));                                                 \
        } else {                                                                                                      \
            return;                                                                                                   \
        }

#define UNION_MATCH_RVALUE_REF_CASE(mems, args) UNION_MATCH_RVALUE_REF_CASE_CALL((UNPACK mems, UNPACK args))
#define UNION_MATCH_RVALUE_REF_CASE_CALL(sums) UNION_MATCH_RVALUE_REF_CASE_IMPL sums
#define UNION_MATCH_RVALUE_REF_CASE_IMPL(type_name, field_type, field_name)                                                    \
    case tag_t::field_name:                                                                                                    \
        if constexpr (requires { std::forward<Matcher>(matcher).case_##field_name(std::move(*this).m_storage.field_name); }) { \
            return std::forward<Matcher>(matcher).case_##field_name(std::move(*this).m_storage.field_name);                    \
        } else if constexpr (requires { std::forward<Matcher>(matcher).otherwise(std::move(*this)); }) {                       \
            return std::forward<Matcher>(matcher).otherwise(std::move(*this));                                                 \
        } else {                                                                                                               \
            return;                                                                                                            \
        }

#define UNION(type_name, ...)                                                         \
    struct type_name {                                                                \
        enum class tag_t {                                                            \
            FOR_EACH(UNION_TAG_FIELD, (type_name), __VA_ARGS__)                       \
        };                                                                            \
                                                                                      \
        template<tag_t tag>                                                           \
        struct alternative;                                                           \
        FOR_EACH(UNION_ALTERNATIVE_TYPE, (type_name), __VA_ARGS__)                    \
                                                                                      \
        template<tag_t tag>                                                           \
        using alternative_t = typename alternative<tag>::type;                        \
                                                                                      \
        union storage_t {                                                             \
            storage_t() {}                                                            \
            ~storage_t() {}                                                           \
            FOR_EACH(UNION_STORAGE_FIELD, (type_name), __VA_ARGS__)                   \
        };                                                                            \
                                                                                      \
        type_name(type_name const &other) : m_tag(other.m_tag) {                      \
            switch (m_tag) {                                                          \
                FOR_EACH(UNION_COPY_CASE, (type_name), __VA_ARGS__)                   \
            default:                                                                  \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
        type_name(type_name &&other) : m_tag(other.m_tag) {                           \
            switch (m_tag) {                                                          \
                FOR_EACH(UNION_MOVE_CASE, (type_name), __VA_ARGS__)                   \
            default:                                                                  \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
        ~type_name() {                                                                \
            switch (m_tag) {                                                          \
                FOR_EACH(UNION_DESTRUCT_CASE, (type_name), __VA_ARGS__)               \
            default:                                                                  \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
        type_name &operator=(type_name const &other) {                                \
            if (this != &other) {                                                     \
                this->~type_name();                                                   \
                new (this) type_name((other));                                        \
            }                                                                         \
            return *this;                                                             \
        }                                                                             \
                                                                                      \
        type_name &operator=(type_name &&other) {                                     \
            if (this != &other) {                                                     \
                this->~type_name();                                                   \
                new (this) type_name(std::move(other));                               \
            }                                                                         \
            return *this;                                                             \
        }                                                                             \
                                                                                      \
        template<tag_t tag, typename... Args>                                         \
        type_name(tu::in_place_tag_t<tag>, Args &&...args) : m_tag(tag) {             \
            FOR_EACH(UNION_CONSTRUCT_IF_ELSE, (type_name), __VA_ARGS__) {             \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<tag_t tag, typename... Args>                                         \
        static type_name create(Args &&...args) {                                     \
            return type_name(tu::in_place_tag<tag>, std::forward<Args>(args)...);     \
        }                                                                             \
                                                                                      \
        template<tag_t tag, typename... Args>                                         \
        alternative_t<tag> &emplace(Args &&...args) {                                 \
            this->~type_name();                                                       \
            new (this) type_name(tu::in_place_tag<tag>, std::forward<Args>(args)...); \
            return get_ref<tag>();                                                    \
        }                                                                             \
                                                                                      \
        template<tag_t tag>                                                           \
        alternative_t<tag> *get_ptr() {                                               \
            FOR_EACH(UNION_GET_PTR_IF_ELSE, (type_name), __VA_ARGS__) {               \
                static_assert(false, "bad tag");                                      \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<tag_t tag>                                                           \
        alternative_t<tag> const *get_ptr() const {                                   \
            FOR_EACH(UNION_GET_PTR_IF_ELSE, (type_name), __VA_ARGS__) {               \
                static_assert(false, "bad tag");                                      \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<tag_t tag>                                                           \
        alternative_t<tag> &get_ref() & {                                             \
            FOR_EACH(UNION_GET_LVALUE_REF_IF_ELSE, (type_name), __VA_ARGS__) {        \
                static_assert(false, "bad tag");                                      \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<tag_t tag>                                                           \
        alternative_t<tag> const &get_ref() const & {                                 \
            FOR_EACH(UNION_GET_LVALUE_REF_IF_ELSE, (type_name), __VA_ARGS__) {        \
                static_assert(false, "bad tag");                                      \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<tag_t tag>                                                           \
        alternative_t<tag> &&get_ref() && {                                           \
            FOR_EACH(UNION_GET_RVALUE_REF_IF_ELSE, (type_name), __VA_ARGS__) {        \
                static_assert(false, "bad tag");                                      \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<tag_t tag>                                                           \
        alternative_t<tag> const &&get_ref() const && {                               \
            FOR_EACH(UNION_GET_RVALUE_REF_IF_ELSE, (type_name), __VA_ARGS__) {        \
                static_assert(false, "bad tag");                                      \
            }                                                                         \
        }                                                                             \
                                                                                      \
        tag_t get_tag() const {                                                       \
            return m_tag;                                                             \
        }                                                                             \
                                                                                      \
        template<tag_t tag>                                                           \
        bool holds() const {                                                          \
            return m_tag == tag;                                                      \
        }                                                                             \
                                                                                      \
        template<typename ReturnType, typename Visitor>                               \
        ReturnType visit(Visitor &&visitor) & {                                       \
            switch (m_tag) {                                                          \
                FOR_EACH(UNION_VISIT_LVALUE_REF_CASE, (type_name), __VA_ARGS__)       \
            default:                                                                  \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<typename ReturnType, typename Visitor>                               \
        ReturnType visit(Visitor &&visitor) const & {                                 \
            switch (m_tag) {                                                          \
                FOR_EACH(UNION_VISIT_LVALUE_REF_CASE, (type_name), __VA_ARGS__)       \
            default:                                                                  \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<typename ReturnType, typename Visitor>                               \
        ReturnType visit(Visitor &&visitor) && {                                      \
            switch (m_tag) {                                                          \
                FOR_EACH(UNION_VISIT_RVALUE_REF_CASE, (type_name), __VA_ARGS__)       \
            default:                                                                  \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<typename ReturnType, typename Visitor>                               \
        ReturnType visit(Visitor &&visitor) const && {                                \
            switch (m_tag) {                                                          \
                FOR_EACH(UNION_VISIT_RVALUE_REF_CASE, (type_name), __VA_ARGS__)       \
            default:                                                                  \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
        FOR_EACH(UNION_SPECIFIC_METHOD, (type_name), __VA_ARGS__)                     \
                                                                                      \
        template<typename ReturnType, typename Matcher>                               \
        ReturnType match(Matcher &&matcher) & {                                       \
            switch (m_tag) {                                                          \
                FOR_EACH(UNION_MATCH_LVALUE_REF_CASE, (type_name), __VA_ARGS__)       \
            default:                                                                  \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<typename ReturnType, typename Matcher>                               \
        ReturnType match(Matcher &&matcher) const & {                                 \
            switch (m_tag) {                                                          \
                FOR_EACH(UNION_MATCH_LVALUE_REF_CASE, (type_name), __VA_ARGS__)       \
            default:                                                                  \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<typename ReturnType, typename Matcher>                               \
        ReturnType match(Matcher &&matcher) && {                                      \
            switch (m_tag) {                                                          \
                FOR_EACH(UNION_MATCH_RVALUE_REF_CASE, (type_name), __VA_ARGS__)       \
            default:                                                                  \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
        template<typename ReturnType, typename Matcher>                               \
        ReturnType match(Matcher &&matcher) const && {                                \
            switch (m_tag) {                                                          \
                FOR_EACH(UNION_MATCH_RVALUE_REF_CASE, (type_name), __VA_ARGS__)       \
            default:                                                                  \
                assert(false && "bad tag");                                           \
            }                                                                         \
        }                                                                             \
                                                                                      \
    private:                                                                          \
        tag_t m_tag;                                                                  \
        storage_t m_storage;                                                          \
    }

// Pattern matching implementation

#define MATCH_GEN_FUNC(mems, args) MATCH_GEN_FUNC_CALL((UNPACK mems, UNPACK args))
#define MATCH_GEN_FUNC_CALL(sums) MATCH_GEN_FUNC_IMPL sums
#define MATCH_GEN_FUNC_IMPL(return_type, value, kind, ...) \
    MATCH_GEN_##kind##_FUNC(return_type, value, __VA_ARGS__)

#define CASE(field_name, var_name, block) (CASE, field_name, var_name, block)
#define MATCH_GEN_CASE_FUNC(return_type, value, field_name, var_name, block) \
    [&](tu::in_place_tag_t<std::remove_reference_t<decltype((value))>::tag_t::field_name>, auto &&var_name) block,

#define OTHERWISE(var_name, block) (OTHERWISE, var_name, block)
#define MATCH_GEN_OTHERWISE_FUNC(return_type, value, var_name, block) \
    [&](auto &&var_name) block,

#define MATCH(return_type, value, ...) \
    (value).visit<return_type>(tu::combined_visitor{FOR_EACH(MATCH_GEN_FUNC, (return_type, value), __VA_ARGS__)})

namespace tu {
template<auto tag>
struct in_place_tag_t {
    static constexpr auto value = tag;
};

template<auto tag>
static constexpr in_place_tag_t<tag> in_place_tag;

template<typename... Visitors>
struct combined_visitor : Visitors... {
    using Visitors::operator()...;
};

template<typename... Visitors>
combined_visitor(Visitors...) -> combined_visitor<Visitors...>;
}
