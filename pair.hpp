#ifndef __PAIR_HPP
#define __PAIR_HPP 1

template<class First, class Second> struct pair;
template<class, size_t...> struct array;

namespace std
{
	template<class...> class tuple;

	template<class, class> struct pair;

	template<class, size_t N> class array;
}

#include "util.hpp"
#include "object.hpp"

template<class T> struct pair<T, T>
{
	using first_type = T;
	using second_type = T;

	pair() = default;
	pair(pair const&) = default;
	pair(pair&&) = default;

	template<class First, class Second, type_if<int, convertible_v<First&&, first_type>, convertible_v<Second&&, second_type>> = 0>
	constexpr pair(First&& first, Second&& second) noexcept(is_nothrow_constructible_v<first_type, First&&> && is_nothrow_constructible_v<second_type, Second&&>)
		: first(static_cast<First&&>(first)), second(static_cast<Second&&>(second)) {
	}

	template<class First, class Second, type_if<int, convertible_v<First const&, first_type>, convertible_v<Second const&, second_type>> = 0>
	constexpr pair(pair<First, Second> const& o) noexcept(is_nothrow_constructible_v<first_type, First const&> && is_nothrow_constructible_v<second_type, Second const&>)
		: first(o.first), second(o.second) {
	}

	template<class First, class Second, type_if<int, convertible_v<First&&, first_type>, convertible_v<Second&&, second_type>> = 0>
	constexpr pair(pair<First, Second>&& o) noexcept(is_nothrow_constructible_v<first_type, First&&> && is_nothrow_constructible_v<second_type, Second&&>)
		: first(static_cast<First&&>(o.first)), second(static_cast<Second&&>(o.second)) {
	}

	template<class U, type_if<int, convertible_v<U const&, first_type>, convertible_v<U const&, second_type>> = 0>
	constexpr pair(array<U, 2> const& arr) noexcept(is_nothrow_constructible_v<first_type, U const&>&& is_nothrow_constructible_v<second_type, U const&>)
		: first(std::get<0>(arr)), second(std::get<1>(arr)) {
	}

	template<class U, type_if<int, convertible_v<U&&, first_type>, convertible_v<U&&, second_type>> = 0>
	constexpr pair(array<U, 2>&& arr) noexcept(is_nothrow_constructible_v<first_type, U&&>&& is_nothrow_constructible_v<second_type, U&&>)
		: first(static_cast<U&&>(std::get<0>(arr))), second(static_cast<U&&>(std::get<1>(arr))) {
	}

	template<class First, class Second, type_if<int, convertible_v<First const&, first_type>, convertible_v<Second const&, second_type>> = 0>
	constexpr pair(std::tuple<First, Second> const& o) noexcept(is_nothrow_constructible_v<first_type, First const&> && is_nothrow_constructible_v<second_type, Second const&>)
		: first(std::get<0>(o)), second(std::get<1>(o)) {
	}

	template<class First, class Second, type_if<int, convertible_v<First&&, first_type>, convertible_v<Second&&, second_type>> = 0>
	constexpr pair(std::tuple<First, Second>&& o) noexcept(is_nothrow_constructible_v<first_type, First&&> && is_nothrow_constructible_v<second_type, Second&&>)
		: first(static_cast<First&&>(std::get<0>(o))), second(static_cast<Second&&>(std::get<1>(o))) {
	}

	constexpr void swap(pair& o) noexcept { util::swap_bytes(*this, o); }

	constexpr T* data() noexcept { return (T*)this; }
	constexpr T const* data() const noexcept { return (T const*)this; }

	first_type first;
	second_type second;

	constexpr operator std::tuple<first_type, second_type>& () & noexcept { return reinterpret_cast<std::tuple<first_type, second_type>&>(*this); }
	constexpr operator std::tuple<first_type, second_type> const& () const& noexcept { return reinterpret_cast<std::tuple<first_type, second_type> const&>(*this); }
	constexpr operator std::tuple<first_type, second_type>&& () && noexcept { return reinterpret_cast<std::tuple<first_type, second_type>&&>(*this); }
	constexpr operator std::tuple<first_type, second_type> const&& () const&& noexcept { return reinterpret_cast<std::tuple<first_type, second_type> const&&>(*this); }

	constexpr operator std::pair<first_type, second_type>& () & noexcept { return reinterpret_cast<std::pair<first_type, second_type>&>(*this); }
	constexpr operator std::pair<first_type, second_type> const& () const& noexcept { return reinterpret_cast<std::pair<first_type, second_type> const&>(*this); }
	constexpr operator std::pair<first_type, second_type>&& () && noexcept { return reinterpret_cast<std::pair<first_type, second_type>&&>(*this); }
	constexpr operator std::pair<first_type, second_type> const&& () const&& noexcept { return reinterpret_cast<std::pair<first_type, second_type> const&&>(*this); }

	constexpr operator std::array<T, 2>& () & noexcept { return reinterpret_cast<std::array<T, 2>&>(*this); }
	constexpr operator std::array<T, 2> const& () const& noexcept { return reinterpret_cast<std::array<T, 2> const&>(*this); }
	constexpr operator std::array<T, 2>&& () && noexcept { return reinterpret_cast<std::array<T, 2>&&>(*this); }
	constexpr operator std::array<T, 2> const&& () const&& noexcept { return reinterpret_cast<std::array<T, 2> const&&>(*this); }

	constexpr operator array<T, 2>& () & noexcept { return reinterpret_cast<array<T, 2>&>(*this); }
	constexpr operator array<T, 2> const& () const& noexcept { return reinterpret_cast<array<T, 2> const&>(*this); }
	constexpr operator array<T, 2> && () && noexcept { return reinterpret_cast<array<T, 2>&&>(*this); }
	constexpr operator array<T, 2> const&& () const&& noexcept { return reinterpret_cast<array<T, 2> const&&>(*this); }
};

template<class First, class Second> struct pair
{
	using first_type = First;
	using second_type = Second;

	pair() = default;
	pair(pair const&) = default;
	pair(pair&&) = default;

	template<class First, class Second, type_if<int, convertible_v<First&&, first_type>, convertible_v<Second&&, second_type>> = 0>
	constexpr pair(First&& first, Second&& second) noexcept(is_nothrow_constructible_v<first_type, First&&> && is_nothrow_constructible_v<second_type, Second&&>)
		: first(static_cast<First&&>(first)), second(static_cast<Second&&>(second)) {
	}

	template<class First, class Second, type_if<int, convertible_v<First const&, first_type>, convertible_v<Second const&, second_type>> = 0>
	constexpr pair(pair<First, Second> const& o) noexcept(is_nothrow_constructible_v<first_type, First const&> && is_nothrow_constructible_v<second_type, Second const&>)
		: first(o.first), second(o.second) {
	}

	template<class First, class Second, type_if<int, convertible_v<First&&, first_type>, convertible_v<Second&&, second_type>> = 0>
	constexpr pair(pair<First, Second>&& o) noexcept(is_nothrow_constructible_v<first_type, First&&> && is_nothrow_constructible_v<second_type, Second&&>)
		: first(static_cast<First&&>(o.first)), second(static_cast<Second&&>(o.second)) {
	}

	template<class U, type_if<int, convertible_v<U const&, first_type>, convertible_v<U const&, second_type>> = 0>
	constexpr pair(array<U, 2> const& arr) noexcept(is_nothrow_constructible_v<first_type, U const&>&& is_nothrow_constructible_v<second_type, U const&>)
		: first(std::get<0>(arr)), second(std::get<1>(arr)) {
	}

	template<class U, type_if<int, convertible_v<U&&, first_type>, convertible_v<U&&, second_type>> = 0>
	constexpr pair(array<U, 2>&& arr) noexcept(is_nothrow_constructible_v<first_type, U&&>&& is_nothrow_constructible_v<second_type, U&&>)
		: first(static_cast<U&&>(std::get<0>(arr))), second(static_cast<U&&>(std::get<1>(arr))) {
	}

	template<class First, class Second, type_if<int, convertible_v<First const&, first_type>, convertible_v<Second const&, second_type>> = 0>
	constexpr pair(std::tuple<First, Second> const& o) noexcept(is_nothrow_constructible_v<first_type, First const&> && is_nothrow_constructible_v<second_type, Second const&>)
		: first(std::get<0>(o)), second(std::get<1>(o)) {
	}

	template<class First, class Second, type_if<int, convertible_v<First&&, first_type>, convertible_v<Second&&, second_type>> = 0>
	constexpr pair(std::tuple<First, Second>&& o) noexcept(is_nothrow_constructible_v<first_type, First&&> && is_nothrow_constructible_v<second_type, Second&&>)
		: first(static_cast<First&&>(std::get<0>(o))), second(static_cast<Second&&>(std::get<1>(o))) {
	}

	constexpr void swap(pair& o) noexcept { util::swap_bytes(*this, o); }

	first_type first;
	second_type second;

	constexpr operator std::tuple<first_type, second_type>& () & noexcept { return reinterpret_cast<std::tuple<first_type, second_type>&>(*this); }
	constexpr operator std::tuple<first_type, second_type> const& () const& noexcept { return reinterpret_cast<std::tuple<first_type, second_type> const&>(*this); }
	constexpr operator std::tuple<first_type, second_type>&& () && noexcept { return reinterpret_cast<std::tuple<first_type, second_type>&&>(*this); }
	constexpr operator std::tuple<first_type, second_type> const&& () const&& noexcept { return reinterpret_cast<std::tuple<first_type, second_type> const&&>(*this); }

	constexpr operator std::pair<first_type, second_type>& () & noexcept { return reinterpret_cast<std::pair<first_type, second_type>&>(*this); }
	constexpr operator std::pair<first_type, second_type> const& () const& noexcept { return reinterpret_cast<std::pair<first_type, second_type> const&>(*this); }
	constexpr operator std::pair<first_type, second_type>&& () && noexcept { return reinterpret_cast<std::pair<first_type, second_type>&&>(*this); }
	constexpr operator std::pair<first_type, second_type> const&& () const&& noexcept { return reinterpret_cast<std::pair<first_type, second_type> const&&>(*this); }
};

#if _HAS_CXX17
template <class First, class Second>
pair(First, Second)->pair<First, Second>;
#endif // _HAS_CXX17

template<class LFirst, class LSecond, class RFirst, class RSecond, class = decltype(std::declval<LFirst>() == std::declval<RFirst>()), class = decltype(std::declval<LSecond>() == std::declval<RSecond>())>
constexpr bool operator==(pair<LFirst, LSecond> const& _Left, pair<RFirst, RSecond> const& _Right) noexcept
{
	return _Left.first == _Right.first && _Left.second == _Right.second;
}

template<class LFirst, class LSecond, class RFirst, class RSecond, class = decltype(std::declval<LFirst>() != std::declval<RFirst>()), class = decltype(std::declval<LSecond>() != std::declval<RSecond>())>
constexpr bool operator!=(pair<LFirst, LSecond> const& _Left, pair<RFirst, RSecond> const& _Right) noexcept
{
	return _Left.first != _Right.first || _Left.second != _Right.second;
}

namespace std
{
	template<class LFirst, class LSecond, class RFirst, class RSecond>
	auto swap(::pair<LFirst, LSecond>& _Left, ::pair<RFirst, RSecond>& _Right) noexcept(noexcept(_Left.swap(_Right)))
		-> decltype(_Left.swap(_Right)) {
		return _Left.swap(_Right);
	}

	template<size_t I, class First, class Second>
	::type_if<First&, I == 0> get(::pair<First, Second>& o) noexcept {
		return o.first;
	}
	template<size_t I, class First, class Second>
	::type_if<First const&, I == 0> get(::pair<First, Second> const& o) noexcept {
		return o.first;
	}
	template<size_t I, class First, class Second>
	::type_if<First&&, I == 0> get(::pair<First, Second>&& o) noexcept {
		return static_cast<First&&>(o.first);
	}

	template<size_t I, class First, class Second>
	::type_if<Second&, I == 1> get(::pair<First, Second>& o) noexcept {
		return o.second;
	}
	template<size_t I, class First, class Second>
	::type_if<Second const&, I == 1> get(::pair<First, Second> const& o) noexcept {
		return o.second;
	}
	template<size_t I, class First, class Second>
	::type_if<Second&&, I == 1> get(::pair<First, Second>&& o) noexcept {
		return static_cast<Second&&>(o.second);
	}

	template<class Res, class _Ty2>
	constexpr Res& get(::pair<Res, _Ty2>& o) noexcept {
		return o.first;
	}
	template<class Res, class _Ty2>
	constexpr Res const& get(::pair<Res, _Ty2> const& o) noexcept {
		return o.first;
	}
	template<class Res, class _Ty2>
	constexpr Res&& get(::pair<Res, _Ty2>&& o) noexcept {
		return static_cast<Res&&>(o.first);
	}

	template<class Res, class _Ty1>
	constexpr Res& get(::pair<_Ty1, Res>& o) noexcept {
		return o.second;
	}
	template<class Res, class _Ty1>
	constexpr Res const& get(::pair<_Ty1, Res> const& o) noexcept {
		return o.second;
	}
	template<class Res, class _Ty1>
	constexpr Res&& get(::pair<_Ty1, Res>&& o) noexcept {
		return static_cast<Res&&>(o.second);
	}
}

#endif // !__PAIR_HPP
