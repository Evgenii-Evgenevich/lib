#include "CMakeProject1.h"
#include "util.hpp"


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


int main()
{
	int fdf = 4;

	//array<CMakeProject1, 6> d;

	auto sdfsd = [&fdf](int dd) { return fdf += dd; };

	int dsd = sizeof sdfsd;

	CMakeProject1 m[]{ CMakeProject1(5) };

	decltype(&CMakeProject1::__sum) adasd;

	util::invoke(gdsfs, m[0]);

	auto m0_sum = util::member_ptr(&CMakeProject1::_sum, m[0]);

	auto m0_sum_res = util::member_ptr(&CMakeProject1::_sum_res, (CMakeProject1*)m);

	auto m0__sum = util::member_ptr(&CMakeProject1::__sum, std::ref(m[0]));

	println(m[0]._sum_res);

	(float&)m0_sum_res = 4.f;

	println(m[0]._sum_res);

	return 0;
}

