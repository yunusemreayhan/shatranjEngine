#include "evaluate.h"
#include <limits>

namespace Stockfish {

struct MobilityCalculator {
    int movecount;
    int mobility;

    inline MobilityCalculator& operator()(const Stockfish::Position& pos) {
        auto movelist = MoveList<LEGAL>(pos);

        if (pos.st->previous != nullptr && pos.st->previous->movesize != 0)
            cmcount = pos.st->previous->movesize;
        else
        {
            const auto [cmbegin, cmcount_] = all_possible_counter_moves(pos, *movelist.begin());
            cmcount                        = cmcount_;
        }

        if (pos.side_to_move() == WHITE)
        {
            movecount_w = movelist.size();
            movecount_b = cmcount;
        }
        else
        {
            movecount_b = movelist.size();
            movecount_w = cmcount;
        }
        mobility  = (movecount_w - movecount_b) * MobilityW;
        movecount = pos.side_to_move() == WHITE ? movecount_w : movecount_b;
        return (*this);
    }

   private:
    int cmcount;
    int movecount_w;
    int movecount_b;
};


int evaluate(Stockfish::Position pos, Color maximColor) {
    MobilityCalculator mobCalculator;
    auto               mobCalculatorRes = mobCalculator(pos);
    // checkmate control
    if (mobCalculatorRes.movecount == 0)
    {
        if (pos.side_to_move() == maximColor)
        {
            return -VALUE_INFINITE;
        }
        else
        {
            return VALUE_INFINITE;
        }
    }

    int materialScore = 0;
    materialScore += (pos.count<PAWN>(WHITE) - pos.count<PAWN>(BLACK)) * PawnValue;
    materialScore += (pos.count<KNIGHT>(WHITE) - pos.count<KNIGHT>(BLACK)) * KnightValue;
    materialScore += (pos.count<BISHOP>(WHITE) - pos.count<BISHOP>(BLACK)) * BishopValue;
    materialScore += (pos.count<ROOK>(WHITE) - pos.count<ROOK>(BLACK)) * RookValue;
    materialScore += (pos.count<QUEEN>(WHITE) - pos.count<QUEEN>(BLACK)) * QueenValue;

    int ret = materialScore + mobCalculatorRes.mobility;
    ret *= WHITE == maximColor ? 1 : -1;

    return ret;
}
}
