#pragma once
#include <array>
#include <vector>
#include <assert.h>
#include <spdlog/spdlog.h>
#include <initializer_list>


namespace stack
{
	template<typename T, size_t stack_capacity>
	class FixedAllocator
	{
	public:
		typedef typename std::allocator<T>::pointer pointer;
		typedef typename std::allocator<T>::size_type size_type;
		typedef typename std::allocator<T>::value_type value_type;

		FixedAllocator(uint8_t* data, size_type* size): m_data(data), m_size(size) {};

		template<typename U>
		struct rebind {
			typedef FixedAllocator<U, stack_capacity> other;
		};

		template<typename U, size_t other_capacity>
		FixedAllocator(const FixedAllocator<U, other_capacity>& other): m_data(other.m_data), m_size(other.m_size)
		{}

		pointer allocate(size_type n, const void* hint = 0)
		{
			if (stack_capacity - *m_size >= n)
			{
				pointer r = (pointer)m_data + *m_size;
				*m_size += n;
				return r;
			}
			else
			{
				// spdlog::warn("Bounds overrun. Stack capacity {}, already allocated {}, but requested {} more", stack_capacity, *m_size, n);
				return (pointer)malloc(n * sizeof(T));
			}
		}
		FixedAllocator(const FixedAllocator<T, stack_capacity>& other): m_data(other.m_data), m_size(other.m_size) {}

		void deallocate(pointer p, size_type n)
		{
			if (p >= (pointer)m_data && p <= (pointer)m_data + stack_capacity)
			{
				if (p + n == (pointer)m_data + *m_size)
				{
					*m_size -= n;
				}
			}
			else
			{
				free(p);
			}
		}

	private:
		uint8_t* m_data;
		size_type* m_size;
	};

	template<typename T, int Size>
	class vector
	{
	public:
		typedef std::vector<T, FixedAllocator<T, Size> > tvector;
		typedef typename tvector::size_type size_type;

	private:
		uint8_t m_data[Size * sizeof(T)];
		size_type m_size;
		tvector _vector;

	public:
		typedef typename tvector::value_type value_type;
		typedef typename tvector::pointer pointer;
		typedef typename tvector::const_pointer const_pointer;
		typedef typename tvector::reference reference;
		typedef typename tvector::const_reference const_reference;
		typedef typename tvector::iterator iterator;
		typedef typename tvector::const_iterator const_iterator;
		typedef typename tvector::reverse_iterator reverse_iterator;
		typedef typename tvector::const_reverse_iterator const_reverse_iterator;
		typedef typename tvector::difference_type difference_type;

		vector():  m_size(0), _vector(FixedAllocator<T, Size>(m_data, &m_size))
		{
			_vector.reserve(Size);
			memset(m_data, 0, sizeof(T) * Size);
		}
		vector(std::initializer_list<T> list):  m_size(0), _vector(list, FixedAllocator<T, Size>(m_data, &m_size))
		{
		}

		iterator begin() { return _vector.begin(); }
		const_iterator begin() const { return _vector.begin(); }
		iterator end() { return _vector.end(); }
		const_iterator end() const { return _vector.end(); }
		reverse_iterator rbegin() { return _vector.rbegin(); }
		const_reverse_iterator rbegin() const { return _vector.rbegin(); }
		reverse_iterator rend() { return _vector.rend(); }
		const_reverse_iterator rend() const { return _vector.rend(); }

		const_iterator cbegin() const { return _vector.cbegin(); }
		const_iterator cend() const { return _vector.cend(); }
		const_reverse_iterator crbegin() const { return _vector.crbegin(); }
		const_reverse_iterator crend() const { return _vector.crend(); }
		size_type size() const { return _vector.size(); }
		bool empty() const { return begin() == end(); }

        reference operator[](size_type n) noexcept { return _vector[n]; }
        const_reference operator[](size_type n)  const noexcept { return _vector[n]; }

        reference at(size_type n) { return _vector[n];}
        const_reference at(size_type n) const { return _vector[n];}

		reference front() { return *begin(); }
		const_reference front() const { return *begin(); }
		reference back() { return *(end() - 1); }
		const_reference back() const { return *(end() - 1); }

		T* data() { return _vector.data(); }
		const T* data() const { return _vector.data(); }

		template<typename... _Args>
		void emplace_back(_Args&&... args) { _vector.emplace_back(args...); }

		void push_back(const value_type& x) { _vector.push_back(x); }
        void push_back(value_type&& __x) { emplace_back(std::move(__x)); }
		void pop_back() noexcept { _vector.pop_back(); }

		void clear() noexcept { _vector.clear(); }
	};
}
