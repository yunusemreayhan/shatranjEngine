#pragma once

#include <cctype>
#include <iostream>

#include "types.h"
#include "bitboard.h"
#include "../shatranj_simple/shatranj.h"

namespace Stockfish {

const inline std::string_view square_to_string(Square sq) {
    switch (sq)
    {
    case SQ_A1 :
        return "a1";
    case SQ_B1 :
        return "b1";
    case SQ_C1 :
        return "c1";
    case SQ_D1 :
        return "d1";
    case SQ_E1 :
        return "e1";
    case SQ_F1 :
        return "f1";
    case SQ_G1 :
        return "g1";
    case SQ_H1 :
        return "h1";
    case SQ_A2 :
        return "a2";
    case SQ_B2 :
        return "b2";
    case SQ_C2 :
        return "c2";
    case SQ_D2 :
        return "d2";
    case SQ_E2 :
        return "e2";
    case SQ_F2 :
        return "f2";
    case SQ_G2 :
        return "g2";
    case SQ_H2 :
        return "h2";
    case SQ_A3 :
        return "a3";
    case SQ_B3 :
        return "b3";
    case SQ_C3 :
        return "c3";
    case SQ_D3 :
        return "d3";
    case SQ_E3 :
        return "e3";
    case SQ_F3 :
        return "f3";
    case SQ_G3 :
        return "g3";
    case SQ_H3 :
        return "h3";
    case SQ_A4 :
        return "a4";
    case SQ_B4 :
        return "b4";
    case SQ_C4 :
        return "c4";
    case SQ_D4 :
        return "d4";
    case SQ_E4 :
        return "e4";
    case SQ_F4 :
        return "f4";
    case SQ_G4 :
        return "g4";
    case SQ_H4 :
        return "h4";
    case SQ_A5 :
        return "a5";
    case SQ_B5 :
        return "b5";
    case SQ_C5 :
        return "c5";
    case SQ_D5 :
        return "d5";
    case SQ_E5 :
        return "e5";
    case SQ_F5 :
        return "f5";
    case SQ_G5 :
        return "g5";
    case SQ_H5 :
        return "h5";
    case SQ_A6 :
        return "a6";
    case SQ_B6 :
        return "b6";
    case SQ_C6 :
        return "c6";
    case SQ_D6 :
        return "d6";
    case SQ_E6 :
        return "e6";
    case SQ_F6 :
        return "f6";
    case SQ_G6 :
        return "g6";
    case SQ_H6 :
        return "h6";
    case SQ_A7 :
        return "a7";
    case SQ_B7 :
        return "b7";
    case SQ_C7 :
        return "c7";
    case SQ_D7 :
        return "d7";
    case SQ_E7 :
        return "e7";
    case SQ_F7 :
        return "f7";
    case SQ_G7 :
        return "g7";
    case SQ_H7 :
        return "h7";
    case SQ_A8 :
        return "a8";
    case SQ_B8 :
        return "b8";
    case SQ_C8 :
        return "c8";
    case SQ_D8 :
        return "d8";
    case SQ_E8 :
        return "e8";
    case SQ_F8 :
        return "f8";
    case SQ_G8 :
        return "g8";
    case SQ_H8 :
        return "h8";
    case SQ_NONE :
        return "none";
    default :
        return "unknown";
    }

    return "unknown";
}

inline Square string_to_square(std::string_view str) {
    if (str.size() != 2)
        return SQ_NONE;

    if (str[0] < 'a' || str[0] > 'h' || str[1] < '1' || str[1] > '8')
        return SQ_NONE;

    char lowerfirst = tolower(str[0]);
    return static_cast<Square>(static_cast<int>(SQ_A1) + (lowerfirst - 'a') + (str[1] - '1') * 8);
}

inline Square coord_to_square(uint8_t x, uint8_t y) {
    if (x > 7 || y > 7)
        return SQ_NONE;

    return static_cast<Square>(static_cast<int>(SQ_A1) + x + y * 8);
}

inline Move strToMove(std::string str) {
    if (str.size() != 4)
    {
        std::cerr << "Invalid move: " << str << std::endl;
        return Move::none();
    }

    return Move(string_to_square(str.substr(0, 2)), string_to_square(str.substr(2, 2)));
}

inline Move ShatranjMoveToBitboardMove(shatranj::Movement movement) {
    return Move(coord_to_square(movement.from.Getx(), movement.from.Gety()),
                coord_to_square(movement.to.Getx(), movement.to.Gety()));
}

const inline std::string_view piece_type_to_string(PieceType type) {
    switch (type)
    {
    case PAWN :
        return "pawn";
    case KNIGHT :
        return "knight";
    case BISHOP :
        return "bishop";
    case ROOK :
        return "rook";
    case QUEEN :
        return "queen";
    case KING :
        return "king";
    default :
        return "unknown";
    }
}

inline void dump_bitboard_as_one_zero(const std::string& title, const Bitboard& bb) {
    std::cout << title << std::endl;

    std::cout << bb << std::endl;

    // print column numbers
    std::cout << "    ";
    for (int x = 0; x < 8; x++)
        std::cout << char(x + 'a') << " ";
    std::cout << std::endl;
    for (int y = 7; y >= 0; y--)
    {
        std::cout << " " << y + 1 << "  ";
        for (int x = 0; x < 8; x++)
        {
            if (bb & (1ULL << (y * 8 + x)))
                std::cout << 1 << " ";
            else
                std::cout << 0 << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


inline Bitboard attacks_bb(PieceType pt, Square sq, Color color = WHITE, Bitboard occ = 0) {
    switch (pt)
    {
    case PAWN :
        return PawnAttacks[color][sq];
    case KNIGHT :
    case BISHOP :
    case QUEEN :
    case KING :
        return PseudoAttacks[pt][sq];
    case ROOK :
        return attacks_bb<ROOK>(sq, occ);
    case PIECE_TYPE_NB :
    case NO_PIECE_TYPE :
    default :
        return 0;
    }
}

inline void dump_pseudo_attacks() {

    for (Square s2 = SQ_A1; s2 <= SQ_H8; ++s2)
    {
        for (PieceType pt : {PieceType::PAWN, PieceType::ROOK, PieceType::BISHOP, PieceType::KNIGHT,
                             PieceType::QUEEN, PieceType::KING})
        {
            if (pt != PieceType::PAWN)
                dump_bitboard_as_one_zero("PseudoAttacks " + std::string(piece_type_to_string(pt))
                                            + " " + std::string(square_to_string(s2)) + " "
                                            + std::to_string(attacks_bb(pt, s2)),
                                          attacks_bb(pt, s2));
            else
            {
                dump_bitboard_as_one_zero("PseudoAttacks WHITE "
                                            + std::string(piece_type_to_string(pt)) + " "
                                            + std::string(square_to_string(s2)) + " "
                                            + std::to_string(attacks_bb(pt, s2, WHITE)),
                                          attacks_bb(pt, s2, WHITE));
                dump_bitboard_as_one_zero("PseudoAttacks BLACK "
                                            + std::string(piece_type_to_string(pt)) + " "
                                            + std::string(square_to_string(s2)) + " "
                                            + std::to_string(attacks_bb(pt, s2, BLACK)),
                                          attacks_bb(pt, s2, BLACK));
            }
        }
        std::cout << std::endl;
    }
}

inline std::string MoveToStr(Move& m) {
    std::string ret = std::string(square_to_string(m.from_sq()));
    ret += " -> ";
    ret += square_to_string(m.to_sq());
    return ret;
}
}
