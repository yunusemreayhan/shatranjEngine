#include "perft.h"
#include "helper.h"

using namespace Stockfish;

long long perft(Stockfish::Position pos, int depth) {
    if (depth == 0)
        return 0;
    Stockfish::StateInfo st;
    long long            ret = 0;
    for (auto move : MoveList<Stockfish::LEGAL>(pos))
    {
        pos.do_move(move, st);
        ret += perft(pos, depth - 1);
        pos.undo_move(move);
        ret++;
    }
    return ret;
}

std::tuple<long long, long long> perft_time(Stockfish::Position pos, int depth) {
    auto ret    = 0;
    auto resdur = timeit_us([&]() { ret = perft(pos, depth); });
    return std::make_tuple(resdur, ret);
}
