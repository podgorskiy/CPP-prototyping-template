#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <tuple>


namespace vec2d
{
	//get line equation coefficients
	template<typename T>
	inline void GetLine(T& a, T& b, T& c, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2)
	{
		a = p1.y - p2.y;
		b = p2.x - p1.x;
		c = p1.x * p2.y - p2.x * p1.y;
	}

	//get line equation coefficients
	template<typename T>
	inline std::tuple<T, T, T> GetLine(const glm::vec<2, T>& p1, const glm::vec<2, T>& p2)
	{
		T a = p1.y - p2.y;
		T b = p2.x - p1.x;
		T c = p1.x * p2.y - p2.x * p1.y;
		return std::tuple<T, T, T>(a, b, c);
	}

	//get cross point on line to a point
	template<typename T>
	inline T GetClosestPoint(T a, T b, T c, const glm::vec<2, T>& p)
	{
		T c1 = b * p.x - a * p.y;
		T det = a * a + b * b;
		return glm::vec<2, T>(-(a * c - b * c1) / det, -(a * c1 + b * c) / det);
	}

	//get cross point of segment to a point
	template<typename T, int Dim>
	inline glm::vec<Dim, T>
	GetClosestPoint(const glm::vec<Dim, T>& a, const glm::vec<Dim, T>& b, const glm::vec<Dim, T>& p)
	{
		glm::vec<Dim, T> ap = p - a;
		glm::vec<Dim, T> ab_dir = b - a;
		T dot = glm::dot(ap, ab_dir);
		if (dot < T(0))
		{
			return a;
		}
		T ab_len_sqr = glm::length2(ab_dir);
		if (dot > ab_len_sqr)
		{
			return b;
		}
		return a + ab_dir * (dot / ab_len_sqr);
	}

	template<typename T>
	inline glm::vec<2, T> GetClosestPoint(const glm::vec<2, T>& a, const glm::vec<2, T>& b, const glm::vec<2, T>& p)
	{
		glm::vec<2, T> ap = p - a;
		glm::vec<2, T> ab_dir = b - a;
		T dot = ap.x * ab_dir.x + ap.y * ab_dir.y;
		if (dot < T(0))
		{
			return a;
		}
		T ab_len_sqr = ab_dir.x * ab_dir.x + ab_dir.y * ab_dir.y;
		if (dot > ab_len_sqr)
		{
			return b;
		}
		return a + ab_dir * (dot / ab_len_sqr);
	}

	//does the segment contain a point
	template<typename T>
	inline bool IsPointInSegment(const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& c)
	{
		return (
				(((p2.x >= c.x) && (c.x >= p1.x)) ||
				 ((p2.x <= c.x) && (c.x <= p1.x))) &&
				(((p2.y >= c.y) && (c.y >= p1.y)) ||
				 ((p2.y <= c.y) && (c.y <= p1.y))));
	}

	/* calc crossing point
	a1x+b1y+c1=0
	a2x+b2y+c2=0
	-c1-b1y
	x = ---------
	a1
	a2	   a2
	- ----c1 - ----b1y + b2y+c2 = 0
	a1	   a1
	a2
	---- c1  - c2
	a1
	y = -------------
	a2
	b2 - ---- b1
	a1
	a2c1 - a1c2
	y = -------------
	a1b2 - a2b1
	*/
	template<typename T>
	inline glm::vec<2, T> GetCrossPoint(T a1, T b1, T c1, T a2, T b2, T c2)
	{
		T tmp = a1 * b2 - a2 * b1;
		if (tmp == T(0))
		{
			return glm::vec<2, T>(std::numeric_limits<T>().max());
		}
		return glm::vec<2, T>((b1 * c2 - b2 * c1) / tmp, (a2 * c1 - a1 * c2) / tmp);
	}

	template<typename T>
	inline glm::vec<2, T>
	GetCrossPoint(const glm::vec<2, T>& l1p1, const glm::vec<2, T>& l1p2, const glm::vec<2, T>& l2p1,
	              const glm::vec<2, T>& l2p2)
	{
		T a1, b1, c1, a2, b2, c2;
		GetLine(a1, b1, c1, l1p1, l1p2);
		GetLine(a2, b2, c2, l2p1, l2p2);
		return GetCrossPoint(a1, b1, c1, a2, b2, c2);
	}
}
