#include "custom_search.h"
#include "custommovepicker.h"
#include "evaluate.h"
#include "game_over_check.h"

namespace Stockfish {

template<bool HaveTimeout>
template<SearchRunType nodeType>
Value search<HaveTimeout>::negmax(Stack* ss, int depth, Value alpha, Value beta, bool cutNode) {
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
    ss->ttHit                      = ttHit;
    ss->ttMove                     = ttData.move;
    bool ttCapture                 = ttData.move && m_pos.capture_stage(ttData.move);
    if (ttHit && ttData.depth >= depth && !PvNode)
    {
        return ttData.eval;
    }

    auto moves = CustomMovePicker(m_pos, m_tt);

    if (moves.size() == 0)
    {
        Value ret = evaluate(m_pos);
        ttWriter.write(posKey, VALUE_ZERO, false, Stockfish::BOUND_UPPER, depth, Move::none(), ret,
                       m_tt->generation());
        return ret;
    }

    if (depth == 0)
    {
        auto res = qnegmax<nodeType>(ss, alpha, beta);
        /* if (PvNode)  // Update pv even in fail-high case
                update_pv(ss->pv, m, (ss + 1)->pv); */
        ttWriter.write(posKey, VALUE_ZERO, false, Stockfish::BOUND_UPPER, depth, Move::none(), res,
                       m_tt->generation());
        return res;
    }

    // futility pruning
    Value eval             = evaluate(m_pos);
    ss->staticEval         = eval;
    bool improving         = ss->staticEval > (ss - 2)->staticEval;
    bool opponentWorsening = ss->staticEval + (ss - 1)->staticEval > 2;
    ss->improving          = improving;

    Value futilityMargin = 100;
    futilityMargin += depth * (improving && ss->improving ? -10 : 10);
    futilityMargin += cutNode && !ss->ttHit ? -10 : 10;
    futilityMargin += opponentWorsening ? -10 : 10;

    if (m_pos.checkers() == 0)
    {
        if ((ss - 1)->move != Move::null() && depth > 3 && eval - futilityMargin >= beta
            && eval >= beta && (!ttData.move || ttCapture))
        {
            return beta + (eval - beta) / 3;
        }

        // maybe noise but slows down 42s to 45s ~ in stockfish_evaluation_function_tests
        // will keep it probably might be usefull for big boards with more pieces
        if (cutNode && (ss - 1)->move != Move::null() && depth > 3)
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
        uint64_t nodeCount = rootNode ? uint64_t(nodes) : 0;

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

        if (rootNode)
        {
            RootMove& rm = *std::find(rootMoves.begin(), rootMoves.end(), m);

            rm.effort += nodes - nodeCount;

            rm.averageScore =
              rm.averageScore != -VALUE_INFINITE ? (recCalc + rm.averageScore) / 2 : recCalc;

            std::vector<Move>& rmpv = rm.pv;
            if (moveCount == 1 || recCalc > alpha)
            {
                rmpv.resize(1);
                for (Move* mptr = (ss + 1)->pv; *mptr != Move::none(); ++mptr)
                {
                    rmpv.push_back(*mptr);
                }
                pv_manager2.insert_or_replace({.move = m, .value = recCalc});
                rm.score = recCalc;

                if (moveCount > 1)
                    this->bestMoveChanges.fetch_add(1, std::memory_order_relaxed);
            }
            else
            {
                rm.score = -VALUE_INFINITE;
            }
        }

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
    }
    ttWriter.write(posKey, value, false,
                   besteval >= beta     ? BOUND_LOWER
                   : PvNode && bestmove ? BOUND_EXACT
                                        : BOUND_UPPER,
                   depth, bestmove, besteval, m_tt->generation());
    return besteval;
}

template<bool HaveTimeout>
template<SearchRunType nodeType>
Value search<HaveTimeout>::qnegmax(Stack* ss, Value alpha, Value beta) {

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
        Value ret = evaluate(m_pos);
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

template<bool HaveTimeout>
Move search<HaveTimeout>::iterative_deepening_background(int d) {
    if (m_pos.gameEndDetector.Analyse(m_pos) != Stockfish::GameEndDetector::None)
        return Move::none();

    auto moves = MoveList<Stockfish::LEGAL>(m_pos);
    if (moves.size() == 0)
    {
        return Move::none();
    }
    for (auto move : moves)
    {
        rootMoves.emplace_back(move);
    }

    // Allocate stack with extra size to allow access from (ss - 7) to (ss + 2):
    // (ss - 7) is needed for update_continuation_histories(ss - 1) which accesses (ss - 6),
    // (ss + 2) is needed for initialization of cutOffCnt.
    Move   pv[MAX_PLY + 1];
    Stack  stack[MAX_PLY + 10] = {};
    Stack* ss                  = stack + 7;

    for (int i = 7; i > 0; --i)
    {
        (ss - i)->staticEval = VALUE_NONE;
    }

    for (int i = 0; i <= MAX_PLY + 2; ++i)
        (ss + i)->ply = i;

    ss->pv = pv;

    Value alpha = -VALUE_INFINITE, beta = VALUE_INFINITE;
    //int   searchAgainCounter = 0;

    for (rootDepth = 1; rootDepth <= d; rootDepth++)
    {
        if constexpr (HaveTimeout)
        {
            if (stopflag)
                break;
        }
        // MultiPV loop. We perform a full root search for each PV line

        // Save the last iteration's scores before the first PV line is searched and
        // all the move scores except the (new) PV are set to -VALUE_INFINITE.
        for (RootMove& rm : rootMoves)
            rm.previousScore = rm.score;
        Depth adjustedDepth = rootDepth;

        size_t pvFirst = 0;
        (void) pvFirst;
        pvLast          = 0;
        Value bestValue = -VALUE_INFINITE;
        Value avg       = 0;

        // searchAgainCounter++;

        for (pvIdx = 0; pvIdx < multiPV; ++pvIdx)
        {
            if constexpr (HaveTimeout)
            {
                if (stopflag)
                    break;
            }
            if (pvIdx == pvLast)
            {
                pvFirst = pvLast;
                for (pvLast++; pvLast < rootMoves.size(); pvLast++)
                    continue;
            }
            avg   = rootMoves[pvIdx].averageScore;
            delta = 5 + avg * avg / 13424;
            alpha = std::max(avg - delta, -VALUE_INFINITE);
            beta  = std::min(avg + delta, VALUE_INFINITE);

            int failedHighCnt = 0;
            while (true)
            {
                if constexpr (HaveTimeout)
                {
                    if (stopflag)
                        break;
                }
                adjustedDepth =
                  std::max(1, rootDepth - failedHighCnt /* - 3 * (searchAgainCounter + 1) / 4 */);
                rootDelta = beta - alpha;
                std::cout << "-- current depth = " << rootDepth
                          << ", adjusted depth = " << adjustedDepth << ", pvIdx = " << pvIdx
                          << ", bestValue = " << bestValue << ", delta = " << delta
                          << ", alpha = " << alpha << ", beta = " << beta << ", avg = " << avg
                          << ", stopper flag = " << stopflag << std::endl;
                bestValue = negmax<Root>(ss, adjustedDepth, alpha, beta);
                std::stable_sort(rootMoves.begin() + pvIdx, rootMoves.begin() + pvLast);

                if (bestValue <= alpha)
                {
                    beta          = (alpha + beta) / 2;
                    alpha         = std::max(bestValue - delta, -VALUE_INFINITE);
                    failedHighCnt = 0;
                }
                else if (bestValue >= beta)
                {
                    beta = std::min(bestValue + delta, VALUE_INFINITE);
                    ++failedHighCnt;
                } /*
                else if (adjustedDepth != rootDepth)
                {
                    if (failedHighCnt > 0)
                        --failedHighCnt;
                } */
                else
                {
                    break;
                }
                delta += delta / 3;

                assert(alpha >= -VALUE_INFINITE && beta <= VALUE_INFINITE);
            }

            // Sort the PV lines searched so far and update the GUI
            std::stable_sort(rootMoves.begin() + pvFirst, rootMoves.begin() + pvIdx + 1);
        }
        std::cout << "current depth = " << rootDepth << ", adjusted depth = " << adjustedDepth
                  << ", pvIdx = " << pvIdx << ", bestValue = " << bestValue << ", delta = " << delta
                  << ", alpha = " << alpha << ", beta = " << beta << ", avg = " << avg
                  << ", stopper flag = " << stopflag << std::endl;
        if (std::abs(rootMoves[0].score) == VALUE_MATE)
        {
            break;
        }
    }

    //pv_manager2.dump();
    dump_root_moves();
    auto [ttHit, ttData, ttWriter] = m_tt->probe(m_pos.key());
    if (ttHit && ttData.depth >= d && ttData.move != Move::none())
    {
        return ttData.move;
    }
    return rootMoves[0].pv[0];
}
template class search<true>;

template class search<false>;
}
