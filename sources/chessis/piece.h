#pragma once
#include <stdint.h>


namespace chessis
{
#pragma pack(push, 1)

	struct Piece
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
				case Piece::Pawn:
					return 100 * health;
				case Piece::Knight:
					return 200 * health;
				case Piece::Rook:
					return 300 * health;
				default:
					return 0;
			}
		}
	};

#pragma pack(pop)

	inline const Piece& nop()
	{
		static Piece n({Piece::Dead, 0, 0, 0, 0, 0});
		return n;
	}
}
