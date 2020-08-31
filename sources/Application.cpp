#include "Application.h"
#include "utils/buffer.h"
#include "utils/stack_buffer.h"
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <stdio.h>
#include <string.h>
#include <fsal.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <bits/stdc++.h>


#pragma pack(push,1)
struct Operator
{

	enum Type : uint8_t
	{
		Dead = 0x00,
		Pawn = 0x01,
		Knight = 0x02,
		Rook = 0x03,
	};

	Type type : 8;
	int8_t health : 8;
	uint8_t attribute : 6;
	uint8_t x : 4;
	uint8_t y : 4;
	uint8_t o : 2;

	int get_cost() const
	{
		switch (type)
		{
			case Operator::Pawn: return 100 * health;
			case Operator::Knight: return 200 * health;
			case Operator::Rook: return 300 * health;
		}
		return 0;
	}
};
#pragma pack(pop)

Operator noop({Operator::Dead, 0, 0, 0, 0, 0});

enum Direction
{
	Left,
	Right,
	Down,
	Up,
	End,
	NoDir = End
};

struct Command
{
	enum Action
	{
		move,
		attack,
		end
	};
	int new_eval = 0;
	Action action = end;
	Direction dir = NoDir;
	int op_id = -1;

	Command(): new_eval(INT_MIN), action(end), op_id(-1) {}
	Command(int new_eval): new_eval(new_eval), action(end), op_id(-1) {}
	Command(int new_eval, Action action, Direction dir, int op_id): new_eval(new_eval), action(action), dir(dir), op_id(op_id) {}

	int operator ()() const { return new_eval; }
	bool operator < (const Command& other) const { return new_eval < other.new_eval; }
	Command operator -() { return Command(-new_eval, action, dir, op_id); };
};



enum Turn
{
	WhitePLay,
	BlackPlay
};

enum
{
	MAX_OPS = 12,
	MAX_MOVES = 4,
	MAX_SUCCESSORS = MAX_OPS * MAX_MOVES,
	MAX_DEPTH = 30,
};


Turn Next(Turn t)
{
	switch(t)
	{
		case WhitePLay: return BlackPlay;
		case BlackPlay: return WhitePLay;
	}
	return (Turn)-1;
}

struct Board
{
	enum state
	{
		Walkable = 0,
		Block = 0x1,
		HasW = 0x1000,
		HasB = 0x2000,
	};
	static Direction GetOpposit(Direction dir)
	{
		switch(dir)
		{
			case Left: return Right;
			case Right: return Left;
			case Down: return Up;
			case Up: return Down;
			default:
				return NoDir;
		}
	}

	static void MoveCoord(int& x, int& y, Direction dir)
	{
		switch(dir)
		{
			case Left: --x; break;
			case Right: ++x; break;
			case Down: ++y; break;
			case Up: --y; break;
			default:
				return;
		}
	}

	glm::ivec2 size;
	buffer<int> cell_state;

	Operator white_ops[MAX_OPS];
	Operator black_ops[MAX_OPS];

	stack_buffer<std::pair<Operator, int>, MAX_DEPTH> op_history;
	buffer<std::pair<Command, Turn>> cmd_history;

	bool valid_coord(int x, int y) const { return !(x < 0 || y < 0 || x >= size.x || y >= size.y); }
	const int get_cell(int x, int y) const { return valid_coord(x, y) ? cell_state[x + y * size.x] : Block; }

	const int get_cell(int x, int y, Direction dir) const
	{
		MoveCoord(x, y, dir);
		return valid_coord(x, y) ? cell_state[x + y * size.x] : Block;
	}

	void set_cell(int x, int y, int n) { if (valid_coord(x, y)) cell_state[x + y * size.x] = n; }
	void set_cell_and_or(int x, int y, int a, int o) { if (valid_coord(x, y)) { auto& c = cell_state[x + y * size.x]; c &= a; c |= o; }}
	void clear_cell(int x, int y) { set_cell_and_or(x, y, 0xFF, 0); }
	void set_cell_op(int x, int y, int op_id, Turn turn) { set_cell_and_or(x, y, 0xFF, (op_id << 8) | (turn == WhitePLay ? Board::HasW:Board::HasB)); }

	void move_piece(int x, int y, Direction dir, Turn turn)
	{
		int piece = get_opp_id(x, y, turn == WhitePLay, turn == BlackPlay);
		assert(piece != -1);
		clear_cell(x, y);
		MoveCoord(x, y, dir);
		Operator* ops = turn == WhitePLay ? white_ops  : black_ops;
		ops[piece].x = x;
		ops[piece].y = y;
		set_cell_op(x, y, piece, turn);
	}

	Operator& get_opp(int x, int y, bool white, bool black)
	{
		return const_cast<Operator&>(const_cast<const Board*>(this)->get_opp(x, y, white, black));
	}

	const Operator& get_opp(int x, int y, bool white, bool black) const
	{
		if (valid_coord(x, y))
		{
			auto c = get_cell(x, y);
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
		return noop;
	}

	int get_opp_id(int x, int y, bool white, bool black) const
	{
		if (valid_coord(x, y))
		{
			auto c = get_cell(x, y);
			if ((c & Board::HasW) && white)
			{
				return  (c & 0xF00) >> 8;
			}
			else if ((c & Board::HasB) && black)
			{
				return (c & 0xF00) >> 8;
			}
		}
		return -1;
	}

	uint8_t b_ops_count = 0;
	uint8_t w_ops_count = 0;
};

const char* labels[6] = { "", "Block", "WP", "BP", "WK", "BK" };
const ImVec4 colors[6] = {
		ImVec4(0.255, 0.255, 0.255, 1.0),
		ImVec4(0.7, 0.8, 0.7, 1.0),
		ImVec4(0.4, 0.4, 0.9, 1.0),
		ImVec4(0.9, 0.4, 0.4, 1.0),
		ImVec4(0.4, 0.7, 0.9, 1.0),
		ImVec4(0.9, 0.7, 0.4, 1.0),
};



Board board;


Board::state get_cell_type(char c)
{
	switch(c)
	{
		case ' ':
		case '.':
			return Board::Walkable;
		case 'P':
		case 'K':
		case 'p':
		case 'k':
			return Board::Walkable;
		case '#':
			return Board::Block;
	}
	return Board::Block;
}

int sqr(int x) {return x * x;}


int leaves = 0;

int Evaluate(const Board& board, Turn turn, int depth)
{
	int ew = 0;
	for (int i = 0; i < board.w_ops_count; ++i)
	{
		ew += board.white_ops[i].get_cost();
	}
	int eb = 0;
	for (int i = 0; i < board.b_ops_count; ++i)
	{
		eb -= board.black_ops[i].get_cost();
	}
	int distance = 0;
	for (int i = 0; i < board.w_ops_count; ++i)
	{
		for (int j = 0; j < board.b_ops_count; ++j)
		{
			if (board.black_ops[j].health != 0 && board.white_ops[i].health != 0)
			{
				float d = sqr(board.black_ops[j].x - board.white_ops[i].x) + sqr(board.black_ops[j].y - board.white_ops[i].y);
				distance += d;// * (for_black ? (gs.white_ops[i].health / gs.black_ops[j].health + 1) : (gs.black_ops[j].health / gs.white_ops[i].health + 1));
			}
		}
	}

	if (ew == 0) ew = -1000 * 100;
	if (eb == 0) eb = 1000 * 100;
	int e = ew + eb - distance;
	e += depth;

	++leaves;
	return (turn == WhitePLay ? e : -e);

	return -distance;
}

bool game_over(const Board& board)
{
	int ew = 0;
	for (int i = 0; i < board.w_ops_count; ++i)
	{
		ew += board.white_ops[i].get_cost();
	}
	int eb = 0;
	for (int i = 0; i < board.b_ops_count; ++i)
	{
		eb -= board.black_ops[i].get_cost();
	}
	return ew == 0 || eb == 0;
}

template<typename C>
typename C::value_type maximum(const C& v)
{
	typename C::size_type imax = 0;
	auto max = v[0]();
	for(typename C::size_type i = 0, l = v.size(); i < l; ++i)
	{
		auto c = v[i]();
		if (c > max)
		{
			imax = i;
			max = c;
		}
	}
	return v[imax];
}

template<typename C>
typename C::size_type argmaximum(const C& v)
{
	typename C::size_type imax = 0;
	auto max = v[0]();
	for(typename C::size_type i = 0, l = v.size(); i < l; ++i)
	{
		auto c = v[i]();
		if (c > max)
		{
			imax = i;
			max = c;
		}
	}
	return imax;
}


void GenerateMoves(const Board& board, Turn turn, stack_buffer<Command, MAX_SUCCESSORS>& successors)
{
	const Operator* ops = turn == WhitePLay ? board.white_ops  : board.black_ops;
	int count = turn == WhitePLay ? board.w_ops_count  : board.b_ops_count;

	for (int i = 0; i < count; ++i)
	{
		const Operator& op = ops[i];

		if (op.type == Operator::Dead)
		{
			continue;
		}

		// attack
		for (int _dir = 0; _dir < End; ++_dir)
		{
			Direction dir = (Direction) _dir;
			int x = op.x;
			int y = op.y;
			Board::MoveCoord(x, y, dir);
			auto& enemy_opp = board.get_opp(x, y, turn == BlackPlay, turn == WhitePLay);

			if (enemy_opp.type != 0)
			{
				successors.emplace_back(INT_MIN, Command::attack, dir, i);
			}
		}

		// move
		for (int _dir = 0; _dir < End; ++_dir)
		{
			Direction dir = (Direction) _dir;

			if (board.get_cell(op.x, op.y, dir) == Board::Walkable)
			{
				successors.emplace_back(INT_MIN, Command::move, dir, i);
			}
		}
	}
}


Command AlphaBetaNegamax(Board& board, int depth, int alpha, int betta, Turn turn);


void DoMove(Board& board, Command cmd, Turn turn, bool final=false)
{
	Operator* ops = turn == WhitePLay ? board.white_ops : board.black_ops;
	Operator* enemy_ops = turn == BlackPlay ? board.white_ops : board.black_ops;
	Operator& op = ops[cmd.op_id];
	if (!final)
		board.cmd_history.emplace_back(cmd, turn);
	switch (cmd.action)
	{
		case Command::move:
		{
			board.move_piece(op.x, op.y, cmd.dir, turn);
		}
		break;
		case Command::attack:
		{
			int x = op.x;
			int y = op.y;
			Board::MoveCoord(x, y, cmd.dir);
			int enemy_opp_id = board.get_opp_id(x, y, turn == BlackPlay, turn == WhitePLay);
			assert(enemy_opp_id != -1);
			auto& enemy_opp = enemy_ops[enemy_opp_id];
			if (!final)
				board.op_history.emplace_back(enemy_opp, enemy_opp_id);

			enemy_opp.health -= 1;
			if (enemy_opp.health <= 0)
			{
				enemy_opp.health = 0;
				enemy_opp.type = Operator::Dead;
				board.clear_cell(enemy_opp.x, enemy_opp.y);
			}
		}
		break;
	}
}


void UndoMove(Board& board)
{
	if (board.cmd_history.empty())
		return;
	auto& _cmd = board.cmd_history.back();
	Command cmd = _cmd.first;
	Turn turn = _cmd.second;
	board.cmd_history.pop_back();

	Operator* ops = turn == WhitePLay ? board.white_ops : board.black_ops;
	Operator* enemy_ops = turn == BlackPlay ? board.white_ops : board.black_ops;
	Operator& op = ops[cmd.op_id];

	switch (cmd.action)
	{
		case Command::move:
		{
			board.move_piece(op.x, op.y, Board::GetOpposit(cmd.dir), turn);
		}
		break;
		case Command::attack:
		{
			auto& backup = board.op_history.back();
			auto& enemy_opp = enemy_ops[backup.second];
			enemy_opp = backup.first;
			board.set_cell_op(enemy_opp.x, enemy_opp.y, backup.second, Next(turn));
			board.op_history.pop_back();
		}
		break;
	}
}


Command AlphaBetaNegamax(Board& board, int depth, int alpha, int betta, Turn turn)
{
	if (depth == 0 || game_over(board))
	{
		return Command(Evaluate(board, turn, depth));
	}

	stack_buffer<Command, MAX_SUCCESSORS> successors;
	GenerateMoves(board, turn, successors);

	if (successors.empty())
	{
		return Command(Evaluate(board, turn, depth));
	}

	Command value;

	for(typename stack_buffer<Command, MAX_SUCCESSORS>::size_type i = 0, l = successors.size(); i < l; ++i)
	{
		Command& cmd = successors[i];

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

Application::Application()
{
	static_assert(sizeof(Operator) == sizeof(uint32_t));

	fsal::FileSystem fs;
	std::string b = fs.Open("../board.txt");
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

	board.size = glm::ivec2(width, height);
	board.cell_state.resize(board.size.x * board.size.y);
	memset(board.cell_state.data(), 0, board.size.x * board.size.y * sizeof(int));

	for (int i = 0; i < board.size.y; ++i)
	{
		for (int j = 0; j < board.size.x; ++j)
		{
			board.cell_state.data()[i * board.size.x + j] = get_cell_type(ptr[i * (width + 1) + j]);
			switch(ptr[i * (width + 1) + j])
			{
					case 'p': board.black_ops[board.b_ops_count] = Operator({Operator::Pawn, 2, 0, uint8_t(j), uint8_t(i), 0});
						board.cell_state.data()[i * board.size.x + j] |= Board::HasB | (board.b_ops_count++ << 8);
						break;
					case 'P': board.white_ops[board.w_ops_count] = Operator({Operator::Pawn, 2,  0, uint8_t(j), uint8_t(i), 0});
						board.cell_state.data()[i * board.size.x + j] |= Board::HasW | (board.w_ops_count++ << 8);
						break;
					case 'k': board.black_ops[board.b_ops_count] = Operator({Operator::Knight, 5,  0, uint8_t(j), uint8_t(i), 0});
						board.cell_state.data()[i * board.size.x + j] |= Board::HasB | (board.b_ops_count++ << 8);
						break;
					case 'K': board.white_ops[board.w_ops_count] = Operator({Operator::Knight, 5,  0, uint8_t(j), uint8_t(i), 0});
						board.cell_state.data()[i * board.size.x + j] |= Board::HasW | (board.w_ops_count++ << 8);
						break;
			}
		}
	}
}


Application::~Application()
{
}

Command InferCommand(int op_id, Turn turn, int x, int y)
{
	Operator* ops = turn == WhitePLay ? board.white_ops : board.black_ops;
	Operator& op = ops[op_id];
	if (board.valid_coord(x, y))
	{
		if (board.get_cell(x, y) == Board::Walkable)
		{
			if (x == op.x - 1 && y == op.y) return Command(0, Command::move, Left, op_id);
			else if (x == op.x + 1 && y == op.y) return Command(0, Command::move, Right, op_id);
			else if (x == op.x && y == op.y - 1) return Command(0, Command::move, Up, op_id);
			else if (x == op.x && y == op.y + 1) return Command(0, Command::move, Down, op_id);
		}
		else
		{
			const Operator& enemy = board.get_opp(x, y, turn == BlackPlay, turn == WhitePLay);
			if (enemy.type != Operator::Dead)
			{
				if (x == op.x - 1 && y == op.y) return Command(0, Command::attack, Left, op_id);
				else if (x == op.x + 1 && y == op.y) return Command(0, Command::attack, Right, op_id);
				else if (x == op.x && y == op.y - 1) return Command(0, Command::attack, Up, op_id);
				else if (x == op.x && y == op.y + 1) return Command(0, Command::attack, Down, op_id);
			}
		}
	}
	return Command(0, Command::end, NoDir,  -1);
}

void Application::Draw(float time)
{
	static Turn turn = WhitePLay;

	ImGui::Begin("Board");

	ImGui::Columns(2);

	static Command enemy_cmd(0);

	bool do_move = false;

	for (int i = 0; i < board.size.y; ++i)
	{
		for (int j = 0; j < board.size.x; ++j)
		{
			ImGui::PushID(i * board.size.x + j);
			int status = board.cell_state.data()[i * board.size.x + j];
			if (status & Board::HasW)
			{
				int id = (status & 0xF00) >> 8;
				status = ((board.white_ops[id].type) << 1u) | 0u;
			}
			if (status & Board::HasB)
			{
				int id = (status & 0xF00) >> 8;
				status = ((board.black_ops[id].type) << 1u) | 1u;
			}
			ImGui::PushStyleColor(ImGuiCol_Button, colors[status]);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[status]);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[status]);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.0, 0.0, 1.0));
			ImGui::Button(labels[status], ImVec2(60, 60));

			if (turn == WhitePLay)
			{
				int playable_op = board.get_opp_id(j, i, true, false);
				if (playable_op != -1 && ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("op", &playable_op, sizeof(int));
					ImGui::EndDragDropSource();
				}

				if (ImGui::BeginDragDropTarget())
				{
					auto* payload = ImGui::AcceptDragDropPayload("op", ImGuiDragDropFlags_AcceptPeekOnly);
					if (payload)
					{
						int op_id = *((int*) (payload->Data));
						Command cmd = InferCommand(op_id, turn, j, i);
						if (cmd.action != Command::end)
						{
							if (ImGui::AcceptDragDropPayload("op"))
							{
								DoMove(board, cmd, turn, true);
								turn = Next(turn);
								do_move = true;
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
			}

			ImGui::PopStyleColor(4);
			if (j + 1 != board.size.x) ImGui::SameLine();
			ImGui::PopID();
		}
	}

	ImGui::NextColumn();

	int t = leaves;
	ImGui::Text("Evaluation: %d", Evaluate(board, WhitePLay, 0));
	leaves = t;

	static int depth = 6;
	ImGui::InputInt("Depth", &depth);

	if (ImGui::Button("Skip"))
	{
		turn = Next(turn);
		do_move = true;
	}
	if (ImGui::Button("Make move") || (turn == BlackPlay && do_move))
	{
		int alpha = INT_MIN;
		int beta = INT_MAX;
		leaves = 0;
		//if (turn == BlackPlay)
		//	std::swap(alpha, beta);

		enemy_cmd = AlphaBetaNegamax(board, depth, alpha, beta, turn);

		DoMove(board, enemy_cmd, turn);
		turn = Next(turn);
	}

	if (ImGui::Button("Undo move"))
	{
		UndoMove(board);
		turn = Next(turn);
	}

	ImGui::Text("Evaluations: %d", leaves);

	switch (enemy_cmd.action)
	{
		case Command::move:
		{
			ImGui::Text("Enemy action: move");
		}
		break;
		case Command::attack:
		{
			ImGui::Text("Enemy action: attack");
		}
		break;
	}

	switch (enemy_cmd.dir)
	{
		case Left:
			ImGui::Text("Enemy dir: Left");
			break;
		case Right:
			ImGui::Text("Enemy dir: Right");
			break;
		case Down:
			ImGui::Text("Enemy dir: Down");
			break;
		case Up:
			ImGui::Text("Enemy dir: Up");
			break;
	}
	int player_health = 0;
	for (int i = 0; i < board.w_ops_count; ++i)
	{
		player_health += board.white_ops[i].get_cost();
	}
	int enemy_health = 0;
	for (int i = 0; i < board.b_ops_count; ++i)
	{
		enemy_health += board.black_ops[i].get_cost();
	}
	ImGui::Text("Health: %d : %d", player_health, enemy_health);
	// ImGui::ShowDemoWindow();
	ImGui::End();
}

void Application::Resize(int width, int height, int display_w, int display_h)
{
}
