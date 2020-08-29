#include "wild_mask.h"

bool utils::wild_mask(const char* str, const char* mask)
{
	while (*mask)
	{
		if (*mask == '?')
		{
			if (!*str)
			{
				return false;
			}

			++str;
			++mask;
		}
		else if (*mask == '*')
		{
			if (wild_mask(str, mask + 1))
			{
				return true;
			}

			if (*str && wild_mask(str + 1, mask))
			{
				return true;
			}

			return false;
		}
		else
		{
			if (*str++ != *mask++)
			{
				return false;
			}
		}
	}

	return !*str && !*mask;
}

