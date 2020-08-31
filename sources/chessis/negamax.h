#pragma once
#include "board.h"
#include "eval.h"
#include "rules.h"


namespace chessis
{
	Move AlphaBetaNegamax(Board& board, int depth, int alpha, int betta, Turn::Enum turn)
	{
		if (depth == 0 || game_over(board))
		{
			return Move(Evaluate(board, turn, depth));
		}

		stack_buffer<Move, MAX_SUCCESSORS> successors;
		GenerateMoves(board, turn, successors);

		if (successors.empty())
		{
			return Move(Evaluate(board, turn, depth));
		}

		Move value;

		for (typename stack_buffer<Move, MAX_SUCCESSORS>::size_type i = 0, l = successors.size(); i < l; ++i)
		{
			Move& cmd = successors[i];

			DoMove(board, cmd, turn, false);
			cmd.new_eval = -AlphaBetaNegamax(board, depth - 1, -betta, -alpha, Next(turn)).new_eval;
			UndoMove(board);

			value = std::max(value, cmd);
			alpha = std::max(alpha, value.new_eval);
			if (alpha > betta)
			{
				// return value;
			}
		}

		return value;
	}
}
