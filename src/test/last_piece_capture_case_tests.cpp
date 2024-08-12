#include "custom_search.h"
#include "evaluate.h"
#include "game_over_check.h"
#include "movegen.h"
#include "stockfish_position.h"
#include "tt.h"
#include "types.h"
#include <gtest/gtest.h>

TEST(LastCaptureRemoveTests, DoesWhiteWinsWhenAllBlackPiecesCaptured) {
    const std::string    fen = "4k3/8/8/8/8/6R1/4K3/8 b - - 0 1";
    Stockfish::StateInfo st;
    Stockfish::Position  pos;
    pos.set(fen, &st, false);
    std::cout << pos << std::endl;

    TranspositionTable tt1;
    tt1.resize(512);
    search s1(&tt1, pos);

    EXPECT_EQ(pos.gameEndDetector.Analyse(pos), Stockfish::GameEndDetector::GameEnd::WhiteWin);
    EXPECT_EQ(VALUE_MATE, -evaluate(pos));
    EXPECT_EQ(MoveList<LEGAL>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::QUIETS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::QUIET_CHECKS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::NON_EVASIONS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::CAPTURES>(pos).size(), 0);
}

TEST(LastCaptureRemoveTests, DoesBlackWinsWhenAllWhitePiecesCaptured) {
    const std::string    fen = "4k3/6r1/8/8/8/8/4K3/8 w - - 0 1";
    Stockfish::StateInfo st;
    Stockfish::Position  pos;
    pos.set(fen, &st, false);
    std::cout << pos << std::endl;

    TranspositionTable tt1;
    tt1.resize(512);
    search s1(&tt1, pos);

    EXPECT_EQ(pos.gameEndDetector.Analyse(pos), Stockfish::GameEndDetector::GameEnd::BlackWin);
    EXPECT_EQ(VALUE_MATE, -evaluate(pos));
    EXPECT_EQ(MoveList<LEGAL>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::QUIETS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::QUIET_CHECKS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::NON_EVASIONS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::CAPTURES>(pos).size(), 0);
}

TEST(LastCaptureRemoveTests,
     DoesDrawWhenAllBlackPiecesCapturedAndBlackCanCaptureLastOpponentPiece) {
    const std::string    fen = "4k3/5R2/8/8/8/8/4K3/8 b - - 0 1";
    Stockfish::StateInfo st;
    Stockfish::Position  pos;
    pos.set(fen, &st, false);
    std::cout << pos << std::endl;

    TranspositionTable tt1;
    tt1.resize(512);
    search s1(&tt1, pos);

    EXPECT_EQ(pos.gameEndDetector.Analyse(pos), Stockfish::GameEndDetector::GameEnd::Draw);
    EXPECT_EQ(0, -evaluate(pos));
    EXPECT_EQ(MoveList<LEGAL>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::QUIETS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::QUIET_CHECKS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::NON_EVASIONS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::CAPTURES>(pos).size(), 0);
}

TEST(LastCaptureRemoveTests,
     DoesDrawWhenAllWhitePiecesCapturedAndWhiteCanCaptureLastOpponentPiece) {
    const std::string    fen = "4k3/8/8/8/8/5r2/4K3/8 w - - 0 1";
    Stockfish::StateInfo st;
    Stockfish::Position  pos;
    pos.set(fen, &st, false);
    std::cout << pos << std::endl;

    TranspositionTable tt1;
    tt1.resize(512);
    search s1(&tt1, pos);

    EXPECT_EQ(pos.gameEndDetector.Analyse(pos), Stockfish::GameEndDetector::GameEnd::Draw);
    EXPECT_EQ(0, -evaluate(pos));
    EXPECT_EQ(MoveList<LEGAL>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::QUIETS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::QUIET_CHECKS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::NON_EVASIONS>(pos).size(), 0);
    EXPECT_EQ(MoveList<Stockfish::CAPTURES>(pos).size(), 0);
}
