#ifndef __ARRAY_HPP
#define __ARRAY_HPP 1

namespace std
{
    template<class, size_t> class array;
}

#include "util.hpp"
#include "optional.hpp"
#include "iterator.hpp"
#include "container.hpp"
#include "object.hpp"

template<class = void, size_t...> struct array;

template<> struct array<void>
{
protected:
    static void _check_range() {
        std::_Xout_of_range("arrays::_check_range");
    }

    template<class SizeT, SizeT Size> constexpr static void _check_range(SizeT pos) {
        if (Size <= pos)
            _check_range();
    }

    template<class SizeT> constexpr static void _check_range(SizeT pos, SizeT size) {
        if (size <= pos)
            _check_range();
    }

    template<class D, class S> constexpr static void _copy_assign(D* dst, D const* end, S src) noexcept(is_nothrow_assignable_v<D&, decltype(*src)>) {
        for (; dst != end; ++dst, (void)++src)
            *dst = *src;
    }

    template<class D, class S> constexpr static void _move_assign(D* dst, D const* end, S src) noexcept(is_nothrow_assignable_v<D&, remove_ref_t<decltype(*src)>&&>) {
        for (; dst != end; ++dst, (void)++src)
            *dst = static_cast<remove_ref_t<decltype(*src)>&&>(*src);
    }

    template<class T, class Arg> constexpr static void _args_assign(T* data, Arg&& arg) noexcept(is_nothrow_assignable_v<T&, Arg&&>) {
        *data = static_cast<Arg&&>(arg);
    }

    template<class T, class Arg, class... Args> constexpr static void _args_assign(T* data, Arg&& arg, Args&&... args) noexcept(is_nothrow_assignable_v<T&, Arg&&> && noexcept(_args_assign(data, static_cast<Args&&>(args)...))) {
        *data = static_cast<Arg&&>(arg);
        _args_assign(data + 1, static_cast<Args&&>(args)...);
    }

    template<class D, class S> constexpr static D* _copy_construct(D* dst, D const* end, S src) noexcept(is_nothrow_constructible_v<D, decltype(*src)>) {
        for (; dst != end; ++dst, (void)++src)
            new(dst) D(*src);

        return dst;
    }

    template<class D, class S> constexpr static D* _move_construct(D* dst, D const* end, S src) noexcept(is_nothrow_constructible_v<D, remove_ref_t<decltype(*src)>&&>) {
        for (; dst != end; ++dst, (void)++src)
            new(dst) D(static_cast<remove_ref_t<decltype(*src)>&&>(*src));

        return dst;
    }

    template<class D, class S, class Mapper, class... Args> constexpr static D* _map_construct(D* dst, D const* end, S src, Mapper& mapper, Args&&... args)
        noexcept(is_nothrow_constructible_v<D, util::invoke_result_t<Mapper, decltype(*src), Args...>> && util::nothrow_invocable_v<Mapper, decltype(*src), Args...>) {
        for (; dst != end; ++dst, (void)++src)
            new(dst) D(util::invoke(mapper, *src, static_cast<Args&&>(args)...));

        return dst;
    }

    template<class T, class Arg> constexpr static void _args_construct(T* data, Arg&& arg) noexcept(is_nothrow_constructible_v<T, Arg&&>) {
        new(data) T(static_cast<Arg&&>(arg));
    }

    template<class T, class Arg, class... Args> constexpr static void _args_construct(T* data, Arg&& arg, Args&&... args) noexcept(is_nothrow_constructible_v<T, Arg&&> && noexcept(_args_construct(data, static_cast<Args&&>(args)...))) {
        new(data) T(static_cast<Arg&&>(arg));
        _args_construct(data + 1, static_cast<Args&&>(args)...);
    }

    template<class Array> _INLINE_VAR constexpr static auto _size = null<Array>->size();

    template<class T, size_t N> _INLINE_VAR constexpr static size_t _size<T[N]> = N;

    template<class C> static sfinae<decltype(_size<remove_ref_t<C>>)> _static_size(int);
    template<class C> static std::false_type _static_size(...);

public:
    template<class T, class... Args>
    constexpr _INLINE_VAR static bool nothrow_args_construct = noexcept(_args_construct<T>(0, std::declval<Args>()...));

    template<class T, class... Args>
    constexpr _INLINE_VAR static bool nothrow_args_assign = noexcept(_args_assign<T>(0, std::declval<Args>()...));

    template<class Array> constexpr _INLINE_VAR static auto size = _size<remove_ref_t<Array>>;

    template<class C> struct has_constexpr_size : decltype(_static_size<C>(0)) {};

    template<class C> constexpr _INLINE_VAR static bool has_constexpr_size_v = has_constexpr_size<C>::value;

protected:
    struct _dummy {};
    struct _map {};

    template<class C, class Proc, class... Args> constexpr static auto _foreach(C& c, Proc&& proc, Args&&... args) noexcept(util::nothrow_invocable_v<Proc, decltype(*c._Unchecked_begin()), Args...>)
        -> type_if<decltype(c.size()), util::invocable_v<Proc, decltype(*c._Unchecked_begin()), Args...>> {
        for (auto i = c._Unchecked_begin(), end = c._Unchecked_end(); i != end; ++i)
        {
            util::invoke(static_cast<Proc&&>(proc), *i, static_cast<Args&&>(args)...);
        }
        return c.size();
    }

    template<class C, class Proc, class... Args> constexpr static auto _rforeach(C& c, Proc&& proc, Args&&... args) noexcept(util::nothrow_invocable_v<Proc, decltype(*c._Unchecked_begin()), Args...>)
        -> type_if<decltype(c.size()), util::invocable_v<Proc, decltype(*c._Unchecked_begin()), Args...>> {
        for (auto i = c._Unchecked_end(), rend = c._Unchecked_begin(); i != rend; )
        {
            --i;
            util::invoke(static_cast<Proc&&>(proc), *i, static_cast<Args&&>(args)...);
        }
        return c.size();
    }

public:
    template<class... Args> _NODISCARD static array<common_type<Args...>, sizeof...(Args)> of(Args&&... args) noexcept(nothrow_args_construct<common_type<Args...>, Args...>) {
        return { static_cast<Args&&>(args)... };
    }

    template<class Array, class Mapper, class... Args, class T = type_if<util::invoke_result_t<Mapper, container::const_reference<Array>, Args...>, container::data_availability_v<Array>>>
    _NODISCARD static array<T, size<Array>> map(Array const& arr, Mapper&& mapper, Args&&... args) {
        return { _map{}, container::data(arr), mapper, static_cast<Args&&>(args)... };
    }

    template<class I> _NODISCARD static auto from(I begin, I end)
        -> type_if<array<remove_const_t<remove_ref_t<decltype(*begin)>>>, iterators::fwd_iter_v<I>> {
        return { begin, end };
    }

    template<class T> _NODISCARD static array<T> from(typename array<T>::size_type size, T const* data) {
        return { size, data };
    }

    template<class T> _NODISCARD static array<T> from(std::nothrow_t, typename array<T>::size_type size, T const* data) noexcept(is_nothrow_constructible_v<T, T const&>) {
        return { std::nothrow, size, data };
    }

    template<class, size_t...> friend struct array;
};

using arrays = array<void>;

template<class T> struct array<T, 0>
{
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;

    using iterator = ::iterator<pointer>;
    using const_iterator = ::iterator<const_pointer>;

    using reverse_iterator = ::reverse_iterator<pointer>;
    using const_reverse_iterator = ::reverse_iterator<const_pointer>;

    array() = default;

    array(array const&) = default;
    array& operator=(array const&) = default;

    array(array&&) = default;
    array& operator=(array&&) = default;

    template<class C, class Pred = std::function<bool(const_reference, container::const_reference<C>)>>
    _NODISCARD constexpr type_if<bool, container::sizeable_v<C>, convertible_v<util::invoke_result_t<Pred, const_reference, container::const_reference<C>>, bool>>
        equal(C const& c, Pred&& pred) const noexcept {
        return container::size(c) == 0;
    }

    template<class Arg> _NODISCARD constexpr auto equal(Arg&& arg) const noexcept
        -> decltype(equal(static_cast<Arg&&>(arg), equal_to{})) {
        return equal(static_cast<Arg&&>(arg), equal_to{});
    }

    _NODISCARD constexpr iterator begin() noexcept { return _Unchecked_begin(); }
    _NODISCARD constexpr const_iterator begin() const noexcept { return _Unchecked_begin(); }
    _NODISCARD constexpr const_iterator cbegin() const noexcept { return _Unchecked_begin(); }

    _NODISCARD constexpr iterator end() noexcept { return _Unchecked_end(); }
    _NODISCARD constexpr const_iterator end() const noexcept { return _Unchecked_end(); }
    _NODISCARD constexpr const_iterator cend() const noexcept { return _Unchecked_end(); }

    _NODISCARD constexpr reverse_iterator rbegin() noexcept { return _Unchecked_end(); }
    _NODISCARD constexpr const_reverse_iterator rbegin() const noexcept { return _Unchecked_end(); }
    _NODISCARD constexpr const_reverse_iterator crbegin() const noexcept { return _Unchecked_end(); }

    _NODISCARD constexpr reverse_iterator rend() noexcept { return _Unchecked_begin(); }
    _NODISCARD constexpr const_reverse_iterator rend() const noexcept { return _Unchecked_begin(); }
    _NODISCARD constexpr const_reverse_iterator crend() const noexcept { return _Unchecked_begin(); }

    constexpr size_type fill(T const&) noexcept { return 0; }

    constexpr void swap(array& other) noexcept { objects::swap_bytes(other, *this); }

    _NODISCARD constexpr pointer _Unchecked_begin() noexcept { return m_data; }

    _NODISCARD constexpr const_pointer _Unchecked_begin() const noexcept { return m_data; }

    _NODISCARD constexpr pointer _Unchecked_end() noexcept { return m_data; }

    _NODISCARD constexpr const_pointer _Unchecked_end() const noexcept { return m_data; }

    _NODISCARD constexpr size_type size() const noexcept { return 0; }

    _NODISCARD constexpr size_type max_size() const noexcept { return 0; }

    _NODISCARD constexpr bool empty() const noexcept { return true; }

    _NODISCARD constexpr pointer data() noexcept { return m_data; }

    _NODISCARD constexpr const_pointer data() const noexcept { return m_data; }

    constexpr void reverse() noexcept {}

    _NODISCARD constexpr array reversed() noexcept { return {}; }

    ~array() noexcept { objects::destroy(m_data); }

protected:
    union
    {
        char m_[sizeof T]{};
        T m_data[1];
    };

};

template<class T, size_t N> struct array<T, N>
{
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;

    using iterator = ::iterator<pointer>;
    using const_iterator = ::iterator<const_pointer>;

    using reverse_iterator = ::reverse_iterator<pointer>;
    using const_reverse_iterator = ::reverse_iterator<const_pointer>;

    template<class Int = type_if<int, is_constructible_v<T>>, Int = 0>
    constexpr array() noexcept(is_nothrow_constructible_v<T>) : m_elems{} {
    }

    template<class... Args, type_if<int, sizeof...(Args) == N, is_constructible_from_each_v<T, Args...>> = 0>
    constexpr array(Args&&... args) noexcept(arrays::nothrow_args_construct<T, Args...>)
        : m_() {
        arrays::_args_construct(_Unchecked_begin(), static_cast<Args&&>(args)...);
    }

    constexpr array(type_if<array, is_copy_constructible_v<T>> const& o) noexcept(is_nothrow_copy_constructible_v<T>)
        : m_() {
        arrays::_copy_construct(_Unchecked_begin(), _Unchecked_end(), o._Unchecked_begin());
    }

    constexpr array(type_if<array, is_move_constructible_v<T>>&& o) noexcept(is_nothrow_move_constructible_v<T>)
        : m_() {
        arrays::_move_construct(_Unchecked_begin(), _Unchecked_end(), o._Unchecked_begin());
    }

    template<class Array, class V = container::value_type<Array>, type_if<int, N == arrays::size<Array>, container::data_availability_v<Array>, is_constructible_v<T, V const&>> = 0>
    constexpr array(Array const& arr) noexcept(is_nothrow_constructible_v<T, V const&>)
        : m_() {
        arrays::_copy_construct(_Unchecked_begin(), _Unchecked_end(), container::data(arr));
    }

    template<class Array, class V = container::value_type<Array>, type_if<int, N == arrays::size<Array>, container::data_availability_v<Array>, is_constructible_v<T, V&&>> = 0>
    constexpr array(Array&& arr) noexcept(is_nothrow_constructible_v<T, V&&>)
        : m_() {
        arrays::_move_construct(_Unchecked_begin(), _Unchecked_end(), container::data(arr));
    }

    template<class U, type_if<int, is_constructible_v<T, U const&>> = 0>
    constexpr array(U const(&data)[N]) noexcept(is_nothrow_constructible_v<T, U const&>)
        : m_() {
        arrays::_copy_construct(_Unchecked_begin(), _Unchecked_end(), data);
    }

    template<class U, type_if<int, is_constructible_v<T, U&&>> = 0>
    constexpr array(U(&& data)[N]) noexcept(is_nothrow_constructible_v<T, U&&>)
        : m_() {
        arrays::_move_construct(_Unchecked_begin(), _Unchecked_end(), data);
    }

    constexpr array& operator=(type_if<array, is_copy_assignable_v<T>> const& o) noexcept(is_nothrow_copy_assignable_v<T>) {
        if (this != std::addressof(o))
            arrays::_copy_assign(_Unchecked_begin(), _Unchecked_end(), o._Unchecked_begin());
        return *this;
    }

    constexpr array& operator=(type_if<array, is_move_assignable_v<T>>&& o) noexcept(is_nothrow_move_assignable_v<T>) {
        if (this != std::addressof(o))
            arrays::_move_assign(_Unchecked_begin(), _Unchecked_end(), o._Unchecked_begin());
        return *this;
    }

    template<class Array, class V = container::value_type<Array>, type_if<int, N == arrays::size<Array>, container::data_availability_v<Array>, is_assignable_v<T, V const&>> = 0>
    constexpr array& operator=(Array const& arr) noexcept(is_nothrow_assignable_v<T, V const&>) {
        arrays::_copy_assign(_Unchecked_begin(), _Unchecked_end(), container::data(arr));
        return *this;
    }

    template<class Array, class V = container::value_type<Array>, type_if<int, N == arrays::size<Array>, container::data_availability_v<Array>, is_assignable_v<T, V&&>> = 0>
    constexpr array& operator=(Array&& arr) noexcept(is_nothrow_assignable_v<T, V&&>) {
        arrays::_move_assign(_Unchecked_begin(), _Unchecked_end(), container::data(arr));
        return *this;
    }

    template<class U, type_if<int, is_assignable_v<T, U const&>> = 0>
    constexpr array& operator=(U const(&data)[N]) noexcept(is_nothrow_assignable_v<T, U const&>) {
        arrays::_copy_assign(_Unchecked_begin(), _Unchecked_end(), data);
        return *this;
    }

    template<class U, type_if<int, is_assignable_v<T, U&&>> = 0>
    constexpr array& operator=(U(&& data)[N]) noexcept(is_nothrow_assignable_v<T, U&&>) {
        arrays::_move_assign(_Unchecked_begin(), _Unchecked_end(), data);
        return *this;
    }

    template<class... Args, type_if<int, sizeof...(Args) == N, is_assignable_from_each_v<T, Args...>> = 0>
    constexpr array& operator=(Args&&... args) noexcept(arrays::nothrow_args_assign<T, Args...>) {
        arrays::_args_assign(_Unchecked_begin(), static_cast<Args&&>(args)...);
        return *this;
    }

    template<class... Args>
    constexpr type_if<array&, sizeof...(Args) == N, is_assignable_from_each_v<T, Args...>>
        assign(Args&&... args) noexcept(arrays::nothrow_args_assign<T, Args...>) {
        arrays::_args_assign(_Unchecked_begin(), static_cast<Args&&>(args)...);
        return *this;
    }

    template<class Array, class Pred = std::function<bool(const_reference, container::const_reference<Array>)>>
    _NODISCARD constexpr type_if<bool, N == arrays::size<Array>, container::data_availability_v<Array>, convertible_v<util::invoke_result_t<Pred, const_reference, container::const_reference<Array>>, bool>>
        equal(Array const& arr, Pred&& pred) const {
        return std::equal(_Unchecked_begin(), _Unchecked_end(), container::data(arr), static_cast<Pred&&>(pred));
    }

    template<class U, class Pred = std::function<bool(T const&, U const&)>>
    _NODISCARD constexpr type_if<bool, convertible_v<util::invoke_result_t<Pred, T const&, U const&>, bool>>
        equal(U const(&arr)[N], Pred&& pred) const {
        return std::equal(_Unchecked_begin(), _Unchecked_end(), arr, static_cast<Pred&&>(pred));
    }

    template<class C, class Pred = std::function<bool(const_reference, container::const_reference<C>)>>
    _NODISCARD type_if<bool, !arrays::has_constexpr_size_v<C>, container::sizeable_v<C>, container::iterable_v<C>, convertible_v<util::invoke_result_t<Pred, const_reference, container::const_reference<C>>, bool>>
        equal(C const& c, Pred&& pred) const {
        return N == container::size(c) && std::equal(_Unchecked_begin(), _Unchecked_end(), container::begin(c), static_cast<Pred&&>(pred));
    }

    template<class C, class Pred = std::function<bool(const_reference, container::const_reference<C>)>>
    _NODISCARD type_if<bool, !arrays::has_constexpr_size_v<C>, !container::sizeable_v<C>, container::iterable_v<C>, convertible_v<util::invoke_result_t<Pred, const_reference, container::const_reference<C>>, bool>>
        equal(C const& c, Pred&& pred) const {
        return std::equal(_Unchecked_begin(), _Unchecked_end(), container::begin(c), container::end(c), static_cast<Pred&&>(pred));
    }

    template<class Arg> _NODISCARD constexpr auto equal(Arg&& arg) const
        -> decltype(equal(static_cast<Arg&&>(arg), equal_to{})) {
        return equal(static_cast<Arg&&>(arg), equal_to{});
    }

    template<class U> _NODISCARD constexpr auto equal(U const(&arr)[N]) const
        -> decltype(equal(arr, equal_to{})) {
        return equal(arr, equal_to{});
    }

    _NODISCARD constexpr iterator begin() noexcept { return _Unchecked_begin(); }
    _NODISCARD constexpr const_iterator begin() const noexcept { return _Unchecked_begin(); }
    _NODISCARD constexpr const_iterator cbegin() const noexcept { return begin(); }

    _NODISCARD constexpr iterator end() noexcept { return _Unchecked_end(); }
    _NODISCARD constexpr const_iterator end() const noexcept { return _Unchecked_end(); }
    _NODISCARD constexpr const_iterator cend() const noexcept { return end(); }

    _NODISCARD constexpr reverse_iterator rbegin() noexcept { return _Unchecked_end() - 1; }
    _NODISCARD constexpr const_reverse_iterator rbegin() const noexcept { return _Unchecked_end() - 1; }
    _NODISCARD constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

    _NODISCARD constexpr reverse_iterator rend() noexcept { return _Unchecked_begin() - 1; }
    _NODISCARD constexpr const_reverse_iterator rend() const noexcept { return _Unchecked_begin() - 1; }
    _NODISCARD constexpr const_reverse_iterator crend() const noexcept { return rend(); }

    constexpr size_type fill(T const& value) {
        std::fill_n(m_elems, N, value);
        return N;
    }

    constexpr void swap(array& other) noexcept { objects::swap_bytes(other, *this); }

    _NODISCARD constexpr pointer _Unchecked_begin() noexcept { return m_elems; }

    _NODISCARD constexpr const_pointer _Unchecked_begin() const noexcept { return m_elems; }

    _NODISCARD constexpr pointer _Unchecked_end() noexcept { return m_elems + N; }

    _NODISCARD constexpr const_pointer _Unchecked_end() const noexcept { return m_elems + N; }

    _NODISCARD constexpr size_type size() const noexcept { return N; }

    _NODISCARD constexpr size_type max_size() const noexcept { return N; }

    _NODISCARD constexpr bool empty() const noexcept { return false; }

    _NODISCARD constexpr reference operator[](size_type pos) noexcept {
        return m_elems[pos];
    }

    _NODISCARD constexpr const_reference operator[](size_type pos) const noexcept {
        return m_elems[pos];
    }

    _NODISCARD constexpr reference at(size_type pos) {
        arrays::_check_range<size_type, N>(pos);
        return (*this)[pos];
    }

    _NODISCARD constexpr const_reference at(size_type pos) const {
        arrays::_check_range<size_type, N>(pos);
        return (*this)[pos];
    }

    type_if<optional<T>, !is_const_v<T>> put(size_type pos, const_reference value) {
        arrays::_check_range<size_type, N>(pos);
        optional<T> res = value;
        objects::swap_bytes((*this)[pos], *res);
        return res;
    }

    _NODISCARD constexpr reference front() noexcept { return m_elems[0]; }

    _NODISCARD constexpr const_reference front() const noexcept { return m_elems[0]; }

    _NODISCARD constexpr reference back() noexcept { return m_elems[N - 1]; }

    _NODISCARD constexpr const_reference back() const noexcept { return m_elems[N - 1]; }

    _NODISCARD constexpr pointer data() noexcept { return m_elems; }

    _NODISCARD constexpr const_pointer data() const noexcept { return m_elems; }

    constexpr void reverse() noexcept {
        for (auto i = _Unchecked_begin(), j = _Unchecked_end(); i != j && i != --j; ++i)
        {
            objects::swap_bytes<sizeof T>(i, j);
        }
    }

    _NODISCARD constexpr array reversed() const {
        array reversed = arrays::_dummy{};
        arrays::_copy_construct(reversed.m_elems, reversed.m_elems + N, crbegin());
        return reversed;
    }

    template<class Comp = default_comporator, class U = T, class R = util::invoke_result_t<Comp, T const&, U const&>>
    _NODISCARD type_if<R, objects::is_ordering_v<R>> compare(array<U, N> const& other, Comp&& comp = Comp{}) const noexcept(util::nothrow_invocable_v<Comp, T const&, U const&>) {
        auto i = this->_Unchecked_begin();
        auto j = other._Unchecked_begin();
        R res = util::invoke(static_cast<Comp&&>(comp), *i, *j);
        if (res != 0) return res;
        ++i; ++j;
        for (auto const end = this->_Unchecked_end(); i != end; ++i, (void)++j) {
            res = util::invoke(static_cast<Comp&&>(comp), *i, *j);
            if (res != 0) return res;
        }
        return res;
    }

    template<class Proc = std::function<void(reference)>>
    constexpr type_if<size_type, util::invocable_v<Proc, reference>> foreach(Proc&& proc) { return arrays::_foreach(*this, static_cast<Proc&&>(proc)); }

    template<class Proc = std::function<void(const_reference)>>
    constexpr type_if<size_type, util::invocable_v<Proc, const_reference>> foreach(Proc&& proc) const { return arrays::_foreach(*this, static_cast<Proc&&>(proc)); }

    template<class... Args, class Proc = std::function<void(reference, Args...)>>
    constexpr type_if<size_type, util::invocable_v<Proc, reference, Args...>> foreach(Proc&& proc, Args&&... args) { return arrays::_foreach(*this, static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

    template<class... Args, class Proc = std::function<void(const_reference, Args...)>>
    constexpr type_if<size_type, util::invocable_v<Proc, const_reference, Args...>> foreach(Proc&& proc, Args&&... args) const { return arrays::_foreach(*this, static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

    template<class Proc = std::function<void(reference)>>
    constexpr type_if<size_type, util::invocable_v<Proc, reference>> rforeach(Proc&& proc) { return arrays::_rforeach(*this, static_cast<Proc&&>(proc)); }

    template<class Proc = std::function<void(const_reference)>>
    constexpr type_if<size_type, util::invocable_v<Proc, const_reference>> rforeach(Proc&& proc) const { return arrays::_rforeach(*this, static_cast<Proc&&>(proc)); }

    template<class... Args, class Proc = std::function<void(reference, Args...)>>
    constexpr type_if<size_type, util::invocable_v<Proc, reference, Args...>> rforeach(Proc&& proc, Args&&... args) { return arrays::_rforeach(*this, static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

    template<class... Args, class Proc = std::function<void(const_reference, Args...)>>
    constexpr type_if<size_type, util::invocable_v<Proc, const_reference, Args...>> rforeach(Proc&& proc, Args&&... args) const { return arrays::_rforeach(*this, static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

    template<class Mapper, class... Args, class U = util::invoke_result_t<Mapper, const_reference, Args...>>
    constexpr _NODISCARD type_if<array<U, N>, !is_same_v<void, U>> map(Mapper&& mapper, Args&&... args) const {
        return { arrays::_map{}, _Unchecked_begin(), mapper, static_cast<Args&&>(args)... };
    }

    ~array() noexcept {
        objects::destroy_range(_Unchecked_begin(), _Unchecked_end());
    }

protected:
    union {
        remove_const_t<T> m_elems[N];
        char m_;
    };

    constexpr array(arrays::_dummy) noexcept : m_() {}

    template<class S, class Mapper, class... Args> constexpr array(arrays::_map, S s, Mapper& mapper, Args&&... args)
        : m_() {
        arrays::_map_construct(_Unchecked_begin(), _Unchecked_end(), s, mapper, static_cast<Args&&>(args)...);
    }

    template<class, size_t...> friend struct array;

public:
    constexpr operator std::array<T, N>& () & noexcept { return reinterpret_cast<std::array<T, N>&>(*this); }
    constexpr operator std::array<T, N> const& () const& noexcept { return reinterpret_cast<std::array<T, N> const&>(*this); }
    constexpr operator std::array<T, N> && () && noexcept { return reinterpret_cast<std::array<T, N>&&>(*this); }
    constexpr operator std::array<T, N> const&& () const&& noexcept { return reinterpret_cast<std::array<T, N> const&&>(*this); }

};

#if _HAS_CXX17

//template<class... Args> array(Args...)->array<common_type<Args...>, sizeof...(Args)>;

template<class T, size_t N> array(T const(&)[N])->array<T, N>;

template<class T, size_t N> array(T(&&)[N])->array<T, N>;

template<class T, size_t N> array(array<T, N> const&)->array<T, N>;

template<class T, size_t N> array(array<T, N>&&)->array<T, N>;

#endif // _HAS_CXX17

namespace std
{
    template<size_t I, class T, size_t N>
    ::type_if<T&, (I < N)> get(::array<T, N>& o) noexcept {
        return o[I];
    }

    template<size_t I, class T, size_t N>
    ::type_if<T const&, (I < N)> get(::array<T, N> const& o) noexcept {
        return o[I];
    }
}

template<class T, size_t N, size_t M> struct array<T, N, M> : array<array<T, M>, N>
{
    static_assert(N != 0, "N == 0");
    static_assert(M != 0, "M == 0");

    using base = array<array<T, M>, N>;
    using size_type = typename base::size_type;

    using base::at;

    _NODISCARD constexpr T& at(size_type x, size_type y) {
        return at(x).at(y);
    }

    _NODISCARD constexpr T const& at(size_type x, size_type y) const {
        return at(x).at(y);
    }
};

template<class T, size_t N, size_t... M> struct array<T, N, M...> : array<array<T, M...>, N>
{
    static_assert(N != 0, "N == 0");

    using base = array<array<T, M...>, N>;
    using size_type = typename base::size_type;

    using base::at;

    template<class... size_t> _NODISCARD auto& at(size_type x, size_type y, size_t... z) {
        return at(x).at(y, z...);
    }

    template<class... size_t> _NODISCARD auto const& at(size_type x, size_type y, size_t... z) const {
        return at(x).at(y, z...);
    }
};

template<class T> struct array<T>
{
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;

    using iterator = ::iterator<pointer>;
    using const_iterator = ::iterator<const_pointer>;

    using reverse_iterator = ::reverse_iterator<pointer>;
    using const_reverse_iterator = ::reverse_iterator<const_pointer>;

    template<class... Args> constexpr static type_if<array<T, sizeof...(Args)>, is_constructible_from_each_v<T, Args...>>
        of(Args&&... args) noexcept(arrays::nothrow_args_construct<T, Args...>) {
        return { static_cast<Args&&>(args)... };
    }

    template<class Int = type_if<int, is_constructible_v<T>>, Int = 0>
    array(size_type const n) : array(_n_copies(n), n) {
    }

    template<class V = T, type_if<int, is_constructible_v<T, V const&>> = 0>
    array(size_type const n, V const& value) : array(_n_copies(n, value), n) {
    }

    template<class I, type_if<int, iterators::fwd_iter_v<I>, is_constructible_v<T, decltype(*std::declval<I>())>> = 0>
    array(I begin, I end) : array() {
        array::_init(m_elems, m_size, begin, end);
    }

    template<class I, class Filter, class U = decltype(*std::declval<I>()), type_if<int, iterators::fwd_iter_v<I>, convertible_v<util::invoke_result_t<Filter, U>, bool>, is_constructible_v<T, U>> = 0>
    array(I begin, I end, Filter&& filter)
        : m_elems(nullptr), m_size(array::_filtered(m_elems, 0, begin, end, filter)) {
    }

    constexpr array(array&& other) noexcept : array(_move{}, other) {
    }

    template<class U, type_if<int, convertible_v<remove_const_t<U>*, T*>> = 0>
    constexpr array(array<U>&& other) noexcept : array(_move{}, other) {
    }

    template<class U, type_if<int, convertible_v<remove_const_t<U>*, T*>> = 0>
    constexpr array(size_type const size, std::unique_ptr<U[]>&& ptr) noexcept
        : array(ptr.release(), size) {
    }

    array(type_if<array, is_copy_constructible_v<T>> const& other)
        : array(other.size(), other.data()) {
    }

    template<class U, type_if<int, is_constructible_v<T, U const&>> = 0>
    array(array<U> const& other)
        : array(other.size(), other.data()) {
    }

    template<class U, type_if<int, is_constructible_v<T, U const&>> = 0>
    array(std::nothrow_t, array<U> const& other) noexcept(is_nothrow_constructible_v<T, U const&>)
        : array(std::nothrow, other.size(), other.data()) {
    }

    template<class U, type_if<int, is_constructible_v<T, U const&>> = 0>
    array(size_type const size, U const* data) : array(_alloc(size), size) {
        if (data)
            arrays::_copy_construct(m_elems, m_elems + size, data);
    }

    template<class U, type_if<int, is_constructible_v<T, U const&>> = 0>
    array(std::nothrow_t, size_type const size, U const* data) noexcept(is_nothrow_constructible_v<T, U const&>)
        : m_elems(_alloc(std::nothrow, size)), m_size(m_elems ? size : 0) {
        if (data)
            if (m_elems) arrays::_copy_construct(m_elems, m_elems + size, data);
    }

    template<class C, class V = container::value_type<C>, class Pred = std::function<bool(T const&, V const&)>>
    _NODISCARD type_if<bool, container::sizeable_v<C>, container::iterable_v<C>, convertible_v<util::invoke_result_t<Pred, T const&, V const&>, bool>>
        equal(C const& c, Pred&& pred) const {
        return size() == container::size(c) && std::equal(_Unchecked_begin(), _Unchecked_end(), container::begin(c), static_cast<Pred&&>(pred));
    }

    template<class V, size_t N, class Pred>
    _NODISCARD type_if<bool, convertible_v<util::invoke_result_t<Pred, T const&, V const&>, bool>>
        equal(V const(&arr)[N], Pred&& pred) const {
        return size() == N && std::equal(_Unchecked_begin(), _Unchecked_end(), arr, static_cast<Pred&&>(pred));
    }

    template<class Arg> _NODISCARD auto equal(Arg&& arg) const
        -> decltype(equal(static_cast<Arg&&>(arg), equal_to{})) {
        return equal(static_cast<Arg&&>(arg), equal_to{});
    }

    template<class V, size_t N> _NODISCARD auto equal(V const(&arr)[N]) const
        -> decltype(equal(arr, equal_to{})) {
        return equal(arr, equal_to{});
    }

    constexpr void swap(array& other) noexcept { objects::swap_bytes(other, *this); }

    _NODISCARD iterator begin() noexcept { return _Unchecked_begin(); }
    _NODISCARD const_iterator begin() const noexcept { return _Unchecked_begin(); }
    _NODISCARD const_iterator cbegin() const noexcept { return begin(); }

    _NODISCARD iterator end() noexcept { return _Unchecked_end(); }
    _NODISCARD const_iterator end() const noexcept { return _Unchecked_end(); }
    _NODISCARD const_iterator cend() const noexcept { return end(); }

    _NODISCARD reverse_iterator rbegin() noexcept { return _Unchecked_end() - 1; }
    _NODISCARD const_reverse_iterator rbegin() const noexcept { return _Unchecked_end() - 1; }
    _NODISCARD const_reverse_iterator crbegin() const noexcept { return rbegin(); }

    _NODISCARD reverse_iterator rend() noexcept { return _Unchecked_begin() - 1; }
    _NODISCARD const_reverse_iterator rend() const noexcept { return _Unchecked_begin() - 1; }
    _NODISCARD const_reverse_iterator crend() const noexcept { return rend(); }

    constexpr T* _Unchecked_begin() noexcept { return m_elems; }

    constexpr T const* _Unchecked_begin() const noexcept { return m_elems; }

    constexpr T* _Unchecked_end() noexcept { return m_elems + size(); }

    constexpr T const* _Unchecked_end() const noexcept { return m_elems + size(); }

    _NODISCARD size_type size() const noexcept { return m_size; }

    _NODISCARD size_type max_size() const noexcept { return m_size; }

    _NODISCARD bool empty() const noexcept { return m_size == 0; }

    _NODISCARD reference operator[](size_type pos) noexcept {
        return m_elems[pos];
    }

    _NODISCARD const_reference operator[](size_type pos) const noexcept {
        return m_elems[pos];
    }

    _NODISCARD reference at(size_type pos) {
        arrays::_check_range<size_type>(pos, size());
        return (*this)[pos];
    }

    _NODISCARD const_reference at(size_type pos) const {
        arrays::_check_range<size_type>(pos, size());
        return (*this)[pos];
    }

    type_if<optional<T>, !is_const_v<T>> put(size_type pos, const_reference value) {
        arrays::_check_range<size_type>(pos, size());
        optional<T> res = value;
        objects::swap_bytes((*this)[pos], res.value());
        return res;
    }

    _NODISCARD reference front() noexcept { return m_elems[0]; }

    _NODISCARD const_reference front() const noexcept { return m_elems[0]; }

    _NODISCARD reference back() noexcept { return m_elems[size() - 1]; }

    _NODISCARD const_reference back() const noexcept { return m_elems[size() - 1]; }

    _NODISCARD pointer data() noexcept { return m_elems; }

    _NODISCARD const_pointer data() const noexcept { return m_elems; }

    ~array() noexcept {
        objects::destroy_range(_Unchecked_begin(), _Unchecked_end());
        array::_free(m_elems);
        m_elems = nullptr;
    }

    void reverse() noexcept {
        for (auto i = _Unchecked_begin(), j = _Unchecked_end(); i != j && i != --j; ++i) {
            objects::swap_bytes<sizeof T>(i, j);
        }
    }

    _NODISCARD array reversed() const {
        return { crbegin(), crend() };
    }

    template<class Filter>
    _NODISCARD type_if<array, convertible_v<util::invoke_result_t<Filter, const_reference>, bool>> filtered(Filter&& filter) const {
        return { _Unchecked_begin(), _Unchecked_end(), static_cast<Filter&&>(filter) };
    }

    template<class Proc = std::function<void(reference)>>
    type_if<size_type, util::invocable_v<Proc, reference>> foreach(Proc&& proc) { return arrays::_foreach(*this, static_cast<Proc&&>(proc)); }

    template<class Proc = std::function<void(const_reference)>>
    type_if<size_type, util::invocable_v<Proc, const_reference>> foreach(Proc&& proc) const { return arrays::_foreach(*this, static_cast<Proc&&>(proc)); }

    template<class... Args, class Proc = std::function<void(reference, Args...)>>
    type_if<size_type, util::invocable_v<Proc, reference, Args...>> foreach(Proc&& proc, Args&&... args) { return arrays::_foreach(*this, static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

    template<class... Args, class Proc = std::function<void(const_reference, Args...)>>
    type_if<size_type, util::invocable_v<Proc, const_reference, Args...>> foreach(Proc&& proc, Args&&... args) const { return arrays::_foreach(*this, static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

    template<class Proc = std::function<void(reference)>>
    type_if<size_type, util::invocable_v<Proc, reference>> rforeach(Proc&& proc) { return arrays::_rforeach(*this, static_cast<Proc&&>(proc)); }

    template<class Proc = std::function<void(const_reference)>>
    type_if<size_type, util::invocable_v<Proc, const_reference>> rforeach(Proc&& proc) const { return arrays::_rforeach(*this, static_cast<Proc&&>(proc)); }

    template<class... Args, class Proc = std::function<void(reference, Args...)>>
    type_if<size_type, util::invocable_v<Proc, reference, Args...>> rforeach(Proc&& proc, Args&&... args) { return arrays::_rforeach(*this, static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

    template<class... Args, class Proc = std::function<void(const_reference, Args...)>>
    type_if<size_type, util::invocable_v<Proc, const_reference, Args...>> rforeach(Proc&& proc, Args&&... args) const { return arrays::_rforeach(*this, static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

protected:
    remove_const_t<T>* m_elems;
    size_type m_size;

    template<bool construct> struct _bytes
    {
        char _[sizeof T];
    };

    template<> struct _bytes<true>
    {
        char _[sizeof T]{};
    };

    struct _move {};

    constexpr array() noexcept : array(nullptr, 0) {}

    template<class U = T> constexpr array(_move, array<U>&& other) noexcept
        : array(std::exchange(other.m_elems, nullptr), std::exchange(other.m_size, 0)) {
    }

    constexpr array(remove_const_t<T>* elems, size_type size) noexcept
        : m_elems(elems), m_size(size) {
    }

    template<class... Args> static remove_const_t<T>* _n_copies(size_type const size, Args&&... args) {
        auto* const data = new _bytes<false>[size];
        for (auto i = data, end = data + size; i != end; ++i)
        {
            new(i) T(static_cast<Args&&>(args)...);
        }
        return (remove_const_t<T>*) data;
    }

    static remove_const_t<T>* _alloc(size_type const size) {
        return (remove_const_t<T>*) new _bytes<true>[size];
    }

    static remove_const_t<T>* _alloc(std::nothrow_t, size_type const size) noexcept {
        return (remove_const_t<T>*) new(std::nothrow) _bytes<true>[size];
    }

    static void _free(T* ptr) noexcept {
        delete[]((_bytes<true>*)ptr);
    }

    template<class I, bool = iterators::random_iter_v<I>> struct _initial
    {
        remove_const_t<T>*& res_data;
        size_type& res_size;
        I iterator;
        I end;

        constexpr _initial(remove_const_t<T>*& res_data, size_type& res_size, I iterator, I end) noexcept
            : res_data(res_data), res_size(res_size), iterator(iterator), end(end) {
        }

        void operator()() {
            auto const size = res_size;
            I cur = iterator;
            if (cur == end)
            {
                if (size)
                {
                    res_data = _alloc(size);
                }
                return;
            }
            ++res_size;
            ++iterator;
            operator()();
            new(res_data + size) T(*cur);
        }
    };

    template<class I> struct _initial<I, true>
    {
        remove_const_t<T>*& res_data;
        size_type& res_size;
        I begin;
        I end;

        constexpr _initial(remove_const_t<T>*& res_data, size_type& res_size, I begin, I end) noexcept
            : res_data(res_data), res_size(res_size), begin(begin), end(end) {
        }

        void operator()() {
            res_size = end - begin;
            if (res_size)
            {
                res_data = _alloc(res_size);
                for (remove_const_t<T>* i = res_data; begin != end; ++i, (void)++begin)
                {
                    new(i) T(*begin);
                }
            }
        }
    };

    template<class I> static void _init(remove_const_t<T>*& res_data, size_type& res_size, I begin, I end)
    {
        _initial<I>{ res_data, res_size, begin, end }();
    }

    template<class I, class Filter> struct _init_filtered
    {
        remove_const_t<T>*& res_data;
        size_type& res_size;
        I iterator;
        I end;
        Filter& filter;

        constexpr _init_filtered(remove_const_t<T>*& res_data, size_type& res_size, I iterator, I end, Filter& filter) noexcept
            : res_data(res_data), res_size(res_size), iterator(iterator), end(end), filter(filter) {
        }

        void operator()() {
            auto const n = res_size;
            if (iterator == end)
            {
                if (n)
                {
                    res_data = _alloc(n);
                }
                return;
            }
            auto& value = *iterator;
            ++iterator;
            if (filter(value))
            {
                ++res_size;
                operator()();
                new(res_data + n) T(value);
            }
            else
            {
                operator()();
            }
        }
    };

    template<class I> struct _init_filtered<I, always_true> : _initial<I>
    {
        constexpr _init_filtered(remove_const_t<T>*& res_data, size_type& res_size, I iterator, I end, always_true&) noexcept
            : _initial<I>{ res_data, res_size, iterator, end } {
        }

        using _initial<I>::operator();
    };

    template<class I> struct _init_filtered<I, always_false>
    {
        constexpr _init_filtered(remove_const_t<T>*&, size_type&, I, I, always_false&) noexcept {}
        constexpr void operator()() noexcept {}
    };

    template<class I, class Filter> static size_type _filtered(remove_const_t<T>*& res_data, I iterator, I end, Filter& filter) {
        size_t res_size = 0;
        _init_filtered<I, Filter>{ res_data, res_size, iterator, end, filter }();
        return res_size;
    }

    template<class, size_t...> friend struct array;
};

#if _HAS_CXX17

template<class I> array(I, I)->array<remove_ref_t<decltype(*std::declval<I>())>>;

template<class T> array(array<T> const&)->array<T>;

template<class T> array(array<T>&&)->array<T>;

#endif // _HAS_CXX17

namespace std
{
    template<class T, size_t... N>
    constexpr auto swap(::array<T, N...>& l, ::array<T, N...>& r) noexcept(
        noexcept(l.swap(r))) -> decltype(l.swap(r))
    {
        return l.swap(r);
    }
}

template<class T, class U, size_t... N> constexpr auto operator==(array<T, N...> const& l, array<U, N...> const& r)
-> decltype(l.equal(r)) {
    return l.equal(r);
}

template<class T, class U, size_t N> constexpr auto operator==(array<T, N> const& l, U const(&r)[N])
-> decltype(l.equal(r)) {
    return l.equal(r);
}

template<class T, class U, size_t N> constexpr auto operator==(U const(&l)[N], array<T, N> const& r)
-> decltype(r.equal(l)) {
    return r.equal(l);
}

template<class T, class U, size_t... N> constexpr auto operator!=(array<T, N...> const& l, array<U, N...> const& r)
-> decltype(!l.equal(r)) {
    return !l.equal(r);
}

template<class T, class U, size_t N> constexpr auto operator!=(array<T, N> const& l, U const(&r)[N])
-> decltype(!l.equal(r)) {
    return !l.equal(r);
}

template<class T, class U, size_t N> constexpr auto operator!=(U const(&l)[N], array<T, N> const& r)
-> decltype(!r.equal(l)) {
    return !r.equal(l);
}

#endif // !__ARRAY_HPP