#ifndef __COMPORATOR_HPP
#define __COMPORATOR_HPP 1

#include "util.hpp"
#include "object.hpp"

template<class, class> struct comporator;

template<class EqualTo = equal_to, class Less = less>
struct comporator : protected object<EqualTo>, protected object<Less>
{
	comporator(comporator const&) = default;
	comporator(comporator&&) = default;

	template<class Int = type_if<int, objects::is_class_v<EqualTo>, is_constructible_v<EqualTo>, objects::is_class_v<Less>, is_constructible_v<Less>>, Int = 0>
	constexpr comporator() noexcept(is_nothrow_constructible_v<EqualTo>&& is_nothrow_constructible_v<Less>)
		: object<EqualTo>(), object<Less>() {
	}

	template<class Int = type_if<int, is_copy_constructible_v<EqualTo>, is_copy_constructible_v<Less>>, Int = 0>
	constexpr comporator(EqualTo const& equal_to, Less const& less) noexcept(is_nothrow_copy_constructible_v<EqualTo>&& is_nothrow_copy_constructible_v<Less>)
		: object<EqualTo>{ equal_to }, object<Less>{ less } {
	}

	template<class Int = type_if<int, is_copy_constructible_v<EqualTo>, objects::is_class_v<Less>, is_constructible_v<Less>>, Int = 0>
	constexpr comporator(EqualTo const& equal_to) noexcept(is_nothrow_copy_constructible_v<EqualTo>&& is_nothrow_constructible_v<Less>)
		: object<EqualTo>{ equal_to }, object<Less>() {
	}

	template<class Int = type_if<int, objects::is_class_v<EqualTo>, is_constructible_v<EqualTo>, is_copy_constructible_v<Less>>, Int = 0>
	constexpr comporator(Less const& less) noexcept(is_nothrow_constructible_v<EqualTo>&& is_nothrow_copy_constructible_v<Less>)
		: object<EqualTo>(), object<Less>{ less } {
	}

	template<class Int = type_if<int, is_move_constructible_v<EqualTo>, is_move_constructible_v<Less>>, Int = 0>
	constexpr comporator(EqualTo&& equal_to, Less&& less) noexcept(is_nothrow_move_constructible_v<EqualTo>&& is_nothrow_move_constructible_v<Less>)
		: object<EqualTo>{ static_cast<EqualTo&&>(equal_to) }, object<Less>{ static_cast<Less&&>(less) } {
	}

	template<class Int = type_if<int, is_move_constructible_v<EqualTo>, objects::is_class_v<Less>, is_constructible_v<Less>>, Int = 0>
	constexpr comporator(EqualTo&& equal_to) noexcept(is_nothrow_move_constructible_v<EqualTo>&& is_nothrow_constructible_v<Less>)
		: object<EqualTo>{ static_cast<EqualTo&&>(equal_to) }, object<Less>() {
	}

	template<class Int = type_if<int, objects::is_class_v<EqualTo>, is_constructible_v<EqualTo>, is_move_constructible_v<Less>>, Int = 0>
	constexpr comporator(Less&& less) noexcept(is_nothrow_constructible_v<EqualTo>&& is_nothrow_move_constructible_v<Less>)
		: object<EqualTo>(), object<Less>{ static_cast<Less&&>(less) } {
	}

	template<class L, class R>
	constexpr type_if<signed int, convertible_v<util::invoke_result_t<Less, L, R>, bool>, convertible_v<util::invoke_result_t<EqualTo, L, R>, bool>>
		operator()(L&& l, R&& r) noexcept(util::nothrow_invocable_v<EqualTo, L, R>&& util::nothrow_invocable_v<Less, L, R>)
	{
		if (static_cast<EqualTo&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)))
			return 0;

		return static_cast<Less&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)) ? -1 : 1;
	}

	template<class L, class R>
	constexpr type_if<signed int, convertible_v<util::invoke_result_t<Less const, L, R>, bool>, convertible_v<util::invoke_result_t<EqualTo const, L, R>, bool>>
		operator()(L&& l, R&& r) const noexcept(util::nothrow_invocable_v<EqualTo, L, R>&& util::nothrow_invocable_v<Less, L, R>)
	{
		if (static_cast<EqualTo const&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)))
			return 0;

		return static_cast<Less const&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)) ? -1 : 1;
	}

	using is_transparent = int;
};

template<class Less>
struct comporator<void, Less> : protected object<Less>
{
	comporator(comporator const&) = default;
	comporator(comporator&&) = default;

	template<class Int = type_if<int, objects::is_class_v<Less>, is_constructible_v<Less>>, Int = 0>
	constexpr comporator() noexcept(is_nothrow_constructible_v<Less>)
		: object<Less>() {
	}

	template<class Int = type_if<int, is_copy_constructible_v<Less>>, Int = 0>
	constexpr comporator(Less const& less) noexcept(is_nothrow_copy_constructible_v<Less>)
		: object<Less>{ less } {
	}

	template<class Int = type_if<int, is_move_constructible_v<Less>>, Int = 0>
	constexpr comporator(Less&& less) noexcept(is_nothrow_move_constructible_v<Less>)
		: object<Less>{ static_cast<Less&&>(less) } {
	}

	template<class L, class R>
	constexpr type_if<signed int, convertible_v<util::invoke_result_t<Less, L, R>, bool>, convertible_v<util::invoke_result_t<Less, R, L>, bool>>
		operator()(L&& l, R&& r) noexcept(util::nothrow_invocable_v<Less, L, R>&& util::nothrow_invocable_v<Less, R, L>)
	{
		if (static_cast<Less&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)))
			return -1;
		if (static_cast<Less&>(*this)(static_cast<R&&>(r), static_cast<L&&>(l)))
			return 1;
		return 0;
	}

	template<class L, class R>
	constexpr type_if<signed int, convertible_v<util::invoke_result_t<Less const, L, R>, bool>, convertible_v<util::invoke_result_t<Less const, R, L>, bool>>
		operator()(L&& l, R&& r) const noexcept(util::nothrow_invocable_v<Less, L, R>&& util::nothrow_invocable_v<Less, R, L>)
	{
		if (static_cast<Less const&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)))
			return -1;
		if (static_cast<Less const&>(*this)(static_cast<R&&>(r), static_cast<L&&>(l)))
			return 1;
		return 0;
	}

	using is_transparent = int;
};

#if _HAS_CXX20

struct comporator2
{
protected:
	template<class L, class R> static sfinae<decltype(std::declval<L>() <=> std::declval<R>())> _comporable(int);
	template<class, class> static std::false_type _comporable(...);

public:
	template<class L, class R> _INLINE_VAR constexpr static bool comporable_v =
		decltype(_comporable<L, R>(0))::value;

	template<class L, class R> constexpr auto operator()(L&& l, R&& r) const
		noexcept(noexcept(static_cast<L&&>(l) <=> static_cast<R&&>(r)))
		-> decltype(static_cast<L&&>(l) <=> static_cast<R&&>(r))
	{
		return static_cast<L&&>(l) <=> static_cast<R&&>(r);
	}

	template<class L, class R>
	constexpr type_if<signed int, !comporable_v<L, R>, convertible_v<decltype(std::declval<L>() < std::declval<R>()), bool>, convertible_v<decltype(std::declval<R>() < std::declval<L>()), bool>>
		operator()(L&& l, R&& r) const noexcept(
			noexcept(static_cast<L&&>(l) < static_cast<R&&>(r)) && noexcept(static_cast<R&&>(r) < static_cast<L&&>(l)))
	{
		return static_cast<L&&>(l) < static_cast<R&&>(r) ? -1 :
			(static_cast<R&&>(r) < static_cast<L&&>(l) ? 1 : 0);
	}
	
	using is_transparent = int;
};

struct default_comporator : comporator2 {};

#else

struct default_comporator : comporator<> {};

#endif // _HAS_CXX20

struct always_eq
{
	template<class... Args> constexpr signed int operator()(Args&&...) const noexcept { return 0; }
};

struct always_lt
{
	template<class... Args> constexpr signed int operator()(Args&&...) const noexcept { return -1; }
};

struct always_gt
{
	template<class... Args> constexpr signed int operator()(Args&&...) const noexcept { return 1; }
};

#endif // !__COMPORATOR_HPP
