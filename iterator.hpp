#ifndef __ITERATOR_HPP
#define __ITERATOR_HPP 1

namespace std
{
	template<class> class function;
}

#include "util.hpp"
#include "object.hpp"
#include <iterator>

template<class...> struct iterator;
template<class...> struct reverse_iterator;

struct complex_iterator_tag {};

template<> struct iterator<> {
	template<class I> using iterator_category = typename std::iterator_traits<remove_ref_t<I>>::iterator_category;
	template<class I> using value_type = typename std::iterator_traits<remove_ref_t<I>>::value_type;
	template<class I> using difference_type = typename std::iterator_traits<remove_ref_t<I>>::difference_type;
	template<class I> using pointer = typename std::iterator_traits<remove_ref_t<I>>::pointer;
	template<class I> using reference = typename std::iterator_traits<remove_ref_t<I>>::reference;

	template<class I> using object = conditional_value<std::is_pointer<I>, iterator<I>, I>;

	template<class I> constexpr _INLINE_VAR static bool nothrow_inc_v = noexcept(++std::declval<I>());

protected:
	template<class I, class Category> static constexpr std::is_convertible<iterator_category<I>, Category> _tagged(int);
	template<class...> static constexpr std::false_type _tagged(...);
	 
public:
	template<class I> struct input_iter : decltype(_tagged<I, std::input_iterator_tag>(0)) {};

	template<class I> constexpr _INLINE_VAR static bool input_iter_v = input_iter<I>::value;

	template<class I> struct fwd_iter : decltype(_tagged<I, std::forward_iterator_tag>(0)) {};

	template<class I> constexpr _INLINE_VAR static bool fwd_iter_v = fwd_iter<I>::value;

	template<class I> struct bidi_iter : decltype(_tagged<I, std::bidirectional_iterator_tag>(0)) {};

	template<class I> constexpr _INLINE_VAR static bool bidi_iter_v = bidi_iter<I>::value;

	template<class I> struct random_iter : decltype(_tagged<I, std::random_access_iterator_tag>(0)) {};

	template<class I> constexpr _INLINE_VAR static bool random_iter_v = random_iter<I>::value;

protected:
	template<class I, bool = random_iter_v<I>> struct _foreach {
		template<class Proc, class... Args>
		constexpr auto operator()(I begin, I end, Proc& proc, Args&&... args) const noexcept(util::nothrow_invocable_v<Proc, decltype(*begin), Args...> && noexcept(*begin))
			-> type_if<size_t, util::invocable_v<Proc, decltype(*begin), Args...>> {
			size_t const res = end - begin;
			for (I i = begin; i != end; ++i) {
				util::invoke(proc, *i, static_cast<Args&&>(args)...);
			}
			return res;
		}
	};
	template<class I> struct _foreach<I, /*random_iter_v=*/false> {
		template<class Proc, class... Args>
		constexpr auto operator()(I begin, I end, Proc& proc, Args&&... args) const noexcept(util::nothrow_invocable_v<Proc, decltype(*begin), Args...> && noexcept(*begin))
			->type_if<size_t, util::invocable_v<Proc, decltype(*begin), Args...>> {
			size_t res = 0;
			for (I i = begin; i != end; ++i, (void)++res) {
				util::invoke(proc, *i, static_cast<Args&&>(args)...);
			}
			return res;
		}
	};

public:
	template<class I, class... Args, class Proc = std::function<void(reference<I>, Args...)>, class Foreach = _foreach<I>>
	constexpr static auto foreach(I begin, I end, Proc&& proc, Args&&... args) noexcept(noexcept(Foreach{}(begin, end, proc, static_cast<Args&&>(args)...)))
		-> decltype(Foreach{}(begin, end, proc, static_cast<Args&&>(args)...)) {
		return Foreach{}(begin, end, proc, static_cast<Args&&>(args)...);
	}

protected:
	template<class I, int = ((int)bidi_iter_v<I>) + ((int)random_iter_v<I>)> struct _rforeach;

	template<class I> struct _rforeach<I, /*bidi_iter_v=false && random_iter_v=false*/ 0> {
		template<class Proc, class... Args>
		constexpr auto operator()(I i, I end, Proc& proc, Args&&... args) const noexcept(util::nothrow_invocable_v<Proc, decltype(*i), Args...> && noexcept(*i))
			-> type_if<size_t, util::invocable_v<Proc, decltype(*i), Args...>> {
			I next = i; ++next;
			size_t const res = next != end ? _rforeach::operator()(next, end, proc, static_cast<Args&&>(args)...) : 0;
			util::invoke(proc, *i, static_cast<Args&&>(args)...);
			return res + 1;
		}
	};
	template<class I> struct _rforeach<I, /*bidi_iter_v=true && random_iter_v=false*/ 1> {
		template<class Proc, class... Args>
		constexpr auto operator()(I begin, I end, Proc& proc, Args&&... args) const noexcept(util::nothrow_invocable_v<Proc, decltype(*begin), Args...> && noexcept(*begin))
			-> type_if<size_t, util::invocable_v<Proc, decltype(*begin), Args...>> {
			size_t res = 0;
			for (I i = end; i != begin; ++res) {
				--i;
				util::invoke(proc, *i, static_cast<Args&&>(args)...);
			}
			return res;
		}
	};
	template<class I> struct _rforeach<I, /*bidi_iter_v=true && random_iter_v=true*/ 2> {
		template<class Proc, class... Args>
		constexpr auto operator()(I begin, I end, Proc& proc, Args&&... args) const noexcept(util::nothrow_invocable_v<Proc, decltype(*begin), Args...> && noexcept(*begin))
			-> type_if<size_t, util::invocable_v<Proc, decltype(*begin), Args...>> {
			size_t const res = end - begin;
			for (I i = end; i != begin; ) {
				--i;
				util::invoke(proc, *i, static_cast<Args&&>(args)...);
			}
			return res;
		}
	};

public:
	template<class I, class... Args, class Proc = std::function<void(reference<I>, Args...)>, class Rforeach = _rforeach<I>>
	constexpr static auto rforeach(I begin, I end, Proc&& proc, Args&&... args) noexcept(noexcept(Rforeach{}(begin, end, proc, static_cast<Args&&>(args)...)))
		-> decltype(Rforeach{}(begin, end, proc, static_cast<Args&&>(args)...)) {
		return Rforeach{}(begin, end, proc, static_cast<Args&&>(args)...);
	}

protected:
	template<class I> struct _find_if {
		template<class Pred, class... Args>
		constexpr auto operator()(I begin, I end, Pred& pred, Args&&... args) const noexcept(util::nothrow_invocable_v<Pred, decltype(*begin), Args...> && noexcept(*begin))
			-> type_if<I, convertible_v<util::invoke_result_t<Pred, decltype(*begin), Args...>, bool>> {
			for (I i = begin; i != end; ++i) {
				if (util::invoke(pred, *i, static_cast<Args&&>(args)...)) {
					return i;
				}
			}
			return end;
		}
	};

public:
	template<class I, class... Args, class Pred = std::function<bool(reference<I>, Args...)>, class Find = _find_if<I>>
	constexpr static auto find_if(I begin, I end, Pred&& pred, Args&&... args) noexcept(noexcept(Find{}(begin, end, pred, static_cast<Args&&>(args)...)))
		-> decltype(Find{}(begin, end, pred, static_cast<Args&&>(args)...)) {
		return Find{}(begin, end, pred, static_cast<Args&&>(args)...);
	}

public:
	template<class Comp = default_comporator, class L, class R, class Ord = util::invoke_result_t<Comp, reference<L>, reference<R>>>
	constexpr static auto compare(L const l_begin, L const l_end, R const r_begin, R const r_end, Comp&& comp = Comp{})
		noexcept(util::nothrow_invocable_v<Comp, decltype(*l_begin), decltype(*r_begin)> && noexcept(*l_begin) && noexcept(*r_begin))
		-> type_if<util::invoke_result_t<Comp, decltype(*l_begin), decltype(*r_begin)>, objects::is_ordering_v<Ord>> {
		auto l_i = l_begin;
		auto r_i = r_begin;
		for (; l_i != l_end && r_i != r_end; ++l_i, (void)++r_i) {
			auto res = util::invoke(comp, *l_i, *r_i);
			if (res != 0) return res;
		}
		if (r_i != r_end) {
			return { -1 };
		}
		if (l_i != l_end) {
			return { 1 };
		}
		return { 0 };
	}
};

using iterators = iterator<>;

template<class T> struct iterator<T*> {
	using value_type = T;
	using reference = T&;
	using const_reference = T const&;
	using pointer = T*;
	using const_pointer = T const*;
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = ptrdiff_t;

	pointer cur;

	iterator() = default;
	iterator(iterator const&) = default;
	iterator& operator=(iterator const&) = default;

	constexpr iterator(T* const p) noexcept : cur(p) {}

	constexpr iterator(nullptr_t) noexcept : cur(nullptr) {}

	constexpr reference operator*() noexcept { return *cur; }
	constexpr const_reference operator*() const noexcept { return *cur; }

	constexpr pointer operator->() noexcept { return cur; }
	constexpr const_pointer operator->() const noexcept { return cur; }

	constexpr iterator operator++(int) const noexcept { iterator temp = cur; ++cur; return temp; }
	constexpr iterator operator--(int) const noexcept { iterator temp = cur; --cur; return temp; }

	constexpr iterator operator+(difference_type dif) const noexcept { return cur + dif; }
	constexpr iterator operator-(difference_type dif) const noexcept { return cur - dif; }

	constexpr iterator& operator++() noexcept { ++cur; return *this; }
	constexpr iterator& operator--() noexcept { --cur; return *this; }

	constexpr iterator& operator+=(difference_type dif) noexcept { cur += dif; return *this; }
	constexpr iterator& operator-=(difference_type dif) noexcept { cur -= dif; return *this; }

	template<class U> constexpr auto operator-(iterator<U*> const& other) const noexcept
		-> type_if<difference_type, convertible_v<decltype(cur - other.cur), difference_type>> {
		return cur - other.cur;
	}

	template<class U> constexpr auto operator==(iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur == other.cur), bool>> {
		return cur == other.cur;
	}
	template<class U> constexpr auto operator!=(iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur != other.cur), bool>> {
		return cur != other.cur;
	}

	template<class U> constexpr auto operator<(iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur < other.cur), bool>> {
		return cur < other.cur;
	}
	template<class U> constexpr auto operator<=(iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur <= other.cur), bool>> {
		return cur <= other.cur;
	}
	template<class U> constexpr auto operator>(iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur > other.cur), bool>> {
		return cur > other.cur;
	}
	template<class U> constexpr auto operator>=(iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur >= other.cur), bool>> {
		return cur >= other.cur;
	}

	template<class Int = type_if<int, !is_const_v<T>>, Int = 0>
	constexpr operator iterator<T const*>() const noexcept { return cur; }
};

template<class T> constexpr iterator<T*> operator+(typename iterator<T*>::difference_type dif, iterator<T*> const& right) noexcept { return right + dif; }
template<class T> constexpr iterator<T*> operator-(typename iterator<T*>::difference_type dif, iterator<T*> const& right) noexcept { return right - dif; }

template<class T> struct reverse_iterator<T*> {
	using value_type = T;
	using reference = T&;
	using const_reference = T const&;
	using pointer = T*;
	using const_pointer = T const*;
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = ptrdiff_t;

	pointer cur;

	reverse_iterator() = default;
	reverse_iterator(reverse_iterator const&) = default;
	reverse_iterator& operator=(reverse_iterator const&) = default;

	constexpr reverse_iterator(T* const p) noexcept : cur(p) {}

	constexpr reverse_iterator(nullptr_t) noexcept : cur(nullptr) {}

	constexpr reference operator*() noexcept { return *cur; }
	constexpr const_reference operator*() const noexcept { return *cur; }

	constexpr pointer operator->() noexcept { return cur; }
	constexpr const_pointer operator->() const noexcept { return cur; }

	constexpr reverse_iterator operator++(int) const noexcept { reverse_iterator temp = cur; --cur; return temp; }
	constexpr reverse_iterator operator--(int) const noexcept { reverse_iterator temp = cur; ++cur; return temp; }

	constexpr reverse_iterator operator+(difference_type dif) const noexcept { return cur - dif; }
	constexpr reverse_iterator operator-(difference_type dif) const noexcept { return cur + dif; }

	constexpr reverse_iterator& operator++() noexcept { --cur; return *this; }
	constexpr reverse_iterator& operator--() noexcept { ++cur; return *this; }

	constexpr reverse_iterator& operator+=(difference_type dif) noexcept { cur -= dif; return *this; }
	constexpr reverse_iterator& operator-=(difference_type dif) noexcept { cur += dif; return *this; }

	template<class U> constexpr auto operator-(reverse_iterator<U*> const& other) const noexcept
		-> type_if<difference_type, convertible_v<decltype(cur - other.cur), difference_type>> {
		return other.cur - cur;
	}

	template<class U> constexpr auto operator==(reverse_iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur == other.cur), bool>> {
		return cur == other.cur;
	}
	template<class U> constexpr auto operator!=(reverse_iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur != other.cur), bool>> {
		return cur != other.cur;
	}

	template<class U> constexpr auto operator<(reverse_iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur > other.cur), bool>> {
		return cur > other.cur;
	}
	template<class U> constexpr auto operator<=(reverse_iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur >= other.cur), bool>> {
		return cur >= other.cur;
	}
	template<class U> constexpr auto operator>(reverse_iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur < other.cur), bool>> {
		return cur < other.cur;
	}
	template<class U> constexpr auto operator>=(reverse_iterator<U*> const& other) const noexcept
		-> type_if<bool, convertible_v<decltype(cur <= other.cur), bool>> {
		return cur <= other.cur;
	}

	template<class Int = type_if<int, !is_const_v<T>>, Int = 0>
	constexpr operator reverse_iterator<T const*>() const noexcept { return cur; }
};

template<class T> constexpr reverse_iterator<T*> operator+(typename reverse_iterator<T*>::difference_type dif, reverse_iterator<T*> const& right) noexcept { return right + dif; }
template<class T> constexpr reverse_iterator<T*> operator-(typename reverse_iterator<T*>::difference_type dif, reverse_iterator<T*> const& right) noexcept { return right - dif; }


#endif // !__ITERATOR_HPP