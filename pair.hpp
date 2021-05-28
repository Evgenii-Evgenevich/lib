#ifndef __PAIR_HPP
#define __PAIR_HPP 1

template<class = void, class = void> struct pair;
template<class, size_t...> struct array;

namespace std
{
	template<class...> class tuple;
	template<class, class> struct pair;
	template<class, size_t N> class array;

	template<size_t I, class First, class Second>
	constexpr ::type_if<First&, (I == 0)> get(::pair<First, Second>&) noexcept;
	template<size_t I, class First, class Second>
	constexpr ::type_if<First const&, (I == 0)> get(::pair<First, Second> const&) noexcept;
	template<size_t I, class First, class Second>
	constexpr ::type_if<First&&, (I == 0)> get(::pair<First, Second>&&) noexcept;
	template<size_t I, class First, class Second>
	constexpr ::type_if<First const&&, (I == 0)> get(::pair<First, Second> const&&) noexcept;

	template<size_t I, class First, class Second>
	constexpr ::type_if<Second&, (I == 1)> get(::pair<First, Second>&) noexcept;
	template<size_t I, class First, class Second>
	constexpr ::type_if<Second const&, (I == 1)> get(::pair<First, Second> const&) noexcept;
	template<size_t I, class First, class Second>
	constexpr ::type_if<Second&&, (I == 1)> get(::pair<First, Second>&&) noexcept;
	template<size_t I, class First, class Second>
	constexpr ::type_if<Second const&&, (I == 1)> get(::pair<First, Second> const&&) noexcept;

	template<size_t I, class T, size_t N>
	constexpr ::type_if<T&, (I < N)> get(::array<T, N>&) noexcept;
	template<size_t I, class T, size_t N>
	constexpr ::type_if<T const&, (I < N)> get(::array<T, N> const&) noexcept;
	template<size_t I, class T, size_t N>
	constexpr ::type_if<T&&, (I < N)> get(::array<T, N>&&) noexcept;
	template<size_t I, class T, size_t N>
	constexpr ::type_if<T const&&, (I < N)> get(::array<T, N> const&&) noexcept;

	template<class> struct tuple_size;
	template<size_t, class> struct tuple_element;
}

#include "util.hpp"
#include "object.hpp"

using pairs = pair<>;

template<> struct pair<void, void> {
protected:
	pair() = delete;

	template<class, class> friend struct pair;

	struct _dummy_t {};

	template<class First, class Second, bool = std::is_reference<First>::value || std::is_reference<Second>::value>
	struct _base {
		static_assert(!objects::is_polymorphic_v<First>, "requared not polymorphic");
		static_assert(!objects::is_polymorphic_v<Second>, "requared not polymorphic");

		using first_type = First;
		using second_type = Second;

	protected:
		union {
			char _[sizeof(first_type) + sizeof(second_type)];
			struct {
				first_type first;
				second_type second;
			};
		};

		~_base() noexcept {
			objects::destroy(first);
			objects::destroy(second);
		}

	public:
		constexpr _base(_dummy_t) noexcept : _{} {}

		template<class Int = type_if<int, is_constructible_v<first_type>, is_constructible_v<second_type>>, Int = 0>
		constexpr _base() noexcept(is_nothrow_constructible_v<first_type> && is_nothrow_constructible_v<second_type>)
			: first(), second() {
		}

		template<class First, class Second, type_if<int, convertible_v<First&&, first_type>, convertible_v<Second&&, second_type>> = 0>
		constexpr _base(First&& first, Second&& second) noexcept(is_nothrow_constructible_v<first_type, First&&>&& is_nothrow_constructible_v<second_type, Second&&>)
			: first(static_cast<First&&>(first)), second(static_cast<Second&&>(second)) {
		}

		template<class Int = type_if<int, is_copy_constructible_v<first_type>, is_copy_constructible_v<second_type>>, Int = 0>
		constexpr _base(_base const& o) noexcept(is_nothrow_copy_constructible_v<first_type>&& is_nothrow_copy_constructible_v<second_type>)
			: first(o.first), second(o.second) {
		}

		template<class Int = type_if<int, is_move_constructible_v<first_type>, is_move_constructible_v<second_type>>, Int = 0>
		constexpr _base(_base&& o) noexcept(is_nothrow_move_constructible_v<first_type>&& is_nothrow_move_constructible_v<second_type>)
			: first(static_cast<First&&>(o.first)), second(static_cast<Second&&>(o.second)) {
		}

		template<class Other, class First = typename std::tuple_element<0, Other>::type, class Second = typename std::tuple_element<1, Other>::type,
			type_if<int, (std::tuple_size<Other>::value == 2), convertible_v<First const&, first_type>, convertible_v<Second const&, second_type>> = 0>
		constexpr _base(Other const& other) noexcept(is_nothrow_constructible_v<first_type, First const&>&& is_nothrow_constructible_v<second_type, Second const&>)
			: first(std::get<0>(other)), second(std::get<1>(other)) {
		}

		template<class Other, class First = typename std::tuple_element<0, Other>::type, class Second = typename std::tuple_element<1, Other>::type,
			type_if<int, (std::tuple_size<Other>::value == 2), convertible_v<First&&, first_type>, convertible_v<Second&&, second_type>> = 0>
		constexpr _base(Other&& other) noexcept(is_nothrow_constructible_v<first_type, First&&>&& is_nothrow_constructible_v<second_type, Second&&>)
			: first(static_cast<First&&>(std::get<0>(other))), second(static_cast<Second&&>(std::get<1>(other))) {
		}

		template<class First, class Second, size_t... Indexes1, size_t... Indexes2, 
			type_if_value<int, is_constructible_from_tuple<first_type, First, std::index_sequence<Indexes1...>>, is_constructible_from_tuple<second_type, Second, std::index_sequence<Indexes2...>>> = 0>
		constexpr _base(First&& first, Second&& second, std::index_sequence<Indexes1...>, std::index_sequence<Indexes2...>)
			noexcept(all_value<is_nothrow_constructible_from_tuple<first_type, First, std::index_sequence<Indexes1...>>, is_nothrow_constructible_from_tuple<second_type, Second, std::index_sequence<Indexes2...>>>)
			: first(std::get<Indexes1>(static_cast<First&&>(first))...), second(std::get<Indexes2>(static_cast<Second&&>(second))...) {
		}

		template<class First, class Second, class Indexes1 = std::make_index_sequence<std::tuple_size<remove_ref_t<First>>::value>, class Indexes2 = std::make_index_sequence<std::tuple_size<remove_ref_t<Second>>::value>,
			type_if<int, !convertible_v<First&&, first_type>, !convertible_v<Second&&, second_type>> = 0>
		constexpr _base(First&& first, Second&& second) noexcept(is_nothrow_constructible_v<_base, First&&, Second&&, Indexes1, Indexes2>)
			: _base(static_cast<First&&>(first), static_cast<Second&&>(second), Indexes1{}, Indexes2{}) {
		}

		constexpr operator std::tuple<first_type, second_type>& () & noexcept { return reinterpret_cast<std::tuple<first_type, second_type>&>(*this); }
		constexpr operator std::tuple<first_type, second_type> const& () const& noexcept { return reinterpret_cast<std::tuple<first_type, second_type> const&>(*this); }
		constexpr operator std::tuple<first_type, second_type>&& () && noexcept { return reinterpret_cast<std::tuple<first_type, second_type>&&>(*this); }
		constexpr operator std::tuple<first_type, second_type> const&& () const&& noexcept { return reinterpret_cast<std::tuple<first_type, second_type> const&&>(*this); }

		constexpr operator std::pair<first_type, second_type>& () & noexcept { return reinterpret_cast<std::pair<first_type, second_type>&>(*this); }
		constexpr operator std::pair<first_type, second_type> const& () const& noexcept { return reinterpret_cast<std::pair<first_type, second_type> const&>(*this); }
		constexpr operator std::pair<first_type, second_type>&& () && noexcept { return reinterpret_cast<std::pair<first_type, second_type>&&>(*this); }
		constexpr operator std::pair<first_type, second_type> const&& () const&& noexcept { return reinterpret_cast<std::pair<first_type, second_type> const&&>(*this); }
	};

	template<class First, class Second>
	struct _base<First, Second, true> {
		using first_type = First;
		using second_type = Second;

		first_type first;
		second_type second;

		_base(_base const&) = default;
		_base(_base&&) = default;

		template<class First, class Second, type_if<int, convertible_v<First&&, first_type>, convertible_v<Second&&, second_type>> = 0>
		constexpr _base(First&& first, Second&& second) noexcept(is_nothrow_constructible_v<first_type, First&&>&& is_nothrow_constructible_v<second_type, Second&&>)
			: first(static_cast<First&&>(first)), second(static_cast<Second&&>(second)) {
		}

		template<class Other, class First = typename std::tuple_element<0, Other>::type, class Second = typename std::tuple_element<1, Other>::type,
			type_if<int, (std::tuple_size<Other>::value == 2), convertible_v<First&&, first_type>, convertible_v<Second&&, second_type>> = 0>
		constexpr _base(Other&& other) noexcept(is_nothrow_constructible_v<first_type, First&&>&& is_nothrow_constructible_v<second_type, Second&&>)
			: first(static_cast<First&&>(std::get<0>(other))), second(static_cast<Second&&>(std::get<1>(other))) {
		}

		constexpr operator std::tuple<first_type, second_type>& () & noexcept { return reinterpret_cast<std::tuple<first_type, second_type>&>(*this); }
		constexpr operator std::tuple<first_type, second_type> const& () const& noexcept { return reinterpret_cast<std::tuple<first_type, second_type> const&>(*this); }
		constexpr operator std::tuple<first_type, second_type>&& () && noexcept { return reinterpret_cast<std::tuple<first_type, second_type>&&>(*this); }
		constexpr operator std::tuple<first_type, second_type> const&& () const&& noexcept { return reinterpret_cast<std::tuple<first_type, second_type> const&&>(*this); }

		constexpr operator std::pair<first_type, second_type>& () & noexcept { return reinterpret_cast<std::pair<first_type, second_type>&>(*this); }
		constexpr operator std::pair<first_type, second_type> const& () const& noexcept { return reinterpret_cast<std::pair<first_type, second_type> const&>(*this); }
		constexpr operator std::pair<first_type, second_type>&& () && noexcept { return reinterpret_cast<std::pair<first_type, second_type>&&>(*this); }
		constexpr operator std::pair<first_type, second_type> const&& () const&& noexcept { return reinterpret_cast<std::pair<first_type, second_type> const&&>(*this); }
	};

public:
	constexpr _INLINE_VAR static _dummy_t dummy{};

	template<class First, class Second>
	constexpr pair<First, Second> of(First&& first, Second&& second) noexcept(is_nothrow_constructible_v<First, First&&>&& is_nothrow_constructible_v<Second, Second&&>) {
		return { first, second };
	}
};

template<class T> struct pair<T, T> : pair<void, void>::_base<T, T> {
	using value_type = T;
	using reference = T&;
	using const_reference = T const&;
	using size_type = size_t;

	using first_type = T;
	using second_type = T;

	using pair<void, void>::_base<first_type, second_type>::_base;

	using pair<void, void>::_base<first_type, second_type>::first;
	using pair<void, void>::_base<first_type, second_type>::second;

	template<class First, class Second, type_if<int, is_assignable_v<first_type, First const&>, is_assignable_v<second_type, Second const&>> = 0>
	constexpr auto& operator=(pair<First, Second> const& o) noexcept(is_nothrow_assignable_v<first_type, First const&>&& is_nothrow_assignable_v<second_type, Second const&>) {
		this->first = o.first;
		this->second = o.second;
		return *this;
	}

	template<class First, class Second, type_if<int, is_assignable_v<first_type, First&&>, is_assignable_v<second_type, Second&&>> = 0>
	constexpr auto& operator=(pair<First, Second>&& o) noexcept(is_nothrow_assignable_v<first_type, First&&>&& is_nothrow_assignable_v<second_type, Second&&>) {
		this->first = static_cast<first_type&&>(o.first);
		this->second = static_cast<second_type&&>(o.second);
		return *this;
	}

	constexpr void swap(pair& o) noexcept { util::swap_bytes(*this, o); }

	constexpr size_t size() const noexcept { return 2; }

	constexpr T* data() noexcept { return reinterpret_cast<T*>(this); }
	constexpr T const* data() const noexcept { return reinterpret_cast<T const*>(this); }

	constexpr operator std::array<T, 2>& () & noexcept { return reinterpret_cast<std::array<T, 2>&>(*this); }
	constexpr operator std::array<T, 2> const& () const& noexcept { return reinterpret_cast<std::array<T, 2> const&>(*this); }
	constexpr operator std::array<T, 2>&& () && noexcept { return reinterpret_cast<std::array<T, 2>&&>(*this); }
	constexpr operator std::array<T, 2> const&& () const&& noexcept { return reinterpret_cast<std::array<T, 2> const&&>(*this); }

	constexpr operator array<T, 2>& () & noexcept { return reinterpret_cast<array<T, 2>&>(*this); }
	constexpr operator array<T, 2> const& () const& noexcept { return reinterpret_cast<array<T, 2> const&>(*this); }
	constexpr operator array<T, 2> && () && noexcept { return reinterpret_cast<array<T, 2>&&>(*this); }
	constexpr operator array<T, 2> const&& () const&& noexcept { return reinterpret_cast<array<T, 2> const&&>(*this); }
};

template<class First, class Second> struct pair : pair<void, void>::_base<First, Second> {
	using first_type = First;
	using second_type = Second;

	using pair<void, void>::_base<first_type, second_type>::_base;

	using pair<void, void>::_base<first_type, second_type>::first;
	using pair<void, void>::_base<first_type, second_type>::second;

	template<class First, class Second, type_if<int, is_assignable_v<first_type, First const&>, is_assignable_v<second_type, Second const&>> = 0>
	constexpr auto& operator=(pair<First, Second> const& o) noexcept(is_nothrow_assignable_v<first_type, First const&>&& is_nothrow_assignable_v<second_type, Second const&>) {
		this->first = o.first;
		this->second = o.second;
		return *this;
	}

	template<class First, class Second, type_if<int, is_assignable_v<first_type, First&&>, is_assignable_v<second_type, Second&&>> = 0>
	constexpr auto& operator=(pair<First, Second>&& o) noexcept(is_nothrow_assignable_v<first_type, First&&>&& is_nothrow_assignable_v<second_type, Second&&>) {
		this->first = static_cast<first_type&&>(o.first);
		this->second = static_cast<second_type&&>(o.second);
		return *this;
	}

	constexpr void swap(pair& o) noexcept { util::swap_bytes(*this, o); }
};

#if _HAS_CXX17
template<class First, class Second> pair(First const&, Second const&)->pair<First, Second>;
template<class First, class Second> pair(First&&, Second const&)->pair<First, Second>;
template<class First, class Second> pair(First const&, Second&&)->pair<First, Second>;
template<class First, class Second> pair(First&&, Second&&)->pair<First, Second>;
template<class First, class Second> pair(std::reference_wrapper<First>, Second const&)->pair<First&, Second>;
template<class First, class Second> pair(std::reference_wrapper<First>, Second&&)->pair<First&, Second>;
template<class First, class Second> pair(First const&, std::reference_wrapper<Second>)->pair<First, Second&>;
template<class First, class Second> pair(First&&, std::reference_wrapper<Second>)->pair<First, Second&>;
template<class First, class Second> pair(std::reference_wrapper<First>, std::reference_wrapper<Second>)->pair<First&, Second&>;

template<class T> pair(array<T, 2> const&)->pair<T, T>;
template<class T> pair(array<T, 2>&&)->pair<T, T>;
#endif // _HAS_CXX17

template<class LFirst, class LSecond, class RFirst, class RSecond, class = decltype(std::declval<LFirst>() == std::declval<RFirst>()), class = decltype(std::declval<LSecond>() == std::declval<RSecond>())>
constexpr bool operator==(pair<LFirst, LSecond> const& _Left, pair<RFirst, RSecond> const& _Right) noexcept(
	noexcept(std::declval<LFirst>() == std::declval<RFirst>()) && noexcept(std::declval<LSecond>() == std::declval<RSecond>())) {
	return _Left.first == _Right.first && _Left.second == _Right.second;
}

template<class LFirst, class LSecond, class RFirst, class RSecond, class = decltype(std::declval<LFirst>() != std::declval<RFirst>()), class = decltype(std::declval<LSecond>() != std::declval<RSecond>())>
constexpr bool operator!=(pair<LFirst, LSecond> const& _Left, pair<RFirst, RSecond> const& _Right) noexcept(
	noexcept(std::declval<LFirst>() != std::declval<RFirst>()) && noexcept(std::declval<LSecond>() != std::declval<RSecond>())) {
	return _Left.first != _Right.first || _Left.second != _Right.second;
}

#if _HAS_CXX20
template<class LFirst, class LSecond, class RFirst, class RSecond, class = decltype(std::declval<LFirst>() <=> std::declval<RFirst>()), class = decltype(std::declval<LSecond>() <=> std::declval<RSecond>())>
constexpr std::partial_ordering operator<=>(pair<LFirst, LSecond> const& _Left, pair<RFirst, RSecond> const& _Right) noexcept(
	noexcept(std::declval<LFirst>() <=> std::declval<RFirst>()) && noexcept(std::declval<LSecond>() <=> std::declval<RSecond>())) {
	auto const first_ord = _Left.first <=> _Right.first;
	return first_ord == 0 ? _Left.second <=> _Right.second : first_ord;
}
#endif // _HAS_CXX20

namespace std
{
	template<class LFirst, class LSecond, class RFirst, class RSecond>
	auto swap(::pair<LFirst, LSecond>& _Left, ::pair<RFirst, RSecond>& _Right) noexcept(noexcept(_Left.swap(_Right)))
		-> decltype(_Left.swap(_Right)) {
		return _Left.swap(_Right);
	}

	template<size_t I, class First, class Second>
	inline constexpr ::type_if<First&, (I == 0)> get(::pair<First, Second>& o) noexcept {
		return o.first;
	}
	template<size_t I, class First, class Second>
	inline constexpr ::type_if<First const&, (I == 0)> get(::pair<First, Second> const& o) noexcept {
		return o.first;
	}
	template<size_t I, class First, class Second>
	inline constexpr ::type_if<First&&, (I == 0)> get(::pair<First, Second>&& o) noexcept {
		return static_cast<First&&>(o.first);
	}
	template<size_t I, class First, class Second>
	inline constexpr ::type_if<First const&&, (I == 0)> get(::pair<First, Second> const&& o) noexcept {
		return static_cast<First const&&>(o.first);
	}

	template<size_t I, class First, class Second>
	inline constexpr ::type_if<Second&, (I == 1)> get(::pair<First, Second>& o) noexcept {
		return o.second;
	}
	template<size_t I, class First, class Second>
	inline constexpr ::type_if<Second const&, (I == 1)> get(::pair<First, Second> const& o) noexcept {
		return o.second;
	}
	template<size_t I, class First, class Second>
	inline constexpr ::type_if<Second&&, (I == 1)> get(::pair<First, Second>&& o) noexcept {
		return static_cast<Second&&>(o.second);
	}
	template<size_t I, class First, class Second>
	inline constexpr ::type_if<Second const&&, (I == 1)> get(::pair<First, Second> const&& o) noexcept {
		return static_cast<Second const&&>(o.second);
	}

	template<class Res, class _Ty2>
	inline constexpr Res& get(::pair<Res, _Ty2>& o) noexcept {
		return o.first;
	}
	template<class Res, class _Ty2>
	inline constexpr Res const& get(::pair<Res, _Ty2> const& o) noexcept {
		return o.first;
	}
	template<class Res, class _Ty2>
	inline constexpr Res&& get(::pair<Res, _Ty2>&& o) noexcept {
		return static_cast<Res&&>(o.first);
	}

	template<class Res, class _Ty1>
	inline constexpr Res& get(::pair<_Ty1, Res>& o) noexcept {
		return o.second;
	}
	template<class Res, class _Ty1>
	inline constexpr Res const& get(::pair<_Ty1, Res> const& o) noexcept {
		return o.second;
	}
	template<class Res, class _Ty1>
	inline constexpr Res&& get(::pair<_Ty1, Res>&& o) noexcept {
		return static_cast<Res&&>(o.second);
	}

	template<class First, class Second>
	struct tuple_size<::pair<First, Second>> : std::integral_constant<size_t, 2> {};

	template<class First, class Second>
	struct tuple_element<0, ::pair<First, Second>> {
		using type = First;
	};

	template<class First, class Second>
	struct tuple_element<1, ::pair<First, Second>> {
		using type = Second;
	};
}

#endif // !__PAIR_HPP
