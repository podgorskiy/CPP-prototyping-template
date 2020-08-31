#pragma once
#include <bits/stdc++.h>
#include "types.h"


namespace chessis
{
	struct Move
	{
		enum Action
		{
			move,
			attack,
			end
		};

		int new_eval = 0;
		Action action = end;
		Direction::Enum dir = Direction::NoDir;
		int op_id = -1;

		Move(): new_eval(INT_MIN), action(end), op_id(-1)
		{}

		Move(int new_eval): new_eval(new_eval), action(end), op_id(-1)
		{}

		Move(int new_eval, Action action, Direction::Enum dir, int op_id): new_eval(new_eval), action(action), dir(dir), op_id(op_id)
		{}

		int operator()() const { return new_eval; }

		bool operator<(const Move& other) const { return new_eval < other.new_eval; }

		Move operator-() { return Move(-new_eval, action, dir, op_id); };
	};
}
