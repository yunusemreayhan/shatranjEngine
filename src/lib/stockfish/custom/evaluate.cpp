#include "evaluate.h"
#include "game_over_check.h"
#include "pesto_evaluate.h"
#include <limits>

namespace Stockfish {

struct MobilityCalculator {
    int movecount;
    int mobility;

    inline MobilityCalculator& operator()(const Stockfish::Position& pos) {

        if (pos.st->previous != nullptr && pos.st->previous->movesize != 0)
        {
            otherSideMobilityCount = pos.st->previous->movesize;
        }
        else
        {
            const auto [cmbegin, cmcount_] =
              all_possible_counter_moves(pos, *MoveList<LEGAL>(pos).begin());
            otherSideMobilityCount = cmcount_;
        }

        if (pos.side_to_move() == WHITE)
        {
            movecount_w = pos.st->movesize < 0 ? MoveList<LEGAL>(pos).size() : pos.st->movesize;
            movecount_b = otherSideMobilityCount;
            movecount   = movecount_w;
        }
        else
        {
            movecount_b = pos.st->movesize < 0 ? MoveList<LEGAL>(pos).size() : pos.st->movesize;
            movecount_w = otherSideMobilityCount;
            movecount   = movecount_b;
        }
        mobility = (movecount_w - movecount_b) * MobilityW;
        return (*this);
    }

   private:
    int otherSideMobilityCount;
    int movecount_w;
    int movecount_b;
};


int16_t evaluate(Stockfish::Position pos) {
    //MobilityCalculator mobCalculator;
    //auto               mobCalculatorRes = mobCalculator(pos);

    auto gameresult = pos.gameEndDetector.Analyse(pos);
    if (gameresult != GameEndDetector::None)
    {
        if (gameresult == GameEndDetector::Draw)
        {
            return 0;
        }
        else if (pos.side_to_move() == WHITE)
        {
            if (gameresult == GameEndDetector::BlackWin)
                return -VALUE_MATE;
            else
                return VALUE_MATE;
        }
        else
        {
            if (gameresult == GameEndDetector::WhiteWin)
                return -VALUE_MATE;
            else
                return VALUE_MATE;
        }
    }

    int materialScore = eval_PeSTO(pos);

    int ret = materialScore /* + mobCalculatorRes.mobility */;
    assert(ret == ((int16_t) ret));

    return ret;
}
}
