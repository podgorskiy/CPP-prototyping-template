#pragma once
#include <stdint.h>
#include <vector>
#include <mutex>
#include <algorithm>


struct _LessOp
{
	template<typename I1, typename I2>
    bool operator()(const I1& it1, const I2& it2) const
    {
    	return it1 < it2;
    }
};


template<typename T, class Compare = _LessOp>
class Heap
{
public:
	typedef T value_type;

    template<typename... Args>
	explicit Heap(Args&&... args): compare(args...) {}

	void build_heap()
	{
		int64_t length = values.size();
		for (int64_t i = length / 2 - 1; i >= 0; --i)
		{
			heapify_down(i);
		}
	}

	void heapify_down(int64_t index)
	{
		int64_t length = values.size();
		int64_t left_child = 2 * index + 1;
		int64_t right_child = 2 * index + 2;

		if (left_child >= length)
		{
			return;
		}

		int64_t min_index = index;

		if (compare(values[left_child], values[index]))
		{
			min_index = left_child;
		}

		if ((right_child < length) && compare(values[right_child], values[min_index]))
		{
			min_index = right_child;
		}

		if (min_index != index)
		{
			std::swap(values[index], values[min_index]);
			heapify_down(min_index);
		}
	}

	void heapify_up(int64_t index)
	{
		int64_t parentIndex = (index - 1) / 2;

		if (index != 0 && compare(values[index], values[parentIndex]))
		{
			std::swap(values[index], values[parentIndex]);
			heapify_up(parentIndex);
		}
	}

	std::vector<T> values;
private:
	Compare compare;
};


template<typename T, class Compare = _LessOp>
class PriorityQueue: public Heap<T, Compare>
{
	typedef std::lock_guard<std::mutex> Lock;
public:
	typedef T value_type;

    template<typename... Args>
	explicit PriorityQueue(Args&&... args): Heap<T, Compare>(args...) {}

	void push(T&& x)
	{
		Lock lock(m);
		Heap<T, Compare>::values.push_back(std::move(x));
		Heap<T, Compare>::heapify_up(Heap<T, Compare>::values.size() - 1);
	}

    template<typename... Args>
	void emplace(Args&&... args)
	{
		Lock lock(m);
		Heap<T, Compare>::values.emplace(args...);
		Heap<T, Compare>::heapify_up(Heap<T, Compare>::values.size() - 1);
	}

	T peek() const
	{
		Lock lock(m);
		return Heap<T, Compare>::values.front();
	}

	T pop()
	{
		Lock lock(m);
		int64_t length = Heap<T, Compare>::values.size();

		if (length == 0)
		{
			return T();
		}

		std::swap(Heap<T, Compare>::values[length - 1], Heap<T, Compare>::values[0]);
		T r = Heap<T, Compare>::values.back();
		Heap<T, Compare>::values.pop_back();

		Heap<T, Compare>::heapify_down(0);
		return r;
	}

	size_t size() const
	{
		return Heap<T, Compare>::values.size();
	}

private:
	std::mutex m;
};
