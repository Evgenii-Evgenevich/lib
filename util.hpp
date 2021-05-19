#ifndef __UTIL_HPP
#define __UTIL_HPP 1

#include "types.hpp"

struct util {
protected:
	template<class...> struct _invoker {
		template<class F, class... Args> constexpr auto operator()(F&& f, Args&&... args) const
			noexcept(noexcept(static_cast<F&&>(f)(static_cast<Args&&>(args)...)))
			-> decltype(static_cast<F&&>(f)(static_cast<Args&&>(args)...)) {
			return static_cast<F&&>(f)(static_cast<Args&&>(args)...);
		}
	};

	template<class R, class Base, class Arg, class... Args> struct _invoker<R Base::*, Arg, Args...> {
		constexpr _INLINE_VAR static bool is_member_function_pointer_v = std::is_member_function_pointer<R Base::*>::value;
		constexpr _INLINE_VAR static bool is_member_object_pointer_v = std::is_member_object_pointer<R Base::*>::value;

		template<class Derived> constexpr _INLINE_VAR static bool derived_v = std::is_base_of<remove_cv_t<Base>, remove_cv_t<remove_ref_t<Derived>>>::value;

		constexpr static auto get(Arg&& arg) noexcept
			-> type_if<Arg&&, derived_v<Arg>> {
			return static_cast<Arg&&>(arg);
		}

		constexpr static auto get(Arg&& arg) noexcept
			-> type_if<decltype(*static_cast<Arg&&>(arg)), derived_v<decltype(*static_cast<Arg&&>(arg))>> {
			return *static_cast<Arg&&>(arg);
		}

		constexpr static auto get(Arg&& arg) noexcept
			-> type_if<decltype(static_cast<Arg&&>(arg).get()), derived_v<decltype(static_cast<Arg&&>(arg).get())>> {
			return static_cast<Arg&&>(arg).get();
		}

		constexpr auto operator()(R Base::* fun, Arg&& arg, Args&&... args) const
			noexcept(noexcept(_invoker::get(static_cast<Arg&&>(arg)).*fun)(static_cast<Args&&>(args)...))
			-> type_if<decltype((_invoker::get(static_cast<Arg&&>(arg)).*fun)(static_cast<Args&&>(args)...)), is_member_function_pointer_v> {
			return (_invoker::get(static_cast<Arg&&>(arg)).*fun)(static_cast<Args&&>(args)...);
		}

		constexpr auto operator()(R Base::* obj, Arg&& arg) const noexcept
			-> type_if<decltype(_invoker::get(static_cast<Arg&&>(arg)).*obj), is_member_object_pointer_v, sizeof...(Args) == 0> {
			return _invoker::get(static_cast<Arg&&>(arg)).*obj;
		}
	};

public:
	template<class F, class... Args> using invoke_result_t = decltype(_invoker<F, Args...>{}(std::declval<F>(), std::declval<Args>()...));

	template<class F, class... Args> constexpr _INLINE_VAR static bool nothrow_invocable_v = noexcept(_invoker<F, Args...>{}(std::declval<F>(), std::declval<Args>()...));

	template<class F, class... Args> struct nothrow_invocable : conditional<nothrow_invocable_v<F, Args...>> {};

protected:
	template<class F, class... Args> static sfinae<invoke_result_t<F, Args...>> _is_invocable(int);
	template<class...> static false_type _is_invocable(...);

public:
	template<class F, class... Args> struct invocable : decltype(_is_invocable<F, Args...>(0)) {};

	template<class F, class... Args> constexpr _INLINE_VAR static bool invocable_v = invocable<F, Args...>::value;

	template<class F, class... Args> constexpr static invoke_result_t<F, Args...> invoke(F&& f, Args&&... args) noexcept(nothrow_invocable_v<F, Args...>) {
		return _invoker<F, Args...>{}(static_cast<F&&>(f), static_cast<Args&&>(args)...);
	}

	struct _invoke {
		template<class F, class... Args> constexpr invoke_result_t<F, Args...> operator()(F&& f, Args&&... args) const noexcept(nothrow_invocable_v<F, Args...>) {
			return _invoker<F, Args...>{}(static_cast<F&&>(f), static_cast<Args&&>(args)...);
		}
	};

	constexpr _INLINE_VAR static _invoke invoke2{};

protected:
	template<class R, class B, class T> struct _member_ptr {
		using P = R B::*;
		constexpr _INLINE_VAR static bool is_member_function_pointer_v = std::is_member_function_pointer<P>::value;
		constexpr _INLINE_VAR static bool is_member_object_pointer_v = std::is_member_object_pointer<P>::value;

		P m_ptr;
		T& m_obj;

		constexpr _member_ptr(P ptr, T& obj) noexcept : m_ptr(ptr), m_obj(obj) {}

		template<class... Args>
		constexpr auto operator()(Args&&... args) noexcept(noexcept((m_obj.*m_ptr)(static_cast<Args&&>(args)...)))
			-> decltype((m_obj.*m_ptr)(static_cast<Args&&>(args)...)) {
			return (m_obj.*m_ptr)(static_cast<Args&&>(args)...);
		}

		template<class M = decltype(m_obj.*m_ptr)> constexpr operator M& () const noexcept {
			return m_obj.*m_ptr;
		}
	};

public:
	template<class R, class B, class O>
	constexpr static type_if<_member_ptr<R, B, remove_ref_t<O>>, is_base_of_v<B, remove_ref_t<O>>> member_ptr(R B::* mem_ptr, O&& o) noexcept {
		return { mem_ptr, static_cast<O&&>(o) };
	}

	template<class R, class B, class P>
	constexpr static type_if<_member_ptr<R, B, remove_ref_t<decltype(*std::declval<P>())>>, is_base_of_v<B, remove_ref_t<decltype(*std::declval<P>())>>> member_ptr(R B::* mem_ptr, P&& p) noexcept {
		return { mem_ptr, *static_cast<P&&>(p) };
	}

	template<class R, class B, class W>
	constexpr static type_if<_member_ptr<R, B, remove_ref_t<decltype(std::declval<W>().get())>>, is_base_of_v<B, remove_ref_t<decltype(std::declval<W>().get())>>> member_ptr(R B::* mem_ptr, W&& w) noexcept {
		return { mem_ptr, static_cast<W&&>(w).get() };
	}

};

#endif // !__UTIL_HPP