
#include "gtest/gtest.h"
#include "helper.h"
#include "custom_search.h"
#include "stockfish_position.h"
#include "tt.h"
#include "json_game_exporter.h"
#include "types.h"

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
    StateInfo         sts[600]  = {};
    long              totaltime = 0;
    size_t            fp_depth  = 5;
    size_t            sp_depth  = 3;
    JsonExporter      exporter("DummyPlayTests", "/home/yunus/Desktop", fp_depth, sp_depth);

    for (int i = 0; i < 300; i++)
    {
        std::cout << pos << std::endl;
        search<false> s1(&tt1, pos);
        search<false> s2(&tt2, pos);
        Move   res;
        auto   timelong = timeit_us([&]() {
            res =
              (i % 2 == 0) ? s1.iterative_deepening(fp_depth) : s2.iterative_deepening(sp_depth);
        });
        std::cout << "picking took: " << (double) timelong / 1000000 << "s" << std::endl;
        totaltime += timelong;
        std::cout << "total time: " << (double) totaltime / 1000000 << "s" << std::endl;
        if (res == Move::none())
        {
            auto winner = pos.gameEndDetector.Analyse(pos);
            if (winner == GameEndDetector::WhiteWin)
            {
                std::cout << "White wins" << std::endl;
                exporter.set_winner(WHITE);
            }
            else if (winner == GameEndDetector::BlackWin)
            {
                std::cout << "Black wins" << std::endl;
                exporter.set_winner(BLACK);
            }
            else if (winner == GameEndDetector::Draw)
            {
                std::cout << "Draw" << std::endl;
                exporter.set_winner(Color::COLOR_NB);
            }

            std::cout << "Game over" << std::endl;
            break;
        }
        std::cout << "move: " << res << std::endl;

        played.push_back(res);
        pos.do_move(res, sts[i]);
        exporter.add_move({.fen               = pos.fen(false),
                           .move              = MoveToStr(res),
                           .calculationtime_s = (double) timelong / 1000000});
        exporter.write();
        std::cout << "pos : " << pos << std::endl;
    }

    exporter.write();

    std::cout << "played: ";
    for (auto m : played)
    {
        std::cout << " " << m;
    }

    std::cout << std::endl;
}
