#pragma once
#include "board.h"
#include "eval.h"
#include "rules.h"
#include <stdlib.h>
#include <unordered_map>


namespace chessis
{
	template <bool do_prunning>
	inline int AlphaBetaNegamax(Board& board, int depth, int alpha, int betta, std::unordered_map<BoardBrief, std::pair<int, int>>* cache)
	{
		auto s = cache->find(board);
		if (s != cache->end() && s->second.second >= depth)
		{
			return s->second.first;
		}

		if (depth == 0 || game_over(board))
		{
			return Evaluate(board, depth);
		}

		stack_buffer<Move, MAX_SUCCESSORS> successors;
		GenerateMoves(board, successors);

		if (successors.empty())
		{
			return Evaluate(board, depth);
		}

		int value = INT_MIN;

		for (typename stack_buffer<Move, MAX_SUCCESSORS>::size_type i = 0, l = successors.size(); i < l; ++i)
		{
			Move& cmd = successors[i];

			DoMove(board, cmd, false);
			int new_eval = -AlphaBetaNegamax<do_prunning>(board, depth - 1, -betta, -alpha, cache);
			UndoMove(board);

			if (new_eval >= betta && do_prunning)
			{
				return new_eval;
			}
			value = std::max(value, new_eval);
			alpha = std::max(alpha, new_eval);
		}
        cache->emplace(std::make_pair(BoardBrief(board), std::make_pair(value, depth)));

		return value;
	}


	inline Move FindBestMove(Board& board, int depth)
	{
		std::unordered_map<BoardBrief, std::pair<int, int> > cache;
		if (game_over(board))
		{
			return Move(Evaluate(board, depth));
		}

		stack_buffer<Move, MAX_SUCCESSORS> successors;
		stack_buffer<Move, MAX_SUCCESSORS> best_moves;
		GenerateMoves(board, successors);

		if (successors.empty())
		{
			return Move(Evaluate(board, depth));
		}

		int best_value = INT_MIN;
		int alpha = INT_MIN + 1;
		int beta = INT_MAX - 1;

		for (typename stack_buffer<Move, MAX_SUCCESSORS>::size_type i = 0, l = successors.size(); i < l; ++i)
		{
			Move& cmd = successors[i];

			DoMove(board, cmd, false);
			cmd.new_eval = -AlphaBetaNegamax<true>(board, depth - 1, -beta, -alpha, &cache);
			// int tmp = -AlphaBetaNegamax<false>(board, depth - 1, -beta, -alpha, Next(turn));
			// assert(tmp == cmd.new_eval);
			UndoMove(board);

			// alpha = std::max(alpha, cmd.new_eval);

			if (best_value < cmd.new_eval)
			{
				best_value = cmd.new_eval;
				best_moves.clear();
				best_moves.push_back(cmd);
			}
			else if (best_value == cmd.new_eval)
			{
				best_moves.push_back(cmd);
			}
		}

		// printf("Best moves: %d\n", (int)best_moves.size());

		return best_moves[rand() % best_moves.size()];
	}


	template <bool do_prunning>
	std::vector<Move> ReturnAllMoves(Board& board, int depth)
	{
		std::unordered_map<BoardBrief, std::pair<int, int>> cache;
		if (game_over(board))
		{
			return { Move(Evaluate(board, depth)) };
		}

		stack_buffer<Move, MAX_SUCCESSORS> successors;
		std::vector<Move> moves;
		GenerateMoves(board, successors);

		if (successors.empty())
		{
			return { Move(Evaluate(board, depth)) };
		}

		int alpha = INT_MIN + 1;
		int beta = INT_MAX - 1;

		for (typename stack_buffer<Move, MAX_SUCCESSORS>::size_type i = 0, l = successors.size(); i < l; ++i)
		{
			Move& cmd = successors[i];

			DoMove(board, cmd, false);
			cmd.new_eval = -AlphaBetaNegamax<do_prunning>(board, depth - 1, -beta, -alpha, &cache);
			// int tmp = -AlphaBetaNegamax<false>(board, depth - 1, -beta, -alpha, Next(turn));
			// assert(tmp == cmd.new_eval);
			UndoMove(board);
			// alpha = std::max(alpha, cmd.new_eval);

			moves.push_back(cmd);
		}

		return moves;
	}
}
