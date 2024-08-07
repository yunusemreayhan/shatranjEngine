#pragma once

#include "../stockfish_position.h"
#include "gtest/gtest.h"

#include <cstddef>
#include <functional>

using namespace Stockfish;


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
inline bool testfen(const std::string&       fen,
                    bool                     shatranj,
                    const std::vector<Move>& expectedmoves,
                    MoveFindingFunctionType  oper) {
    bool               ret = true;
    TranspositionTable tt;
    tt.resize(2048);
    Position  pos;
    StateInfo st[100];
    int       j = 0;
    pos.set(fen, &st[j++], shatranj);
    std::cout << pos << std::endl;
    size_t i = 0;
    std::cout << "===========================================================" << std::endl;
    std::cout << "game start " << std::endl;
    for (i = 0; i < expectedmoves.size(); ++i)
    {
        Move picked   = Move::none();
        long duration = timeit_us([&]() { picked = oper(tt, pos); });
        if (picked == Move::none())
            break;
        std::cout << "move " << picked << " found in " << duration << std::endl;
        std::cout << "-----------------------------------------------------" << std::endl;
        if (picked == Move::none())
        {
            std::cout << "===========================================================" << std::endl;
            std::cout << pos << std::endl;
            std::cout << "game is over " << std::endl;
            std::cout << "===========================================================" << std::endl;
            break;
        }

        play(pos, picked, st[j++]);
        if (i < expectedmoves.size())
        {
            EXPECT_EQ(expectedmoves[i], picked);
            if (expectedmoves[i] != picked)
                ret &= false;
            else
                std::cout << "expected move predicted correctly : " << picked << std::endl;
        }
    }

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
