#ifndef __CONTAINER_HPP
#define __CONTAINER_HPP 1

namespace std
{
    template<class> class function;
}

#include "util.hpp"
#include "iterator.hpp"
#include "object.hpp"

template<class T, class Arg, class... Args> struct is_constructible_from_each :
    conditional<is_constructible_v<T, Arg>, is_constructible_from_each<T, Args...>> {};

template<class T, class Arg> struct is_constructible_from_each<T, Arg> :
    conditional<is_constructible_v<T, Arg>> {};

template<class T, class Arg, class... Args> constexpr _INLINE_VAR bool is_constructible_from_each_v = is_constructible_from_each<T, Arg, Args...>::value;

template<class T, class Arg, class... Args> struct is_assignable_from_each :
    conditional<is_assignable_v<T, Arg>, is_assignable_from_each<T, Args...>> {};

template<class T, class Arg> struct is_assignable_from_each<T, Arg> :
    conditional<is_assignable_v<T, Arg>> {};

template<class T, class Arg, class... Args> constexpr _INLINE_VAR bool is_assignable_from_each_v = is_assignable_from_each<T, Arg, Args...>::value;

struct container
{
protected:
    template<class C, bool = std::is_class<C>::value> struct _
    {
        using value_type = void;
        using reference = void;
        using const_reference = void;
        using size_type = void;
    };
    template<class T, size_t N, bool Ignore> struct _<T[N], Ignore>
    {
        using value_type = T;
        using reference = T&;
        using const_reference = T const&;
        using size_type = size_t;
    };
    template<class C> struct _<C, true>
    {
        using value_type = typename C::value_type;
        using reference = typename C::reference;
        using const_reference = typename C::const_reference;
        using size_type = typename C::size_type;
    };

public:
    template<class C> using value_type = typename _<remove_ref_t<C>>::value_type;
    template<class C> using reference = typename _<remove_ref_t<C>>::reference;
    template<class C> using const_reference = typename _<remove_ref_t<C>>::const_reference;
    template<class C> using size_type = typename _<remove_ref_t<C>>::size_type;

    template<class C> using iterator = decltype(std::begin(std::declval<remove_cvref_t<C>&>()));
    template<class C> using const_iterator = decltype(std::begin(std::declval<remove_ref_t<C> const&>()));

#if __cpp_lib_make_reverse_iterator >= 201402L
    template<class C> using reverse_iterator = decltype(std::rbegin(std::declval<remove_cvref_t<C>&>()));
    template<class C> using const_reverse_iterator = decltype(std::rbegin(std::declval<remove_ref_t<C> const&>()));
#else
    template<class C> using reverse_iterator = typename remove_ref_t<C>::reverse_iterator;
    template<class C> using const_reverse_iterator = typename remove_ref_t<C>::const_reverse_iterator;
#endif // __cpp_lib_make_reverse_iterator

public:
    template<class C> _NODISCARD constexpr static auto begin(C&& _iterable) noexcept -> decltype(std::begin(static_cast<C&&>(_iterable))) {
        return std::begin(static_cast<C&&>(_iterable));
    }
    template<class C> _NODISCARD constexpr static auto end(C&& _iterable) noexcept -> decltype(std::end(static_cast<C&&>(_iterable))) {
        return std::end(static_cast<C&&>(_iterable));
    }

#if __cpp_lib_make_reverse_iterator >= 201402L
    template<class C> _NODISCARD constexpr static auto rbegin(C&& _reverse_iterable) noexcept -> decltype(std::rbegin(static_cast<C&&>(_reverse_iterable))) {
        return std::rbegin(static_cast<C&&>(_reverse_iterable));
    }
    template<class C> _NODISCARD constexpr static auto rend(C&& _reverse_iterable) noexcept -> decltype(std::rend(static_cast<C&&>(_reverse_iterable))) {
        return std::rend(static_cast<C&&>(_reverse_iterable));
    }
#else
    template<class C> _NODISCARD constexpr static auto rbegin(C&& _reverse_iterable) noexcept -> decltype(static_cast<C&&>(_reverse_iterable).rbegin()) {
        return static_cast<C&&>(_reverse_iterable).rbegin();
    }
    template<class C> _NODISCARD constexpr static auto rend(C&& _reverse_iterable) noexcept -> decltype(static_cast<C&&>(_reverse_iterable).rend()) {
        return static_cast<C&&>(_reverse_iterable).rend();
    }

    template<class T, size_t N> constexpr static ::reverse_iterator<T*> rbegin(T(&data)[N]) noexcept {
        return (T*)data + N - 1;
    }
    template<class T, size_t N> constexpr static ::reverse_iterator<T*> rend(T(&data)[N]) noexcept {
        return (T*)data - 1;
    }
#endif // __cpp_lib_make_reverse_iterator

public:
#if __cpp_lib_nonmember_container_access >= 201411
    template<class C> _NODISCARD constexpr static auto data(C&& _data_availability) noexcept -> decltype(std::data(static_cast<C&&>(_data_availability))) {
        return std::data(static_cast<C&&>(_data_availability));
    }
    template<class C> _NODISCARD constexpr static auto size(C&& _sizeable) noexcept -> decltype(std::size(static_cast<C&&>(_sizeable))) {
        return std::size(static_cast<C&&>(_sizeable));
    }
#else
    template<class C> _NODISCARD constexpr static auto data(C&& c) noexcept -> decltype(static_cast<C&&>(c).data()) {
        return static_cast<C&&>(c).data();
    }
    template<class C> _NODISCARD constexpr static auto size(C&& _sizeable) noexcept -> decltype(static_cast<C&&>(_sizeable).size()) {
        return static_cast<C&&>(_sizeable).size();
    }

    template<class T, size_t N> constexpr static T* data(T(&data)[N]) noexcept { return data; }
    template<class T, size_t N> constexpr static size_t size(T(&)[N]) noexcept { return N; }
#endif // __cpp_lib_nonmember_container_access

public:
    template<class List, class... Args> constexpr static auto push_back(List& list, Args&&... args)
        -> decltype(list.push_back(static_cast<Args&&>(args)...)) {
        return list.push_back(static_cast<Args&&>(args)...);
    }

    template<class List, class V> constexpr static auto push_back(V&& value, List& list)
        -> decltype(list.push_back(static_cast<V&&>(value))) {
        return list.push_back(static_cast<V&&>(value));
    }

    template<class List, class... Args> constexpr static auto push_front(List& list, Args&&... args)
        -> decltype(list.push_front(static_cast<Args&&>(args)...)) {
        return list.push_front(static_cast<Args&&>(args)...);
    }

    template<class List, class V> constexpr static auto push_front(V&& value, List& list)
        -> decltype(list.push_front(static_cast<V&&>(value))) {
        return list.push_front(static_cast<V&&>(value));
    }

    template<class List, class... Args> constexpr static auto insert(List& list, Args&&... args)
        -> decltype(list.insert(static_cast<Args&&>(args)...)) {
        return list.insert(static_cast<Args&&>(args)...);
    }

    template<class List, class... Args> constexpr static auto emplace_back(List& list, Args&&... args)
        -> decltype(list.emplace_back(static_cast<Args&&>(args)...)) {
        return list.emplace_back(static_cast<Args&&>(args)...);
    }

    template<class List, class... Args> constexpr static auto emplace_front(List& list, Args&&... args)
        -> decltype(list.emplace_front(static_cast<Args&&>(args)...)) {
        return list.emplace_front(static_cast<Args&&>(args)...);
    }

    template<class List, class... Args> constexpr static auto insert_after(List& list, Args&&... args)
        -> decltype(list.insert_after(static_cast<Args&&>(args)...)) {
        return list.insert_after(static_cast<Args&&>(args)...);
    }

protected:
    struct _back_pushable
    {
        template<class C, class... Args> static sfinae<decltype(std::declval<C>().push_back(std::declval<Args>()...))> test(int);
        template<class...> static std::false_type test(...);
    };
public:
    template<class C, class... Args> struct back_pushable : decltype(_back_pushable::test<C, Args...>(0)) {};

    template<class C, class... Args> constexpr _INLINE_VAR static bool back_pushable_v = back_pushable<C, Args...>::value;

protected:
    struct _front_pushable
    {
        template<class C, class... Args> static sfinae<decltype(std::declval<C>().push_front(std::declval<Args>()...))> test(int);
        template<class...> static std::false_type test(...);
    };
public:
    template<class C, class... Args> struct front_pushable : decltype(_front_pushable::test<C, Args...>(0)) {};

    template<class C, class... Args> constexpr _INLINE_VAR static bool front_pushable_v = front_pushable<C, Args...>::value;

protected:
    struct _insertable
    {
        template<class C, class... Args> static sfinae<decltype(std::declval<C>().insert(std::declval<Args>()...))> test(int);
        template<class...> static std::false_type test(...);
    };
public:
    template<class C, class... Args> struct insertable : decltype(_insertable::test<C, Args...>(0)) {};

    template<class C, class... Args> constexpr _INLINE_VAR static bool insertable_v = insertable<C, Args...>::value;

protected:
    struct _after_insertable
    {
        template<class C, class... Args> static sfinae<decltype(std::declval<C>().insert_after(std::declval<Args>()...))> test(int);
        template<class...> static std::false_type test(...);
    };
public:
    template<class C, class... Args> struct after_insertable : decltype(_after_insertable::test<C, Args...>(0)) {};

    template<class C, class... Args> constexpr _INLINE_VAR static bool after_insertable_v = after_insertable<C, Args...>::value;

protected:
    struct _begin_iterable
    {
        template<class C> static sfinae<decltype(std::declval<C>().begin())> test(int);
        template<class C> static sfinae<decltype(std::begin(std::declval<lvalue_ref_t<C>>()))> test(long);
        template<class> static std::false_type test(...);
    };
    struct _end_iterable
    {
        template<class C> static sfinae<decltype(std::declval<C>().end())> test(int);
        template<class C> static sfinae<decltype(std::end(std::declval<lvalue_ref_t<C>>()))> test(long);
        template<class> static std::false_type test(...);
    };
public:
    template<class C> constexpr _INLINE_VAR static bool iterable_v =
        decltype(_begin_iterable::test<C>(0))::value && decltype(_end_iterable::test<C>(0))::value;

    template<class C> struct iterable :
        conditional<iterable_v<C>, decltype(_begin_iterable::test<C>(0))> {};

protected:
    struct _reverse_begin_iterable
    {
        template<class C> static sfinae<decltype(std::declval<C>().rbegin())> test(int);
#if __cpp_lib_make_reverse_iterator >= 201402L
        template<class C> static sfinae<decltype(std::rbegin(std::declval<lvalue_ref_t<C>>()))> test(long);
#endif // __cpp_lib_make_reverse_iterator
        template<class> static std::false_type test(...);
    };
    struct _reverse_end_iterable
    {
        template<class C> static sfinae<decltype(std::declval<C>().rend())> test(int);
#if __cpp_lib_make_reverse_iterator >= 201402L
        template<class C> static sfinae<decltype(std::rend(std::declval<lvalue_ref_t<C>>()))> test(long);
#endif // __cpp_lib_make_reverse_iterator
        template<class> static std::false_type test(...);
    };
public:
    template<class C> constexpr _INLINE_VAR static bool reverse_iterable_v =
        decltype(_reverse_begin_iterable::test<C>(0))::value && decltype(_reverse_end_iterable::test<C>(0))::value;

    template<class C> struct reverse_iterable :
        conditional<reverse_iterable_v<C>, decltype(_reverse_begin_iterable::test<C>(0))> {};

protected:
    struct _sizeable
    {
        template<class C> static sfinae<decltype(std::declval<C>().size())> test(int);
#if __cpp_lib_nonmember_container_access >= 201411
        template<class C> static sfinae<decltype(std::size(std::declval<lvalue_ref_t<C>>()))> test(long);
#endif // __cpp_lib_nonmember_container_access
        template<class> static std::false_type test(...);
    };
public:
    template<class C> struct sizeable : decltype(_sizeable::test<C>(0)) {};

    template<class C> constexpr _INLINE_VAR static bool sizeable_v = sizeable<C>::value;

protected:
    struct _data_availability
    {
        template<class C> static sfinae<decltype(std::declval<C>().data())> test(int);
#if __cpp_lib_nonmember_container_access >= 201411
        template<class C> static sfinae<decltype(std::data(std::declval<lvalue_ref_t<C>>()))> test(long);
#endif // __cpp_lib_nonmember_container_access
        template<class> static std::false_type test(...);
    };
public:
    template<class C> struct data_availability : decltype(_data_availability::test<C>(0)) {};

    template<class C> constexpr _INLINE_VAR static bool data_availability_v = data_availability<C>::value;

protected:
    struct _foreachable
    {
        template<class C, class Proc, class... Args> static sfinae<decltype(std::declval<C>().foreach(std::declval<Proc>(), std::declval<Args>()...))> test(int);
        template<class...> static std::false_type test(...);
    };
public:
    template<class C, class Proc = std::function<void(const_reference<C>)>, class... Args> struct foreachable :
        decltype(_foreachable::test<C, Proc, Args...>(0)) {};

    template<class C, class Proc = std::function<void(const_reference<C>)>, class... Args> constexpr _INLINE_VAR static bool foreachable_v =
        foreachable<C, Proc, Args...>::value;

protected:
    struct _reverse_foreachable
    {
        template<class C, class Proc = std::function<void(const_reference<C>)>, class... Args> static sfinae<decltype(std::declval<C>().rforeach(std::declval<Proc>(), std::declval<Args>()...))> test(int);
        template<class...> static std::false_type test(...);
    };
public:
    template<class C, class Proc = std::function<void(const_reference<C>)>, class... Args> struct reverse_foreachable :
        decltype(_reverse_foreachable::test<C, Proc, Args...>(0)) {};

    template<class C, class Proc = std::function<void(const_reference<C>)>, class... Args> constexpr _INLINE_VAR static bool reverse_foreachable_v =
        reverse_foreachable<C, Proc, Args...>::value;

};

#endif // !__CONTAINER_HPP