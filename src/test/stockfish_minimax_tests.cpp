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
#include "minimax.h"

namespace {

constexpr auto StartFEN         = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w 0 1";
constexpr auto StartFENShatranj = "rhfvsfhr/pppppppp/8/8/8/8/PPPPPPPP/RHFVSFHR w 0 1";

using namespace Stockfish;

TEST(StockfishMinimax, MinimaxTest) {
    StateInfo st;
    Position  pos;
    pos.set(StartFENShatranj, &st, true);

    auto res = Stockfish::minimax(pos, 5);

    for (auto m : res)
    {
        std::cout << "move: " << m.first << " score: " << m.second << std::endl;
    }
}

}
