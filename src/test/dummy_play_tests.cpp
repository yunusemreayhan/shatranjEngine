
#include "gtest/gtest.h"
#include "helper.h"
#include "search_prototypes.h"
#include "stockfish_position.h"
#include "tt.h"

using namespace Stockfish;


TEST(DummyPlayTests, StockfishVariantCode) {
    TranspositionTable tt1;
    tt1.resize(2048);
    TranspositionTable tt2;
    tt2.resize(2048);
    constexpr auto StartFENShatranj = "rhfvsfhr/pppppppp/8/8/8/8/PPPPPPPP/RHFVSFHR w 0 1";
    Position       pos;
    StateInfo      st;

    pos.set(StartFENShatranj, &st, true);

    std::vector<Move> played;
    StateInfo         sts[100]  = {};
    long              totaltime = 0;
    for (int i = 0; i < 100; i++)
    {
        std::cout << pos << std::endl;
        search s1(&tt1, pos);
        search s2(&tt2, pos);
        Move   res;
        auto   timelong = timeit_us(
          [&]() { res = (i % 2 == 0) ? s1.iterative_deepening(4) : s2.iterative_deepening(3); });
        std::cout << "picking took: " << (double) timelong / 1000000 << "s" << std::endl;
        totaltime += timelong;
        std::cout << "total time: " << (double) totaltime / 1000000 << "s" << std::endl;
        if (res == Move::none())
        {
            std::cout << "Game over" << std::endl;
            break;
        }
        std::cout << "move: " << res << std::endl;

        played.push_back(res);
        pos.do_move(res, sts[i]);
        std::cout << "pos : " << pos << std::endl;
    }

    std::cout << "played: ";
    for (auto m : played)
    {
        std::cout << " " << m;
    }

    std::cout << std::endl;
}
