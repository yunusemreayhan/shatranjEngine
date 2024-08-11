
#include "custommovepicker.h"
#include "minimax.h"
#include "movegen.h"

#include "gtest/gtest.h"
#include "stockfish_position.h"
#include <cstdint>
#include <limits>
#include <sys/types.h>

#include "evaluate.h"

#include "custom_search.h"
#include "testhelper.h"
#include "tt.h"
#include "types.h"
#include "pesto_evaluate.h"
#include "evaluate.h"

using namespace Stockfish;

struct fencik {
    std::string fen;
    bool        shatranj;
};
struct testitem {
    fencik            fen;
    std::vector<Move> expectedmoves;
    size_t            searchdepth;
    bool              enabled;
};

TEST(EvaluationTests, evaluation_test_min_max_black_white) {
    {  // only black shah in the board, all white in place, result must be positive
        Position  pos;
        int       j = 0;
        StateInfo st[100];
        pos.set("4k3/8/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1", &st[j++], false);
        EXPECT_GE(evaluate(pos), 0);
    }
    {  // only white shah in the board, all black in place, result must be negative
        Position  pos;
        int       j = 0;
        StateInfo st[100];
        pos.set("rnbqkbnr/pppppppp/8/8/8/8/8/4K3 w - - 0 1", &st[j++], false);
        EXPECT_GE(0, evaluate(pos));
    }
    {  // only black shah in the board, all white in place, result must be negative
        Position  pos;
        int       j = 0;
        StateInfo st[100];
        pos.set("4k3/8/8/8/8/8/PPPPPPPP/RNBQKBNR b - - 0 1", &st[j++], false);
        EXPECT_GE(0, evaluate(pos));
    }
    {  // only white shah in the board, all black in place, result must be positive
        Position  pos;
        int       j = 0;
        StateInfo st[100];
        pos.set("rnbqkbnr/pppppppp/8/8/8/8/8/4K3 b - - 0 1", &st[j++], false);
        EXPECT_GE(evaluate(pos), 0);
    }
}
TEST(EvaluationTests, evaluation_test1) {
    std::cout << "=========================================================" << std::endl;
    {
        int16_t calc1, calc2;
        {
            Position  pos;
            int       j = 0;
            StateInfo st[100];

            pos.set("k6r/8/3B4/6P1/4n3/2P5/1K3R2/8 w 0 1", &st[j++], false);
            calc1 = evaluate(pos);
        }
        {
            Position  pos;
            int       j = 0;
            StateInfo st[100];

            pos.set("k6r/8/3B4/6P1/4n3/2P5/1K3R2/8 b 0 1", &st[j++], false);
            calc2 = evaluate(pos);
            std::cout << pos << std::endl;
        }
        std::cout << "w:" << calc1 << " b:" << calc2 << std::endl;
        EXPECT_GE(calc1, -calc2);
    }
    std::cout << "=========================================================" << std::endl;
    {
        int16_t calc1, calc2;
        {
            Position  pos;
            int       j = 0;
            StateInfo st[100];

            pos.set("kn5r/8/8/8/8/8/8/KN5R w - - 0 1", &st[j++], false);
            calc1 = evaluate(pos);
        }
        {
            Position  pos;
            int       j = 0;
            StateInfo st[100];

            pos.set("kn5r/8/8/8/8/8/8/KN5R b - - 0 1", &st[j++], false);
            calc2 = evaluate(pos);
            std::cout << pos << std::endl;
        }
        std::cout << "w:" << calc1 << " b:" << calc2 << std::endl;
        EXPECT_EQ(calc1, calc2);
    }
    std::cout << "=========================================================" << std::endl;
    {
        int16_t calc1, calc2;
        {
            Position  pos;
            int       j = 0;
            StateInfo st[100];

            pos.set("8/8/2k5/8/3n4/5K2/8/8 w - - 0 1", &st[j++], false);
            calc1 = evaluate(pos);
        }
        {
            Position  pos;
            int       j = 0;
            StateInfo st[100];

            pos.set("8/8/2k5/8/3n4/5K2/8/8 b - - 0 1", &st[j++], false);
            calc2 = evaluate(pos);
            std::cout << pos << std::endl;
        }
        std::cout << "w:" << calc1 << " b:" << calc2 << std::endl;
        EXPECT_GE(calc2, calc1);
    }
    std::cout << "=========================================================" << std::endl;
}

const inline std::vector<testitem> simple_puzzles_handmade = {
  {{"3k3r/pp1r3p/8/8/6n1/8/2PPP3/2NKRR2 b - - 0 1", false}, {Move(SQ_G4, SQ_E3)}, 4, true},
  {{"3k3r/pp1r3p/8/8/6n1/8/2PPP3/2NKRR2 b - - 0 1", false}, {Move(SQ_G4, SQ_E3)}, 5, true},
  {{"k6r/8/3B4/6P1/4n3/2P5/1K3R2/8 b - - 0 1", false}, {Move(SQ_E4, SQ_F2)}, 4, true},
  {{"k6r/8/3B4/6P1/4n3/2P5/1K3R2/8 b - - 0 1", false}, {Move(SQ_E4, SQ_F2)}, 5, true},
  {{"4k3/8/8/8/1n6/3N4/8/R3K3 b - - 0 1", false}, {Move(SQ_B4, SQ_C2)}, 4, true},
  {{"4k3/8/8/8/1n6/3N4/8/R3K3 b - - 0 1", false}, {Move(SQ_B4, SQ_C2)}, 5, true},
  {{"4k3/r7/8/8/4n1PK/8/8/8 b - - 0 1", false}, {Move(SQ_A7, SQ_H7)}, 4, true},
  {{"4k3/r7/8/8/4n1PK/8/8/8 b - - 0 1", false}, {Move(SQ_A7, SQ_H7)}, 5, true},
  {{"r3k3/8/1b6/3N4/8/8/8/4K3 w - - 0 1", false}, {Move(SQ_D5, SQ_C7)}, 4, true},
  {{"r3k3/8/1b6/3N4/8/8/8/4K3 w - - 0 1", false}, {Move(SQ_D5, SQ_C7)}, 5, true},
  {{"8/7R/8/kp1N4/6PK/8/8/8 w - - 0 1", false}, {Move(SQ_H7, SQ_A7)}, 4, true},
  {{"8/7R/8/kp1N4/6PK/8/8/8 w - - 0 1", false}, {Move(SQ_H7, SQ_A7)}, 5, true},
  {{"r3k3/8/1n6/3N4/8/8/8/4K3 w - - 0 1", false}, {Move(SQ_D5, SQ_B6)}, 4, true},
  {{"r3k3/8/1n6/3N4/8/8/8/4K3 w - - 0 1", false}, {Move(SQ_D5, SQ_B6)}, 5, true},
};

TEST(EvaluationTests, single_move_simple_puzzles) {
    size_t successes = 0;
    size_t wrongs    = 0;
    for (auto& testitem : simple_puzzles_handmade)
    {
        auto res = testfen(testitem.fen.fen, testitem.fen.shatranj, testitem.expectedmoves,
                           [&](TranspositionTable& tt, Position& pos) -> Move {
                               search s(&tt, pos);
                               return s.iterative_deepening(5);
                           });

        if (res)
            successes++;
        else
            wrongs++;
    }
    std::cout << "successes: " << successes << " wrongs: " << wrongs << std::endl;
}

const inline std::vector<testitem> book_puzzles = {
  {{"1r1r4/8/1h6/2p5/2P5/1HS5/R3R3/1s6 b 0 10", true},
   {Move(SQ_B6, SQ_A4), Move(SQ_A2, SQ_A4), Move(SQ_B8, SQ_B3), Move(SQ_C3, SQ_B3),
    Move(SQ_D8, SQ_D3)},
   7,
   true},
  {{"1r4s1/8/5PP1/S1h5/6HR/7F/1r6/7R w 0 10", true},
   {Move(SQ_H4, SQ_H8), Move(SQ_G8, SQ_H8), Move(SQ_H3, SQ_F5), Move(SQ_B2, SQ_H2),
    Move(SQ_H1, SQ_H2), Move(SQ_H8, SQ_G8), Move(SQ_H2, SQ_H8), Move(SQ_G8, SQ_H8),
    Move(SQ_G6, SQ_G7), Move(SQ_H8, SQ_G8), Move(SQ_G4, SQ_H6)},
   11,
   true},
  {{"2bn2Q1/rQ2p1kP/P2p3p/p1pPQ3/1n3P1N/q5NP/Prp2pp1/KBB4R w 0 10", false},
   {Move(SQ_G3, SQ_H5), Move(SQ_G7, SQ_F8), Move(SQ_H4, SQ_G6), Move(SQ_F8, SQ_E8),
    Move(SQ_H5, SQ_G7), Move(SQ_E8, SQ_D7), Move(SQ_G6, SQ_F8), Move(SQ_D7, SQ_C7),
    Move(SQ_G7, SQ_E8), Move(SQ_C7, SQ_B6), Move(SQ_F8, SQ_D7), Move(SQ_B6, SQ_B5),
    Move(SQ_E8, SQ_C7), Move(SQ_B5, SQ_C4), Move(SQ_D7, SQ_B6), Move(SQ_C4, SQ_C3),
    Move(SQ_C7, SQ_B5), Move(SQ_C3, SQ_D2), Move(SQ_B6, SQ_C4), Move(SQ_D2, SQ_E2),
    Move(SQ_B5, SQ_C3), Move(SQ_E2, SQ_F3), Move(SQ_C4, SQ_D2), Move(SQ_F3, SQ_G3),
    Move(SQ_C3, SQ_E2), Move(SQ_G3, SQ_H4), Move(SQ_D2, SQ_F3), Move(SQ_H4, SQ_H5),
    Move(SQ_E2, SQ_G3), Move(SQ_H5, SQ_G6), Move(SQ_F3, SQ_H4), Move(SQ_G6, SQ_G7)},
   32,
   false},
  {{"QRn4k/3P1PR1/1q1pb1PP/5pp1/1NP2bnp/3P3P/Qq1N1qK1/q4B1r b 0 10", false},
   {Move(SQ_G3, SQ_H5), Move(SQ_G7, SQ_F8), Move(SQ_H1, SQ_G1), Move(SQ_G2, SQ_F3),
    Move(SQ_G1, SQ_G3), Move(SQ_F3, SQ_E2), Move(SQ_G3, SQ_E3), Move(SQ_E2, SQ_D1),
    Move(SQ_E3, SQ_E1), Move(SQ_D1, SQ_C2), Move(SQ_E1, SQ_C1), Move(SQ_C2, SQ_B3),
    Move(SQ_C1, SQ_C3), Move(SQ_B3, SQ_A4), Move(SQ_C3, SQ_A3), Move(SQ_A4, SQ_B5),
    Move(SQ_A3, SQ_A5), Move(SQ_B5, SQ_C6), Move(SQ_A5, SQ_C5), Move(SQ_C6, SQ_B7),
    Move(SQ_C5, SQ_C7), Move(SQ_B7, SQ_A6), Move(SQ_C7, SQ_A7), Move(SQ_A6, SQ_B5),
    Move(SQ_A7, SQ_A5), Move(SQ_B5, SQ_C6)},
   32,
   false}};

TEST(EvaluationTests, book_questions) {
    size_t successes = 0;
    size_t wrongs    = 0;
    for (auto& testitem : book_puzzles)
    {
        if (!testitem.enabled)
            continue;
        auto res = testfen(testitem.fen.fen, testitem.fen.shatranj, testitem.expectedmoves,
                           [&](TranspositionTable& tt, Position& pos) -> Move {
                               search s(&tt, pos);
                               return s.iterative_deepening(testitem.expectedmoves.size() + 1);
                           });

        if (res)
            successes++;
        else
            wrongs++;
    }
    std::cout << "successes: " << successes << " wrongs: " << wrongs << std::endl;
}
