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

template<GenType t>
void dumpMoveList(const std::string& title, Position& pos) {
    if (t == Stockfish::EVASIONS && pos.checkers() == 0)
        return;
    auto res = MoveList<t>(pos);
    std::cout << title << " size: " << res.size() << std::endl;
    for (auto m : res)
    {
        std::cout << m << " ";
    }
    std::cout << std::endl;
}

void dumpPositions(Position& pos) {
    std::cout << pos << std::endl;
    dumpMoveList<Stockfish::CAPTURES>("Captures", pos);
    dumpMoveList<Stockfish::QUIETS>("Quites", pos);
    dumpMoveList<Stockfish::QUIET_CHECKS>("QuiteChecks", pos);
    dumpMoveList<Stockfish::EVASIONS>("Evasions", pos);
    dumpMoveList<Stockfish::NON_EVASIONS>("NonEvasions", pos);
    dumpMoveList<Stockfish::LEGAL>("LEGAL", pos);
}

TEST(StockfishMinimax, MinimaxTest) {
    Position                               pos;
    StateInfo                              st;

    pos.set("rnbqkb1r/4n2p/5pp1/1p6/pPp1PP2/P2P2PP/8/RNBQKBNR w 0 1", &st, false);
    tt.resize(4096);

    StateInfo states[1000];
    for (int i = 0; i < 5; i++)
    {
        auto res = Stockfish::minimax(tt, pos, 3);

        for (auto m : res)
        {
            std::cout << "move: " << m << " score: " << m.value << std::endl;
        }

        pos.do_move(*res.begin(), states[i]);
        dumpPositions(pos);
    }
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
        auto res = Stockfish::minimax(tt, pos, 2);

        auto picked = *res.begin();
        for (auto m : res)
        {
            std::cout << "move: " << m << " score: " << m.value << std::endl;
        }

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
        auto res = Stockfish::iterative_deepening(tt, pos, 7);
        for (auto m : res)
        {
            std::cout << "move: " << m << " score: " << m.value << std::endl;
        }

        std::cout << "move: " << *(res.begin()) << std::endl;

        played.push_back(*(res.begin()));
        pos.do_move(*(res.begin()), sts[i]);
        dumpPositions(pos);
    }

    std::cout << "played: ";
    for (auto m : played)
    {
        std::cout << " " << m;
    }

    std::cout << std::endl;
}
}
