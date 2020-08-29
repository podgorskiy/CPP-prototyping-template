#pragma once
#include "aabb.h"
#include "glm_printers.h"
#include <glm/glm.hpp>

namespace glm
{
	template <int Dim, typename Type>
	inline Type sum(vec<Dim, Type> x)
	{
		Type s = Type(0);
		for (int i = 0; i < Dim; ++i)
			s += x[i];
		return s;
	}

	template <int Dim, typename Type>
	inline Type mean(vec<Dim, Type> x)
	{
		return sum(x) / Dim;
	}
}
