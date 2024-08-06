
#include "custommovepicker.h"
#include "minimax.h"
#include "movegen.h"

#include "gtest/gtest.h"
#include "stockfish_position.h"
#include <cstdint>
#include <limits>
#include <sys/types.h>

#include "evaluate.h"

#include "search_prototypes.h"
#include "testhelper.h"
#include "tt.h"
#include "types.h"
#include "pesto_evaluate.h"
#include "evaluate.h"

using namespace Stockfish;

struct testitem {
    std::string       fen;
    std::vector<Move> expectedmoves;
    size_t            searchdepth;
};

const inline std::vector<testitem> testitemlist = {
  {"3k3r/pp1r3p/8/8/6n1/8/2PPP3/2NKRR2 b - - 0 1", {Move(SQ_G4, SQ_E3)}, 4},
  {"3k3r/pp1r3p/8/8/6n1/8/2PPP3/2NKRR2 b - - 0 1", {Move(SQ_G4, SQ_E3)}, 5},
  {"k6r/8/3B4/6P1/4n3/2P5/1K3R2/8 b - - 0 1", {Move(SQ_E4, SQ_F2)}, 4},
  {"k6r/8/3B4/6P1/4n3/2P5/1K3R2/8 b - - 0 1", {Move(SQ_E4, SQ_F2)}, 5},
  {"4k3/8/8/8/1n6/3N4/8/R3K3 b - - 0 1", {Move(SQ_B4, SQ_C2)}, 4},
  {"4k3/8/8/8/1n6/3N4/8/R3K3 b - - 0 1", {Move(SQ_B4, SQ_C2)}, 5},
  {"4k3/r7/8/8/4n1PK/8/8/8 b - - 0 1", {Move(SQ_A7, SQ_H7)}, 4},
  {"4k3/r7/8/8/4n1PK/8/8/8 b - - 0 1", {Move(SQ_A7, SQ_H7)}, 5},
  {"r3k3/8/1b6/3N4/8/8/8/4K3 w - - 0 1", {Move(SQ_D5, SQ_C7)}, 4},
  {"r3k3/8/1b6/3N4/8/8/8/4K3 w - - 0 1", {Move(SQ_D5, SQ_C7)}, 5},
  {"8/7R/8/kp1N4/6PK/8/8/8 w - - 0 1", {Move(SQ_H7, SQ_A7)}, 4},
  {"8/7R/8/kp1N4/6PK/8/8/8 w - - 0 1", {Move(SQ_H7, SQ_A7)}, 5},
  {"r3k3/8/1n6/3N4/8/8/8/4K3 w - - 0 1", {Move(SQ_D5, SQ_B6)}, 4},
  {"r3k3/8/1n6/3N4/8/8/8/4K3 w - - 0 1", {Move(SQ_D5, SQ_B6)}, 5},
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

TEST(EvaluationTests, single_move_checkmatefinding) {
    size_t successes = 0;
    size_t wrongs    = 0;
    for (auto& testitem : testitemlist)
    {
        auto res = testfen(testitem.fen, "single move win", "iterative deepening",
                           testitem.expectedmoves, testitem.searchdepth,
                           [&](TranspositionTable& tt, Position& pos, size_t d) -> Move {
                               search s(&tt, pos);
                               return s.iterative_deepening(d);
                           });

        if (res)
            successes++;
        else
            wrongs++;
    }
    std::cout << "successes: " << successes << " wrongs: " << wrongs << std::endl;
}
