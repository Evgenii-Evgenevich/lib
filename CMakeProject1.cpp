#include "CMakeProject1.h"
#include "util.hpp"
#include "comporator.hpp"
#include "array.hpp"
#include "tree_node.hpp"
#include "singly_linked_node.hpp"
#include "front_linked_list.hpp"


struct CMakeProject1
{
	CMakeProject1(int)
	{
	}

	//CMakeProject1(CMakeProject1 const&) = delete;

	constexpr static float& sum(float const& e, float& sum_res) noexcept
	{
		return sum_res += e;
	}

	float& _sum(float const& e, float& sum_res) const noexcept
	{
		return sum_res += e;
	}

	void __sum(float& e) noexcept
	{
		_sum_res += e;
	}

	float _sum_res = 0;
};

_INLINE_VAR constexpr auto CMakeProject1_sum = &CMakeProject1::__sum;


int hkk(float f, int i)
{
	return f + i;
}

void println(int const& i)
{
	std::cout << i << std::endl;
}

void gdsfs(CMakeProject1 const& i)
{
}

constexpr bool is_zero(int i) noexcept
{
	return i == 0;
}

class MyClass
{
public:
	MyClass()
	{
		std::cout << "MyClass was constructed\n";
	}

	~MyClass()
	{
		std::cout << "MyClass was destructed\n";
	}

private:
	int i;
};

int main()
{
	int fdf = 4;

	auto sdfsd = [&fdf](int dd) { return fdf += dd; };

	int dsd = sizeof sdfsd;

	{
		array<MyClass> cc(5);

		std::cout << "\ntest\n";
	}

	array<int, 2, 2, 2, 2> a{};
	array<int, 2, 2, 2>& i1 = a.at(0);
	array<int, 2, 2>& i2 = a.at(0, 0);
	array<int, 2>& i3 = a.at(0, 0, 0);
	int& i4 = a.at(0, 0, 0, 0);

	array<int volatile, 1> efsfasf;

	array<int volatile, 1> hdfgdfhts;

	hdfgdfhts = efsfasf;

	efsfasf.assign(5);

	CMakeProject1 m[]{ CMakeProject1(5) };

	decltype(&CMakeProject1::__sum) adasd;

	util::invoke(gdsfs, m[0]);

	auto m0_sum = util::member_ptr(&CMakeProject1::_sum, m[0]);

	auto m0_sum_res = util::member_ptr(&CMakeProject1::_sum_res, (CMakeProject1*)m);

	auto m0__sum = util::member_ptr(&CMakeProject1::__sum, std::ref(m[0]));

	println(m[0]._sum_res);

	(float&)m0_sum_res = 4.f;

	println(m[0]._sum_res);

	array<CMakeProject1, 1> r = m;

	array<float, 3> f(1, 22, 4);

	float sf = 0;
	f.foreach(m0_sum, sf);

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "f";
	std::cout << std::endl;

	std::cout << sf << std::endl
		<< f.size() << std::endl
		<< f[0] << std::endl
		<< f[1] << std::endl
		<< f[2] << std::endl;

	constexpr size_t arr_s = arrays::size<array<int, 5>>;

	tree_node<int, comporator<>> treenode(nullptr, 5);

	constexpr bool c222 = convertible_v<iterator<int const*>, iterator<int*>>;
	constexpr bool c333 = convertible_v<iterator<int*>, iterator<int const*>>;

	//constexpr bool c22 = objects::is_ordering_v<std::strong_ordering>;

	treenode.insert(1);
	treenode.insert(2);
	treenode.insert(3);
	treenode.insert(4);
	treenode.insert(5);
	treenode.insert(5);
	treenode.insert(6);
	treenode.insert(7);
	treenode.insert(8);
	auto f9t = treenode.insert(9);

	decltype(treenode)::remove(f9t.first);

	treenode.find(4);

	treenode.foreach(println);

	front_linked_list<int> fll;

	return 0;
}

