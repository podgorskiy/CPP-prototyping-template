#pragma once
#include "types.h"
#include "piece.h"
#include "move.h"
#include "utils/buffer.h"
#include "utils/stack_buffer.h"


namespace chessis
{
	struct Board
	{
		enum state
		{
			Walkable = 0,
			Block = 0x1,
			HasW = 0x1000,
			HasB = 0x2000,
		};

		int size_x;
		int size_y;
		buffer<int> cell_state;

		Piece white_ops[MAX_OPS];
		Piece black_ops[MAX_OPS];

		uint8_t b_ops_count = 0;
		uint8_t w_ops_count = 0;

		int white_total_health = 0;
		int black_total_health = 0;

		stack_buffer <std::tuple<Piece, int, int>, MAX_DEPTH> op_history;
		buffer <std::pair<Move, Turn::Enum> > cmd_history;

		mutable int positions = 0;

		bool valid_coord(int x, int y) const { return !(x < 0 || y < 0 || x >= size_x || y >= size_y); }

		const int get_cell(int x, int y) const { return valid_coord(x, y) ? cell_state[x + y * size_x] : Block; }

		const int get_cell_nch(int x, int y) const { return cell_state[x + y * size_x]; }

		const int get_cell(int x, int y, Direction::Enum dir) const
		{
			MoveCoord(x, y, dir);
			return valid_coord(x, y) ? cell_state[x + y * size_x] : Block;
		}

		void set_cell(int x, int y, int n) { if (valid_coord(x, y)) cell_state[x + y * size_x] = n; }

		void set_cell_and_or(int x, int y, int a, int o)
		{
			if (valid_coord(x, y))
			{
				auto& c = cell_state[x + y * size_x];
				c &= a;
				c |= o;
			}
		}

		void set_cell_and_or_nch(int x, int y, int a, int o)
		{
			auto& c = cell_state[x + y * size_x];
			c &= a;
			c |= o;
		}

		void clear_cell(int x, int y) { set_cell_and_or(x, y, 0xFF, 0); }

		void clear_cell_nch(int x, int y)
		{
			auto& c = cell_state[x + y * size_x];
			c &= 0xFF;
			c |= 0;
		}

		void set_cell_op(int x, int y, int op_id, Turn::Enum turn)
		{
			set_cell_and_or(x, y, 0xFF, (op_id << 8) | (turn == Turn::WhitePLay ? Board::HasW : Board::HasB));
		}

		void set_cell_op_nch(int x, int y, int op_id, Turn::Enum turn)
		{
			set_cell_and_or_nch(x, y, 0xFF, (op_id << 8) | (turn == Turn::WhitePLay ? Board::HasW : Board::HasB));
		}

		void move_piece(int x, int y, Direction::Enum dir, Turn::Enum turn)
		{
			int piece = get_opp_id_nch(x, y, turn == Turn::WhitePLay, turn == Turn::BlackPlay);
			// assert(piece != -1);
			clear_cell_nch(x, y);
			MoveCoord(x, y, dir);
			Piece* ops = turn == Turn::WhitePLay ? white_ops : black_ops;
			ops[piece].x = x;
			ops[piece].y = y;
			set_cell_op_nch(x, y, piece, turn);
		}

		Piece& get_opp(int x, int y, bool white, bool black)
		{
			return const_cast<Piece&>(const_cast<const Board*>(this)->get_opp(x, y, white, black));
		}

		const Piece& get_opp(int x, int y, bool white, bool black) const
		{
			if (valid_coord(x, y))
			{
				auto c = get_cell_nch(x, y);
				if (c & Board::HasW && white)
				{
					int id = (c & 0xF00) >> 8;
					return white_ops[id];
				}
				else if (c & Board::HasB && black)
				{
					int id = (c & 0xF00) >> 8;
					return black_ops[id];
				}
			}
			return nop();
		}

		int get_opp_id(int x, int y, bool white, bool black) const
		{
			if (valid_coord(x, y))
			{
				auto c = get_cell_nch(x, y);
				if ((c & Board::HasW) && white)
				{
					return (c & 0xF00) >> 8;
				}
				else if ((c & Board::HasB) && black)
				{
					return (c & 0xF00) >> 8;
				}
			}
			return -1;
		}

		int get_opp_id_nch(int x, int y, bool white, bool black) const
		{
			auto c = get_cell_nch(x, y);
			if ((c & Board::HasW) && white)
			{
				return (c & 0xF00) >> 8;
			}
			else if ((c & Board::HasB) && black)
			{
				return (c & 0xF00) >> 8;
			}
		}

		bool init_eval()
		{
			int ew = 0;
			for (int i = 0; i < w_ops_count; ++i)
			{
				ew += white_ops[i].get_cost();
			}
			int eb = 0;
			for (int i = 0; i < b_ops_count; ++i)
			{
				eb += black_ops[i].get_cost();
			}
			white_total_health = ew;
			black_total_health = eb;
		}

		void dump_board()
		{
			const char* labels[8] = {".", "#", "P", "p", "K", "k", "A", "a"};
			const chessis::Piece* p;
			for (int i = 0; i < size_y; ++i)
			{
				for (int j = 0; j < size_x; ++j)
				{
					int status = cell_state.data()[i * size_x + j];
					if (status & Board::HasW)
					{
						int id = (status & 0xF00) >> 8;
						p = &white_ops[id];
						status = ((white_ops[id].type) << 1u) | 0u;
					}
					if (status & Board::HasB)
					{
						int id = (status & 0xF00) >> 8;
						p = &black_ops[id];
						status = ((black_ops[id].type) << 1u) | 1u;
					}
					printf("%s", labels[status]);
				}
				printf("\n");
			}
		}
	};
}
