#ifndef __CONTAINER_HPP
#define __CONTAINER_HPP 1

namespace std
{
    template<class> class function;
}

#include "util.hpp"
#include "iterator.hpp"
#include "object.hpp"

template<class T, class Arg, class... Args> struct is_constructible_from_each : conditional<is_constructible_v<T, Arg>, is_constructible_from_each<T, Args...>> {};

template<class T, class Arg> struct is_constructible_from_each<T, Arg> : conditional<is_constructible_v<T, Arg>> {};

template<class T, class Arg, class... Args> constexpr _INLINE_VAR bool is_constructible_from_each_v = is_constructible_from_each<T, Arg, Args...>::value;

template<class T, class Arg, class... Args> struct is_assignable_from_each : conditional<is_assignable_v<T, Arg>, is_assignable_from_each<T, Args...>> {};

template<class T, class Arg> struct is_assignable_from_each<T, Arg> : conditional<is_assignable_v<T, Arg>> {};

template<class T, class Arg, class... Args> constexpr _INLINE_VAR bool is_assignable_from_each_v = is_assignable_from_each<T, Arg, Args...>::value;

template<class T, class Arg, class... Args> constexpr _INLINE_VAR bool convertible_from_each_v = convertible_v<Arg, T> && convertible_from_each_v<T, Args...>;

template<class T, class Arg> constexpr _INLINE_VAR bool convertible_from_each_v<T, Arg> = convertible_v<Arg, T>;

template<class T, class Arg, class... Args> struct convertible_from_each : conditional<convertible_from_each_v<T, Arg, Args...>> {};

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

protected:
    struct _back_pushable
    {
        template<class C, class... Args> static sfinae<decltype(std::declval<C>().push_back(std::declval<Args>()...))> test(int);
        template<class...> static false_type test(...);
    };
public:
    template<class C, class... Args> struct back_pushable : decltype(_back_pushable::test<C, Args...>(0)) {};

    template<class C, class... Args> constexpr _INLINE_VAR static bool back_pushable_v = back_pushable<C, Args...>::value;

protected:
    struct _front_pushable
    {
        template<class C, class... Args> static sfinae<decltype(std::declval<C>().push_front(std::declval<Args>()...))> test(int);
        template<class...> static false_type test(...);
    };
public:
    template<class C, class... Args> struct front_pushable : decltype(_front_pushable::test<C, Args...>(0)) {};

    template<class C, class... Args> constexpr _INLINE_VAR static bool front_pushable_v = front_pushable<C, Args...>::value;

protected:
    struct _insertable
    {
        template<class C, class... Args> static sfinae<decltype(std::declval<C>().insert(std::declval<Args>()...))> test(int);
        template<class...> static false_type test(...);
    };
public:
    template<class C, class... Args> struct insertable : decltype(_insertable::test<C, Args...>(0)) {};

    template<class C, class... Args> constexpr _INLINE_VAR static bool insertable_v = insertable<C, Args...>::value;

protected:
    struct _after_insertable
    {
        template<class C, class... Args> static sfinae<decltype(std::declval<C>().insert_after(std::declval<Args>()...))> test(int);
        template<class...> static false_type test(...);
    };
public:
    template<class C, class... Args> struct after_insertable : decltype(_after_insertable::test<C, Args...>(0)) {};

    template<class C, class... Args> constexpr _INLINE_VAR static bool after_insertable_v = after_insertable<C, Args...>::value;

protected:
    struct _begin_iterable
    {
        template<class C> static sfinae<decltype(std::declval<C>().begin())> test(int);
        template<class C> static sfinae<decltype(std::begin(std::declval<lvalue_ref_t<C>>()))> test(long);
        template<class> static false_type test(...);
    };
    struct _end_iterable
    {
        template<class C> static sfinae<decltype(std::declval<C>().end())> test(int);
        template<class C> static sfinae<decltype(std::end(std::declval<lvalue_ref_t<C>>()))> test(long);
        template<class> static false_type test(...);
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
        template<class> static false_type test(...);
    };
    struct _reverse_end_iterable
    {
        template<class C> static sfinae<decltype(std::declval<C>().rend())> test(int);
#if __cpp_lib_make_reverse_iterator >= 201402L
        template<class C> static sfinae<decltype(std::rend(std::declval<lvalue_ref_t<C>>()))> test(long);
#endif // __cpp_lib_make_reverse_iterator
        template<class> static false_type test(...);
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
        template<class> static false_type test(...);
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
        template<class> static false_type test(...);
    };
public:
    template<class C> struct data_availability : decltype(_data_availability::test<C>(0)) {};

    template<class C> constexpr _INLINE_VAR static bool data_availability_v = data_availability<C>::value;

protected:
    struct _foreachable
    {
        template<class C, class Proc, class... Args> static sfinae<decltype(std::declval<C>().foreach(std::declval<Proc>(), std::declval<Args>()...))> test(int);
        template<class...> static false_type test(...);
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
        template<class...> static false_type test(...);
    };
public:
    template<class C, class Proc = std::function<void(const_reference<C>)>, class... Args> struct reverse_foreachable :
        decltype(_reverse_foreachable::test<C, Proc, Args...>(0)) {};

    template<class C, class Proc = std::function<void(const_reference<C>)>, class... Args> constexpr _INLINE_VAR static bool reverse_foreachable_v =
        reverse_foreachable<C, Proc, Args...>::value;

public:
    template<class C> _NODISCARD constexpr static auto begin(C&& _iterable) noexcept -> decltype(std::begin(static_cast<C&&>(_iterable))) {
        return std::begin(static_cast<C&&>(_iterable));
    }
    template<class C> _NODISCARD constexpr static auto end(C&& _iterable) noexcept -> decltype(std::end(static_cast<C&&>(_iterable))) {
        return std::end(static_cast<C&&>(_iterable));
    }

    struct _begin
    {
        template<class C> _NODISCARD constexpr auto operator()(C&& _iterable) const noexcept -> decltype(container::begin(static_cast<C&&>(_iterable))) {
            return container::begin(static_cast<C&&>(_iterable));
        }
    };

    constexpr _INLINE_VAR static _begin begin2{};

    struct _end
    {
        template<class C> _NODISCARD constexpr auto operator()(C& _iterable) const noexcept -> decltype(container::end(static_cast<C&&>(_iterable))) {
            return container::end(static_cast<C&&>(_iterable));
        }
    };

    constexpr _INLINE_VAR static _end end2{};

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

    struct _rbegin
    {
        template<class C> _NODISCARD constexpr auto operator()(C&& _reverse_iterable) const noexcept -> decltype(container::rbegin(static_cast<C&&>(_reverse_iterable))) {
            return container::rbegin(static_cast<C&&>(_reverse_iterable));
        }
    };

    constexpr _INLINE_VAR static _rbegin rbegin2{};

    struct _rend
    {
        template<class C> _NODISCARD constexpr auto operator()(C&& _reverse_iterable) const noexcept -> decltype(container::rend(static_cast<C&&>(_reverse_iterable))) {
            return container::rend(static_cast<C&&>(_reverse_iterable));
        }
    };

    constexpr _INLINE_VAR static _rend rend2{};

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

    struct _data
    {
        template<class C> _NODISCARD constexpr auto operator()(C&& _data_availability) const noexcept -> decltype(container::data(static_cast<C&&>(_data_availability))) {
            return container::data(static_cast<C&&>(_data_availability));
        }
    };

    constexpr _INLINE_VAR static _data data2{};

    struct _size
    {
        template<class C> _NODISCARD constexpr auto operator()(C&& _sizeable) const noexcept -> decltype(container::size(static_cast<C&&>(_sizeable))) {
            return container::size(static_cast<C&&>(_sizeable));
        }
    };

    constexpr _INLINE_VAR static _size size2{};

public:
    template<class List, class V, type_if<int, !back_pushable_v<V, List&>> = 0>
    constexpr static auto push_back(V&& value, List& list)
        -> decltype(list.push_back(static_cast<V&&>(value))) {
        return list.push_back(static_cast<V&&>(value));
    }

    template<class List, class V, type_if<int, !front_pushable_v<V, List&>> = 0>
    constexpr static auto push_front(V&& value, List& list)
        -> decltype(list.push_front(static_cast<V&&>(value))) {
        return list.push_front(static_cast<V&&>(value));
    }

    template<class List, class... Args> constexpr static auto push_back(List& list, Args&&... args)
        -> decltype(list.push_back(static_cast<Args&&>(args)...)) {
        return list.push_back(static_cast<Args&&>(args)...);
    }

    template<class List, class... Args> constexpr static auto push_front(List& list, Args&&... args)
        -> decltype(list.push_front(static_cast<Args&&>(args)...)) {
        return list.push_front(static_cast<Args&&>(args)...);
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

public:
    template<class> struct _push_back;
    template<class> struct _emplace_back;

    template<> struct _push_back<void>
    {
        template<class Container, class... Args> constexpr auto operator()(Container& container, Args&&... args) const
            -> decltype(container.push_back(static_cast<Args&&>(args)...)) {
            return container.push_back(static_cast<Args&&>(args)...);
        }
    };
    template<class Container = void> struct _push_back : private _push_back<void>
    {
        Container& container;

        constexpr _push_back(Container& container) noexcept : container(container) {}

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> decltype(_push_back<void>::operator()(container, static_cast<Args&&>(args)...)) {
            return _push_back<void>::operator()(container, static_cast<Args&&>(args)...);
        }
    };

    constexpr _INLINE_VAR static _push_back<> push_back2{};

public:
    template<class> struct _push_front;
    template<class> struct _emplace_front;

    template<> struct _push_front<void>
    {
        template<class Container, class... Args> constexpr auto operator()(Container& container, Args&&... args) const
            -> decltype(container.push_front(static_cast<Args&&>(args)...))
        {
            return container.push_front(static_cast<Args&&>(args)...);
        }
    };
    template<class Container = void> struct _push_front : private _push_front<void>
    {
        Container& container;

        constexpr _push_front(Container& container) noexcept : container(container) {}

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> decltype(_push_front<void>::operator()(container, static_cast<Args&&>(args)...)) {
            return _push_front<void>::operator()(container, static_cast<Args&&>(args)...);
        }
    };

    constexpr _INLINE_VAR static _push_front<> push_front2{};

public:
    template<class> struct _insert;
    template<class> struct _emplace;

    template<> struct _insert<void>
    {
        template<class Container, class... Args> constexpr auto operator()(Container& container, Args&&... args) const
            -> decltype(container.insert(static_cast<Args&&>(args)...)) {
            return container.insert(static_cast<Args&&>(args)...);
        }
    };
    template<class Container = void> struct _insert : private _insert<void>
    {
        Container& container;

        constexpr _insert(Container& container) noexcept : container(container) {}

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> decltype(_insert<void>::operator()(container, static_cast<Args&&>(args)...)) {
            return _insert<void>::operator()(container, static_cast<Args&&>(args)...);
        }
    };

    constexpr _INLINE_VAR static _insert<> insert2{};

public:
    template<class Container = void> struct _emplace_back
    {
        Container& container;

        constexpr _emplace_back(Container& container) noexcept : container(container) {}

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> decltype(container.emplace_back(static_cast<Args&&>(args)...)) {
            return container.emplace_back(static_cast<Args&&>(args)...);
        }
    };
    template<> struct _emplace_back<void>
    {
        template<class Container, class... Args> constexpr auto operator()(Container& container, Args&&... args) const
            -> decltype(container.emplace_back(static_cast<Args&&>(args)...)) {
            return container.emplace_back(static_cast<Args&&>(args)...);
        }
    };

    constexpr _INLINE_VAR static _emplace_back<> emplace_back2{};

public:
    template<class Container = void> struct _emplace_front
    {
        Container& container;

        constexpr _emplace_front(Container& container) noexcept : container(container) {}

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> decltype(container.emplace_front(static_cast<Args&&>(args)...)) {
            return container.emplace_front(static_cast<Args&&>(args)...);
        }
    };
    template<> struct _emplace_front<void>
    {
        template<class Container, class... Args> constexpr auto operator()(Container& container, Args&&... args) const
            -> decltype(container.emplace_front(static_cast<Args&&>(args)...)) {
            return container.emplace_front(static_cast<Args&&>(args)...);
        }
    };

    constexpr _INLINE_VAR static _emplace_front<> emplace_front2{};

public:
    template<class Container = void> struct _emplace
    {
        Container& container;

        constexpr _emplace(Container& container) noexcept : container(container) {}

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> decltype(container.emplace(static_cast<Args&&>(args)...)) {
            return container.emplace(static_cast<Args&&>(args)...);
        }
    };
    template<> struct _emplace<void>
    {
        template<class Container, class... Args> constexpr auto operator()(Container& container, Args&&... args) const
            -> decltype(container.emplace(static_cast<Args&&>(args)...)) {
            return container.emplace(static_cast<Args&&>(args)...);
        }
    };

    constexpr _INLINE_VAR static _emplace<> emplace2{};

public:
    template<class Container = void> struct _insert_after
    {
        Container& container;

        constexpr _insert_after(Container& container) noexcept : container(container) {}

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> decltype(container.insert_after(static_cast<Args&&>(args)...)) {
            return container.insert_after(static_cast<Args&&>(args)...);
        }
    };
    template<> struct _insert_after<void>
    {
        template<class Container, class... Args> constexpr auto operator()(Container& container, Args&&... args) const
            -> decltype(container.insert_after(static_cast<Args&&>(args)...)) {
            return container.insert_after(static_cast<Args&&>(args)...);
        }
    };

    constexpr _INLINE_VAR static _insert_after<> insert_after2{};

public:
    template<class Container = void> struct _emplace_after
    {
        Container& container;

        constexpr _emplace_after(Container& container) noexcept : container(container) {}

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> decltype(container.emplace_after(static_cast<Args&&>(args)...)) {
            return container.emplace_after(static_cast<Args&&>(args)...);
        }
    };
    template<> struct _emplace_after<void>
    {
        template<class Container, class... Args> constexpr auto operator()(Container& container, Args&&... args) const
            -> decltype(container.emplace_after(static_cast<Args&&>(args)...)) {
            return container.emplace_after(static_cast<Args&&>(args)...);
        }
    };

    constexpr _INLINE_VAR static _emplace_after<> emplace_after2{};

public:
    template<class Container> constexpr static _push_back<Container> back_pusher(Container& container) noexcept {
        return { container };
    }

    template<class Container> constexpr static _push_front<Container> front_pusher(Container& container) noexcept {
        return { container };
    }

    template<class Container> constexpr static _insert<Container> inserter(Container& container) noexcept {
        return { container };
    }

    template<class Container> constexpr static _emplace_back<Container> back_emplacer(Container& container) noexcept {
        return { container };
    }

    template<class Container> constexpr static _emplace_front<Container> front_emplacer(Container& container) noexcept {
        return { container };
    }

    template<class Container> constexpr static _emplace<Container> emplacer(Container& container) noexcept {
        return { container };
    }

    template<class Container> constexpr static _insert_after<Container> after_inserter(Container& container) noexcept {
        return { container };
    }

    template<class Container> constexpr static _emplace_after<Container> after_emplacer(Container& container) noexcept {
        return { container };
    }

    template<class C> struct _emplacer;

    template<class C> struct _inserter
    {
        C& container;
        iterator<C> pos;

        constexpr _inserter(C& container, iterator<C> pos) noexcept
            : container(container), pos(pos) {
        }

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> type_if<iterator<C>, convertible_v<decltype(container.insert(pos, static_cast<Args&&>(args)...)), iterator<C>>> {
            pos = container.insert(pos, static_cast<Args&&>(args)...);
            return pos;
        }
    };

    template<class C> constexpr static _inserter<C> inserter(C& container, iterator<C> pos) noexcept {
        return { container, pos };
    }

    template<class C> struct _emplacer
    {
        C& container;
        iterator<C> pos;

        constexpr _emplacer(C& container, iterator<C> pos) noexcept
            : container(container), pos(pos) {
        }

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> type_if<iterator<C>, convertible_v<decltype(container.emplace(pos, static_cast<Args&&>(args)...)), iterator<C>>> {
            pos = container.emplace(pos, static_cast<Args&&>(args)...);
            return pos;
        }
    };

    template<class C> constexpr static _emplacer<C> emplacer(C& container, iterator<C> pos) noexcept {
        return { container, pos };
    }

    template<class C> struct _after_emplacer;

    template<class C> struct _after_inserter
    {
        C& container;
        iterator<C> pos;

        constexpr _after_inserter(C& container, iterator<C> pos) noexcept
            : container(container), pos(pos) {
        }

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> type_if<iterator<C>, convertible_v<decltype(container.insert_after(pos, static_cast<Args&&>(args)...)), iterator<C>>> {
            pos = container.insert_after(pos, static_cast<Args&&>(args)...);
            return pos;
        }
    };

    template<class C> constexpr static _after_inserter<C> after_inserter(C& container, iterator<C> pos) noexcept {
        return { container, pos };
    }

    template<class C> struct _after_emplacer
    {
        C& container;
        iterator<C> pos;

        constexpr _after_emplacer(C& container, iterator<C> pos) noexcept
            : container(container), pos(pos) {
        }

        template<class... Args> constexpr auto operator()(Args&&... args)
            -> type_if<iterator<C>, convertible_v<decltype(container.emplace_after(pos, static_cast<Args&&>(args)...)), iterator<C>>> {
            pos = container.emplace_after(pos, static_cast<Args&&>(args)...);
            return pos;
        }
    };

    template<class C> constexpr static _after_emplacer<C> after_emplacer(C& container, iterator<C> pos) noexcept {
        return { container, pos };
    }

#if _HAS_CXX17
    template<class C> _push_back(C&)->_push_back<C>;
    template<class C> _push_front(C&)->_push_front<C>;
    template<class C> _insert(C&)->_insert<C>;
    template<class C> _emplace_back(C&)->_emplace_back<C>;
    template<class C> _emplace_front(C&)->_emplace_front<C>;
    template<class C> _emplace(C&)->_emplace<C>;
    template<class C> _insert_after(C&)->_insert_after<C>;
    template<class C> _emplace_after(C&)->_emplace_after<C>;

    template<class C> _inserter(C&, iterator<C>)->_inserter<C>;
    template<class C> _emplacer(C&, iterator<C>)->_emplacer<C>;
    template<class C> _after_inserter(C&, iterator<C>)->_after_inserter<C>;
    template<class C> _after_emplacer(C&, iterator<C>)->_after_emplacer<C>;
#endif // _HAS_CXX17

};

#endif // !__CONTAINER_HPP