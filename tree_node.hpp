#ifndef __TREE_NODE_HPP
#define __TREE_NODE_HPP 1

namespace std
{
	template<class> class function;
}

#include "util.hpp"
#include "object.hpp"
#include "iterator.hpp"
#include "comporator.hpp"
#include "pair.hpp"
#include <memory>

template<class T, class Comparator, class Derived> struct tree_node;

template<> struct tree_node<void, void, void>
{
	template<class Nodeptr> static auto minimum(Nodeptr minimum) noexcept
		-> type_if<Nodeptr, convertible_v<decltype(minimum->left()), Nodeptr>, convertible_v<decltype(minimum == nullptr), bool>, convertible_v<nullptr_t, Nodeptr>, convertible_v<decltype(minimum->left() != nullptr), bool>>
	{
		if (minimum == nullptr)
			return nullptr;
		while (minimum->left() != nullptr) {
			minimum = minimum->left();
		}
		return minimum;
	}

	template<class Nodeptr> static auto maximum(Nodeptr maximum) noexcept
		-> type_if<Nodeptr, convertible_v<decltype(maximum->right()), Nodeptr>, convertible_v<decltype(maximum == nullptr), bool>, convertible_v<nullptr_t, Nodeptr>, convertible_v<decltype(maximum->right() != nullptr), bool>>
	{
		if (maximum == nullptr)
			return nullptr;
		while (maximum->right() != nullptr) {
			maximum = maximum->right();
		}
		return maximum;
	}

	template<class Nodeptr> static Nodeptr _leftrotate(Nodeptr _this) noexcept
	{
		Nodeptr node = _this->m_right;
		if (node == nullptr)
			return nullptr;

		if (node->m_left) {
			_this->m_right = node->m_left;
			_this->m_right->m_parent = _this;
		} else {
			_this->m_right = nullptr;
		}

		if (_this->m_parent) _this->_subling() = node;
		node->m_parent = _this->m_parent;
		_this->m_parent = node;
		node->m_left = _this;
		return node;
	}

	template<class Nodeptr> static Nodeptr _rightrotate(Nodeptr _this) noexcept
	{
		Nodeptr node = _this->m_left;
		if (node == nullptr)
			return nullptr;

		if (node->m_right) {
			_this->m_left = node->m_right;
			_this->m_left->m_parent = _this;
		} else {
			_this->m_left = nullptr;
		}

		if (_this->m_parent) _this->_subling() = node;
		node->m_parent = _this->m_parent;
		_this->m_parent = node;
		node->m_right = _this;
		return node;
	}

protected:
	tree_node() = default;

	tree_node(tree_node const&) = delete;
	tree_node& operator=(tree_node const&) = delete;

	tree_node(tree_node&&) = delete;
	tree_node& operator=(tree_node&&) = delete;

};

template<class Derived> struct tree_node<void, void, Derived> : tree_node<void, void, void>
{
	using _base = tree_node<void, void, void>;

	using nodeptr = Derived*;
	using const_nodeptr = Derived const*;

	constexpr tree_node(nodeptr parent) noexcept
		: m_parent(parent), m_left(nullptr), m_right(nullptr) {
	}

	~tree_node() noexcept {
		if (m_right) {
			m_right->m_parent = nullptr;
			delete m_right;
			m_right = nullptr;
		}
		if (m_left) {
			m_left->m_parent = nullptr;
			delete m_left;
			m_left = nullptr;
		}
		if (m_parent) {
			if (m_parent->m_left == this) {
				m_parent->m_left = nullptr;
			} else if (m_parent->m_right == this) {
				m_parent->m_right = nullptr;
			}
		}
	}

	constexpr nodeptr parent() noexcept { return m_parent; }

	constexpr const_nodeptr parent() const noexcept { return m_parent; }

	constexpr nodeptr left() noexcept { return m_left; }

	constexpr const_nodeptr left() const noexcept { return m_left; }

	constexpr nodeptr right() noexcept { return m_right; }

	constexpr const_nodeptr right() const noexcept { return m_right; }

	static pair<std::unique_ptr<Derived>, int> remove(Derived* node) noexcept {
		if (node == nullptr)
			return { std::unique_ptr<Derived>(), 0 };

		using T = typename Derived::value_type;

		if (nodeptr successor = node->m_right) {
			successor = _base::minimum(successor);
			objects::swap_bytes(static_cast<T&>(*node), static_cast<T&>(*successor));
			return remove(successor);
		}
		if (nodeptr predeccessor = node->m_left) {
			predeccessor = _base::maximum(predeccessor);
			objects::swap_bytes(static_cast<T&>(*node), static_cast<T&>(*predeccessor));
			return remove(predeccessor);
		}
		if (nodeptr parent = node->m_parent) {
			if (parent->m_left == node) {
				parent->m_left = nullptr;
				return { std::unique_ptr<Derived>(node), -1 };
			} else {
				parent->m_right = nullptr;
				return { std::unique_ptr<Derived>(node), 1 };
			}
		}
		return { std::unique_ptr<Derived>(node), 0 };
	}

protected:
	nodeptr m_parent;
	nodeptr m_left;
	nodeptr m_right;

	constexpr nodeptr& _subling() noexcept
	{
		return m_parent->m_left == this ? m_parent->m_right : m_parent->m_left;
	}

	template<class, class, class> friend struct tree_node;
};

template<class T, class Comparator = default_comporator, class Derived = void>
struct tree_node
	: protected object<Comparator>
	, protected object<T>
	, public tree_node<void, void, conditional<!is_same_v<void, Derived>, Derived, tree_node<T, Comparator>>>
{
private:
	using _node = conditional<!is_same_v<void, Derived>, Derived, tree_node>;
	using _base = tree_node<void, void, _node>;

public:
	using value_type = T;
	using comporator_t = object<Comparator>;
	using nodeptr = typename _base::nodeptr;
	using const_nodeptr = typename _base::const_nodeptr;

	template<class U> constexpr _INLINE_VAR static bool comporable_v = objects::is_ordering_v<util::invoke_result_t<Comparator, U, T const&>>;
	template<class U> constexpr _INLINE_VAR static bool const_comporable_v = objects::is_ordering_v<util::invoke_result_t<Comparator const, U, T const&>>;
	template<class U> constexpr _INLINE_VAR static bool nothrow_comporable_v = util::nothrow_invocable_v<Comparator, U, T const&>;
	template<class U> constexpr _INLINE_VAR static bool nothrow_const_comporable_v = util::nothrow_invocable_v<Comparator const, U, T const&>;

	static_assert(comporable_v<T>, "not comporable");
	static_assert(is_constructible_v<comporator_t, Comparator const&>, "not comporable");

	template<class V, type_if<int, is_constructible_v<T, V&&>, objects::is_class_v<Comparator>, is_constructible_v<comporator_t>> = 0>
	constexpr tree_node(nodeptr const parent, V&& value) noexcept(is_nothrow_constructible_v<comporator_t>&& is_nothrow_constructible_v<T, V&&>)
		: comporator_t(), object<T>{ static_cast<V&&>(value) }, _base(parent) {
	}

	template<class V, type_if<int, is_constructible_v<T, V&&>> = 0>
	constexpr tree_node(Comparator const& cmp, nodeptr const parent, V&& value) noexcept(is_nothrow_constructible_v<comporator_t, Comparator const&>&& is_nothrow_constructible_v<T, V&&>)
		: comporator_t{ cmp }, object<T>{ static_cast<V&&>(value) }, _base(parent) {
	}

protected:
	template<class V = T const&>
	static type_if<pair<nodeptr, bool>, is_constructible_v<_node, Comparator const&, nodeptr, V&&>> _insert(nodeptr& node, nodeptr parent, V&& value)
	{
		if (node)
			return insert(node, static_cast<V&&>(value));

		node = new _node(static_cast<Comparator const&>(*parent), parent, static_cast<V&&>(value));
		return { node, true };
	}

public:
	/**
	* @param [notnull] node
	* @param [] value
	*
	* @returns the pointer to the node
	*/
	template<class V = T const&>
	static auto insert(nodeptr node, V&& value)
		-> decltype(_insert(std::declval<nodeptr&>(), node, static_cast<V&&>(value)))
	{
		auto const c = node->lcompare(static_cast<V&&>(value));

		if (c == 0)
			return { node, false };

		return _insert(c < 0 ? node->m_left : node->m_right, node, static_cast<V&&>(value));
	}

	template<class V = T const&>
	static type_if<nodeptr, comporable_v<V>> find(nodeptr node, V&& value) noexcept(nothrow_comporable_v<V>) {
		if (node == nullptr)
			return nullptr;

		auto const c = node->lcompare(static_cast<V&&>(value));

		if (c == 0)
			return node;

		return find(c < 0 ? node->m_left : node->m_right, static_cast<V&&>(value));
	}

	template<class V = T const&>
	static type_if<nodeptr, const_comporable_v<V>> find(const_nodeptr node, V&& value) noexcept(nothrow_const_comporable_v<V>) {
		if (node == nullptr)
			return nullptr;

		auto const c = node->lcompare(static_cast<V&&>(value));

		if (c == 0)
			return const_cast<nodeptr>(node);

		return find(c < 0 ? node->m_left : node->m_right, static_cast<V&&>(value));
	}

	template<class V = T const&>
	constexpr util::invoke_result_t<Comparator, V&&, T const&> lcompare(V&& left) noexcept(nothrow_comporable_v<V>) {
		return util::invoke(static_cast<Comparator&>(*this), static_cast<V&&>(left), value());
	}

	template<class V = T const&>
	constexpr util::invoke_result_t<Comparator const, V&&, T const&> lcompare(V&& left) const noexcept(nothrow_const_comporable_v<V>) {
		return util::invoke(static_cast<Comparator const&>(*this), static_cast<V&&>(left), value());
	}

	constexpr T const& value() const& noexcept { return static_cast<T const&>(*this); }

	constexpr T const&& value() const&& noexcept { return static_cast<T const&&>(*this); }

	template<class Proc = std::function<void(T const&)>, class = util::invoke_result_t<Proc, T const&>>
	size_t foreach(Proc&& proc) const noexcept(util::nothrow_invocable_v<Proc, T const&>) {
		size_t res = 1;
		if (m_left) res += m_left->foreach(static_cast<Proc&&>(proc));
		util::invoke(static_cast<Proc&&>(proc), value());
		if (m_right) res += m_right->foreach(static_cast<Proc&&>(proc));
		return res;
	}

	template<class... Args, class Proc = std::function<void(T const&, Args...)>, class = util::invoke_result_t<Proc, T const&, Args...>>
	size_t foreach(Proc&& proc, Args&&... args) const noexcept(util::nothrow_invocable_v<Proc, T const&, Args...>) {
		size_t res = 1;
		if (m_left) res += m_left->foreach(static_cast<Proc&&>(proc), static_cast<Args&&>(args)...);
		util::invoke(static_cast<Proc&&>(proc), value(), static_cast<Args&&>(args)...);
		if (m_right) res += m_right->foreach(static_cast<Proc&&>(proc), static_cast<Args&&>(args)...);
		return res;
	}

	template<class V = T const&> auto find(V&& value) noexcept(noexcept(find(this, static_cast<V&&>(value))))
		-> decltype(find(this, static_cast<V&&>(value))) {
		return find(this, static_cast<V&&>(value));
	}

	template<class V = T const&> auto find(V&& value) const noexcept(noexcept(find(this, static_cast<V&&>(value))))
		-> decltype(find(this, static_cast<V&&>(value))) {
		return find(this, static_cast<V&&>(value));
	}

	template<class V = T const&> auto insert(V&& value)
		-> decltype(insert(this, static_cast<V&&>(value))) {
		return insert(this, static_cast<V&&>(value));
	}

protected:
	using _base::m_parent;
	using _base::m_left;
	using _base::m_right;

	template<class, class, class> friend struct tree_node;
};

template<class T, class Derived>
struct tree_node<T, void, Derived>
	: protected object<T>
	, public tree_node<void, void, conditional<!is_same_v<void, Derived>, Derived, tree_node<T, void, Derived>>>
{
private:
	using _node = conditional<!is_same_v<void, Derived>, Derived, tree_node>;
	using _base = tree_node<void, void, _node>;

public:
	using value_type = T;
	using nodeptr = typename _base::nodeptr;
	using const_nodeptr = typename _base::const_nodeptr;

	template<class U, type_if<int, is_constructible_v<T, U&&>> = 0>
	constexpr tree_node(nodeptr const parent, U&& value) noexcept(is_nothrow_constructible_v<T, U&&>)
		: object<T>{ static_cast<U&&>(value) }, _base(parent) {
	}

	/**
	* @param [ref] node - the reference of a nullable pointer to a node
	* @param [] value
	* @param [] parent - the nullable pointer to parent node of the new node
	* @param [ref] comp
	*
	* @returns the pointer to the node
	*/
	template<class Comp = default_comporator, class V = T const&>
	static type_if<pair<nodeptr, bool>, is_constructible_v<_node, nodeptr, V&&>, objects::is_ordering_v<util::invoke_result_t<Comp, V&&, T const&>>>
		_insert(nodeptr& node, V&& value, nodeptr parent, Comp&& comp = Comp{})
	{
		if (node)
			return insert(node, static_cast<V&&>(value), static_cast<Comp&&>(comp));

		node = new _node(parent, static_cast<V&&>(value));
		return { node, true };
	}

	/**
	* @param [notnull] node
	* @param [] value
	* @param [ref] comp
	*
	* @returns the pointer to the node
	*/
	template<class Comp = default_comporator, class V = T const&>
	static auto insert(nodeptr node, V&& value, Comp&& comp = Comp{})
		-> decltype(_insert(std::declval<nodeptr&>(), static_cast<V&&>(value), node, static_cast<Comp&&>(comp)))
	{
		auto const c = util::invoke(static_cast<Comp&&>(comp), static_cast<V&&>(value), static_cast<const_nodeptr>(node)->value());

		if (c == 0)
			return { node, false };

		return _insert(c < 0 ? node->m_left : node->m_right, static_cast<V&&>(value), node, static_cast<Comp&&>(comp));
	}

	template<class Comp = default_comporator, class V = T const&>
	static type_if<nodeptr, objects::is_ordering_v<util::invoke_result_t<Comp, V&&, T const&>>>
		find(const_nodeptr node, V&& value, Comp&& comp = Comp{}) noexcept(util::nothrow_invocable_v<Comp, V, T>)
	{
		if (node == nullptr)
			return nullptr;

		auto const c = util::invoke(static_cast<Comp&&>(comp), static_cast<V&&>(value), static_cast<const_nodeptr>(node)->value());

		if (c == 0)
			return const_cast<nodeptr>(node);

		return find(c < 0 ? node->m_left : node->m_right, static_cast<V&&>(value), static_cast<Comp&&>(comp));
	}

	constexpr T& value() & noexcept { return static_cast<T&>(*this); }

	constexpr T const& value() const& noexcept { return static_cast<T const&>(*this); }

	constexpr T&& value() && noexcept { return static_cast<T&&>(*this); }

	constexpr T const&& value() const&& noexcept { return static_cast<T const&&>(*this); }

	template<class Proc = std::function<void(T const&)>, class = util::invoke_result_t<Proc, T const&>>
	size_t foreach(Proc&& proc) const noexcept(util::nothrow_invocable_v<Proc, T const&>) {
		size_t res = 1;
		if (m_left) res += m_left->foreach(static_cast<Proc&&>(proc));
		util::invoke(static_cast<Proc&&>(proc), value());
		if (m_right) res += m_right->foreach(static_cast<Proc&&>(proc));
		return res;
	}

	template<class... Args, class Proc = std::function<void(T const&, Args...)>, class = util::invoke_result_t<Proc, T const&, Args...>>
	size_t foreach(Proc&& proc, Args&&... args) const noexcept(util::nothrow_invocable_v<Proc, T const&, Args...>) {
		size_t res = 1;
		if (m_left) res += m_left->foreach(static_cast<Proc&&>(proc), static_cast<Args&&>(args)...);
		util::invoke(static_cast<Proc&&>(proc), value(), static_cast<Args&&>(args)...);
		if (m_right) res += m_right->foreach(static_cast<Proc&&>(proc), static_cast<Args&&>(args)...);
		return res;
	}

	template<class Comp = default_comporator, class V = T const&>
	auto find(V&& value, Comp&& comp = Comp{}) const
		-> decltype(find(this, static_cast<V&&>(value), static_cast<Comp&&>(comp))) {
		return find(this, static_cast<V&&>(value), static_cast<Comp&&>(comp));
	}

	template<class Comp = default_comporator, class V = T const&>
	auto insert(V&& value, Comp&& comp = Comp{})
		-> decltype(insert(this, static_cast<V&&>(value), static_cast<Comp&&>(comp))) {
		return insert(this, static_cast<V&&>(value), static_cast<Comp&&>(comp));
	}

protected:
	using _base::m_parent;
	using _base::m_left;
	using _base::m_right;

	template<class, class, class> friend struct tree_node;
};


#endif // !__TREE_NODE_HPP