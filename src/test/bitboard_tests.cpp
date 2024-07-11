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
#include "stockfish_position.h"
#include "movegen.h"
namespace {

constexpr auto StartFEN         = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w 0 1";
constexpr auto StartFENShatranj = "rhfvsfhr/pppppppp/8/8/8/8/PPPPPPPP/RHFVSFHR w 0 1";

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

TEST(Bitboard, ApplyFenToPosNormal) {
    StateInfo st;
    Position  pos;
    auto&     p = pos.set(StartFEN, &st);
    ASSERT_EQ(p.fen(), StartFEN);
}

TEST(Bitboard, ApplyFenToPosShatranj) {
    StateInfo st;
    Position  pos;
    auto&     p = pos.set(StartFENShatranj, &st, true);
    ASSERT_EQ(p.fen(true), StartFENShatranj);
}

void CheckMoves(const Position& pos, std::vector<Move>& expected) {
    auto movelist = Stockfish::MoveList<LEGAL>(pos);
    EXPECT_EQ(movelist.size(), expected.size());
    for (size_t i = 0; i < movelist.size(); i++)
    {
        Move currentMove((movelist.begin() + i)->value);
        if (std::find(expected.begin(), expected.end(), currentMove) != expected.end())
        {
            std::cout << "move not found in expected list " << *(movelist.begin() + i) << std::endl;
            EXPECT_TRUE(false);
        }
    }

    for (size_t i = 0; i < expected.size(); i++)
    {
        Move& currentMove = expected[i];
        if (std::find(movelist.begin(), movelist.end(), currentMove) == movelist.end())
        {
            std::cout << "move not found in actual list " << currentMove << std::endl;
            EXPECT_TRUE(false);
        }
    }
}

TEST(Bitboard, StandartBoardGetMoves) {
    StateInfo st;
    Position  pos;
    pos.set(StartFENShatranj, &st, true);
    std::cout << pos << std::endl;
    std::vector<Move> expected = {
      Move(SQ_A2, SQ_A3), Move(SQ_B2, SQ_B3), Move(SQ_C2, SQ_C3), Move(SQ_D2, SQ_D3),
      Move(SQ_E2, SQ_E3), Move(SQ_F2, SQ_F3), Move(SQ_G2, SQ_G3), Move(SQ_H2, SQ_H3),
      Move(SQ_B1, SQ_A3), Move(SQ_B1, SQ_C3), Move(SQ_G1, SQ_F3), Move(SQ_G1, SQ_H3),
      Move(SQ_C1, SQ_A3), Move(SQ_C1, SQ_E3), Move(SQ_F1, SQ_D3), Move(SQ_F1, SQ_H3)};
    CheckMoves(pos, expected);
}

TEST(Bitboard, StandartBoardOneSimpleMove) {
    StateInfo st;
    Position  pos;
    pos.set(StartFENShatranj, &st, true);
    std::cout << pos << std::endl;
    std::vector<Move> expected = {
      Move(SQ_A2, SQ_A3), Move(SQ_B2, SQ_B3), Move(SQ_C2, SQ_C3), Move(SQ_D2, SQ_D3),
      Move(SQ_E2, SQ_E3), Move(SQ_F2, SQ_F3), Move(SQ_G2, SQ_G3), Move(SQ_H2, SQ_H3),
      Move(SQ_B1, SQ_A3), Move(SQ_B1, SQ_C3), Move(SQ_G1, SQ_F3), Move(SQ_G1, SQ_H3),
      Move(SQ_C1, SQ_A3), Move(SQ_C1, SQ_E3), Move(SQ_F1, SQ_D3), Move(SQ_F1, SQ_H3)};
    CheckMoves(pos, expected);
    StateInfo st2;
    StateInfo st3;
    pos.do_move(Move(SQ_A2, SQ_A3), st2);
    pos.do_move(Move(SQ_A7, SQ_A6), st3);
    std::cout << pos << std::endl;
    std::vector<Move> expected2 =
      expected = {Move(SQ_A3, SQ_A4), Move(SQ_B2, SQ_B3), Move(SQ_C2, SQ_C3), Move(SQ_D2, SQ_D3),
                  Move(SQ_E2, SQ_E3), Move(SQ_F2, SQ_F3), Move(SQ_G2, SQ_G3), Move(SQ_H2, SQ_H3),
                  Move(SQ_B1, SQ_C3), Move(SQ_G1, SQ_F3), Move(SQ_G1, SQ_H3), Move(SQ_C1, SQ_E3),
                  Move(SQ_F1, SQ_D3), Move(SQ_F1, SQ_H3), Move(SQ_A1, SQ_A2)};
    CheckMoves(pos, expected2);
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
