#include <algorithm>
#include <cassert>
#include <chrono>
#include <limits.h>
#include <memory>
#include <optional>
#include <sstream>
#include <utility>

#include "gtest/gtest.h"
#include "bitboard.h"
#include "stockfish_helper.h"
namespace {

using namespace Stockfish;

TEST(Bitboard, AttackTableChecks) {
    // dump_pseudo_attacks();
    EXPECT_EQ(attacks_bb(PieceType::QUEEN, SQ_G2), 10485920);
    EXPECT_EQ(attacks_bb(PieceType::KING, SQ_G2), 14721248);
    EXPECT_EQ(attacks_bb(PieceType::KNIGHT, SQ_G2), 2685403152);
    EXPECT_EQ(attacks_bb(PieceType::BISHOP, SQ_G2), 268435456);
    EXPECT_EQ(attacks_bb(PieceType::ROOK, SQ_G2), 4629771061636939584);
    EXPECT_EQ(attacks_bb(PieceType::PAWN, SQ_G2, WHITE), 10485760);
    EXPECT_EQ(attacks_bb(PieceType::PAWN, SQ_G2, BLACK), 160);
}

void AttackTableCheck(
  PieceType pt, Square s2, Color color = WHITE, Bitboard occupied = 0, Bitboard expected = 0) {
    if (attacks_bb(pt, s2, color, occupied) != expected)
    {
        dump_bitboard_as_one_zero("occupied board : ", occupied);
        dump_bitboard_as_one_zero(std::string(piece_type_to_string(pt)) + " attack table : ",
                                  attacks_bb(pt, SQ_G2, color, occupied));
        EXPECT_EQ(attacks_bb(pt, s2, color, occupied), expected);
    }
}

TEST(Bitboard, RookAttackTableOccupation) {

    // dump_pseudo_attacks();
    AttackTableCheck(PieceType::ROOK, SQ_G2, WHITE, attacks_bb(KING, SQ_A1), 4629771061636939328);
    AttackTableCheck(PieceType::ROOK, SQ_G2, WHITE, attacks_bb(KING, SQ_B2), 4629771061636938816);
    AttackTableCheck(PieceType::ROOK, SQ_G2, WHITE, attacks_bb(KING, SQ_C2), 4629771061636937792);
}

/*
    TODO checks:
    * write a test for adapted position class
    * write a test for is fen application to board
    * write a test for getting fen from board
    * write a test for key generation from board
    * write a test for move generation from board
    * write a test for is check
    * write a test for is checkmate
    * write a test for is stalemate
    * write a test for get possible moves
*/

}  // namespace
