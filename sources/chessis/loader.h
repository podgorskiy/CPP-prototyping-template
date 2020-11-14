#pragma once
#include "board.h"
#include <string>

namespace chessis
{
	Board::state get_cell_type(char c);

	inline void make_board(Board& board, const std::string& b)
	{
		const char* ptr = b.c_str();
		int width = strstr(ptr, "\n") - ptr;
		int count = 0;
		const char *tmp = ptr;
		while(tmp = strstr(tmp, "\n"))
		{
		   count++;
		   tmp++;
		}

	    int height = count;

		board.size_x = width;
		board.size_y = height;
		board.cell_state.resize(board.size_x * board.size_y);
		memset(board.cell_state.data(), 0, board.size_x * board.size_y * sizeof(int));
		memset(board.black_ops, 0, MAX_OPS * sizeof(Piece));
		memset(board.white_ops, 0, MAX_OPS * sizeof(Piece));
		board.turn = Turn::WhitePLay;

		for (int i = 0; i < board.size_y; ++i)
		{
			for (int j = 0; j < board.size_x; ++j)
			{
				board.cell_state.data()[i * board.size_x + j] = get_cell_type(ptr[i * (width + 1) + j]);
				switch(ptr[i * (width + 1) + j])
				{
						case 'p': board.black_ops[board.b_ops_count] = Piece({Piece::Pawn, 2, 0, uint8_t(j), uint8_t(i), 0});
							board.cell_state.data()[i * board.size_x + j] |= Board::HasB | (board.b_ops_count++ << 8);
							break;
						case 'P': board.white_ops[board.w_ops_count] = Piece({Piece::Pawn, 2,  0, uint8_t(j), uint8_t(i), 0});
							board.cell_state.data()[i * board.size_x + j] |= Board::HasW | (board.w_ops_count++ << 8);
							break;
						case 'k': board.black_ops[board.b_ops_count] = Piece({Piece::Knight, 3,  0, uint8_t(j), uint8_t(i), 0});
							board.cell_state.data()[i * board.size_x + j] |= Board::HasB | (board.b_ops_count++ << 8);
							break;
						case 'K': board.white_ops[board.w_ops_count] = Piece({Piece::Knight, 3,  0, uint8_t(j), uint8_t(i), 0});
							board.cell_state.data()[i * board.size_x + j] |= Board::HasW | (board.w_ops_count++ << 8);
							break;
						case 'a': board.black_ops[board.b_ops_count] = Piece({Piece::Archer, 1,  0, uint8_t(j), uint8_t(i), 0});
							board.cell_state.data()[i * board.size_x + j] |= Board::HasB | (board.b_ops_count++ << 8);
							break;
						case 'A': board.white_ops[board.w_ops_count] = Piece({Piece::Archer, 1,  0, uint8_t(j), uint8_t(i), 0});
							board.cell_state.data()[i * board.size_x + j] |= Board::HasW | (board.w_ops_count++ << 8);
							break;
				}
			}
		}
		board.init_eval();
	}

	inline Board::state get_cell_type(char c)
	{
		switch (c)
		{
			case ' ':
			case '.':
				return Board::Walkable;
			case 'P':
			case 'K':
			case 'p':
			case 'k':
			case 'a':
			case 'A':
				return Board::Walkable;
			case '#':
				return Board::Block;
		}
		return Board::Block;
	}
}
