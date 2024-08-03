#pragma once

#include "../types.h"
#include "../tt.h"
#include "../stockfish_position.h"
#include "custommovepicker.h"
#include "evaluate.h"
#include "movegen.h"
#include "testhelper.h"
#include <cstddef>
#include <deque>
#include <limits>
#include <string>
#include "pv_manager.h"

using namespace Stockfish;

class search {
    // Sort moves in descending order up to and including
    // a given limit. The order of moves smaller than the limit is left unspecified.
    inline void partial_insertion_sort(ExtMove* begin, ExtMove* end, int limit) {

        for (ExtMove *sortedEnd = begin, *p = begin + 1; p < end; ++p)
            if (p->value >= limit)
            {
                ExtMove tmp = *p, *q;
                *p          = *++sortedEnd;
                for (q = sortedEnd; q != begin && *(q - 1) < tmp; --q)
                    *q = *(q - 1);
                *q = tmp;
            }
    }

    inline Value negmax(TranspositionTable* tt,
                        Position&           pos,
                        int                 plyRemaining,
                        int                 plyFromRoot,
                        Value               alpha,
                        Value               beta,
                        bool                pvnode = false) {
        auto [ttHit, ttData, ttWriter] = tt->probe(pos.key());
        if (ttHit && ttData.depth >= plyRemaining && !pvnode)
        {
            return ttData.eval;
        }

        auto moves = CustomMovePicker(pos, tt, pv_manager, plyFromRoot);

        if (moves.size() == 0)
        {
            Value ret = pos.side_to_move() == Stockfish::WHITE ? -VALUE_INFINITE : +VALUE_INFINITE;
            ttWriter.write(pos.key(), VALUE_ZERO, false, Stockfish::BOUND_UPPER, plyRemaining,
                           Move::none(), ret, tt->generation());
            return ret;
        }

        if (plyRemaining == 0)
        {
            auto res = evaluate(pos);
            ttWriter.write(pos.key(), VALUE_ZERO, false, Stockfish::BOUND_UPPER, plyRemaining,
                           Move::none(), res, tt->generation());
            return res;
        }

        Value value    = -VALUE_INFINITE;
        Value besteval = -VALUE_INFINITE;

        for (auto& m : moves)
        {
            StateInfo st;
            pos.do_move(m, st);
            value = m.value;
            last_moves.set(plyFromRoot, m);
            m.value = -negmax(tt, pos, plyRemaining - 1, plyFromRoot + 1, -beta, -alpha, false);
            if (besteval < m.value)
            {
                besteval = m.value;
                if (pvnode
                    && best_moves.moves[plyFromRoot - 1] == last_moves.moves[plyFromRoot - 1])
                {
                    best_moves.set(plyFromRoot, m);
                    m.value =
                      -negmax(tt, pos, plyRemaining - 1, plyFromRoot + 1, -beta, -alpha, true);
                }
            }
            pos.undo_move(m);


            alpha = std::max(alpha, m.value);  // -min(-a, -b) // max(a, b)
            if (beta <= alpha)
            {
                break;
            }
        }
        ttWriter.write(pos.key(), value, false, Stockfish::BOUND_UPPER, plyRemaining, Move::none(),
                       besteval, tt->generation());
        return besteval;
    }

   public:
    inline Move iterative_deepening(TranspositionTable* tt, Position& pos, int d) {
        auto moves = CustomMovePicker(pos, tt, pv_manager, 0);

        if (moves.size() == 0)
            return Move::none();

        for (int depth = 1; depth <= d; depth++)
        {
            Value besteval = -VALUE_INFINITE;
            Move  move     = Move::none();

            last_moves.clear();
            best_moves.clear();
            for (auto& m : moves)
            {
                StateInfo st;
                pos.do_move(m, st);
                last_moves.set(0, m);
                m.value = (pos.side_to_move() == WHITE ? -1 : 1)
                        * negmax(tt, pos, depth - 1, 1, -VALUE_INFINITE, VALUE_INFINITE);
                if (m.value > besteval)
                {
                    best_moves.set(0, m);
                    move     = m;
                    besteval = m.value;
                    negmax(tt, pos, depth - 1, 1, -VALUE_INFINITE, VALUE_INFINITE, true);
                }
                pos.undo_move(m);
            }
            pv_manager.insert(best_moves);
        }
        partial_insertion_sort(moves.begin(), moves.end(), std::numeric_limits<int>::min());

        return *moves.pickfirst();
    }

    search() :
        pv_manager(3) {}

   private:
    PVManager pv_manager;
    pv_tree   last_moves;
    pv_tree   best_moves;
};
