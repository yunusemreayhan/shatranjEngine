#include <algorithm>
#include <cassert>
#include <chrono>
#include <limits.h>
#include <memory>
#include <optional>
#include <sstream>
#include <utility>

#include "helper.h"
#include "position.h"
#include "shatranc_piece.h"
#include "shatranj.h"
#include "types.h"
#include "gtest/gtest.h"
#include "bitboard.h"

namespace {

using namespace shatranj::stockfish_bitboard;

TEST(Bitboard, AttackTableChecks) {
    BoardWithBitboardRepresentation board;

    // board.dump_pseudo_attacks();
    EXPECT_EQ(board.attacks_bb(PieceType::QUEEN, SQ_G2), 10485920);
    EXPECT_EQ(board.attacks_bb(PieceType::KING, SQ_G2), 14721248);
    EXPECT_EQ(board.attacks_bb(PieceType::KNIGHT, SQ_G2), 2685403152);
    EXPECT_EQ(board.attacks_bb(PieceType::BISHOP, SQ_G2), 268435456);
    EXPECT_EQ(board.attacks_bb(PieceType::ROOK, SQ_G2), 4629771061636939584);
    EXPECT_EQ(board.attacks_bb(PieceType::PAWN, SQ_G2, WHITE), 10485760);
    EXPECT_EQ(board.attacks_bb(PieceType::PAWN, SQ_G2, BLACK), 160);
}

void AttackTableCheck(BoardWithBitboardRepresentation& board,
                      PieceType                        pt,
                      Square                           s2,
                      Color                            color    = WHITE,
                      Bitboard                         occupied = 0,
                      Bitboard                         expected = 0) {
    if (board.attacks_bb(pt, s2, color, occupied) != expected)
    {
        board.dump_bitboard_as_one_zero("occupied board : ", occupied);
        board.dump_bitboard_as_one_zero(std::string(piece_type_to_string(pt)) + " attack table : ",
                                        board.attacks_bb(PieceType::ROOK, SQ_G2, color, occupied));
        EXPECT_EQ(board.attacks_bb(pt, s2, color, occupied), expected);
    }
}

TEST(Bitboard, RookAttackTableOccupation) {
    BoardWithBitboardRepresentation board;

    // board.dump_pseudo_attacks();
    AttackTableCheck(board, PieceType::ROOK, SQ_G2, WHITE, board.attacks_bb(KING, SQ_A1),
                     4629771061636939328);
    AttackTableCheck(board, PieceType::ROOK, SQ_G2, WHITE, board.attacks_bb(KING, SQ_B2),
                     4629771061636938816);
    AttackTableCheck(board, PieceType::ROOK, SQ_G2, WHITE, board.attacks_bb(KING, SQ_C2),
                     4629771061636937792);
}

}  // namespace
