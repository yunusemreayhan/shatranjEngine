
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

const inline std::map<std::string, std::vector<Move>> fenmovelist = {
  {"3k3r/pp1r3p/8/8/6n1/8/2PPP3/2NKRR2 b 0 1", {Move(SQ_G4, SQ_E3)}},
  {"k6r/8/3B4/6P1/4n3/2P5/1K3R2/8 b 0 1", {Move(SQ_E4, SQ_F2)}}};

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
    for (auto& fenmove : fenmovelist)
    {
        testfen(fenmove.first, "single move win", "iterative deepening", fenmove.second, 6,
                [&](TranspositionTable& tt, Position& pos, size_t d) -> Move {
                    search s;
                    return s.iterative_deepening(&tt, pos, d);
                });
    }
}
