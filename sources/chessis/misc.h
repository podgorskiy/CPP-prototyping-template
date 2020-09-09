#pragma once
#include "types.h"


namespace chessis
{
	inline int sqr(int x) {return x * x;}

	template<typename C>
	inline typename C::value_type maximum(const C& v)
	{
		typename C::size_type imax = 0;
		auto max = v[0]();
		for (typename C::size_type i = 0, l = v.size(); i < l; ++i)
		{
			auto c = v[i]();
			if (c > max)
			{
				imax = i;
				max = c;
			}
		}
		return v[imax];
	}

	template<typename C>
	inline typename C::size_type argmaximum(const C& v)
	{
		typename C::size_type imax = 0;
		auto max = v[0]();
		for (typename C::size_type i = 0, l = v.size(); i < l; ++i)
		{
			auto c = v[i]();
			if (c > max)
			{
				imax = i;
				max = c;
			}
		}
		return imax;
	}
}
