#include "movegen.h"
#include "perft.h"

#include "gtest/gtest.h"
#include "shatranj.h"
#include "board.h"
#include "stockfish_position.h"
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <limits>
#include "customtranspositiontable.h"
#include "tt.h"
#include "types.h"

using namespace Stockfish;


TEST(TranspositionTableTests, Test_MyTT) {
    TT tt;
    for (int i = 0; i < 240; i++)
    {
        tt.insert(i, 4, 2, i * 10, Move::none(), Color::WHITE);
        tt.insert(i, 5, 1, i * 20, Move::none(), Color::WHITE);
        tt.insert(i, 6, 0, i * 30, Move::none(), Color::WHITE);
    }
    uint64_t               testkey = 150;
    std::optional<TtEntry> res     = tt.lookup(testkey, WHITE);
    EXPECT_EQ(res->power(), 6);
    EXPECT_EQ(res->value, testkey * 30);
    for (int i = 0; i < 240; i++)
    {
        tt.insert(i, 6, 0, -i * 40, Move::none(), Color::BLACK);
    }
    res = tt.lookup(testkey, WHITE);

    EXPECT_EQ(res->value, testkey * 40);
    tt.dump("check 150", testkey);
    res = tt.lookup(testkey, BLACK);

    EXPECT_EQ(res->value, -testkey * 40);
    tt.dump("check 150", testkey);

    res = tt.lookup(testkey, 7, 0, BLACK);
    EXPECT_EQ(res, std::nullopt);

    res = tt.lookup(testkey, 6, 0, BLACK);
    EXPECT_EQ(res->value, -testkey * 40);

    res = tt.lookup(testkey, 5, 0, BLACK);
    EXPECT_EQ(res->value, -testkey * 40);

    res = tt.lookup(testkey, 4, 0, BLACK);
    EXPECT_EQ(res->value, -testkey * 40);
}

void dump(TTData& todump) {
    std::cout << "------------------------" << std::endl;
    std::cout << "bound : " << todump.bound << std::endl;
    std::cout << "depth : " << todump.depth << std::endl;
    std::cout << "eval : " << todump.eval << std::endl;
    std::cout << "is_pv : " << todump.is_pv << std::endl;
    std::cout << "value : " << todump.value << std::endl;
    std::cout << "------------------------" << std::endl;
}
std::vector<uint64_t> realisticKeys() {
    Position       pos;
    int            j = 0;
    StateInfo      st[500];
    constexpr auto StartFEN = "r1b1kb1r/ppp1qppp/2n2n2/3pp3/3P4/2NBPN2/PPP2PPP/R1BQK2R w 0 1";

    pos.set(StartFEN, &st[j++], false);

    std::vector<uint64_t> keys;
    keys.push_back(pos.key());
    for (int i = 0; i < 300; ++i)
    {
        auto moves = MoveList<Stockfish::LEGAL>(pos);
        auto rmove = (moves.begin() + rand() % moves.size());
        if (*rmove == Move::none())
            break;
        pos.do_move(*rmove, st[j++]);
        keys.push_back(pos.key());
    }
    return keys;
}
TEST(TranspositionTableTests, Test_Stockfish) {
    TranspositionTable tt;
    tt.new_search();
    tt.resize(2048);
    tt.clear();
    auto keys = realisticKeys();
    std::cout << "==========================" << std::endl;
    for (auto i : keys)
    {
        {
            auto [ttHit, ttData, ttWriter] = tt.probe(i);
            ttWriter.write(i, 10, true, Stockfish::BOUND_EXACT, 6, Move::none(), 40,
                           tt.generation());
        }
        {
            auto [ttHit, ttData, ttWriter] = tt.probe(i);
            ttWriter.write(i, 20, true, Stockfish::BOUND_EXACT, 6, Move::none(), 50,
                           tt.generation());
        }
        {
            auto [ttHit, ttData, ttWriter] = tt.probe(i);
            ttWriter.write(i, 30, true, Stockfish::BOUND_EXACT, 6, Move::none(), 60,
                           tt.generation());
        }
        //std::cout << "writing key : " << i << std::endl;
    }
    std::cout << "==========================" << std::endl;
    long tthit  = 0;
    long ttmiss = 0;
    for (auto i : keys)
    {
        auto [ttHit, ttData, ttWriter] = tt.probe(i);
        //EXPECT_EQ(ttHit, true);
        if (ttHit)
        {
            tthit++;
            //std::cout << "key found ---" << i << std::endl;
            EXPECT_EQ(ttData.value, 30);
            //dump(ttData);
        }
        else
        {
            ttmiss++;
            //std::cout << "key not found >>>>>>>>>>>>>>>>>>> " << i << std::endl;
        }

        ttWriter.write(i, 40, true, Stockfish::BOUND_EXACT, 6, Move::none(), 70, tt.generation());
        //std::cout << "writing key : " << i << std::endl;
    }

    std::cout << "tthit : " << tthit << "  %" << tthit * 100 / (tthit + ttmiss) << std::endl;
    std::cout << "ttmiss : " << ttmiss << "  %" << ttmiss * 100 / (tthit + ttmiss) << std::endl;
    std::cout << "==========================" << std::endl;
    EXPECT_GE(tthit * 100 / (tthit + ttmiss), 95);
    tt.new_search();
    tthit  = 0;
    ttmiss = 0;
    for (auto i : keys)
    {
        auto [ttHit, ttData, ttWriter] = tt.probe(i);
        //EXPECT_EQ(ttHit, true);
        if (ttHit)
        {
            tthit++;
            //std::cout << "key found ---" << i << std::endl;
            EXPECT_EQ(ttData.value, 40);
            //dump(ttData);
        }
        else
        {
            ttmiss++;
            //std::cout << "key not found >>>>>>>>>>>>>>>>>>> " << i << std::endl;
        }
    }
    std::cout << "tthit : " << tthit << "  %" << tthit * 100 / (tthit + ttmiss) << std::endl;
    std::cout << "ttmiss : " << ttmiss << "  %" << ttmiss * 100 / (tthit + ttmiss) << std::endl;
    EXPECT_GE(tthit * 100 / (tthit + ttmiss), 95);
}
