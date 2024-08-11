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
#include "stockfish_helper.h"
#include <atomic>

using namespace Stockfish;

struct Stack {
    int   moveCount;
    int   ply;
    Move  move;
    Value eval;
    Move* pv;
};

enum SearchRunType {
    Root,
    PV,
    NonPV
};

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

    Value value_draw(size_t nodes) { return VALUE_DRAW - 1 + Value(nodes & 0x2); }

    template<SearchRunType nodeType>
    inline Value negmax(Stack* ss, int depth, Value alpha, Value beta, bool cutNode = true) {
        Move pv[MAX_PLY];
        if (nodeType == PV)
        {
            pvrun++;
        }
        else if (nodeType == NonPV)
        {
            nonpvrun++;
        }
        else
        {
            rootrun++;
        }

        assert(alpha < beta);
        Key            posKey   = m_pos.key();
        constexpr bool PvNode   = nodeType != NonPV;
        constexpr bool rootNode = nodeType == Root;
        depth                   = std::max(depth, 0);

        // Check if we have an upcoming move that draws by repetition
        if (!rootNode && alpha < VALUE_DRAW && m_pos.upcoming_repetition(ss->ply))
        {
            alpha = value_draw(this->nodes);
            if (alpha >= beta)
            {
                return alpha;
            }
        }

        auto [ttHit, ttData, ttWriter] = m_tt->probe(posKey);
        if (ttHit && ttData.depth >= depth && !PvNode)
        {
            return ttData.eval;
        }

        auto moves = CustomMovePicker(m_pos, m_tt);

        if (moves.size() == 0)
        {
            Value ret = -VALUE_MATE;
            ttWriter.write(posKey, VALUE_ZERO, false, Stockfish::BOUND_UPPER, depth, Move::none(),
                           ret, m_tt->generation());
            return ret;
        }

        if (depth == 0)
        {
            auto res = qnegmax<nodeType>(ss, alpha, beta);
            /* if (PvNode)  // Update pv even in fail-high case
                update_pv(ss->pv, m, (ss + 1)->pv); */
            ttWriter.write(posKey, VALUE_ZERO, false, Stockfish::BOUND_UPPER, depth, Move::none(),
                           res, m_tt->generation());
            return res;
        }

        // maybe noise but slows down 42s to 45s ~ in stockfish_evaluation_function_tests
        // will keep it probably might be usefull for big boards with more pieces
        if (cutNode && (ss - 1)->move != Move::null() && m_pos.checkers() == 0 && depth > 3)
        {
            StateInfo st;

            m_pos.do_null_move(st, *m_tt);
            this->nodes.fetch_add(1, std::memory_order_relaxed);
            ss->move        = Move::none();
            Value nullValue = -negmax<NonPV>(ss + 1, depth - 3, -beta, -beta + 1, false);
            m_pos.undo_null_move();
            if (nullValue >= beta)
            {
                return nullValue;
            }
        }

        Value value    = -VALUE_INFINITE;
        Value besteval = -VALUE_INFINITE;
        Move  bestmove = Move::none();

        int moveCount = 0;
        for (auto& m : moves)
        {
            Value       recCalc = -VALUE_INFINITE;
            std::string mstr    = MoveToStr(m);
            (void) mstr;
            moveCount++;
            StateInfo st;
            m_pos.do_move(m, st);
            ss->move = m;
            value    = m.value;

            if (!PvNode || moveCount > 1)  // root node is also non pv node
            {
                recCalc = -negmax<NonPV>(ss + 1, depth - 1, -(alpha + 1), -alpha, !cutNode);
            }

            if (PvNode && (moveCount == 1 || recCalc > alpha /* || rootNode */))
            {
                (ss + 1)->pv    = pv;
                (ss + 1)->pv[0] = Move::none();
                recCalc         = -negmax<PV>(ss + 1, depth - 1, -beta, -alpha, false);
            }
            m_pos.undo_move(m);
            ss->move = Move::none();

            if (besteval < recCalc)
            {
                besteval = recCalc;
                if (alpha < recCalc)
                {
                    bestmove = m;

                    if (PvNode && !rootNode)  // Update pv even in fail-high case
                        update_pv(ss->pv, m, (ss + 1)->pv);

                    if (beta <= recCalc)
                    {
                        break;
                    }
                    else
                    {
                        alpha = recCalc;
                    }
                }
            }

            if (rootNode)
            {
                std::vector<Move>& rmpv = pv_manager2.rootPvMoves(m);
                //if (moveCount == 1 || recCalc > alpha)
                {
                    rmpv.resize(1);
                    for (Move* mptr = (ss + 1)->pv; *mptr != Move::none(); ++mptr)
                    {
                        rmpv.push_back(*mptr);
                    }
                    pv_manager2.insert_or_replace({.move = m, .value = recCalc});
                }
            }
        }
        ttWriter.write(posKey, value, false,
                       besteval >= beta     ? BOUND_LOWER
                       : PvNode && bestmove ? BOUND_EXACT
                                            : BOUND_UPPER,
                       depth, bestmove, besteval, m_tt->generation());
        return besteval;
    }

    template<SearchRunType nodeType>
    inline Value qnegmax(Stack* ss, Value alpha, Value beta) {

        constexpr bool PvNode = nodeType == PV;

        qrun++;
        Key posKey                     = m_pos.key();
        auto [ttHit, ttData, ttWriter] = m_tt->probe(posKey);
        if (ttHit && ttData.depth >= DEPTH_QS_CHECKS)
        {
            return ttData.eval;
        }

        // Check if we have an upcoming move that draws by repetition (~1 Elo)
        if (alpha < VALUE_DRAW && m_pos.upcoming_repetition(ss->ply))
        {
            alpha = value_draw(this->nodes);
            if (alpha >= beta)
            {
                return alpha;
            }
        }

        auto moves    = CustomMovePickerForQSearch(m_pos, m_tt);
        auto allmoves = MoveList<Stockfish::LEGAL>(m_pos);

        if (allmoves.size() == 0)
        {
            Value ret = -VALUE_MATE;
            ttWriter.write(m_pos.key(), VALUE_ZERO, false, Stockfish::BOUND_UPPER, DEPTH_UNSEARCHED,
                           Move::none(), ret, m_tt->generation());
            return ret;
        }

        Value bestValue        = evaluate(m_pos);
        Move  bestMove         = Move::none();
        Value value            = bestValue;
        bool  played_something = false;

        Move pv[MAX_PLY];
        // Step 1. Initialize node
        if (PvNode)
        {
            (ss + 1)->pv = pv;
            ss->pv[0]    = Move::none();
        }

        size_t movecount = 0;
        for (auto& m : moves)
        {
            (ss + 1)->pv    = pv;
            (ss + 1)->pv[0] = Move::none();
            movecount++;
            bool givesCheck = m_pos.gives_check(m);

            if (!m_pos.legal(m))
                continue;

            if (!givesCheck && movecount > 2)
                break;

            played_something = true;

            StateInfo st;
            this->nodes.fetch_add(1, std::memory_order_relaxed);
            m_pos.do_move(m, st);
            ss->move = m;
            value    = -qnegmax<nodeType>(ss + 1, -beta, -alpha);
            m_pos.undo_move(m);
            ss->move = Move::none();

            assert(value > -VALUE_INFINITE && value < VALUE_INFINITE);

            if (value > bestValue)
            {
                bestValue = value;

                if (value > alpha)
                {
                    bestMove = m;

                    if (PvNode)  // Update pv even in fail-high case
                        update_pv(ss->pv, m, (ss + 1)->pv);

                    if (value >= beta)
                    {
                        break;  // Fail high
                    }
                    else
                    {
                        // Update alpha here!
                        alpha = value;
                    }
                }
            }
        }

        if (moves.size() == 0 || !played_something)
        {
            auto res = evaluate(m_pos);
            ttWriter.write(m_pos.key(), VALUE_ZERO, false, Stockfish::BOUND_EXACT, DEPTH_UNSEARCHED,
                           Move::none(), res, m_tt->generation());
            return res;
        }
        ttWriter.write(posKey, VALUE_ZERO, false, bestValue >= beta ? BOUND_LOWER : BOUND_UPPER,
                       DEPTH_QS_CHECKS, bestMove, bestValue, m_tt->generation());
        assert(bestValue > -VALUE_INFINITE && bestValue < VALUE_INFINITE);

        return bestValue;
    }

    TTData GetFromTT() {
        auto [ttHit, ttData, ttWriter] = m_tt->probe(m_pos.key());

        return ttData;
    }

   public:
    void dumpmoves(int till) {
        for (int i = 7; i <= till + 7; i++)
        {
            if (stack[i].move != Move::none())
                std::cout << stack[i].ply << ". " << stack[i].move << ", ";
            else
                std::cout << stack[i].ply << ". " << "none, ";
        }
        std::cout << std::endl;
    }

    template<SearchRunType nodeType>
    inline Value negmax(int depth, Value alpha, Value beta, bool cutNode = true) {
        return negmax<nodeType>(ss, depth, alpha, beta, cutNode);
    }

    inline Move iterative_deepening(int d = 20) {
        for (int depth = 1; depth <= d; depth++)
        {
            negmax<Root>(ss, depth, -VALUE_INFINITE, VALUE_INFINITE);

            std::cout << "current depth = " << depth << std::endl;
        }

        pv_manager2.dump();
        auto [ttHit, ttData, ttWriter] = m_tt->probe(m_pos.key());
        if (ttHit && ttData.depth >= d && ttData.move != Move::none())
        {
            return ttData.move;
        }
        return pv_manager2.begin()->move;
    }

    search(TranspositionTable* tt, Position& pos) :
        m_tt(tt),
        m_pos(pos) {
        for (int i = -7; i <= MAX_PLY + 3; i++)
        {
            stack[i + 7].ply = i;
        }
        ss = stack + 7;

        ss->pv = pv;
    }

    void update_pv(Move* pv, Move move, const Move* childPv) {
        for (*pv++ = move; childPv && *childPv != Move::none();)
            *pv++ = *childPv++;
        *pv = Move::none();
    }

   private:
    const static int    MAX_PLY = 500;
    TranspositionTable* m_tt;
    Position&           m_pos;
    PVManager2          pv_manager2;
    Stack               stack[MAX_PLY + 10] = {};
    Stack*              ss;
    Move                pv[MAX_PLY];

    long pvrun    = 0;
    long nonpvrun = 0;
    long rootrun  = 0;
    long qrun     = 0;

    std::atomic<uint64_t> nodes, tbHits, bestMoveChanges;
};
