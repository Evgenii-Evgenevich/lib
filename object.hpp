#ifndef __OBJECT_HPP
#define __OBJECT_HPP 1

#include "types.hpp"
#include <xmemory>

namespace std
{
	template<class R, class T>
	type_if<R, convertible_v<T&&, R&&>> get(T&& o) noexcept {
		return static_cast<R&&>(o);
	}

	template<class> struct tuple_size;
	template<size_t, class> struct tuple_element;
}

template<class Tuple> using make_index_sequence = std::make_index_sequence<std::tuple_size<::remove_ref_t<Tuple>>::value>;
template<size_t I, class Tuple> using tuple_element_t = typename std::tuple_element<I, ::remove_ref_t<Tuple>>::type;

template<class T, class... Args> _INLINE_VAR constexpr bool is_constructible_v = std::is_constructible<T, Args...>::value;
template<class T, class... Args> _INLINE_VAR constexpr bool is_trivially_constructible_v = std::is_trivially_constructible<T, Args...>::value;
template<class T, class... Args> _INLINE_VAR constexpr bool is_nothrow_constructible_v = std::is_nothrow_constructible<T, Args...>::value;

template<class T, class Tuple, class Indices = make_index_sequence<Tuple>> _INLINE_VAR constexpr bool is_constructible_from_elements_v = false;
template<class T, class Tuple, size_t... Indices> _INLINE_VAR constexpr bool is_constructible_from_elements_v<T, Tuple, std::index_sequence<Indices...>> = is_constructible_v<T, tuple_element_t<Indices, Tuple>...>;

template<class T, class Tuple, class Indices = make_index_sequence<Tuple>> _INLINE_VAR constexpr bool is_trivially_constructible_from_elements_v = false;
template<class T, class Tuple, size_t... Indices> _INLINE_VAR constexpr bool is_trivially_constructible_from_elements_v<T, Tuple, std::index_sequence<Indices...>> = is_trivially_constructible_v<T, tuple_element_t<Indices, Tuple>...>;

template<class T, class Tuple, class Indices = make_index_sequence<Tuple>> _INLINE_VAR constexpr bool is_nothrow_constructible_from_elements_v = false;
template<class T, class Tuple, size_t... Indices> _INLINE_VAR constexpr bool is_nothrow_constructible_from_elements_v<T, Tuple, std::index_sequence<Indices...>> = is_nothrow_constructible_v<T, tuple_element_t<Indices, Tuple>...>;

template<class T> _INLINE_VAR constexpr bool is_copy_constructible_v = std::is_copy_constructible<T>::value;
template<class T> _INLINE_VAR constexpr bool is_nothrow_copy_constructible_v = std::is_nothrow_copy_constructible<T>::value;
template<class T> _INLINE_VAR constexpr bool is_move_constructible_v = std::is_move_constructible<T>::value;
template<class T> _INLINE_VAR constexpr bool is_nothrow_move_constructible_v = std::is_nothrow_move_constructible<T>::value;

template<class T, class... Args> struct is_constructible : conditional<is_constructible_v<T, Args...>> {};
template<class T, class... Args> struct is_trivially_constructible : conditional<is_trivially_constructible_v<T, Args...>> {};
template<class T, class... Args> struct is_nothrow_constructible : conditional<is_nothrow_constructible_v<T, Args...>> {};

template<class T> struct is_copy_constructible : conditional<is_copy_constructible_v<T>> {};
template<class T> struct is_nothrow_copy_constructible : conditional<is_nothrow_copy_constructible_v<T>> {};
template<class T> struct is_move_constructible : conditional<is_move_constructible_v<T>> {};
template<class T> struct is_nothrow_move_constructible : conditional<is_nothrow_move_constructible_v<T>> {};

template<class L, class R> _INLINE_VAR constexpr bool is_assignable_v = !is_const_v<remove_ref_t<L>> && std::is_assignable<remove_const_t<remove_ref_t<L>>&, R>::value;
template<class L, class R> _INLINE_VAR constexpr bool is_trivially_assignable_v = !is_const_v<remove_ref_t<L>> && std::is_trivially_assignable<remove_const_t<remove_ref_t<L>>&, R>::value;
template<class L, class R> _INLINE_VAR constexpr bool is_nothrow_assignable_v = !is_const_v<remove_ref_t<L>> && std::is_nothrow_assignable<remove_const_t<remove_ref_t<L>>&, R>::value;

template<class T> _INLINE_VAR constexpr bool is_copy_assignable_v = !is_const_v<T> && std::is_copy_assignable<remove_const_t<T>>::value;
template<class T> _INLINE_VAR constexpr bool is_nothrow_copy_assignable_v = !is_const_v<T> && std::is_nothrow_copy_assignable<remove_const_t<T>>::value;
template<class T> _INLINE_VAR constexpr bool is_move_assignable_v = !is_const_v<T> && std::is_move_assignable<remove_const_t<T>>::value;
template<class T> _INLINE_VAR constexpr bool is_nothrow_move_assignable_v = !is_const_v<T> && std::is_nothrow_move_assignable<remove_const_t<T>>::value;

template<class L, class R> struct is_assignable : conditional<is_assignable_v<L, R>> {};
template<class L, class R> struct is_trivially_assignable : conditional<is_trivially_assignable_v<L, R>> {};
template<class L, class R> struct is_nothrow_assignable : conditional<is_nothrow_assignable_v<L, R>> {};

template<class T> struct is_copy_assignable : conditional<is_copy_assignable_v<T>> {};
template<class T> struct is_nothrow_copy_assignable : conditional<is_nothrow_copy_assignable_v<T>> {};
template<class T> struct is_move_assignable : conditional<is_move_assignable_v<T>> {};
template<class T> struct is_nothrow_move_assignable : conditional<is_nothrow_move_assignable_v<T>> {};

template<class T, class Tuple, class Indices = make_index_sequence<Tuple>> struct is_constructible_from_tuple : false_type {};
template<class T, class Tuple, size_t... Indices> struct is_constructible_from_tuple<T, Tuple, std::index_sequence<Indices...>> :
	conditional<is_constructible_v<T, tuple_element_t<Indices, Tuple>...>> {};

template<class T, class Tuple, class Indices = make_index_sequence<Tuple>> struct is_trivially_constructible_from_tuple : false_type {};
template<class T, class Tuple, size_t... Indices> struct is_trivially_constructible_from_tuple<T, Tuple, std::index_sequence<Indices...>> :
	conditional<is_trivially_constructible_v<T, tuple_element_t<Indices, Tuple>...>> {};

template<class T, class Tuple, class Indices = make_index_sequence<Tuple>> struct is_nothrow_constructible_from_tuple : false_type {};
template<class T, class Tuple, size_t... Indices> struct is_nothrow_constructible_from_tuple<T, Tuple, std::index_sequence<Indices...>> :
	conditional<is_nothrow_constructible_v<T, tuple_element_t<Indices, Tuple>...>> {};

template<class T, class Tuple> _INLINE_VAR constexpr bool is_constructible_from_tuple_v = is_constructible_from_tuple<T, Tuple>::value;
template<class T, class Tuple> _INLINE_VAR constexpr bool is_trivially_constructible_from_tuple_v = is_trivially_constructible_from_tuple<T, Tuple>::value;
template<class T, class Tuple> _INLINE_VAR constexpr bool is_nothrow_constructible_from_tuple_v = is_nothrow_constructible_from_tuple<T, Tuple>::value;

struct equal_to {
	template<class L, class R>
	constexpr auto operator()(L&& left, R&& right) const noexcept(noexcept(static_cast<L&&>(left) == static_cast<R&&>(right)))
		-> decltype(static_cast<L&&>(left) == static_cast<R&&>(right)) {
		return static_cast<L&&>(left) == static_cast<R&&>(right);
	}
	using is_transparent = int;
};

struct less {
	template<class L, class R>
	constexpr auto operator()(L&& left, R&& right) const noexcept(noexcept(static_cast<L&&>(left) < static_cast<R&&>(right)))
		-> decltype(static_cast<L&&>(left) < static_cast<R&&>(right)) {
		return static_cast<L&&>(left) < static_cast<R&&>(right);
	}
	using is_transparent = int;
};

struct greater {
	template<class L, class R>
	constexpr auto operator()(L&& left, R&& right) const noexcept(noexcept(static_cast<L&&>(left) > static_cast<R&&>(right)))
		-> decltype(static_cast<L&&>(left) > static_cast<R&&>(right)) {
		return static_cast<L&&>(left) > static_cast<R&&>(right);
	}
	using is_transparent = int;
};

struct always_false {
	template<class... Args> constexpr bool operator()(Args&&...) const noexcept { return false; }
};

struct always_true {
	template<class... Args> constexpr bool operator()(Args&&...) const noexcept { return true; }
};

struct objects {
	template<size_t Bytes> constexpr static void swap_bytes(void* l, void* r) noexcept {
		if (l == r)
			return;

		char temp[Bytes];
		std::memcpy(temp, static_cast<void const*>(l), Bytes);
		std::memcpy(l, static_cast<void const*>(r), Bytes);
		std::memcpy(r, static_cast<void const*>(temp), Bytes);
	}

	template<class T> constexpr static void swap_bytes(T& l, T& r) noexcept {
		swap_bytes<sizeof T>(&l, &r);
	}

protected:
	template<class T> constexpr static std::is_unsigned<T> _is_unsigned(int);
	template<class> constexpr static false_type _is_unsigned(...);

public:
	template<class T> struct is_unsigned : decltype(_is_unsigned<T>(0)) {};

	template<class T> constexpr _INLINE_VAR static bool is_unsigned_v = is_unsigned<T>::value;

	template<class T> constexpr _INLINE_VAR static bool is_class_v = std::is_class<T>::value;

	template<class T> constexpr _INLINE_VAR static bool is_polymorphic_v = std::is_polymorphic<T>::value;

public:
	template<class T, class... Args> constexpr static type_if<remove_cv_t<T>*, is_constructible_v<T, Args...>> contruct(T* where, Args&&... args)
		noexcept(is_nothrow_constructible_v<T, Args...>) {
		remove_cv_t<T>* p = const_cast<remove_cv_t<T>*>(where);
		new(p) T(static_cast<Args&&>(args)...);
		return p;
	}

	template<size_t... Indices, class T, class Tuple> constexpr static auto contruct(T* where, Tuple&& tuple, std::index_sequence<Indices...> = {})
		noexcept(noexcept(contruct(where, std::get<Indices>(static_cast<Tuple&&>(tuple))...)))
		-> type_if<decltype(contruct(where, std::get<Indices>(static_cast<Tuple&&>(tuple))...)), !is_constructible_v<T, Tuple, std::index_sequence<Indices...>>> {
		return contruct(where, std::get<Indices>(static_cast<Tuple&&>(tuple))...);
	}

	template<class T, class Tuple, class Indices = make_index_sequence<Tuple>>
	constexpr static auto contruct(T* where, Tuple&& tuple) noexcept(noexcept(contruct(where, static_cast<Tuple&&>(tuple), Indices{})))
		-> type_if<decltype(contruct(where, static_cast<Tuple&&>(tuple), Indices{})), !is_constructible_v<T, Tuple>> {
		return contruct(where, static_cast<Tuple&&>(tuple), Indices{});
	}

	template<class T> struct _contruct {
		template<class... Args> constexpr auto operator()(T* where, Args&&... args) const
			noexcept(noexcept(objects::contruct(where, static_cast<Args&&>(args)...)))
			-> decltype(objects::contruct(where, static_cast<Args&&>(args)...)) {
			return objects::contruct(where, static_cast<Args&&>(args)...);
		}
	};

protected:
	template<class T, bool = std::is_trivially_destructible<T>::value> struct _destroy;
	template<class T, bool = std::is_trivially_destructible<T>::value> struct _destroy_range;

	template<class T, size_t N, bool Ignore> struct _destroy<T[N], Ignore> : private _destroy_range<T> {
		constexpr void operator()(T(&arr)[N]) const noexcept {
			_destroy_range<T>::operator()(arr, arr + N);
		}
	};
	template<class T> struct _destroy<T, true> {
		constexpr void operator()(T&) const noexcept {}
	};
	template<class T> struct _destroy<T, false> {
		template<bool = is_polymorphic_v<T>> struct _ {
			T value;
		};
		template<> struct _<true> {
			~_() noexcept {
				if (polymorphic)
					value.~T();
			}
			union {
				size_t polymorphic;
				T value;
			};
		};
		constexpr void operator()(T& value) const noexcept {
			((_<>*) &value)->~_();
		}
	};

	template<class T> struct _destroy_range<T, true> {
		constexpr void operator()(T*, T*) const noexcept {}
	};
	template<class T> struct _destroy_range<T, false> : private _destroy<T> {
		constexpr void operator()(T* first, T* last) const noexcept {
			for (; first != last; ++first)
				_destroy<T>::operator()(*first);
		}
	};

public:
	template<class T> constexpr static void destroy_range(T* first, T* last) noexcept {
		_destroy_range<T>{}(first, last);
	}

	template<class T> constexpr static void destroy(T& value) noexcept {
		_destroy<T>{}(value);
	}

protected:
	template<class T> constexpr static conjunction<
		!is_unsigned_v<T>,
		sfinae_v<decltype(T{ -1 }), decltype(T{ 0 }), decltype(T{ 1 })>,
		convertible_v<decltype(std::declval<T>() == 0), bool>,
		convertible_v<decltype(std::declval<T>() != 0), bool>,
		convertible_v<decltype(std::declval<T>() < 0), bool>,
		convertible_v<decltype(std::declval<T>() <= 0), bool>,
		convertible_v<decltype(std::declval<T>() > 0), bool>,
		convertible_v<decltype(std::declval<T>() >= 0), bool>
	> _is_ordering(int);
	template<class> constexpr static false_type _is_ordering(...);

public:
	template<class T> struct is_ordering : decltype(_is_ordering<T>(0)) {};

	template<class T> constexpr _INLINE_VAR static bool is_ordering_v = is_ordering<T>::value;

};

struct default_comporator;

struct _object {
	virtual ~_object() {}
};

template<class T> struct _value_object {
	_value_object(_value_object const&) = default;
	_value_object& operator=(_value_object const&) = default;
	_value_object(_value_object&&) = default;
	_value_object& operator=(_value_object&&) = default;

	template<class... Args, type_if<int, is_constructible_v<T, Args...>> = 0>
	constexpr _value_object(Args&&... args) noexcept(is_nothrow_constructible_v<T, Args...>)
		: value{ static_cast<Args&&>(args)... } {
	}

	constexpr operator T& () & noexcept { return this->value; }
	constexpr operator T const& () const& noexcept { return this->value; }
	constexpr operator T && () && noexcept { return this->value; }
	constexpr operator T const&& () const&& noexcept { return this->value; }

	T value;
};

template<class T> using object = conditional<!objects::is_class_v<T> || objects::is_polymorphic_v<T>, _value_object<T>, T>;

#endif // !__OBJECT_HPP