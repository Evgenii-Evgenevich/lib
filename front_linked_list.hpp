#ifndef __FRONT_LINKED_LIST_HPP
#define __FRONT_LINKED_LIST_HPP 1

#include "util.hpp"
#include "iterator.hpp"
#include "container.hpp"
#include "singly_linked_node.hpp"

template<class> struct front_linked_list;

template<class T>
struct front_linked_list {
    using node = singly_linked_node<T>;

    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;

    using iterator = ::iterator<singly_linked_node<void, node>>;
    using const_iterator = ::iterator<singly_linked_node<void, node> const>;

    template<class V = T>
    type_if<front_linked_list, !is_constructible_v<T, V const&>> n_copies(size_type n, V const& value) {
        front_linked_list res;
        res._insert_after_n_copies(&res.m_before_first, n, value);
        return res;
    }

    type_if<front_linked_list, is_constructible_v<T>> n_copies(size_type n) {
        front_linked_list res;
        res._insert_after_n_copies(&res.m_before_first, n);
        return res;
    }

    constexpr front_linked_list() noexcept {}

    constexpr front_linked_list(front_linked_list&& other) noexcept { swap(other); }

    constexpr front_linked_list& operator=(front_linked_list&& rigth) noexcept {
        if (this != std::addressof(rigth))
        {
            clear();
            swap(rigth);
        }
        return *this;
    }

    front_linked_list(type_if<front_linked_list, is_copy_constructible_v<T>> const& other) {
        this->_insert_after_iter(&m_before_first, other.cbegin(), other.cend());
    }

    front_linked_list& operator=(type_if<front_linked_list, is_copy_constructible_v<T>> const& rigth) {
        if (this != std::addressof(rigth))
            this->assign(rigth.cbegin(), rigth.cend());
        return *this;
    }

    template<class C, type_if<int, !is_constructible_v<T, C const&>, container::iterable_v<C>, is_constructible_v<T, container::const_reference<C>>> = 0>
    front_linked_list(C const& c) {
        this->_insert_after_iter(&m_before_first, container::begin(c), container::end(c));
    }

    template<class C, type_if<int, !is_constructible_v<T, C const&>, container::iterable_v<C>, is_constructible_v<T, container::const_reference<C>>> = 0>
    front_linked_list& operator=(C const& c) {
        if (this != std::addressof(c))
        {
            this->assign(container::begin(c), container::end(c));
        }
        return *this;
    }

    template<class I, type_if<int, is_constructible_v<T, decltype(*std::declval<I>())>> = 0>
    front_linked_list(I first, I last) {
        this->_insert_after_iter(&m_before_first, first, last);
    }

    template<class I, class Filter, class U = decltype(*std::declval<I>()), type_if<int, convertible_v<util::invoke_result_t<Filter, U>, bool>, is_constructible_v<T, U>> = 0>
    front_linked_list(I first, I last, Filter&& filter) {
        this->_insert_after_iter(&m_before_first, first, last, filter);
    }

    template<class... Args, type_if<int, is_constructible_from_each_v<T, Args...>> = 0>
    front_linked_list(Args&&... args) {
        this->_insert_after_args(&m_before_first, static_cast<Args&&>(args)...);
    }

    template<class I, type_if<int, is_constructible_v<T, decltype(*std::declval<I>())>> = 0>
    void assign(I first, I last) {
        node& prev = m_before_first;
        node* cur = prev->next();
        for (I i = first; i != last; ++i, (void)prev = *cur, (void)cur = prev->next()) {
            if (cur == nullptr)
            {
                return this->_insert_after_iter(&prev, i, last);
            }
            cur->assign(*i);
        }
        return _erase_after(cur, nullptr);
    }

    ~front_linked_list() noexcept {
        delete m_before_first.take_next();
        objects::destroy(m_before_first);
    }

    void clear() noexcept {
        delete m_before_first.take_next();
    }

    constexpr void swap(front_linked_list& other) noexcept { objects::swap_bytes(*this, other); }

    _NODISCARD reference front() noexcept { return _first()->value(); }
    _NODISCARD const_reference front() const noexcept { return _first()->value(); }

    _NODISCARD iterator before_begin() noexcept { return &m_before_first; }
    _NODISCARD const_iterator before_begin() const noexcept { return &m_before_first; }
    _NODISCARD const_iterator cbefore_begin() const noexcept { return before_begin(); }

    _NODISCARD iterator begin() noexcept { return _first(); }
    _NODISCARD const_iterator begin() const noexcept { return _first(); }
    _NODISCARD const_iterator cbegin() const noexcept { return begin(); }

    _NODISCARD iterator end() noexcept { return nullptr; }
    _NODISCARD const_iterator end() const noexcept { return nullptr; }
    _NODISCARD const_iterator cend() const noexcept { return end(); }

    _NODISCARD bool empty() const noexcept { return _first() == nullptr; }

    _NODISCARD constexpr size_type max_size() const noexcept { return SIZE_MAX / 2; }

    template<class... Args>
    type_if<reference, is_constructible_v<T, Args...>> emplace_front(Args&&... args) {
        node* first = this->_emplace_after(m_before_first, static_cast<Args&&>(args)...);
        return first->value();
    }

    template<class... Args>
    type_if<iterator, is_constructible_v<T, Args...>> emplace_after(const_iterator pos, Args&&... args) {
        node* after = const_cast<node*>(pos.cur);
        return after == nullptr ? nullptr : this->_emplace_after(*after, static_cast<Args&&>(args)...);
    }

    template<class Arg = const_reference, class... Args>
    type_if<iterator, is_constructible_from_each_v<T, Arg, Args...>> insert_after(const_iterator pos, Arg&& arg, Args&&... args) {
        node* after = const_cast<node*>(pos.cur);
        return after == nullptr ? nullptr : this->_insert_after_args(after, static_cast<Arg&&>(arg), static_cast<Args&&>(args)...);
    }

    template<class I>
    auto insert_after(const_iterator pos, I first, I last)
        -> type_if<iterator, iterators::fwd_iter_v<I>, is_constructible_v<T, decltype(*first)>> {
        node* after = const_cast<node*>(pos.cur);
        return after == nullptr ? nullptr : this->_insert_after_iter(after, first, last);
    }

    template<class C, type_if<int, !is_constructible_v<T, C const&>, !container::foreachable_v<C>, container::iterable_v<C>, is_constructible_v<T, container::const_reference<C>>> = 0>
    iterator insert_after(const_iterator pos, C const& iterable) {
        node* after = const_cast<node*>(pos.cur);
        return after == nullptr ? nullptr : this->_insert_after_iter(after, container::begin(iterable), container::end(iterable));
    }

    template<class C, type_if<int, !is_constructible_v<T, C const&>, container::foreachable_v<C>, is_constructible_v<T, container::const_reference<C>>> = 0>
    iterator insert_after(const_iterator pos, C const& foreachable) {
        node* after = const_cast<node*>(pos.cur);
        if (after == nullptr) return nullptr;
        _insert_after inserter{ *this, after };
        foreachable.foreach(inserter);
        return inserter.after;
    }

    type_if<iterator, is_constructible_v<T>> insert_after_n_copies(const_iterator pos, size_type n) {
        node* after = const_cast<node*>(pos.cur);
        return after == nullptr ? nullptr : this->_insert_after_n_copies(after, n);
    }

    template<class V = T>
    type_if<iterator, is_constructible_v<T, V const&>> insert_after_n_copies(const_iterator pos, size_type n, V const& value) {
        node* after = const_cast<node*>(pos.cur);
        return after == nullptr ? nullptr : this->_insert_after_n_copies(after, n, value);
    }

    iterator erase_after(const_iterator pos) noexcept {
        if (pos == cend())
            return end();

        node& after = *const_cast<node*>(pos.cur);
        this->_remove_after(after);
        return after.next();
    }

    iterator erase_after(const_iterator first, const_iterator last) noexcept {
        return _erase_after(const_cast<node*>(first.cur), last.cur);
    }

    template<class Arg = const_reference, class... Args>
    type_if<iterator, is_constructible_from_each_v<T, Arg, Args...>> push_front(Arg&& arg, Args&&... args) {
        return this->_insert_after_args(&m_before_first, static_cast<Arg&&>(arg), static_cast<Args&&>(args)...);
    }

    template<class I>
    auto push_front(I first, I last)
        -> type_if<iterator, iterators::fwd_iter_v<I>, is_constructible_v<T, decltype(*first)>> {
        return this->_insert_after_iter(&m_before_first, first, last);
    }

    template<class C, type_if<int, !is_constructible_v<T, C const&>, container::iterable_v<C>, !container::foreachable_v<C>, is_constructible_v<T, container::const_reference<C>>> = 0>
    iterator push_front(C const& iterable) {
        return this->_insert_after_iter(&m_before_first, container::begin(iterable), container::end(iterable));
    }

    template<class C, type_if<int, !is_constructible_v<T, C const&>, container::foreachable_v<C>, is_constructible_v<T, container::const_reference<C>>> = 0>
    iterator push_front(C const& foreachable) {
        _insert_after inserter{ *this, &m_before_first };
        foreachable.foreach(inserter);
        return inserter.after;
    }

    type_if<iterator, is_constructible_v<T>> push_front_n_copies(size_type n) {
        return this->_insert_after_n_copies(&m_before_first, n);
    }

    template<class V = T>
    type_if<iterator, is_constructible_v<T, V const&>> push_front_n_copies(size_type n, V const& value) {
        return this->_insert_after_n_copies(&m_before_first, n, value);
    }

    std::unique_ptr<node> pop_front() noexcept {
        return this->_remove_after(m_before_first);
    }

    std::unique_ptr<node> remove_after(const_iterator pos) noexcept {
        return pos == cend() ? nullptr : this->_remove_after(*const_cast<node*>(pos.cur));
    }

    template<class Pred = std::function<bool(const_reference)>>
    type_if<size_type, convertible_v<util::invoke_result_t<Pred, const_reference>, bool>> remove_if(Pred&& pred) {
        return m_before_first.remove_all_next_if(static_cast<Pred&&>(pred));
    }

    template<class... Args, class Pred = std::function<bool(const_reference, Args...)>>
    type_if<size_type, convertible_v<util::invoke_result_t<Pred, const_reference, Args...>, bool>> remove_if(Pred&& pred, Args&&... args) {
        return m_before_first.remove_all_next_if(static_cast<Pred&&>(pred), static_cast<Args&&>(args)...);
    }

    template<class V = const_reference>
    auto remove(V&& value) noexcept(noexcept(remove_if(std::declval<bool(const_reference)>())))
        -> type_if<decltype(remove_if(std::declval<bool(const_reference)>())), convertible_v<decltype(std::declval<V>() == std::declval<const_reference>()), bool>> {
        return remove_if([&value](const_reference cur)->bool{ return value == cur; });
    }

    void reverse() noexcept { m_before_first._reverse(); }

    _NODISCARD front_linked_list reversed() const {
        front_linked_list reversed;
        for (auto i = cbegin(), end = cend(); i != end; ++i) {
            (void)reversed.push_front(*i);
        }
        return reversed;
    }

    template<class Proc = std::function<void(reference)>, class = util::invoke_result_t<Proc, reference>>
    size_type foreach(Proc&& proc) noexcept(util::nothrow_invocable_v<Proc, reference>) { return iterators::foreach(begin(), end(), static_cast<Proc&&>(proc)); }

    template<class Proc = std::function<void(const_reference)>, class = util::invoke_result_t<Proc, const_reference>>
    size_type foreach(Proc&& proc) const noexcept(util::nothrow_invocable_v<Proc, const_reference>) { return iterators::foreach(cbegin(), cend(), static_cast<Proc&&>(proc)); }

    template<class... Args, class Proc = std::function<void(reference, Args...)>, class = util::invoke_result_t<Proc, reference, Args...>>
    size_type foreach(Proc&& proc, Args&&... args) noexcept(util::nothrow_invocable_v<Proc, reference, Args>) { return iterators::foreach(begin(), end(), static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

    template<class... Args, class Proc = std::function<void(const_reference, Args...)>, class = util::invoke_result_t<Proc, const_reference, Args...>>
    size_type foreach(Proc&& proc, Args&&... args) const { return iterators::foreach(cbegin(), cend(), static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

    template<class Proc = std::function<void(reference)>, class = util::invoke_result_t<Proc, reference>>
    size_type rforeach(Proc&& proc) noexcept(util::nothrow_invocable_v<Proc, reference>) { return iterators::rforeach(begin(), end(), static_cast<Proc&&>(proc)); }

    template<class Proc = std::function<void(const_reference)>, class = util::invoke_result_t<Proc, const_reference>>
    size_type rforeach(Proc&& proc) const noexcept(util::nothrow_invocable_v<Proc, const_reference>) { return iterators::rforeach(cbegin(), cend(), static_cast<Proc&&>(proc)); }

    template<class... Args, class Proc = std::function<void(reference, Args...)>, class = util::invoke_result_t<Proc, reference, Args...>>
    size_type rforeach(Proc&& proc, Args&&... args) noexcept(util::nothrow_invocable_v<Proc, reference, Args>) { return iterators::rforeach(begin(), end(), static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

    template<class... Args, class Proc = std::function<void(const_reference, Args...)>, class = util::invoke_result_t<Proc, const_reference, Args...>>
    size_type rforeach(Proc&& proc, Args&&... args) const noexcept(util::nothrow_invocable_v<Proc, const_reference, Args>) { return iterators::rforeach(cbegin(), cend(), static_cast<Proc&&>(proc), static_cast<Args&&>(args)...); }

protected:
    union {
        node m_before_first;
        char m_[sizeof node]{};
    };

    constexpr node* _first() noexcept { return m_before_first.next(); }

    constexpr node const* _first() const noexcept { return m_before_first.next(); }

    template<class... Args> constexpr node* _emplace_after(node& after, Args&&... args) {
        return after._new_next(static_cast<Args&&>(args)...);
    }

    constexpr std::unique_ptr<node> _remove_after(node& after) {
        return after.unlink_next();
    }

    struct _insert_after {
        template<class V> constexpr void operator()(V&& value) {
            after = _this._emplace_after(*after, static_cast<V&&>(value));
        }
        front_linked_list& _this;
        node* after;
    };

    template<class Arg> node* _insert_after_args(node* after, Arg&& arg) {
        return this->_emplace_after(*after, static_cast<Arg&&>(arg));
    }
    template<class Arg, class... Args> node* _insert_after_args(node* after, Arg&& arg, Args&&... args) {
        after = this->_emplace_after(*after, static_cast<Arg&&>(arg));
        return this->_insert_after_args(after, static_cast<Args&&>(args)...);
    }

    template<class I> inline node* _insert_after_iter(node* after, I first, I last) {
        for (; first != last; ++first) {
            after = this->_emplace_after(*after, *first);
        }
        return after;
    }

    template<class I, class Filter> inline node* _insert_after_iter(node* after, I first, I last, Filter& filter) {
        for (; first != last; ++first) {
            auto& value = *first;
            if (util::invoke(filter, value)) {
                after = this->_emplace_after(*after, value);
            }
        }
        return after;
    }

    template<class... Args> inline node* _insert_after_n_copies(node* after, size_type n, Args&&... args) {
        for (; n != 0; --n) {
            after = this->_emplace_after(*after, static_cast<Args&&>(args)...);
        }
        return after;
    }

    inline static node* _erase_after(node* first, node const* const last) noexcept {
        for (; first != last; first = first->next()) {
            _remove_after(*first);
        }
        return first;
    }
};

#endif // !__FRONT_LINKED_LIST_HPP