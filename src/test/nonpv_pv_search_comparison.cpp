#include "custom_search.h"
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
}
