#ifndef __COMPORATOR_HPP
#define __COMPORATOR_HPP 1

#include "util.hpp"
#include "object.hpp"

template<class, class> struct comporator;

template<class EqualTo = equal_to, class Less = less>
struct comporator : protected object<EqualTo>, protected object<Less> {
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
		operator()(L&& l, R&& r) noexcept(util::nothrow_invocable_v<EqualTo, L, R>&& util::nothrow_invocable_v<Less, L, R>) {
		if (static_cast<EqualTo&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)))
			return 0;

		return static_cast<Less&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)) ? -1 : 1;
	}

	template<class L, class R>
	constexpr type_if<signed int, convertible_v<util::invoke_result_t<Less const, L, R>, bool>, convertible_v<util::invoke_result_t<EqualTo const, L, R>, bool>>
		operator()(L&& l, R&& r) const noexcept(util::nothrow_invocable_v<EqualTo, L, R>&& util::nothrow_invocable_v<Less, L, R>) {
		if (static_cast<EqualTo const&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)))
			return 0;

		return static_cast<Less const&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)) ? -1 : 1;
	}

	using is_transparent = int;
};

template<class Less>
struct comporator<void, Less> : protected object<Less> {
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
		operator()(L&& l, R&& r) noexcept(util::nothrow_invocable_v<Less, L, R>&& util::nothrow_invocable_v<Less, R, L>) {
		if (static_cast<Less&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)))
			return -1;
		if (static_cast<Less&>(*this)(static_cast<R&&>(r), static_cast<L&&>(l)))
			return 1;
		return 0;
	}

	template<class L, class R>
	constexpr type_if<signed int, convertible_v<util::invoke_result_t<Less const, L, R>, bool>, convertible_v<util::invoke_result_t<Less const, R, L>, bool>>
		operator()(L&& l, R&& r) const noexcept(util::nothrow_invocable_v<Less, L, R>&& util::nothrow_invocable_v<Less, R, L>) {
		if (static_cast<Less const&>(*this)(static_cast<L&&>(l), static_cast<R&&>(r)))
			return -1;
		if (static_cast<Less const&>(*this)(static_cast<R&&>(r), static_cast<L&&>(l)))
			return 1;
		return 0;
	}

	using is_transparent = int;
};

template<> struct comporator<void, void> {
	template<class L, class R, class... Args, class Ord = decltype(null<L>->compare(std::declval<R>(), std::declval<Args>()...))>
	constexpr auto operator()(L&& l, R&& r, Args&&... args) noexcept(noexcept(static_cast<L&&>(l).compare(static_cast<R&&>(r), static_cast<Args&&>(args)...)))
		-> type_if<decltype(static_cast<L&&>(l).compare(static_cast<R&&>(r), static_cast<Args&&>(args)...)), objects::is_ordering_v<Ord>> {
		return static_cast<L&&>(l).compare(static_cast<R&&>(r), static_cast<Args&&>(args)...);
	}

	using is_transparent = int;
};

#if _HAS_CXX20

struct comporator2
{
private:
	template<class L, class R> static sfinae<decltype(std::declval<L>() <=> std::declval<R>())> _three_way_comporable_v(int);
	template<class, class> static false_type _three_way_comporable_v(...);

public:
	template<class L, class R> constexpr _INLINE_VAR static bool three_way_comporable_v = decltype(_three_way_comporable_v<L, R>(0))::value;

	template<class L, class R> constexpr auto operator()(L&& l, R&& r) const
		noexcept(noexcept(static_cast<L&&>(l) <=> static_cast<R&&>(r)))
		-> decltype(static_cast<L&&>(l) <=> static_cast<R&&>(r)) {
		return static_cast<L&&>(l) <=> static_cast<R&&>(r);
	}

	template<class L, class R>
	constexpr auto operator()(L&& l, R&& r) const noexcept(noexcept(static_cast<L&&>(l) < static_cast<R&&>(r)) && noexcept(static_cast<R&&>(r) < static_cast<L&&>(l)))
		-> type_if<signed int, convertible_v<decltype(static_cast<L&&>(l) < static_cast<R&&>(r)), bool>, convertible_v<decltype(static_cast<R&&>(r) < static_cast<L&&>(l)), bool>,
		!three_way_comporable_v<L, R>> {
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
