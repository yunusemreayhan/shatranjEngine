#include "search_prototypes.h"
#include "stockfish_position.h"
#include "tt.h"
#include "types.h"
#include <gtest/gtest.h>

TEST(NonPV_PV_SearchComparison, CompareNonPVWithPV) {
    const std::string    fen = "4k3/8/4n3/2P3R1/3P1B2/6R1/4K3/8 w - - 0 1";
    Stockfish::StateInfo st;
    Stockfish::Position  pos;
    pos.set(fen, &st, false);

    TranspositionTable tt1;
    TranspositionTable tt2;
    tt1.resize(512);
    tt2.resize(512);
    search s1(&tt1, pos);
    search s2(&tt2, pos);
    auto   pvresult1    = s1.negmax<PV>(1, -VALUE_INFINITE, VALUE_INFINITE);
    auto   pvresult2    = s1.negmax<PV>(2, -VALUE_INFINITE, VALUE_INFINITE);
    auto   nonpvresult1 = s2.negmax<NonPV>(1, -VALUE_INFINITE, VALUE_INFINITE);
    auto   nonpvresult2 = s2.negmax<NonPV>(2, -VALUE_INFINITE, VALUE_INFINITE);
    std::cout << "pvresult1 : " << pvresult1 << ", pvresult2 : " << pvresult2 << std::endl;
    std::cout << "nonpvresult1 : " << nonpvresult1 << ", nonpvresult2 : " << nonpvresult2
              << std::endl;

    ASSERT_EQ(pvresult1, nonpvresult1);
    ASSERT_EQ(nonpvresult2, nonpvresult2);
}
