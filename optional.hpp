#ifndef __OPTIONAL_HPP
#define __OPTIONAL_HPP 1

#if _HAS_CXX17
namespace std
{
    template<class> class optional;
}
#endif // _HAS_CXX17

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
            objects::destroy(m_value);
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

        template<class V> constexpr explicit _base(V&& value) noexcept(is_nothrow_constructible_v<T, V&&>)
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

        _NODISCARD constexpr T* _ptr() noexcept { return std::addressof(m_value); }
        _NODISCARD constexpr T const* _ptr() const noexcept { return std::addressof(m_value); }

        template<class V> void _construct(V&& value) noexcept(is_nothrow_constructible_v<T, V&&>) {
            new(&m_value) T(static_cast<V&&>(value));
            m_present = true;
        }

        template<class V> constexpr void _assign(V&& value, false_type) noexcept(is_nothrow_constructible_v<T, V&&>) {
            _reset();
            _construct(static_cast<V&&>(value));
        }

        template<class V> constexpr void _assign(V&& value, true_type) noexcept(is_nothrow_constructible_v<T, V&&>&& is_nothrow_assignable_v<T, V&&>) {
            if (m_present) {
                m_value = static_cast<V&&>(value);
            } else {
                _assign(static_cast<V&&>(value), false_type);
            }
        }

        template<class V> void _assign(V&& value) noexcept(noexcept(_assign(static_cast<V&&>(value), is_assignable<T&, V&&>))) {
            _assign(static_cast<V&&>(value), is_assignable<T&, V&&>);
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

    template<class Dx, type_if<int, is_move_constructible_v<T>> = 0>
    optional(std::unique_ptr<T, Dx>&& ptr) noexcept(noexcept(_base::_construct(static_cast<T&&>(*ptr)))) {
        if (ptr) _base::_construct(static_cast<T&&>(*ptr));
        ptr.reset();
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

    _NODISCARD constexpr operator bool() const noexcept { return has_value(); }

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

    _NODISCARD constexpr T& or_default(T& deflt) noexcept {
        return has_value() ? value() : deflt;
    }

    _NODISCARD constexpr T const& or_default(T const& deflt) const noexcept {
        return has_value() ? value() : deflt;
    }

    _NODISCARD constexpr T& or_default(T&& deflt) noexcept {
        return has_value() ? value() : deflt;
    }

    _NODISCARD constexpr T const& or_default(T const&& deflt) const noexcept {
        return has_value() ? value() : deflt;
    }

    template<class Comp = default_comporator, class U, class Ord = util::invoke_result_t<default_comporator, T const&, U const&>>
    constexpr type_if<Ord, objects::is_ordering_v<Ord>> compare(optional<U> const& right, Comp&& comp = Comp{}) const
        noexcept(util::nothrow_invocable_v<default_comporator, T const&, U const&>) {
        return has_value() ? (right ? util::invoke(static_cast<Comp&&>(comp), value(), *right) : Ord{1}) : (right ? Ord{-1} : Ord{0});
    }

    template<class Comp = default_comporator, class V, class Ord = util::invoke_result_t<default_comporator, T const&, V&&>>
    constexpr type_if<Ord, objects::is_ordering_v<Ord>> compare(V&& right, Comp&& comp = Comp{}) const
        noexcept(util::nothrow_invocable_v<default_comporator, T const&, V&&>) {
        return has_value() ? util::invoke(static_cast<Comp&&>(comp), value(), static_cast<V&&>(right)) : Ord{-1};
    }

protected:
    using _base::m_present;

    template<class...> friend struct optional;

#if _HAS_CXX17
public:
    constexpr operator std::optional<T>& () & noexcept { return reinterpret_cast<std::optional<T>&>(*this); }
    constexpr operator std::optional<T> const& () const& noexcept { return reinterpret_cast<std::optional<T> const&>(*this); }
    constexpr operator std::optional<T> && () && noexcept { return reinterpret_cast<std::optional<T>&&>(*this); }
    constexpr operator std::optional<T> const&& () const&& noexcept { return reinterpret_cast<std::optional<T> const&&>(*this); }
#endif // _HAS_CXX17
};

#if _HAS_CXX17
template<class T> optional(T const&)->optional<T>;
template<class T> optional(T&&)->optional<T>;
optional(optional<>)->optional<>;
#endif // _HAS_CXX17

template<class L> _NODISCARD constexpr bool operator==(optional<L> const& _Left, nullopt_t) noexcept {
    return !_Left.has_value();
}
template<class R> _NODISCARD constexpr bool operator==(nullopt_t, optional<R> const& _Right) noexcept {
    return !_Right.has_value();
}

template<class L> _NODISCARD constexpr bool operator!=(optional<L> const& _Left, nullopt_t) noexcept {
    return _Left.has_value();
}
template<class R> _NODISCARD constexpr bool operator!=(nullopt_t, optional<R> const& _Right) noexcept {
    return _Right.has_value();
}

template<class L, class R>
_NODISCARD constexpr auto operator==(optional<L> const& _Left, optional<R> const& _Right) noexcept(noexcept(*_Left == *_Right))
-> type_if<bool, convertible_v<decltype(*_Left == *_Right), bool>> {
    bool const _Left_has_value = _Left.has_value();
    return _Left_has_value == _Right.has_value() && (!_Left_has_value || *_Left == *_Right);
}

template<class L, class R>
_NODISCARD constexpr auto operator!=(optional<L> const& _Left, optional<R> const& _Right) noexcept(noexcept(*_Left != *_Right))
-> type_if<bool, convertible_v<decltype(*_Left != *_Right), bool>> {
    bool const _Left_has_value = _Left.has_value();
    return _Left_has_value != _Right.has_value() || (_Left_has_value && *_Left != *_Right);
}

template<class L, class R>
_NODISCARD constexpr auto operator==(optional<L> const& _Left, R const& _Right) noexcept(noexcept(*_Left == _Right))
-> type_if<bool, convertible_v<decltype(*_Left == _Right), bool>> {
    return _Left ? *_Left == _Right : false;
}
template<class L, class R>
_NODISCARD constexpr auto operator==(L const& _Left, optional<R> const& _Right) noexcept(noexcept(_Left == *_Right))
-> type_if<bool, convertible_v<decltype(_Left == *_Right), bool>> {
    return _Right ? _Left == *_Right : false;
}

template<class L, class R>
_NODISCARD constexpr auto operator!=(optional<L> const& _Left, R const& _Right) noexcept(noexcept(*_Left != _Right))
-> type_if<bool, convertible_v<decltype(*_Left != _Right), bool>> {
    return _Left ? *_Left != _Right : true;
}
template<class L, class R>
_NODISCARD constexpr auto operator!=(L const& _Left, optional<R> const& _Right) noexcept(noexcept(_Left != *_Right))
-> type_if<bool, convertible_v<decltype(_Left != *_Right), bool>> {
    return _Right ? _Left != *_Right : true;
}

#if _HAS_CXX20
_NODISCARD constexpr std::strong_ordering operator<=>(nullopt_t, nullopt_t) noexcept {
    return std::strong_ordering::equal;
}
template<class L> _NODISCARD constexpr std::strong_ordering operator<=>(optional<L> const& _Left, nullopt_t) noexcept {
    return _Left ? std::strong_ordering::greater : std::strong_ordering::equal;
}
template<class R> _NODISCARD constexpr std::strong_ordering operator<=>(nullopt_t, optional<R> const& _Right) noexcept {
    return _Right ? std::strong_ordering::less : std::strong_ordering::equal;
}

template<class L, class R, class Ord = decltype(std::declval<L const&>() <=> std::declval<R const&>())>
_NODISCARD constexpr auto operator<=>(optional<L> const& _Left, optional<R> const& _Right) noexcept
-> decltype(*_Left <=> *_Right) {
    return _Left ? (_Right ? *_Left <=> *_Right : Ord{1}) : (_Right ? Ord{-1} : Ord{0});
}
template<class L, class R, class Ord = decltype(std::declval<L const&>() <=> std::declval<R const&>())>
_NODISCARD constexpr auto operator<=>(optional<L> const& _Left, R const& _Right) noexcept
-> decltype(*_Left <=> _Right) {
    return _Left ? *_Left <=> _Right : Ord{-1};
}
template<class L, class R, class Ord = decltype(std::declval<L const&>() <=> std::declval<R const&>())>
_NODISCARD constexpr auto operator<=>(L const& _Left, optional<R> const& _Right) noexcept
-> decltype(_Left <=> *_Right) {
    return _Right ? _Left <=> *_Right : Ord{1};
}
#endif // _HAS_CXX20

#endif // !__OPTIONAL_HPP