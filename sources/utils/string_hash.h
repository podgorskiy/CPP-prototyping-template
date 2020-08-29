#pragma once
#include <string.h>
#include <stdint.h>

namespace detail
{
	// Fowler–Noll–Vo hash function
	constexpr uint64_t fnv_prime_k = 0x100000001b3ULL;
	constexpr uint64_t fnv_basis_k = 0xcbf29ce484222325ULL;

	constexpr uint64_t string_hash(const char* str, uint64_t len, uint64_t n, uint64_t state)
	{

		return n < len ? string_hash(str, len, n + 1, (state ^ static_cast<uint64_t>(str[n])) * fnv_prime_k) : state;
	}

	constexpr uint64_t string_hash(const char* str, uint64_t len) {

		return string_hash(str, len, 0, fnv_basis_k);
	}
}

inline constexpr uint64_t operator"" _hash(const char* str, size_t n) {
	return detail::string_hash(str, (uint64_t)n);
}
