#pragma once
#include <glm/glm.hpp>
#include <limits>

namespace glm
{
	template<typename T>
	class _comp
	{
	public:
		typedef T vec_type;
		typedef _comp<T> type;
		typedef _comp<bool> bool_type;

		vec_type re, im;

		typedef length_t length_type;
		GLM_FUNC_DECL static GLM_CONSTEXPR length_type length()
		{ return 2; }

		GLM_FUNC_DECL GLM_CONSTEXPR T& operator[](length_type i)
		{
			return const_cast<T&>(const_cast<const _comp<T*> >(this)->operator[](i));
		}

		GLM_FUNC_DECL GLM_CONSTEXPR T const& operator[](length_type i) const
		{
			assert(i >= 0 && i < this->length());
			switch(i)
			{
			default:
			case 0:
				return re;
			case 1:
				return im;
			}
		}

		// -- Implicit basic constructors --
		GLM_FUNC_DECL GLM_CONSTEXPR _comp(): re(0), im(0)
		{}

		GLM_FUNC_DECL GLM_CONSTEXPR _comp(_comp const& v) GLM_DEFAULT;

		// -- Explicit basic constructors --
		GLM_FUNC_DECL GLM_CONSTEXPR explicit _comp(T scalar): re(scalar), im(0)
		{}

		GLM_FUNC_DECL GLM_CONSTEXPR explicit _comp(vec<2, T> const& v): re(v.x), im(v.y)
		{}

		template<typename U>
		GLM_FUNC_DECL GLM_CONSTEXPR explicit _comp(_comp<U> const& v): re(static_cast<T>(v.re)), im(static_cast<T>(v.im))
		{}

		GLM_FUNC_DECL GLM_CONSTEXPR _comp(T x, T y): re(x), im(y)
		{}

		// -- Unary arithmetic operators --
		GLM_FUNC_DECL GLM_CONSTEXPR _comp<T>& operator=(_comp const& v) GLM_DEFAULT;

		template<typename U>
		GLM_FUNC_DECL GLM_CONSTEXPR _comp<T>& operator=(_comp<U> const& v)
		{
			re = static_cast<T>(v.re);
			im = static_cast<T>(v.im);
			return *this;
		}

		template<typename U>
		GLM_FUNC_DECL GLM_CONSTEXPR _comp<T>& operator+=(U scalar)
		{
			re += static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		GLM_FUNC_DECL GLM_CONSTEXPR _comp<T>& operator+=(_comp<U> const& v)
		{
			re += static_cast<T>(v.re);
			im += static_cast<T>(v.im);
			return *this;
		}

		template<typename U>
		GLM_FUNC_DECL GLM_CONSTEXPR _comp<T>& operator-=(U scalar)
		{
			re -= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		GLM_FUNC_DECL GLM_CONSTEXPR _comp<T>& operator-=(_comp<U> const& v)
		{
			re -= static_cast<T>(v.re);
			im -= static_cast<T>(v.im);
			return *this;
		}

		template<typename U>
		GLM_FUNC_DECL GLM_CONSTEXPR _comp<T>& operator*=(U scalar)
		{
			re *= static_cast<T>(scalar);
			im *= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		GLM_FUNC_DECL GLM_CONSTEXPR _comp<T>& operator*=(_comp<U> const& v)
		{
			re = re * static_cast<T>(v.re) - im * static_cast<T>(v.im);
			im = re * static_cast<T>(v.im) + im * static_cast<T>(v.re);
			return *this;
		}

		template<typename U>
		GLM_FUNC_DECL GLM_CONSTEXPR _comp<T>& operator/=(U scalar)
		{
			re /= static_cast<T>(scalar);
			im /= static_cast<T>(scalar);
			return *this;
		}

		template<typename U>
		GLM_FUNC_DECL GLM_CONSTEXPR _comp<T>& operator/=(_comp<U> const& v)
		{
			T d = static_cast<T>(1) / (v.re * v.re + v.im * v.im);
			re = d * (re * static_cast<T>(v.re) + im * static_cast<T>(v.im));
			im = d * (im * static_cast<T>(v.re) - re * static_cast<T>(v.im));
			return *this;
		}
	};

	// -- Unary operators --
	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator+(_comp<T> const& v)
	{ return v; }

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator-(_comp<T> const& v)
	{ return _comp<T>(-v.re, -v.im); }

	// -- Binary operators --
	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator+(_comp<T> const& v, T scalar)
	{ return _comp<T>(v.re + scalar, v.im); }

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator+(T scalar, _comp<T> const& v)
	{ return _comp<T>(v.re + scalar, v.im); }

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator+(_comp<T> const& v1, _comp<T> const& v2)
	{ return _comp<T>(v1.re + v2.re, v1.im + v2.im); }

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator-(_comp<T> const& v, T scalar)
	{ return _comp<T>(v.re - scalar, v.im); }

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator-(T scalar, _comp<T> const& v)
	{ return _comp<T>(scalar - v.re, -v.im); }

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator-(_comp<T> const& v1, _comp<T> const& v2)
	{ return _comp<T>(v1.re - v2.re, v1.im - v2.im); }

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator*(_comp<T> const& v, T scalar)
	{ return _comp<T>(v.re * scalar, v.im * scalar); }

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator*(T scalar, _comp<T> const& v)
	{ return _comp<T>(v.re * scalar, v.im * scalar); }

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator*(_comp<T> const& v1, _comp<T> const& v2)
	{ return _comp<T>(v1.re * v2.re - v1.im * v2.im, v1.re * v2.im + v1.im * v2.re); }

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator/(_comp<T> const& v, T scalar)
	{ return _comp<T>(v.re / scalar, v.im / scalar); }

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator/(T scalar, _comp<T> const& v)
	{
		T d = static_cast<T>(1) / (v.re * v.re + v.im * v.im);
		return _comp<T>(v.re * scalar * d, -v.im * scalar * d);
	}

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR _comp<T> operator/(_comp<T> const& v1, _comp<T> const& v2)
	{
		T d = static_cast<T>(1) / (v2.re * v2.re + v2.im * v2.im);
		return _comp<T>(d * (v1.re * v2.re + v1.im * v2.im), d * (v1.im * v2.re - v1.re * v2.im));
	}

	// -- Boolean operators --
	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR bool operator==(_comp<T> const& v1, _comp<T> const& v2)
	{
		return
				detail::compute_equal<T, std::numeric_limits<T>::is_iec559>::call(v1.x, v2.x) &&
				detail::compute_equal<T, std::numeric_limits<T>::is_iec559>::call(v1.y, v2.y);
	}

	template<typename T>
	GLM_FUNC_DECL GLM_CONSTEXPR bool operator!=(_comp<T> const& v1, _comp<T> const& v2)
	{
		return !(v1 == v2);
	}

	template<typename T>
	GLM_FUNC_QUALIFIER _comp<T> conj(_comp<T> const& v)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'conj' accepts only floating-point inputs");
		return _comp<T>(v.re, -v.im);
	}

	template<typename T>
	GLM_FUNC_QUALIFIER _comp<T> exp(_comp<T> const& v)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'exp' accepts only floating-point inputs");
		T d = ::std::exp(v.re);
		return {d * ::std::cos(v.im), d * ::std::sin(v.im)};
	}

	template<typename T>
	GLM_FUNC_QUALIFIER T length(_comp<T> const& x)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'length' accepts only floating-point inputs");
		return x.re * x.re + x.im * x.im;
	}

	template<typename T>
	GLM_FUNC_QUALIFIER T arg(_comp<T> const& x)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'arg' accepts only floating-point inputs");
		return ::std::atan2(x.im, x.re);
	}

	template<typename T>
	GLM_FUNC_QUALIFIER _comp<T> log(_comp<T> const& x)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'log' accepts only floating-point inputs");
		return {::std::log(x.re), arg(x)};
	}

	template<typename T>
	GLM_FUNC_QUALIFIER mat<2, 2, T> to_mat(_comp<T> const& x)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'to_mat' accepts only floating-point inputs");
		return {{x.re, x.im}, {-x.im, x.re}};
	}

	template<typename T>
	GLM_FUNC_QUALIFIER _comp<T> cis(T phi)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'cis' accepts only floating-point inputs");
		return { ::std::cos(phi), ::std::sin(phi)};
	}

	template<typename T>
	GLM_FUNC_QUALIFIER _comp<T> from_polar(T r, T phi)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'from_polar' accepts only floating-point inputs");
		return r * cis(phi);
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<2, T> transform(_comp<T> const& x, vec<2, T, Q> const& v)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'transform' accepts only floating-point inputs");
		return { x.re * v.x - x.im * v.y, x.im * v.x + x.re * v.y };
	}


	typedef _comp<float>  comp;
	typedef _comp<float>  fcomp;
	typedef _comp<f32>    f32comp;
	typedef _comp<double> dcomp;
	typedef _comp<f64>    f64comp;
}
