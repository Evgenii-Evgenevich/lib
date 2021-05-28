#include "CMakeProject1.h"
#include "util.hpp"
#include "pair.hpp"
#include "comporator.hpp"
#include "array.hpp"
#include "tree_node.hpp"
#include "singly_linked_node.hpp"
#include "front_linked_list.hpp"
#include <forward_list>
#include <set>
#include <utility>
#include <array>
#include <vector>
#include <tuple>


struct CMakeProject1
{
	CMakeProject1(int) {
	}

	//CMakeProject1(CMakeProject1 const&) = delete;

	constexpr static float pow(float num, unsigned n) noexcept {
		return n == 0 ? 1 : (n == 1 ? num : (num * pow(num, n - 1)));
	}

	constexpr static bool is_zero(int i) noexcept {
		return i == 0;
	}

	constexpr static float& sum(float const& e, float& sum_res) noexcept {
		return sum_res += e;
	}

	float& _sum(float const& e, float& sum_res) const noexcept {
		return sum_res += e;
	}

	void __sum(float& e) noexcept {
		_sum_res += e;
	}

	float _sum_res = 0;
};

_INLINE_VAR constexpr auto CMakeProject1_sum = &CMakeProject1::__sum;

struct _println
{
	template<class V> constexpr auto operator()(V const& v) const noexcept(noexcept(std::cout << v << std::endl))
		-> decltype(std::cout << v << std::endl) {
		return std::cout << v << std::endl;
	}
};

constexpr _INLINE_VAR _println println{};

void gdsfs(CMakeProject1 const& i)
{
}

class MyClass
{
public:
	MyClass() {
		std::cout << "MyClass was constructed\n";
	}

	~MyClass() {
		std::cout << "MyClass was destructed\n";
	}

private:
	int i = 13;
};

struct MyStruct : pair<MyClass const, MyClass> {
	using pair<MyClass const, MyClass>::first;
	using pair<MyClass const, MyClass>::second;

	MyStruct() : pair<MyClass const, MyClass>(pair<void, void>::dummy) {
		new(unconst(&first)) MyClass();
	}
};


int main()
{

	{
		is_constructible_v<pair<int, float>, array<int, 2>>;
		auto ar10 = std::array<float, 10>{11, 22, 33, 44, 55, 66, 77, 88, 99};
		arrays::from<1, 2, 4, 5, 7>(ar10).foreach(println);
	}

	is_constructible_v<std::pair<int, std::vector<float>>, pair<int, std::vector<float>>>;

	{
		auto q123 = { 1,2,3 };

		container::foreach(q123, println);
	}

	pair<int, int> pipi{ pair<float, int>{ pair<int, float>{ array<int, 2>{ pair<int, int>{ std::tuple<double, float>(5, 4) } } } } };

	arrays::from(pipi);

	is_constructible_from_each_tuple_element<int, std::array<float, 1>>::value;
	
	std::get<0>(pipi);

	auto& rpipi = (pipi = pair<float, int>{ 3, 4 });

	MyStruct pp;

	pair<int, std::vector<float>> pivf{ std::forward_as_tuple(1), std::forward_as_tuple(1), std::index_sequence<0>{}, std::index_sequence<0>{} };

	//int fff = std::forward_as_tuple(1);

	std::cout << std::endl;
	std::cout << " :0: " << ((int*)(&pp))[0];
	std::cout << " :1: " << ((int*)(&pp))[1];
	std::cout << std::endl;
	std::cout << std::endl;

	optional<int> oi;

	auto oico = oi.compare(4);
	//std::cout << oico;
	std::cout << std::endl;

	bool bbb;

	new(&bbb) bool(CMakeProject1::is_zero(1));

	int i = 1;

	i = (oi.or_default(1) = 3);

	std::cout << "i " << i;
	std::cout << std::endl;
	std::cout << std::endl;


	array<int, 6> fffff({ 1,2,3,4,5,6 });

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

	container::foreach(m, gdsfs);

	auto m0_sum = util::member_ptr(&CMakeProject1::_sum, m[0]);

	auto m0_sum_res = util::member_ptr(&CMakeProject1::_sum_res, (CMakeProject1*)m);

	auto m0__sum = util::member_ptr(&CMakeProject1::__sum, std::ref(m[0]));

	println(m[0]._sum_res);

	(float&)m0_sum_res = 4.f;

	println(m[0]._sum_res);

	array<CMakeProject1, 1> r = m;

	array<float, 3> f(1, 22, 4);

	float sf = 0;
	container::rforeach(f, m0_sum, sf);

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "f";
	std::cout << std::endl;

	std::cout << sf << std::endl
		<< f.size() << std::endl
		<< f[0] << std::endl
		<< f[1] << std::endl
		<< f[2] << std::endl;

	array<float> ff(f.begin(), f.end());

	std::vector<float> v{ 2,5,6,8,9 };

	container::push_back(13, v);

	iterators::find_if(f.begin(), f.end(), CMakeProject1::is_zero);

	auto fff = arrays::map(f, CMakeProject1::pow, 4);

	//auto fco = ff <=> ff;

	constexpr size_t arr_s = arrays::size<array<int, 5>>;

	tree_node<int, comporator<>> treenode(nullptr, 5);

	constexpr bool c222 = convertible_v<iterator<int const*>, iterator<int*>>;
	constexpr bool c333 = convertible_v<iterator<int*>&, iterator<int const*>&&>;

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

	decltype(treenode)::remove(treenode.find(1));
	decltype(treenode)::remove(&treenode);

	std::set<int> set;

	treenode.foreach(container::inserter(set));

	container::insert2(set, f);

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "set";
	std::cout << std::endl;

	container::foreach(set, println);

	std::forward_list<int> front{};

	treenode.foreach(container::after_inserter(front, front.before_begin()));

	auto f_insert_after = container::insert_after2(front, front.before_begin(), f);

	f_insert_after = container::insert_after2(front, f_insert_after, sf);

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "front";
	std::cout << std::endl;

	container::foreach(front, println);

	front_linked_list<int> fll = front;

	sizeof singly_linked_node<void, void>;

	fll.remove_if(CMakeProject1::is_zero);

	//front_linked_list<int>::node::find_if(fll.pop_front().get(), [](int) {return true; });

	return 0;
}

