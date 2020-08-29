#pragma once
#include <stdint.h>


namespace misc
{
	template <typename X>
	inline X* align(X* p, int bitCount)
	{
		auto x = reinterpret_cast<uintptr_t>(p);
		uintptr_t mask = bitCount - 1;
		return reinterpret_cast<X*>(~mask & (x + mask));
	}

	template <typename X>
	inline X align(X x, int bitCount)
	{
		X mask = bitCount - 1;
		return (~mask & (x + mask));
	}
}
