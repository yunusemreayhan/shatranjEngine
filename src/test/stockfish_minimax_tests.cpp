#include <algorithm>
#include <cassert>
#include <chrono>
#include <deque>
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
#include "minimax.h"
#include "tt.h"

namespace {

constexpr auto StartFEN         = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w 0 1";
constexpr auto StartFENShatranj = "rhfvsfhr/pppppppp/8/8/8/8/PPPPPPPP/RHFVSFHR w 0 1";

using namespace Stockfish;
TranspositionTable tt;

TEST(StockfishMinimax, MinimaxTest) {
    Position                               pos;
    std::unique_ptr<std::deque<StateInfo>> states =
      std::unique_ptr<std::deque<StateInfo>>(new std::deque<StateInfo>(1));

    pos.set(StartFENShatranj, &states->back(), true);
    tt.resize(4096);

    auto res = Stockfish::minimax(tt, pos, 5);

    for (auto m : res)
    {
        std::cout << "move: " << m.first << " score: " << m.second << std::endl;
    }
}

Move PickBestMove(std::list<std::pair<Move, int>> moves) {
    Move ret  = moves.front().first;
    int  best = std::numeric_limits<int>::min();

    for (auto m : moves)
    {
        if (m.second > best)
        {
            best = m.second;
            ret  = m.first;
        }
    }

    return ret;
}

TEST(StockfishMinimax, DummyGamePlay) {
    Position  pos;
    StateInfo st;

    pos.set(StartFENShatranj, &st, true);

    std::vector<Move> played;
    StateInfo         sts[100];
    for (int i = 0; i < 100; i++)
    {
        std::cout << pos << std::endl;
        auto res = Stockfish::minimax(tt, pos, 5);
        for (auto m : res)
        {
            std::cout << "move: " << m.first << " score: " << m.second << std::endl;
        }

        Move picked = PickBestMove(res);

        std::cout << "move: " << picked << std::endl;

        played.push_back(picked);
        pos.do_move(picked, sts[i]);
        std::cout << pos << std::endl;
    }

    std::cout << "played: ";
    for (auto m : played)
    {
        std::cout << " " << m;
    }

    std::cout << std::endl;
}

TEST(StockfishMinimax, DummyGamePlay2) {
    Position  pos;
    StateInfo st;

    pos.set(StartFENShatranj, &st, true);

    std::vector<Move> played;
    StateInfo         sts[100] = {};
    for (int i = 0; i < 100; i++)
    {
        std::cout << pos << std::endl;
        auto res = Stockfish::minimax(tt, pos, 3);
        for (auto m : res)
        {
            std::cout << "move: " << m.first << " score: " << m.second << std::endl;
        }

        Move picked = PickBestMove(res);

        std::cout << "move: " << picked << std::endl;

        played.push_back(picked);
        pos.do_move(picked, sts[i]);
        std::cout << pos << std::endl;
    }

    std::cout << "played: ";
    for (auto m : played)
    {
        std::cout << " " << m;
    }

    std::cout << std::endl;
}
}
