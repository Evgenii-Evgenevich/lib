#ifndef __TYPES_HPP
#define __TYPES_HPP 1

#include <type_traits>

#ifndef _INLINE_VAR
#if _HAS_CXX17 
#define _INLINE_VAR inline
#else // _HAS_CXX17
#define _INLINE_VAR
#endif // _HAS_CXX17
#endif // !_INLINE_VAR

using std::nullptr_t;

template<class...> constexpr _INLINE_VAR nullptr_t null = nullptr;

template<class T> constexpr _INLINE_VAR T* null<T> = nullptr;

using std::true_type;
using std::false_type;

template<bool _Test, class _True = true_type, class _False = false_type>
using conditional = typename std::conditional<_Test, _True, _False>::type;

template<class _BoolConst, class _True = true_type, class _False = false_type>
using conditional_value = typename std::conditional<_BoolConst::value, _True, _False>::type;

template<bool _Const, bool... _Consts> struct conjunction : conditional<_Const, conjunction<_Consts...>> {};

template<bool _Const> struct conjunction<_Const> : conditional<_Const>{};

struct negate {};

template<class _BoolConst, class... _BoolConsts> struct conjunction_value : conditional_value<_BoolConst, conjunction_value<_BoolConsts...>, _BoolConst> {};

template<class _BoolConst, class... _BoolConsts> struct conjunction_value<negate, _BoolConst, _BoolConsts...> : conditional_value<_BoolConst, false_type, conjunction_value<_BoolConsts...>> {};

template<class _BoolConst> struct conjunction_value<_BoolConst> : _BoolConst {};

template<class _BoolConst> struct conjunction_value<negate, _BoolConst> : conditional_value<_BoolConst, false_type, true_type> {};

template<class _BoolConst, class... _BoolConsts> constexpr _INLINE_VAR bool all_value = _BoolConst::value && all_value<_BoolConsts...>;

template<class _BoolConst> constexpr _INLINE_VAR bool all_value<_BoolConst> = _BoolConst::value;

template<class _BoolConst, class... _BoolConsts> constexpr _INLINE_VAR bool all_value<negate, _BoolConst, _BoolConsts...> = !_BoolConst::value && all_value<_BoolConsts...>;

template<class _BoolConst> constexpr _INLINE_VAR bool all_value<negate, _BoolConst> = !_BoolConst::value;

template<bool _Const, bool... _Consts> constexpr _INLINE_VAR bool all_const = _Const && all_const<_Consts...>;

template<bool _Const> constexpr _INLINE_VAR bool all_const<_Const> = _Const;

template<class...> struct sfinae : true_type {};

template<class T> struct sfinae<T> : true_type
{
	using result_type = T;
};

template<class, class...> constexpr _INLINE_VAR bool sfinae_v = true;

template<class T, bool _Const, bool... _Consts> using type_if = typename std::enable_if<all_const<_Const, _Consts...>, T>::type;

template<class T, class _BoolConst, class... _BoolConsts> using type_if_value = typename std::enable_if<all_value<_BoolConst, _BoolConsts...>, T>::type;

template<class... T> using common_type = typename std::common_type<T...>::type;

template<class T> using remove_ref_t = typename std::remove_reference<T>::type;
template<class T> using lvalue_ref_t = typename std::add_lvalue_reference<T>::type;
template<class T> using rvalue_ref_t = typename std::add_rvalue_reference<T>::type;

template<class T> using remove_const_t = typename std::remove_const<T>::type;
template<class T> using remove_volatile_t = typename std::remove_volatile<T>::type;
template<class T> using remove_cv_t = typename std::remove_cv<T>::type;

template<class T> using remove_cvref_t = remove_cv_t<remove_ref_t<T>>;

template<class T> constexpr remove_cv_t<T>* unconst(T* ptr) noexcept
{
	return const_cast<remove_cv_t<T>*>(ptr);
}

template<class T> constexpr remove_cv_t<T>& unconst(T& lref) noexcept
{
	return const_cast<remove_cv_t<T>&>(lref);
}

template<class T> constexpr _INLINE_VAR bool is_const_v = std::is_const<T>::value;

template<class T> struct is_const : conditional<is_const_v<T>> {};

template<class L, class R> constexpr _INLINE_VAR bool is_same_v = std::is_same<remove_cv_t<L>, remove_cv_t<R>>::value;

template<class L, class R> struct is_same : conditional<is_same_v<L, R>> {};

template<class Base, class Derived> constexpr _INLINE_VAR bool is_base_of_v = std::is_base_of<remove_cv_t<Base>, remove_cv_t<Derived>>::value;

template<class Base, class Derived> struct is_base_of : conditional<is_base_of_v<Base, Derived>> {};

template<class From, class To> constexpr _INLINE_VAR bool convertible_v = std::is_convertible<From, To>::value;

template<class From, class To> struct convertible : conditional<convertible_v<From, To>> {};

#endif // !__TYPES_HPP