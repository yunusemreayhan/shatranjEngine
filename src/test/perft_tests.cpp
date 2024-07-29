#include "perft.h"

#include "gtest/gtest.h"
#include "shatranj.h"
#include "board.h"
#include "stockfish_position.h"
#include <chrono>
#include <iomanip>

using namespace Stockfish;

constexpr auto StartFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w 0 1";

TEST(PerfTests, PerfTest_001) {
    shatranj::Shatranj shatranj;
    Position           pos;
    StateInfo          st;

    pos.set(StartFEN, &st, false);

    for (int d = 0; d < 5; d++)
    {
        {
            const auto& [duration, visited_node_count] = perft_time(pos, d);
            std::cout << std::fixed;
            std::cout << "result1 for depth=" << std::setw(3) << std::setfill(' ') << d << "  -> "
                      << std::setw(12) << std::setfill(' ') << visited_node_count << " in "
                      << std::setw(15) << std::setfill(' ') << duration << "us nodes/sec is "
                      << std::setw(25) << std::setfill(' ')
                      << visited_node_count / (duration / 1000000.0) << std::endl;
        }
        {
            const auto& [duration, visited_node_count] = shatranj.GetBoard()->perft_time(d);
            std::cout << std::fixed;
            std::cout << "result2 for depth=" << std::setw(3) << std::setfill(' ') << d << "  -> "
                      << std::setw(12) << std::setfill(' ') << visited_node_count << " in "
                      << std::setw(15) << std::setfill(' ') << duration << "us nodes/sec is "
                      << std::setw(25) << std::setfill(' ')
                      << visited_node_count / (duration / 1000000.0) << std::endl;
        }
        std::cout << std::endl << std::endl;
    }
}
