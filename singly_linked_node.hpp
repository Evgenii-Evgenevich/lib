#ifndef __SINGLY_LINKED_NODE_HPP
#define __SINGLY_LINKED_NODE_HPP 1

namespace std
{
	template<class> class function;
}

#include "util.hpp"
#include "object.hpp"
#include "iterator.hpp"
#include "optional.hpp"

template<class T, class Derived> struct singly_linked_node;

template<> struct singly_linked_node<void, void> {};

template<class Derived> struct singly_linked_node<void, Derived> : singly_linked_node<void, void>
{
	using nodeptr = Derived*;
	using const_nodeptr = Derived const*;

	_NODISCARD constexpr nodeptr next() noexcept { return m_next; }

	_NODISCARD constexpr const_nodeptr next() const noexcept { return m_next; }

	_NODISCARD constexpr nodeptr take_next() noexcept {
		nodeptr next = m_next;
		m_next = nullptr;
		return next;
	}

	static std::unique_ptr<Derived> unlink_next(nodeptr node) noexcept {
		return node->unlink_next();
	}

	std::unique_ptr<Derived> unlink_next() noexcept {
		nodeptr next = m_next;
		if (next == nullptr) return nullptr;
		m_next = next->take_next();
		return std::unique_ptr<Derived>(next);
	}

	template<class... Args>
	type_if<nodeptr, is_constructible_v<Derived, nodeptr, Args...>> _new_next(Args&&... args) {
		m_next = new Derived(m_next, static_cast<Args&&>(args)...);
		return m_next;
	}

	void _reverse() noexcept {
		nodeptr cur = m_next;
		if (cur == nullptr) return;
		for (nodeptr prev{}, next{}; ; prev = cur, (void)cur = next)
		{
			next = std::exchange(cur->m_next, prev);
			if (next == nullptr)
			{
				m_next = cur;
				return;
			}
		}
	}

	template<class Pred, class... Args, class V = decltype(std::declval<const_nodeptr>()->value())>
	static type_if<size_t, convertible_v<util::invoke_result_t<Pred, V, Args...>, bool>>
		_remove_if(nodeptr prev, Pred&& pred, Args&&... args) noexcept(util::nothrow_invocable_v<Pred, V, Args...>)
	{
		size_t removed = 0;
		for (nodeptr i = prev->next(); i != nullptr; )
		{
			V value = i->value();
			if (util::invoke(static_cast<Pred&&>(pred), value, static_cast<Args&&>(args)...))
			{
				prev->unlink_next();
				i = prev->next();
				++removed;
			}
			else
			{
				prev = i;
				i = prev->next();
			}
		}
		return removed;
	}

	~singly_linked_node() noexcept {
		delete m_next;
		m_next = nullptr;
	}

	template<class Pred, class... Args, class V = decltype(std::declval<const_nodeptr>()->value())>
	_NODISCARD static type_if<nodeptr, convertible_v<util::invoke_result_t<Pred, V, Args...>, bool>>
		_find_next_returning_prev(/*notnull*/ const_nodeptr prev, Pred&& pred, Args&&... args) noexcept(util::nothrow_invocable_v<Pred, V, Args...>)
	{
		for (const_nodeptr node = prev->next(); node != nullptr; prev = node, node = node->next())
		{
			if (util::invoke(static_cast<Pred&&>(pred), node->value(), static_cast<Args&&>(args)...))
			{
				return const_cast<nodeptr>(prev);
			}
		}
		return nullptr;
	}

	template<class Pred, class... Args, class V = decltype(std::declval<const_nodeptr>()->value())>
	_NODISCARD static type_if<nodeptr, convertible_v<util::invoke_result_t<Pred, V, Args...>, bool>>
		_find_if(const_nodeptr node, Pred&& pred, Args&&... args) noexcept(util::nothrow_invocable_v<Pred, V, Args...>)
	{
		for (; node != nullptr; node = node->next())
		{
			if (util::invoke(static_cast<Pred&&>(pred), node->value(), static_cast<Args&&>(args)...))
			{
				return const_cast<nodeptr>(node);
			}
		}
		return nullptr;
	}

	template<class... Args, class Pred = std::function<bool(decltype(std::declval<const_nodeptr>()->value()), Args...)>>
	_NODISCARD static auto find_if(const_nodeptr node, Pred&& pred, Args&&... args) noexcept(noexcept(_find_if(node, static_cast<Pred&&>(pred), static_cast<Args&&>(args)...)))
		-> decltype(_find_if(node, static_cast<Pred&&>(pred), static_cast<Args&&>(args)...)) {
		return _find_if(node, static_cast<Pred&&>(pred), static_cast<Args&&>(args)...);
	}

	template<class Pred = std::function<bool(decltype(std::declval<const_nodeptr>()->value()))>>
	_NODISCARD static auto find_if(const_nodeptr node, Pred&& pred) noexcept(noexcept(_find_if(node, static_cast<Pred&&>(pred))))
		-> decltype(_find_if(node, static_cast<Pred&&>(pred))) {
		return _find_if(node, static_cast<Pred&&>(pred));
	}

protected:
	nodeptr m_next;

	singly_linked_node(singly_linked_node const&) = delete;
	singly_linked_node& operator=(singly_linked_node const&) = delete;

	singly_linked_node(singly_linked_node&&) = delete;
	singly_linked_node& operator=(singly_linked_node&&) = delete;

	constexpr singly_linked_node() noexcept : m_next(nullptr) {}

	constexpr singly_linked_node(nodeptr const next) noexcept : m_next(next) {}

	template<class, class> friend struct singly_linked_node;
};


template<class T, class Derived = void> struct singly_linked_node :
	protected object<T>, public singly_linked_node<void, conditional<!is_same_v<void, Derived>, Derived, singly_linked_node<T>>>
{
private:
	using _node = conditional<!is_same_v<void, Derived>, Derived, singly_linked_node>;
	using _base = singly_linked_node<void, _node>;

public:
	using value_type = T;
	using nodeptr = _node*;
	using const_nodeptr = _node const*;

	constexpr singly_linked_node() noexcept(is_nothrow_constructible_v<T>)
		: object<T>{}, _base() {
	}

	template<class... Args, type_if<int, is_constructible_v<T, Args...>> = 0>
	constexpr singly_linked_node(nodeptr const next, Args&&... args) noexcept(is_nothrow_constructible_v<T, Args...>)
		: object<T>{ static_cast<Args&&>(args)... }, _base(next){
	}

	template<class V>
	constexpr type_if<T&, is_assignable_v<T&, V&&>> assign(V&& rigth) noexcept(is_nothrow_assignable_v<T&, V&&>) {
		return this->value() = static_cast<V&&>(rigth);
	}

	template<class V>
	type_if<T&, !is_const_v<T>, !is_assignable_v<T&, V&&>, is_constructible_v<_node, nodeptr, V&&>> assign(V&& rigth) noexcept(is_nothrow_constructible_v<_node, nodeptr, V&&>) {
		objects::destroy(this->value());
		new(this) _node(m_next, static_cast<V&&>(rigth));
		return this->value();
	}

	_NODISCARD constexpr T& value() & noexcept { return static_cast<T&>(*this); }

	_NODISCARD constexpr T const& value() const& noexcept { return static_cast<T const&>(*this); }

	_NODISCARD constexpr T&& value() && noexcept { return static_cast<T&&>(*this); }

	_NODISCARD constexpr T const&& value() const&& noexcept { return static_cast<T const&&>(*this); }

	template<class Pred = std::function<bool(T const&)>>
	auto remove_all_next_if(Pred&& pred)
		-> decltype(_base::_remove_if(this, static_cast<Pred&&>(pred))) {
		return _base::_remove_if(this, static_cast<Pred&&>(pred));
	}

	template<class... Args, class Pred = std::function<bool(T const&, Args...)>>
	auto remove_all_next_if(Pred&& pred, Args&&... args)
		-> decltype(_base::_remove_if(this, static_cast<Pred&&>(pred), static_cast<Args&&>(args)...)) {
		return _base::_remove_if(this, static_cast<Pred&&>(pred), static_cast<Args&&>(args)...);
	}

protected:
	using _base::m_next;

	singly_linked_node(singly_linked_node const&) = delete;
	singly_linked_node& operator=(singly_linked_node const&) = delete;

	constexpr singly_linked_node(singly_linked_node&& o) noexcept(std::is_nothrow_constructible_v<T, T&&>)
		: object<T>(static_cast<T&&>(o.value())), _base(std::exchange(o.m_next, nullptr)) {
	}

	singly_linked_node& operator=(singly_linked_node&&) = delete;

	template<class, class> friend struct singly_linked_node;
	template<class...> friend struct optional;
};

template<class Node> struct iterator<singly_linked_node<void, Node> const>
{
	using value_type = typename Node::value_type;
	using const_reference = value_type const&;
	using reference = const_reference;
	using const_pointer = value_type const*;
	using pointer = const_pointer;
	using iterator_category = std::forward_iterator_tag;
	using difference_type = ptrdiff_t;

	Node const* cur;

	iterator() = default;
	iterator(iterator const&) = default;
	iterator& operator=(iterator const&) = default;

	constexpr iterator(Node const* const p) : cur(p) {}

	constexpr iterator(nullptr_t) : cur(nullptr) {}

	constexpr iterator& operator++() noexcept { cur = cur->next(); return *this; }

	_NODISCARD constexpr iterator operator++(int) const noexcept { iterator temp = cur; cur = cur->next(); return temp; }

	_NODISCARD constexpr const_reference operator*() const noexcept { return cur->value(); }

	_NODISCARD constexpr const_pointer operator->() const noexcept { return &cur->value(); }

	template<class U> _NODISCARD constexpr auto operator==(iterator<U> const& other) const noexcept
		-> type_if<bool, is_base_of_v<singly_linked_node<void, void>, U>, convertible_v<decltype(cur == other.cur), bool>> {
		return cur == other.cur;
	}
	template<class U> _NODISCARD constexpr auto operator!=(iterator<U> const& other) const noexcept
		-> type_if<bool, is_base_of_v<singly_linked_node<void, void>, U>, convertible_v<decltype(cur != other.cur), bool>> {
		return cur != other.cur;
	}
};

template<class Node> struct iterator<singly_linked_node<void, Node>>
{
	using value_type = typename Node::value_type;
	using reference = value_type&;
	using const_reference = value_type const&;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	using iterator_category = std::forward_iterator_tag;
	using difference_type = ptrdiff_t;

	Node* cur;

	iterator() = default;
	iterator(iterator const&) = default;
	iterator& operator=(iterator const&) = default;

	constexpr iterator(Node* const p) : cur(p) {}

	constexpr iterator(nullptr_t) : cur(nullptr) {}

	constexpr iterator& operator++() noexcept { cur = cur->next(); return *this; }
	_NODISCARD constexpr iterator operator++(int) const noexcept { iterator temp = cur; cur = cur->next(); return temp; }

	_NODISCARD constexpr reference operator*() noexcept { return cur->value(); }
	_NODISCARD constexpr const_reference operator*() const noexcept { return cur->value(); }

	_NODISCARD constexpr pointer operator->() noexcept { return &cur->value(); }
	_NODISCARD constexpr const_pointer operator->() const noexcept { return &cur->value(); }

	template<class U> _NODISCARD constexpr auto operator==(iterator<U> const& other) const noexcept
		-> type_if<bool, is_base_of_v<singly_linked_node<void, void>, U>, convertible_v<decltype(cur == other.cur), bool>> {
		return cur == other.cur;
	}
	template<class U> _NODISCARD constexpr auto operator!=(iterator<U> const& other) const noexcept
		-> type_if<bool, is_base_of_v<singly_linked_node<void, void>, U>, convertible_v<decltype(cur != other.cur), bool>> {
		return cur != other.cur;
	}

	_NODISCARD constexpr operator iterator<singly_linked_node<void, Node> const>() const noexcept { return cur; }
};


#endif // !__SINGLY_LINKED_NODE_HPP