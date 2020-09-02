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
			Archer = 0x03,
		};

		Type type : 8;
		int8_t health : 8;
		uint8_t attribute : 6;
		uint8_t x : 4;
		uint8_t y : 4;
		uint8_t o : 2;

		int get_max_health() const
		{
			switch (type)
			{
				case Piece::Pawn:
					return 2;
				case Piece::Knight:
					return 3;
				case Piece::Archer:
					return 1;
				default:
					return 0;
			}
		}
		int get_cost() const
		{
			switch (type)
			{
				case Piece::Pawn:
					return 100 * (health ? get_max_health() + health : 0);
				case Piece::Knight:
					return 200 * (health ? get_max_health() + health : 0);
				case Piece::Archer:
					return 200 * (health ? get_max_health() + health : 0);
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
