#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <utility>
#include <stdlib.h>
#include <assert.h>


template<typename T, int Size>
class stack_buffer
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

    stack_buffer(const stack_buffer& other):m_size(other.m_size)
    {
		memcpy(m_data, other.m_data, sizeof(T) * m_size);
    }

    stack_buffer& operator=( const stack_buffer& other)
    {
    	m_size = other.size;
		memcpy(m_data, other.m_data, sizeof(T) * m_size);
    }

    bool operator==(const stack_buffer& other) const
    {
    	if (m_size != other.m_size)
    		return false;
    	for (int i = 0; i < m_size; ++i)
    		if (m_data[i] != other.m_data[i])
    			return false;
    	return true;
    }

	stack_buffer(): m_size(0)
	{}

	stack_buffer(pointer data, size_type size): m_size(0)
	{
		if (data != nullptr)
		{
			memcpy(m_data, data, sizeof(T) * size);
			m_size = size;
		}
		else
		{
			m_size = size;
			memset(m_data, 0, sizeof(T) * size);
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
	void safe_clear() noexcept { memset(m_data, 0, sizeof(T) * Size); m_size = 0; }

	void resize(size_type new_size)
    {
		_reserv(new_size);
		m_size = new_size;
    }
private:
	bool _reserv(size_type new_size)
	{
		assert(new_size <= Size);
		return new_size <= Size;
	}

	value_type m_data[Size];
	size_type m_size;
};
