#pragma once

#include <cassert>
#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define CONCAT_IMPL(a, b) a##b

#define VA_NARGS(...) VA_NARGS_IMPL(ignored, ##__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define VA_NARGS_IMPL(ignored, _9, _8, _7, _6, _5, _4, _3, _2, _1, n, ...) n

#define FOR_EACH(action, glob, ...) CONCAT(FOR_EACH_IMPL_, VA_NARGS(__VA_ARGS__))(action, glob, __VA_ARGS__)
#define FOR_EACH_IMPL_9(action, glob, iter, ...) action(glob, iter) FOR_EACH_IMPL_8(action, glob, __VA_ARGS__)
#define FOR_EACH_IMPL_8(action, glob, iter, ...) action(glob, iter) FOR_EACH_IMPL_7(action, glob, __VA_ARGS__)
#define FOR_EACH_IMPL_7(action, glob, iter, ...) action(glob, iter) FOR_EACH_IMPL_6(action, glob, __VA_ARGS__)
#define FOR_EACH_IMPL_6(action, glob, iter, ...) action(glob, iter) FOR_EACH_IMPL_5(action, glob, __VA_ARGS__)
#define FOR_EACH_IMPL_5(action, glob, iter, ...) action(glob, iter) FOR_EACH_IMPL_4(action, glob, __VA_ARGS__)
#define FOR_EACH_IMPL_4(action, glob, iter, ...) action(glob, iter) FOR_EACH_IMPL_3(action, glob, __VA_ARGS__)
#define FOR_EACH_IMPL_3(action, glob, iter, ...) action(glob, iter) FOR_EACH_IMPL_2(action, glob, __VA_ARGS__)
#define FOR_EACH_IMPL_2(action, glob, iter, ...) action(glob, iter) FOR_EACH_IMPL_1(action, glob, __VA_ARGS__)
#define FOR_EACH_IMPL_1(action, glob, iter, ...) action(glob, iter) FOR_EACH_IMPL_0(action, glob, __VA_ARGS__)
#define FOR_EACH_IMPL_0(action, glob, ...)

#define UNPACK(packed) UNPACK_IMPL packed
#define UNPACK_IMPL(...) __VA_ARGS__

#define FORWARD(unpacked) unpacked

#define TRY_UNPACK_PROBE(...) 0, 1
#define TRY_UNPACK_OPERATION(may_packed) TRY_UNPACK_OPERATION_CALL((TRY_UNPACK_PROBE may_packed, UNPACK, FORWARD))
#define TRY_UNPACK_OPERATION_CALL(probe) TRY_UNPACK_OPERATION_IMPL probe
#define TRY_UNPACK_OPERATION_IMPL(_0, _1, operation, ...) operation

#define TRY_UNPACK(may_packed) TRY_UNPACK_OPERATION(may_packed)(may_packed)

// Union implementation

#define UNION_TAG_FIELD(union_info, field_info) UNION_TAG_FIELD_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_TAG_FIELD_CALL(concatenated_info) UNION_TAG_FIELD_IMPL concatenated_info
#define UNION_TAG_FIELD_IMPL(type_name, field_type, field_name) field_name,

#define UNION_ALTERNATIVE_TYPE(union_info, field_info) UNION_ALTERNATIVE_TYPE_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_ALTERNATIVE_TYPE_CALL(concatenated_info) UNION_ALTERNATIVE_TYPE_IMPL concatenated_info
#define UNION_ALTERNATIVE_TYPE_IMPL(type_name, field_type, field_name) \
    template<>                                                         \
    struct alternative<tag_t::field_name> {                            \
        using type = TRY_UNPACK(field_type);                           \
    };

#define UNION_STORAGE_FIELD(union_info, field_info) UNION_STORAGE_FIELD_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_STORAGE_FIELD_CALL(concatenated_info) UNION_STORAGE_FIELD_IMPL concatenated_info
#define UNION_STORAGE_FIELD_IMPL(type_name, field_type, field_name) alternative_t<tag_t::field_name> field_name;

#define UNION_COPY_CASE(union_info, field_info) UNION_COPY_CASE_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_COPY_CASE_CALL(concatenated_info) UNION_COPY_CASE_IMPL concatenated_info
#define UNION_COPY_CASE_IMPL(type_name, field_type, field_name)                                                          \
    case tag_t::field_name:                                                                                              \
        new (std::addressof(this->m_storage.field_name)) alternative_t<tag_t::field_name>((other).m_storage.field_name); \
        break;

#define UNION_MOVE_CASE(union_info, field_info) UNION_MOVE_CASE_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_MOVE_CASE_CALL(concatenated_info) UNION_MOVE_CASE_IMPL concatenated_info
#define UNION_MOVE_CASE_IMPL(type_name, field_type, field_name)                                                                   \
    case tag_t::field_name:                                                                                                       \
        new (std::addressof(this->m_storage.field_name)) alternative_t<tag_t::field_name>(std::move(other).m_storage.field_name); \
        break;

#define UNION_DESTRUCT_CASE(union_info, field_info) UNION_DESTRUCT_CASE_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_DESTRUCT_CASE_CALL(concatenated_info) UNION_DESTRUCT_CASE_IMPL concatenated_info
#define UNION_DESTRUCT_CASE_IMPL(type_name, field_type, field_name)  \
    case tag_t::field_name:                                          \
        std::destroy_at(std::addressof(this->m_storage.field_name)); \
        break;

#define UNION_GET_LVALUE_REF_IF_ELSE(union_info, field_info) UNION_GET_LVALUE_REF_IF_ELSE_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_GET_LVALUE_REF_IF_ELSE_CALL(concatenated_info) UNION_GET_LVALUE_REF_IF_ELSE_IMPL concatenated_info
#define UNION_GET_LVALUE_REF_IF_ELSE_IMPL(type_name, field_type, field_name) \
    if constexpr (tag == tag_t::field_name) {                                \
        return (*this).m_storage.field_name;                                 \
    } else

#define UNION_GET_RVALUE_REF_IF_ELSE(union_info, field_info) UNION_GET_RVALUE_REF_IF_ELSE_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_GET_RVALUE_REF_IF_ELSE_CALL(concatenated_info) UNION_GET_RVALUE_REF_IF_ELSE_IMPL concatenated_info
#define UNION_GET_RVALUE_REF_IF_ELSE_IMPL(type_name, field_type, field_name) \
    if constexpr (tag == tag_t::field_name) {                                \
        return std::move(*this).m_storage.field_name;                        \
    } else

#define UNION_SPECIFIC_METHOD(union_info, field_info) UNION_SPECIFIC_METHOD_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_SPECIFIC_METHOD_CALL(concatenated_info) UNION_SPECIFIC_METHOD_IMPL concatenated_info
#define UNION_SPECIFIC_METHOD_IMPL(type_name, field_type, field_name)                       \
    template<typename... Args>                                                              \
    type_name(tu::in_place_tag_t<tag_t::field_name>, Args &&...args)                        \
        : m_tag(tag_t::field_name) {                                                        \
        new (std::addressof(this->m_storage.field_name))                                    \
            alternative_t<tag_t::field_name>(std::forward<Args>(args)...);                  \
    }                                                                                       \
                                                                                            \
    bool holds_##field_name() const {                                                       \
        return holds<tag_t::field_name>();                                                  \
    }                                                                                       \
                                                                                            \
    alternative_t<tag_t::field_name> &get_##field_name##_ref() & {                          \
        return (*this).m_storage.field_name;                                                \
    }                                                                                       \
                                                                                            \
    alternative_t<tag_t::field_name> const &get_##field_name##_ref() const & {              \
        return (*this).m_storage.field_name;                                                \
    }                                                                                       \
                                                                                            \
    alternative_t<tag_t::field_name> &&get_##field_name##_ref() && {                        \
        return std::move(*this).m_storage.field_name;                                       \
    }                                                                                       \
                                                                                            \
    alternative_t<tag_t::field_name> const &&get_##field_name##_ref() const && {            \
        return std::move(*this).m_storage.field_name;                                       \
    }                                                                                       \
                                                                                            \
    alternative_t<tag_t::field_name> *get_##field_name##_ptr() {                            \
        return holds_##field_name() ? &get_##field_name##_ref() : nullptr;                  \
    }                                                                                       \
                                                                                            \
    alternative_t<tag_t::field_name> const *get_##field_name##_ptr() const {                \
        return holds_##field_name() ? &get_##field_name##_ref() : nullptr;                  \
    }                                                                                       \
                                                                                            \
    template<typename... Args>                                                              \
    static type_name create_##field_name(Args &&...args) {                                  \
        return type_name(tu::in_place_tag<tag_t::field_name>, std::forward<Args>(args)...); \
    }                                                                                       \
                                                                                            \
    template<typename... Args>                                                              \
    alternative_t<tag_t::field_name> &emplace_##field_name(Args &&...args) {                \
        this->~type_name();                                                                 \
        new (this)                                                                          \
            type_name(tu::in_place_tag<tag_t::field_name>, std::forward<Args>(args)...);    \
        return get_##field_name##_ref();                                                    \
    }

#define UNION_VISIT_LVALUE_REF_CASE_EXPR(field_name) std::forward<Visitor>(visitor)(tu::in_place_tag<tag_t::field_name>, (*this).m_storage.field_name)
#define UNION_VISIT_LVALUE_REF_OTHERWISE_EXPR std::forward<Visitor>(visitor)((*this))
#define UNION_VISIT_LVALUE_REF_HAS_CASE_AND(union_info, field_info) UNION_VISIT_LVALUE_REF_HAS_CASE_AND_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_VISIT_LVALUE_REF_HAS_CASE_AND_CALL(concatenated_info) UNION_VISIT_LVALUE_REF_HAS_CASE_AND_IMPL concatenated_info
#define UNION_VISIT_LVALUE_REF_HAS_CASE_AND_IMPL(type_name, field_type, field_name) \
    (requires { { UNION_VISIT_LVALUE_REF_CASE_EXPR(field_name) } -> std::convertible_to<ReturnType>; } || !requires { UNION_VISIT_LVALUE_REF_CASE_EXPR(field_name); } && (requires { { UNION_VISIT_LVALUE_REF_OTHERWISE_EXPR } -> std::convertible_to<ReturnType>; } || !requires { UNION_VISIT_LVALUE_REF_OTHERWISE_EXPR; } && std::same_as<ReturnType, void>)) &&
#define UNION_VISIT_LVALUE_REF_CASE(union_info, field_info) UNION_VISIT_LVALUE_REF_CASE_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_VISIT_LVALUE_REF_CASE_CALL(concatenated_info) UNION_VISIT_LVALUE_REF_CASE_IMPL concatenated_info
#define UNION_VISIT_LVALUE_REF_CASE_IMPL(type_name, field_type, field_name)         \
    case tag_t::field_name:                                                         \
        if constexpr (requires { UNION_VISIT_LVALUE_REF_CASE_EXPR(field_name); }) { \
            return UNION_VISIT_LVALUE_REF_CASE_EXPR(field_name);                    \
        } else if constexpr (requires { UNION_VISIT_LVALUE_REF_OTHERWISE_EXPR; }) { \
            return UNION_VISIT_LVALUE_REF_OTHERWISE_EXPR;                           \
        } else {                                                                    \
            return;                                                                 \
        }

#define UNION_VISIT_RVALUE_REF_CASE_EXPR(field_name) std::forward<Visitor>(visitor)(tu::in_place_tag<tag_t::field_name>, std::move(*this).m_storage.field_name)
#define UNION_VISIT_RVALUE_REF_OTHERWISE_EXPR std::forward<Visitor>(visitor)(std::move(*this))
#define UNION_VISIT_RVALUE_REF_HAS_CASE_AND(union_info, field_info) UNION_VISIT_RVALUE_REF_HAS_CASE_AND_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_VISIT_RVALUE_REF_HAS_CASE_AND_CALL(concatenated_info) UNION_VISIT_RVALUE_REF_HAS_CASE_AND_IMPL concatenated_info
#define UNION_VISIT_RVALUE_REF_HAS_CASE_AND_IMPL(type_name, field_type, field_name) \
    (requires { { UNION_VISIT_RVALUE_REF_CASE_EXPR(field_name) } -> std::convertible_to<ReturnType>; } || !requires { UNION_VISIT_RVALUE_REF_CASE_EXPR(field_name); } && (requires { { UNION_VISIT_RVALUE_REF_OTHERWISE_EXPR } -> std::convertible_to<ReturnType>; } || !requires { UNION_VISIT_RVALUE_REF_OTHERWISE_EXPR; } && std::same_as<ReturnType, void>)) &&
#define UNION_VISIT_RVALUE_REF_CASE(union_info, field_info) UNION_VISIT_RVALUE_REF_CASE_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_VISIT_RVALUE_REF_CASE_CALL(concatenated_info) UNION_VISIT_RVALUE_REF_CASE_IMPL concatenated_info
#define UNION_VISIT_RVALUE_REF_CASE_IMPL(type_name, field_type, field_name)         \
    case tag_t::field_name:                                                         \
        if constexpr (requires { UNION_VISIT_RVALUE_REF_CASE_EXPR(field_name); }) { \
            return UNION_VISIT_RVALUE_REF_CASE_EXPR(field_name);                    \
        } else if constexpr (requires { UNION_VISIT_RVALUE_REF_OTHERWISE_EXPR; }) { \
            return UNION_VISIT_RVALUE_REF_OTHERWISE_EXPR;                           \
        } else {                                                                    \
            return;                                                                 \
        }

#define UNION_MATCH_LVALUE_REF_CASE_EXPR(field_name) std::forward<Matcher>(matcher).case_##field_name((*this).m_storage.field_name)
#define UNION_MATCH_LVALUE_REF_OTHERWISE_EXPR std::forward<Matcher>(matcher).otherwise((*this))
#define UNION_MATCH_LVALUE_REF_HAS_CASE_AND(union_info, field_info) UNION_MATCH_LVALUE_REF_HAS_CASE_AND_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_MATCH_LVALUE_REF_HAS_CASE_AND_CALL(concatenated_info) UNION_MATCH_LVALUE_REF_HAS_CASE_AND_IMPL concatenated_info
#define UNION_MATCH_LVALUE_REF_HAS_CASE_AND_IMPL(type_name, field_type, field_name) \
    (requires { { UNION_MATCH_LVALUE_REF_CASE_EXPR(field_name) } -> std::convertible_to<ReturnType>; } || !requires { UNION_MATCH_LVALUE_REF_CASE_EXPR(field_name); } && (requires { { UNION_MATCH_LVALUE_REF_OTHERWISE_EXPR } -> std::convertible_to<ReturnType>; } || !requires { UNION_MATCH_LVALUE_REF_OTHERWISE_EXPR; } && std::same_as<ReturnType, void>)) &&
#define UNION_MATCH_LVALUE_REF_CASE(union_info, field_info) UNION_MATCH_LVALUE_REF_CASE_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_MATCH_LVALUE_REF_CASE_CALL(concatenated_info) UNION_MATCH_LVALUE_REF_CASE_IMPL concatenated_info
#define UNION_MATCH_LVALUE_REF_CASE_IMPL(type_name, field_type, field_name)         \
    case tag_t::field_name:                                                         \
        if constexpr (requires { UNION_MATCH_LVALUE_REF_CASE_EXPR(field_name); }) { \
            return UNION_MATCH_LVALUE_REF_CASE_EXPR(field_name);                    \
        } else if constexpr (requires { UNION_MATCH_LVALUE_REF_OTHERWISE_EXPR; }) { \
            return UNION_MATCH_LVALUE_REF_OTHERWISE_EXPR;                           \
        } else {                                                                    \
            return;                                                                 \
        }

#define UNION_MATCH_RVALUE_REF_CASE_EXPR(field_name) std::forward<Matcher>(matcher).case_##field_name(std::move(*this).m_storage.field_name)
#define UNION_MATCH_RVALUE_REF_OTHERWISE_EXPR std::forward<Matcher>(matcher).otherwise(std::move(*this))
#define UNION_MATCH_RVALUE_REF_HAS_CASE_AND(union_info, field_info) UNION_MATCH_RVALUE_REF_HAS_CASE_AND_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_MATCH_RVALUE_REF_HAS_CASE_AND_CALL(concatenated_info) UNION_MATCH_RVALUE_REF_HAS_CASE_AND_IMPL concatenated_info
#define UNION_MATCH_RVALUE_REF_HAS_CASE_AND_IMPL(type_name, field_type, field_name) \
    (requires { { UNION_MATCH_RVALUE_REF_CASE_EXPR(field_name) } -> std::convertible_to<ReturnType>; } || !requires { UNION_MATCH_RVALUE_REF_CASE_EXPR(field_name); } && (requires { { UNION_MATCH_RVALUE_REF_OTHERWISE_EXPR } -> std::convertible_to<ReturnType>; } || !requires { UNION_MATCH_RVALUE_REF_OTHERWISE_EXPR; } && std::same_as<ReturnType, void>)) &&
#define UNION_MATCH_RVALUE_REF_CASE(union_info, field_info) UNION_MATCH_RVALUE_REF_CASE_CALL((UNPACK(union_info), UNPACK(field_info)))
#define UNION_MATCH_RVALUE_REF_CASE_CALL(concatenated_info) UNION_MATCH_RVALUE_REF_CASE_IMPL concatenated_info
#define UNION_MATCH_RVALUE_REF_CASE_IMPL(type_name, field_type, field_name)         \
    case tag_t::field_name:                                                         \
        if constexpr (requires { UNION_MATCH_RVALUE_REF_CASE_EXPR(field_name); }) { \
            return UNION_MATCH_RVALUE_REF_CASE_EXPR(field_name);                    \
        } else if constexpr (requires { UNION_MATCH_RVALUE_REF_OTHERWISE_EXPR; }) { \
            return UNION_MATCH_RVALUE_REF_OTHERWISE_EXPR;                           \
        } else {                                                                    \
            return;                                                                 \
        }

#define UNION(type_name, ...)                                                                      \
    struct type_name {                                                                             \
        enum class tag_t {                                                                         \
            FOR_EACH(UNION_TAG_FIELD, (type_name), __VA_ARGS__)                                    \
        };                                                                                         \
                                                                                                   \
        template<tag_t tag>                                                                        \
        struct alternative;                                                                        \
        FOR_EACH(UNION_ALTERNATIVE_TYPE, (type_name), __VA_ARGS__)                                 \
                                                                                                   \
        template<tag_t tag>                                                                        \
        using alternative_t = typename alternative<tag>::type;                                     \
                                                                                                   \
        union storage_t {                                                                          \
            storage_t() {}                                                                         \
            ~storage_t() {}                                                                        \
            FOR_EACH(UNION_STORAGE_FIELD, (type_name), __VA_ARGS__)                                \
        };                                                                                         \
                                                                                                   \
    private:                                                                                       \
        tag_t m_tag;                                                                               \
        storage_t m_storage;                                                                       \
                                                                                                   \
    public:                                                                                        \
        type_name(type_name const &other) : m_tag(other.m_tag) {                                   \
            switch (m_tag) {                                                                       \
                FOR_EACH(UNION_COPY_CASE, (type_name), __VA_ARGS__)                                \
            default:                                                                               \
                assert(false && "bad tag");                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        type_name(type_name &&other) : m_tag(other.m_tag) {                                        \
            switch (m_tag) {                                                                       \
                FOR_EACH(UNION_MOVE_CASE, (type_name), __VA_ARGS__)                                \
            default:                                                                               \
                assert(false && "bad tag");                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        ~type_name() {                                                                             \
            switch (m_tag) {                                                                       \
                FOR_EACH(UNION_DESTRUCT_CASE, (type_name), __VA_ARGS__)                            \
            default:                                                                               \
                assert(false && "bad tag");                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        type_name &operator=(type_name const &other) {                                             \
            if (this != &other) {                                                                  \
                this->~type_name();                                                                \
                new (this) type_name((other));                                                     \
            }                                                                                      \
            return *this;                                                                          \
        }                                                                                          \
                                                                                                   \
        type_name &operator=(type_name &&other) {                                                  \
            if (this != &other) {                                                                  \
                this->~type_name();                                                                \
                new (this) type_name(std::move(other));                                            \
            }                                                                                      \
            return *this;                                                                          \
        }                                                                                          \
                                                                                                   \
        template<tag_t tag>                                                                        \
        bool holds() const {                                                                       \
            return m_tag == tag;                                                                   \
        }                                                                                          \
                                                                                                   \
        template<tag_t tag>                                                                        \
        alternative_t<tag> &get_ref() & {                                                          \
            FOR_EACH(UNION_GET_LVALUE_REF_IF_ELSE, (type_name), __VA_ARGS__) {                     \
                static_assert(tag != tag, "bad tag");                                              \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        template<tag_t tag>                                                                        \
        alternative_t<tag> const &get_ref() const & {                                              \
            FOR_EACH(UNION_GET_LVALUE_REF_IF_ELSE, (type_name), __VA_ARGS__) {                     \
                static_assert(tag != tag, "bad tag");                                              \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        template<tag_t tag>                                                                        \
        alternative_t<tag> &&get_ref() && {                                                        \
            FOR_EACH(UNION_GET_RVALUE_REF_IF_ELSE, (type_name), __VA_ARGS__) {                     \
                static_assert(tag != tag, "bad tag");                                              \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        template<tag_t tag>                                                                        \
        alternative_t<tag> const &&get_ref() const && {                                            \
            FOR_EACH(UNION_GET_RVALUE_REF_IF_ELSE, (type_name), __VA_ARGS__) {                     \
                static_assert(tag != tag, "bad tag");                                              \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        template<tag_t tag>                                                                        \
        alternative_t<tag> *get_ptr() {                                                            \
            return holds<tag>() ? &get_ref<tag>() : nullptr;                                       \
        }                                                                                          \
                                                                                                   \
        template<tag_t tag>                                                                        \
        alternative_t<tag> const *get_ptr() const {                                                \
            return holds<tag>() ? &get_ref<tag>() : nullptr;                                       \
        }                                                                                          \
                                                                                                   \
        template<tag_t tag, typename... Args>                                                      \
        static type_name create(Args &&...args) {                                                  \
            return type_name(tu::in_place_tag<tag>, std::forward<Args>(args)...);                  \
        }                                                                                          \
                                                                                                   \
        template<tag_t tag, typename... Args>                                                      \
        alternative_t<tag> &emplace(Args &&...args) {                                              \
            this->~type_name();                                                                    \
            new (this) type_name(tu::in_place_tag<tag>, std::forward<Args>(args)...);              \
            return get_ref<tag>();                                                                 \
        }                                                                                          \
                                                                                                   \
        FOR_EACH(UNION_SPECIFIC_METHOD, (type_name), __VA_ARGS__)                                  \
                                                                                                   \
        tag_t get_tag() const {                                                                    \
            return m_tag;                                                                          \
        }                                                                                          \
                                                                                                   \
        template<typename ReturnType, typename Visitor>                                            \
        ReturnType visit(Visitor &&visitor) &                                                      \
            requires(FOR_EACH(UNION_VISIT_LVALUE_REF_HAS_CASE_AND, (type_name), __VA_ARGS__) true) \
        {                                                                                          \
            switch (m_tag) {                                                                       \
                FOR_EACH(UNION_VISIT_LVALUE_REF_CASE, (type_name), __VA_ARGS__)                    \
            default:                                                                               \
                assert(false && "bad tag");                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        template<typename ReturnType, typename Visitor>                                            \
        ReturnType visit(Visitor &&visitor) const &                                                \
            requires(FOR_EACH(UNION_VISIT_LVALUE_REF_HAS_CASE_AND, (type_name), __VA_ARGS__) true) \
        {                                                                                          \
            switch (m_tag) {                                                                       \
                FOR_EACH(UNION_VISIT_LVALUE_REF_CASE, (type_name), __VA_ARGS__)                    \
            default:                                                                               \
                assert(false && "bad tag");                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        template<typename ReturnType, typename Visitor>                                            \
        ReturnType visit(Visitor &&visitor) &&                                                     \
            requires(FOR_EACH(UNION_VISIT_RVALUE_REF_HAS_CASE_AND, (type_name), __VA_ARGS__) true) \
        {                                                                                          \
            switch (m_tag) {                                                                       \
                FOR_EACH(UNION_VISIT_RVALUE_REF_CASE, (type_name), __VA_ARGS__)                    \
            default:                                                                               \
                assert(false && "bad tag");                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        template<typename ReturnType, typename Visitor>                                            \
        ReturnType visit(Visitor &&visitor) const &&                                               \
            requires(FOR_EACH(UNION_VISIT_RVALUE_REF_HAS_CASE_AND, (type_name), __VA_ARGS__) true) \
        {                                                                                          \
            switch (m_tag) {                                                                       \
                FOR_EACH(UNION_VISIT_RVALUE_REF_CASE, (type_name), __VA_ARGS__)                    \
            default:                                                                               \
                assert(false && "bad tag");                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        template<typename ReturnType, typename Matcher>                                            \
        ReturnType match(Matcher &&matcher) &                                                      \
            requires(FOR_EACH(UNION_MATCH_LVALUE_REF_HAS_CASE_AND, (type_name), __VA_ARGS__) true) \
        {                                                                                          \
            switch (m_tag) {                                                                       \
                FOR_EACH(UNION_MATCH_LVALUE_REF_CASE, (type_name), __VA_ARGS__)                    \
            default:                                                                               \
                assert(false && "bad tag");                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        template<typename ReturnType, typename Matcher>                                            \
        ReturnType match(Matcher &&matcher) const &                                                \
            requires(FOR_EACH(UNION_MATCH_LVALUE_REF_HAS_CASE_AND, (type_name), __VA_ARGS__) true) \
        {                                                                                          \
            switch (m_tag) {                                                                       \
                FOR_EACH(UNION_MATCH_LVALUE_REF_CASE, (type_name), __VA_ARGS__)                    \
            default:                                                                               \
                assert(false && "bad tag");                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        template<typename ReturnType, typename Matcher>                                            \
        ReturnType match(Matcher &&matcher) &&                                                     \
            requires(FOR_EACH(UNION_MATCH_RVALUE_REF_HAS_CASE_AND, (type_name), __VA_ARGS__) true) \
        {                                                                                          \
            switch (m_tag) {                                                                       \
                FOR_EACH(UNION_MATCH_RVALUE_REF_CASE, (type_name), __VA_ARGS__)                    \
            default:                                                                               \
                assert(false && "bad tag");                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        template<typename ReturnType, typename Matcher>                                            \
        ReturnType match(Matcher &&matcher) const &&                                               \
            requires(FOR_EACH(UNION_MATCH_RVALUE_REF_HAS_CASE_AND, (type_name), __VA_ARGS__) true) \
        {                                                                                          \
            switch (m_tag) {                                                                       \
                FOR_EACH(UNION_MATCH_RVALUE_REF_CASE, (type_name), __VA_ARGS__)                    \
            default:                                                                               \
                assert(false && "bad tag");                                                        \
            }                                                                                      \
        }                                                                                          \
    }
#define FIELD(field_type, field_name) (field_type, field_name)

// Pattern matching implementation

#define MATCH_GEN_FUNC(union_info, field_info) MATCH_GEN_FUNC_CALL((UNPACK(union_info), UNPACK(field_info)))
#define MATCH_GEN_FUNC_CALL(concatenated_info) MATCH_GEN_FUNC_IMPL concatenated_info
#define MATCH_GEN_FUNC_IMPL(return_type, value, kind, ...) \
    MATCH_GEN_##kind##_FUNC(return_type, value, __VA_ARGS__)
#define MATCH_GEN_CASE_FUNC(return_type, value, field_name, var_name, ...) \
    [&](tu::in_place_tag_t<std::remove_reference_t<decltype((value))>::tag_t::field_name>, auto &&var_name)->TRY_UNPACK(return_type) __VA_ARGS__,
#define MATCH_GEN_OTHERWISE_FUNC(return_type, value, var_name, ...) \
    [&](auto &&var_name)->TRY_UNPACK(return_type) __VA_ARGS__,

#define MATCH(return_type, expr, ...) \
    (expr).visit<TRY_UNPACK(return_type)>(tu::combined_visitor{FOR_EACH(MATCH_GEN_FUNC, (return_type, expr), __VA_ARGS__)})
#define CASE(field_name, var_name, ...) (CASE, field_name, var_name, __VA_ARGS__)
#define OTHERWISE(var_name, ...) (OTHERWISE, var_name, __VA_ARGS__)

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
