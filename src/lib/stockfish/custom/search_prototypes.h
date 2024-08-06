#pragma once

#include "../types.h"
#include "../tt.h"
#include "../stockfish_position.h"
#include "custommovepicker.h"
#include "evaluate.h"
#include "movegen.h"
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

    Value null_move_prune(int plyRemaining, int plyFromRoot, Value& beta) {
        StateInfo st;
        m_pos.do_null_move(st, *m_tt);
        Value nullValue = -negmax<NonPV>(plyRemaining - 1, plyFromRoot + 1, -beta, -beta + 1);
        m_pos.undo_null_move();

        if (nullValue >= beta)
        {
            // verification for null value result
            Value v = negmax<NonPV>(plyRemaining - 1, plyFromRoot + 1, -beta, -beta + 1);
            if (v >= beta)
            {
                return nullValue;
            }
        }
        return beta - 1;
    }

    void pv_extraction(int plyRemaining, int plyFromRoot, pv_tree& best_moves) {
        if (plyRemaining <= 0)
        {
            return;
        }
        auto [ttHit, ttData, ttWriter] = m_tt->probe(m_pos.key());
        if (ttHit && ttData.depth >= plyRemaining)
        {
            if (ttData.move.is_ok())
            {
                best_moves.set(plyFromRoot, ExtMove{ttData.move, ttData.eval});
                StateInfo st;
                m_pos.do_move(ttData.move, st);
                pv_extraction(plyRemaining - 1, plyFromRoot + 1, best_moves);
                m_pos.undo_move(ttData.move);
            }
        }
    }

    enum SearchRunType {
        Root,
        PV,
        NonPV
    };

    template<SearchRunType nodeType>
    inline Value negmax(int plyRemaining, int plyFromRoot, Value alpha, Value beta) {
        Key            posKey = m_pos.key();
        constexpr bool PvNode = nodeType != NonPV;
        // constexpr bool rootNode = nodeType == Root;

        auto [ttHit, ttData, ttWriter] = m_tt->probe(posKey);
        if (ttHit && ttData.depth >= plyRemaining && !PvNode)
        {
            return ttData.eval;
        }

        auto moves = CustomMovePicker(m_pos, m_tt, pv_manager, plyFromRoot);

        if (moves.size() == 0)
        {
            Value ret = -VALUE_MATE;
            ttWriter.write(posKey, VALUE_ZERO, false, Stockfish::BOUND_UPPER, plyRemaining,
                           Move::none(), ret, m_tt->generation());
            return ret;
        }

        if (plyRemaining == 0)
        {
            auto res = qnegmax(alpha, beta);
            ttWriter.write(posKey, VALUE_ZERO, false, Stockfish::BOUND_UPPER, plyRemaining,
                           Move::none(), res, m_tt->generation());
            return res;
        }

        Value value    = -VALUE_INFINITE;
        Value besteval = -VALUE_INFINITE;
        Move  bestmove = Move::none();

        int moveCount = 0;
        for (auto& m : moves)
        {
            moveCount++;
            StateInfo st;
            m_pos.do_move(m, st);
            value = m.value;

            if (!PvNode || moveCount > 1)  // root node is also non pv node
                m.value = -negmax<NonPV>(plyRemaining - 1, plyFromRoot + 1, -(alpha + 1), -alpha);

            if (PvNode && (moveCount == 1 || m.value > alpha))
                m.value = -negmax<PV>(plyRemaining - 1, plyFromRoot + 1, -beta, -alpha);

            if (besteval < m.value)
            {
                besteval = m.value;
                bestmove = m;
            }
            m_pos.undo_move(m);

            alpha = std::max(alpha, m.value);  // -min(-a, -b) // max(a, b)
            if (beta <= alpha)
            {
                break;
            }
        }
        ttWriter.write(posKey, value, false, Stockfish::BOUND_UPPER, plyRemaining, bestmove,
                       besteval, m_tt->generation());
        return besteval;
    }

    inline Value qnegmax(Value alpha, Value beta) {
        Key posKey                     = m_pos.key();
        auto [ttHit, ttData, ttWriter] = m_tt->probe(posKey);
        if (ttHit && ttData.depth >= DEPTH_QS_CHECKS)
        {
            return ttData.eval;
        }

        auto moves    = CustomMovePickerForQSearch(m_pos, pv_manager, DEPTH_UNSEARCHED);
        auto allmoves = CustomMovePicker(m_pos, m_tt, pv_manager, DEPTH_UNSEARCHED);

        if (allmoves.size() == 0)
        {
            Value ret = -VALUE_MATE;
            ttWriter.write(m_pos.key(), VALUE_ZERO, false, Stockfish::BOUND_UPPER, DEPTH_UNSEARCHED,
                           Move::none(), ret, m_tt->generation());
            return ret;
        }

        if (moves.size() == 0)
        {
            auto res = evaluate(m_pos);
            ttWriter.write(m_pos.key(), VALUE_ZERO, false, Stockfish::BOUND_EXACT, DEPTH_UNSEARCHED,
                           Move::none(), res, m_tt->generation());
            return res;
        }

        Value bestValue = VALUE_ZERO;
        Move  bestMove  = Move::none();
        Value value     = VALUE_ZERO;

        size_t movecount = 0;
        for (auto& m : moves)
        {
            movecount++;
            bool givesCheck = m_pos.gives_check(m);

            if (!m_pos.legal(m))
                continue;

            if (!givesCheck && movecount > 2)
                break;

            StateInfo st;
            m_pos.do_move(m, st);
            value = -qnegmax(-beta, -alpha);
            m_pos.undo_move(m);

            assert(value > -VALUE_INFINITE && value < VALUE_INFINITE);

            // Step 8. Check for a new best move
            if (value > bestValue)
            {
                bestValue = value;

                if (value > alpha)
                {
                    bestMove = m;

                    if (value < beta)  // Update alpha here!
                        alpha = value;
                    else
                        break;  // Fail high
                }
            }
        }
        ttWriter.write(posKey, VALUE_ZERO, false, bestValue >= beta ? BOUND_LOWER : BOUND_UPPER,
                       DEPTH_QS_CHECKS, bestMove, bestValue, m_tt->generation());
        assert(bestValue > -VALUE_INFINITE && bestValue < VALUE_INFINITE);

        return bestValue;
    }

   public:
    inline Move iterative_deepening(int d) {
        Key posKey                     = m_pos.key();
        auto [ttHit, ttData, ttWriter] = m_tt->probe(posKey);
        if (ttHit && ttData.depth >= d)
        {
            return ttData.move;
        }

        for (int depth = 1; depth <= d; depth++)
        {
            negmax<Root>(depth, 0, -VALUE_INFINITE, VALUE_INFINITE);

            pv_tree bestmoves;
            pv_extraction(d, 0, bestmoves);
            pv_manager.insert(bestmoves);
        }

        return pv_manager.begin()->first;
    }

    search(TranspositionTable* tt, Position& pos) :
        m_tt(tt),
        m_pos(pos),
        pv_manager(3) {}

   private:
    TranspositionTable* m_tt;
    Position&           m_pos;
    PVManager           pv_manager;
};
