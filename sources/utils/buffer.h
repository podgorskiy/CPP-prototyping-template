#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <utility>
#include <stdlib.h>


template<typename T>
class buffer
{
public:
	typedef size_t size_type;

	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;

	typedef ptrdiff_t difference_type;

	typedef pointer iterator;
	typedef const_pointer const_iterator;

    buffer( const buffer& ) = delete; // non construction-copyable
    buffer& operator=( const buffer& ) = delete; // non copyable

	buffer(): m_data(nullptr), m_size(0), m_reserved(0)
	{}

	buffer(pointer data, size_type size): m_data(nullptr), m_size(0), m_reserved(0)
	{
		if (data != nullptr)
		{
			m_data = static_cast<uint8_t*>(malloc(size * sizeof(T)));
			memcpy(m_data, data, size);
			m_reserved = size;
			m_size = size;
		}
		else
		{
			m_data = static_cast<uint8_t*>(malloc(size * sizeof(T)));
			memset(m_data, 0, sizeof(T) * size);
			m_reserved = size;
			m_size = size;
		}
	}

	iterator begin() { return m_data; }
	const_iterator begin() const { return m_data; }
	iterator end() { return m_data + m_size; }
	const_iterator end() const { return m_data + m_size; }

	const_iterator cbegin() const { return m_data; }
	const_iterator cend() const { return m_data; }
	size_type size() const { return m_size; }
	bool empty() const { return begin() == end(); }

	reference operator[](size_type n) noexcept { return m_data[n]; }
	const_reference operator[](size_type n)  const noexcept { return m_data[n]; }

	reference at(size_type n) { return m_data[n];}
	const_reference at(size_type n) const { return m_data[n];}

	reference front() { return *begin(); }
	const_reference front() const { return *begin(); }
	reference back() { return *(end() - 1); }
	const_reference back() const { return *(end() - 1); }

	pointer data() { return m_data; }
	const_pointer data() const { return m_data; }

	template<typename... _Args>
	void emplace_back(_Args&&... args)
	{
		_reserv(1 + m_size);
		new (m_data + m_size++) T(args...);
	}

	void push_back(const value_type& x)
	{
		_reserv(1 + m_size);
		m_data[m_size++] = x;
	}

	void push_back(value_type&& __x) { emplace_back(std::move(__x)); }
	void pop_back() noexcept { --m_size; }

	void clear() noexcept { m_size = 0; }
	void safe_clear() noexcept { memset(m_data, 0, sizeof(T) * m_reserved); m_size = 0; }

	void resize(size_type new_size)
    {
		_reserv(new_size);
		m_size = new_size;
    }
private:
	bool _reserv(size_type new_size)
	{
		if (new_size > m_reserved)
		{
			uint64_t v = new_size + 1;

			v--;
			v |= v >> 1U;
			v |= v >> 2U;
			v |= v >> 4U;
			v |= v >> 8U;
			v |= v >> 16U;
			v |= v >> 32U;
			v++;

			m_reserved = static_cast<size_type>(v);
			m_data = static_cast<pointer>(realloc(m_data, m_reserved * sizeof(value_type)));
		}
		return m_data != nullptr;
	}

	pointer m_data;
	size_type m_size;
	size_type m_reserved;
};
