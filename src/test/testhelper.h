#pragma once

#include "../stockfish_position.h"
#include "game_over_check.h"
#include "gtest/gtest.h"

#include <cstddef>
#include <functional>

using namespace Stockfish;

struct FenWithType {
    std::string fen;
    bool        shatranj;
};

struct testitem {
    FenWithType              fenwithtype;
    std::vector<Move>        expectedmoves;
    size_t                   minsearchdepth;
    bool                     enabled;
    GameEndDetector::GameEnd gameEnd;
};

inline void play(Position& pos, Move& move, StateInfo& st) {
    std::cout << "-----------------------------------------------------" << std::endl;
    std::cout << "board before played: " << pos << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;
    std::cout << "playing: " << move << std::endl;
    pos.do_move(move, st);
    std::cout << "-----------------------------------------------------" << std::endl;
    std::cout << "board after played: " << pos << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;
}


template<typename MoveFindingFunctionType>
inline bool testfen(const testitem& item, MoveFindingFunctionType oper) {
    bool               ret = true;
    TranspositionTable tt;
    tt.resize(2048);
    Position  pos;
    StateInfo st[100];
    int       j = 0;
    pos.set(item.fenwithtype.fen, &st[j++], item.fenwithtype.shatranj);
    std::cout << pos << std::endl;
    size_t i = 0;
    std::cout << "===========================================================" << std::endl;
    std::cout << "game start " << std::endl;
    for (i = 0; i < std::max((size_t) 15, (size_t) item.expectedmoves.size()); ++i)
    {
        Move picked   = Move::none();
        long duration = timeit_us([&]() { picked = oper(tt, pos); });
        if (picked == Move::none())
        {
            std::cout << "===========================================================" << std::endl;
            std::cout << pos << std::endl;
            std::cout << "game is over " << std::endl;
            std::cout << "===========================================================" << std::endl;
            break;
        }
        std::cout << "move " << picked << " found in " << duration << std::endl;
        std::cout << "-----------------------------------------------------" << std::endl;

        play(pos, picked, st[j++]);
        if (i < item.expectedmoves.size())
        {
            EXPECT_EQ(item.expectedmoves[i], picked);
            if (item.expectedmoves[i] != picked)
                ret &= false;
            else
                std::cout << "expected move predicted correctly : " << picked << std::endl;
        }
    }
    std::cout << item.fenwithtype.fen << std::endl;
    EXPECT_EQ(pos.gameEndDetector.Analyse(pos), item.gameEnd);
    pos.gameEndDetector.DumpGameEnd(pos);

    return ret;
}

template<typename movelisttype>
void dumpmovelist(std::string title, movelisttype& movelist) {
    std::cout << "===========================================================" << std::endl;
    std::cout << title << std::endl;
    for (auto& move : movelist)
    {
        if (move.is_ok())
            std::cout << "move : " << move << ", value : " << move.value << std::endl;
    }
    std::cout << "===========================================================" << std::endl;
}
