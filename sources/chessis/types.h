#pragma once

namespace chessis
{
	struct Turn
	{
		enum Enum
		{
			WhitePLay,
			BlackPlay
		};
	};

	enum
	{
		MAX_OPS = 12,
		MAX_MOVES = 4,
		MAX_SUCCESSORS = MAX_OPS * MAX_MOVES,
		MAX_DEPTH = 30,
	};

	inline Turn::Enum Next(Turn::Enum t)
	{
		switch (t)
		{
			case Turn::WhitePLay:
				return Turn::BlackPlay;
			case Turn::BlackPlay:
				return Turn::WhitePLay;
		}
		return (Turn::Enum) -1;
	}

	struct Direction
	{
		enum Enum
		{
			Left,
			Right,
			Down,
			Up,
			End,
			NoDir = End
		};
	};

	inline Direction::Enum GetOpposit(Direction::Enum dir)
	{
		switch (dir)
		{
			case Direction::Left:
				return Direction::Right;
			case Direction::Right:
				return Direction::Left;
			case Direction::Down:
				return Direction::Up;
			case Direction::Up:
				return Direction::Down;
			default:
				return Direction::NoDir;
		}
	}

	inline void MoveCoord(int& x, int& y, Direction::Enum dir)
	{
		switch (dir)
		{
			case Direction::Left:
				--x;
				break;
			case Direction::Right:
				++x;
				break;
			case Direction::Down:
				++y;
				break;
			case Direction::Up:
				--y;
				break;
			default:
				return;
		}
	}
}
