#ifndef __OPTIONAL_HPP
#define __OPTIONAL_HPP 1

#include "util.hpp"
#include "object.hpp"
#include <memory>

template<class...> struct optional
{
    constexpr operator bool() const noexcept { return false; }

protected:
    template<class T, bool = std::is_trivially_destructible<T>::value> struct _data_base
    {
        constexpr _data_base() noexcept
            : m_{}, m_present(false) {
        }

        template<class V> constexpr explicit _data_base(V&& value) noexcept(is_nothrow_constructible_v<T, V&&>)
            : m_value{ static_cast<V&&>(value) }, m_present(true) {
        }

        void _reset() noexcept {
            m_present = false;
        }

        union {
            T m_value;
            char m_[sizeof T];
        };
        bool m_present;
    };

    template<class T> struct _data_base<T, false>
    {
        constexpr _data_base() noexcept
            : m_{}, m_present(false) {
        }

        template<class V> constexpr explicit _data_base(V&& value) noexcept(is_nothrow_constructible_v<T, V&&>)
            : m_value{ static_cast<V&&>(value) }, m_present(true) {
        }

        ~_data_base() noexcept {
            _reset();
        }

        void _reset() noexcept {
            if (m_present) {
                objects::destroy(m_value);
                m_present = false;
            }
        }

        union {
            T m_value;
            char m_[sizeof T];
        };
        bool m_present;
    };

    template<class T> struct _base : _data_base<T>
    {
        using _data_base<T>::_data_base;

        using _data_base = _data_base<T>;

        constexpr _base() noexcept
            : _data_base() {
        }

        template<class V> constexpr explicit _base(V&& value) noexcept(std::is_nothrow_constructible<T, V&&>::value)
            : _data_base(static_cast<V&&>(value)) {
        }

        constexpr _base(_base&& o) noexcept
            : _data_base() {
            objects::swap_bytes(*this, o);
        }

        using _data_base::_reset;

        _NODISCARD constexpr bool _present() const noexcept { return m_present; }

        _NODISCARD constexpr T& _ref() noexcept { return m_value; }
        _NODISCARD constexpr T const& _ref() const noexcept { return m_value; }

        _NODISCARD constexpr T* _ptr() noexcept { return &m_value; }
        _NODISCARD constexpr T const* _ptr() const noexcept { return &m_value; }

        template<class V> void _construct(V&& value) noexcept(is_nothrow_constructible_v<T, V&&>) {
            new(&m_value) T(static_cast<V&&>(value));
            m_present = true;
        }

        template<class V> void _assign(V&& value) noexcept(is_nothrow_constructible_v<T, V&&>&& is_nothrow_assignable_v<T, V&&>) {
            if (m_present)
            {
                m_value = static_cast<V&&>(value);
            }
            else
            {
                _construct(static_cast<V&&>(value));
            }
        }

        using _data_base::m_present;
        using _data_base::m_value;
    };

    template<class...> friend struct optional;
};

using nullopt_t = optional<>;
_INLINE_VAR constexpr nullopt_t nullopt;

template<class T> struct optional<T> : protected optional<>::_base<T>
{
    using _base = optional<>::_base<T>;
    using value_type = T;

    constexpr optional() noexcept : _base() {}

    constexpr optional(nullopt_t) noexcept : _base() {}

    constexpr optional& operator=(nullopt_t) noexcept { reset(); return *this; }

    template<class U, type_if<int, is_constructible_v<T, U&&>> = 0>
    constexpr optional(U&& value) noexcept(noexcept(_base(static_cast<U&&>(value))))
        : _base(static_cast<U&&>(value)) {
    }

    template<class U, type_if<int, is_constructible_v<T, U&&>> = 0>
    constexpr optional(std::unique_ptr<U>&& ptr) noexcept(noexcept(_base::_construct(static_cast<U&&>(*ptr.release())))) {
        if (ptr) _base::_construct(static_cast<U&&>(*ptr.release()));
    }

    constexpr optional(type_if<optional, is_copy_constructible_v<T>> const& other) noexcept(noexcept(_base::_construct(*other))) {
        if (other) _base::_construct(*other);
    }

    template<class U, type_if<int, !std::is_reference<U>::value, is_constructible_v<T, U const&>> = 0>
    constexpr optional(optional<U> const& other) noexcept(noexcept(_base::_construct(*other))) {
        if (other) _base::_construct(*other);
    }

    template<class U, type_if<int, !std::is_reference<U>::value, is_constructible_v<T, U const&>> = 0>
    constexpr optional& operator=(optional<U> const& right) noexcept(noexcept(_base::_assign(*right))) {
        if (this != std::addressof(right))
        {
            if (right) _base::_assign(*right);
            else reset();
        }
        return *this;
    }

    constexpr optional(optional&& other) noexcept(noexcept(_base(static_cast<_base&&>(other))))
        : _base(static_cast<_base&&>(other)) {
    }

    constexpr optional& operator=(optional&& right) noexcept {
        if (this != std::addressof(right))
        {
            reset();
            objects::swap_bytes(*this, right);
        }
        return *this;
    }

    template<class U, type_if<int, !std::is_reference<U>::value, is_constructible_v<T, U&&>> = 0>
    constexpr optional(optional<U>&& other) noexcept(noexcept(_base::_construct(static_cast<U&&>(*other)))) {
        if (other) _base::_construct(static_cast<U&&>(*other));
    }

    template<class U, type_if<int, !std::is_reference<U>::value, is_constructible_v<T, U&&>> = 0>
    constexpr optional& operator=(optional<U>&& right) noexcept(noexcept(_base::_assign(static_cast<U&&>(*right)))) {
        if (this != std::addressof(right))
        {
            if (right) _base::_assign(static_cast<U&&>(*right));
            else reset();
        }
        return *this;
    }

    constexpr void reset() noexcept { _base::_reset(); }

    _NODISCARD constexpr bool has_value() const noexcept { return _base::_present(); }

    constexpr operator bool() const noexcept { return has_value(); }

    _NODISCARD constexpr T& operator*() & noexcept { return _base::_ref(); }

    _NODISCARD constexpr T const& operator*() const& noexcept { return _base::_ref(); }

    _NODISCARD constexpr T&& operator*() && noexcept { return static_cast<T&&>(_base::_ref()); }

    _NODISCARD constexpr T const&& operator*() const&& noexcept { return static_cast<T const&&>(_base::_ref()); }

    _NODISCARD constexpr T* operator->() noexcept { return _base::_ptr(); }

    _NODISCARD constexpr T const* operator->() const noexcept { return _base::_ptr(); }

    _NODISCARD constexpr T& value() & noexcept { return _base::_ref(); }

    _NODISCARD constexpr T const& value() const& noexcept { return _base::_ref(); }

    _NODISCARD constexpr T&& value() && noexcept { return static_cast<T&&>(_base::_ref()); }

    _NODISCARD constexpr T const&& value() const&& noexcept { return static_cast<T const&&>(_base::_ref()); }

    _NODISCARD constexpr T const& or_default(T&& deflt) const noexcept {
        return has_value() ? value() : deflt;
    }

protected:
    using _base::m_present;

    template<class...> friend struct optional;
};

#endif // !__OPTIONAL_HPP