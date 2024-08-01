#include "bitboard.h"
#include "helper.h"
#include "minimax.h"
#include "movegen.h"
#include "movepick.h"

#include "perft.h"

#include "gtest/gtest.h"
#include "shatranj.h"
#include "board.h"
#include "stockfish_position.h"
#include <chrono>
#include <iomanip>
#include <limits>
#include <optional>
#include <queue>
#include <sys/types.h>
#include <tuple>

#include "evaluate.h"
#include "types.h"

#include "custom/custommovepicker.h"
#include "custom/customtranspositiontable.h"


using namespace Stockfish;

// Sort moves in descending order up to and including
// a given limit. The order of moves smaller than the limit is left unspecified.
void partial_insertion_sort(ExtMove* begin, ExtMove* end, int limit) {

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

TEST(EvaluationTests, evaluation_test1) {
    constexpr auto StartFEN = "r1b1kb1r/ppp1qppp/2n2n2/3pp3/3P4/2NBPN2/PPP2PPP/R1BQK2R w 0 1";

    Position  pos;
    int       j = 0;
    StateInfo st[100];

    pos.set(StartFEN, &st[j++], false);

    std::cout << pos << std::endl;
    std::cout << "=====================" << std::endl;
    for (int i = 0; i < 3; ++i)
    {
        auto moves = MoveList<LEGAL>(pos);
        for (auto& m : moves)
        {
            StateInfo st;
            pos.do_move(m, st);
            m.value = evaluate(pos, ~pos.side_to_move());
            pos.undo_move(m);
        }
        partial_insertion_sort(moves.begin(), moves.end(), std::numeric_limits<int>::min());
        for (auto& m : moves)
        {
            std::cout << m << " " << m.value << std::endl;
        }
        std::cout << "=====================" << std::endl;
        auto p = *moves.pick();
        std::cout << "played: " << p << " " << p.value << std::endl;
        std::cout << pos << std::endl;
        pos.do_move(p, st[j++]);
    }
}

int QuiescenceSearch(TT*        tt,
                     Position&  pos,
                     Move       move,
                     int        plyRemaining,
                     int        plyFromRoot,
                     Color      maximizingColor,
                     int        alpha,
                     int        beta,
                     long long& tthits,
                     long long& visitednodes,
                     bool       usett      = true,
                     bool       ordermoves = true) {
    assert(plyRemaining < 0);
    StateInfo st;

    if (usett && move != Move::none())
    {

        auto key   = pos.key_after(move);
        auto ttres = tt->lookup(key, plyRemaining, plyFromRoot, maximizingColor);
        if (ttres != std::nullopt)
        {
            tthits++;
            return ttres->value;
        }
    }
    if (move != Move::none())
    {
        pos.do_move(move, st);
    }
    RollbackerRAII undoer([&]() {
        if (move != Move::none())
        {
            pos.undo_move(move);
        }
    });
    visitednodes++;

    auto moves = CustomMovePicker(pos, tt);

    if (moves.size() == 0 || plyFromRoot < -3)
    {
        auto res = evaluate(pos, maximizingColor);
        if (usett)
        {
            // side to move black
            tt->insert(pos.key(), plyRemaining, plyFromRoot, res, move, maximizingColor);
        }
        return res;
    }
    int movecount = 0;

    if (pos.side_to_move() == maximizingColor)
    {
        int  best     = -VALUE_INFINITE;
        Move bestmove = Move::none();
        for (auto& m : moves)
        {
            if (!move.is_ok())
                continue;
            bool evasions = pos.checkers() != 0;
            if (!evasions)
            {
                if (!pos.capture_stage(m) && !pos.gives_check(m))
                    continue;
            }
            if (movecount++ > 2)
                break;
            m.value =
              QuiescenceSearch(tt, pos, m, plyRemaining - 1, plyFromRoot + 1, maximizingColor,
                               alpha, beta, tthits, visitednodes, usett, ordermoves);
            if (m.value > best)
            {
                best     = m.value;
                bestmove = m;
            }
            alpha = std::max(alpha, best);
            if (beta <= alpha)
            {
                break;
            }
        }

        if (usett)
        {
            tt->insert(pos.key(), plyRemaining, plyFromRoot, alpha, bestmove, maximizingColor);
        }
        return alpha;
    }
    else
    {
        int  best     = +VALUE_INFINITE;
        Move bestmove = Move::none();
        for (auto& m : moves)
        {
            if (!move.is_ok())
                continue;
            // only checks captures
            bool evasions = pos.checkers() != 0;
            if (!evasions)
            {
                if (!pos.capture_stage(m) && !pos.gives_check(m))
                    continue;
            }
            if (movecount++ > 2)
                break;
            m.value =
              QuiescenceSearch(tt, pos, m, plyRemaining - 1, plyFromRoot + 1, maximizingColor,
                               alpha, beta, tthits, visitednodes, usett, ordermoves);
            if (best < m.value)
            {
                bestmove = m;
                best     = m.value;
            }
            beta = std::min(beta, best);
            if (beta <= alpha)
            {
                break;
            }
        }

        if (usett)
        {
            tt->insert(pos.key(), plyRemaining, plyFromRoot, beta, bestmove, maximizingColor);
        }
        return beta;
    }
}

int minmax(TT*        tt,
           Position&  pos,
           Move       move,
           int        plyRemaining,
           int        plyFromRoot,
           Color      maximizingColor,
           int        alpha,
           int        beta,
           long long& tthits,
           long long& visitednodes,
           bool       usett      = true,
           bool       ordermoves = true) {
    StateInfo st;

    if (usett)
    {
        auto key   = pos.key_after(move);
        auto ttres = tt->lookup(key, plyRemaining, plyFromRoot, maximizingColor);
        if (ttres != std::nullopt)
        {
            tthits++;
            return ttres->value;
        }
    }
    if (move != Move::none())
    {
        pos.do_move(move, st);
    }
    RollbackerRAII undoer([&]() {
        if (move != Move::none())
        {
            pos.undo_move(move);
        }
    });
    visitednodes++;
    auto moves = CustomMovePicker(pos, tt);

    if (plyRemaining == 0)
    {
        auto res =
          QuiescenceSearch(tt, pos, Move::none(), plyRemaining - 1, plyFromRoot + 1,
                           maximizingColor, alpha, beta, tthits, visitednodes, usett, ordermoves);
        if (usett)
        {
            // side to move black
            tt->insert(pos.key(), plyRemaining, plyFromRoot, res, move, maximizingColor);
        }
        return res;
    }

    if (moves.size() == 0)
    {
        auto res = evaluate(pos, maximizingColor);
        if (usett)
        {
            // side to move black
            tt->insert(pos.key(), plyRemaining, plyFromRoot, res, move, maximizingColor);
        }
        return res;
    }

    int movecount = 0;
    if (pos.side_to_move() == maximizingColor)
    {
        int  best     = -VALUE_INFINITE;
        Move bestmove = Move::none();
        for (auto& m : moves)
        {
            if (movecount++ > 10)
                break;
            m.value = minmax(tt, pos, m, plyRemaining - 1, plyFromRoot + 1, maximizingColor, alpha,
                             beta, tthits, visitednodes, usett, ordermoves);
            if (m.value > best)
            {
                best     = m.value;
                bestmove = m;
            }
            alpha   = std::max(alpha, best);
            if (beta <= alpha)
            {
                break;
            }
        }

        if (usett)
        {
            tt->insert(pos.key(), plyRemaining, plyFromRoot, alpha, bestmove, maximizingColor);
        }
        return alpha;
    }
    else
    {
        int  best     = +VALUE_INFINITE;
        Move bestmove = Move::none();
        for (auto& m : moves)
        {
            if (movecount++ > 10)
                break;
            m.value = minmax(tt, pos, m, plyRemaining - 1, plyFromRoot + 1, maximizingColor, alpha,
                             beta, tthits, visitednodes, usett, ordermoves);
            if (best < m.value)
            {
                bestmove = m;
                best     = m.value;
            }
            beta    = std::min(beta, best);
            if (beta <= alpha)
            {
                break;
            }
        }

        if (usett)
        {
            tt->insert(pos.key(), plyRemaining, plyFromRoot, beta, bestmove, maximizingColor);
        }
        return beta;
    }
}

Move minmax(
  TT* tt, Position& pos, int depth, Color max, bool usett = true, bool ordermoves = true) {
    auto moves = MoveList<LEGAL>(pos);
    if (moves.size() == 0)
        return Move::none();
    long long tthits       = 0;
    long long visitednodes = 0;
    for (auto& m : moves)
    {
        m.value = minmax(tt, pos, m, depth, 0, max, -VALUE_INFINITE, VALUE_INFINITE, tthits,
                         visitednodes, usett, ordermoves);
    }
    partial_insertion_sort(moves.begin(), moves.end(), std::numeric_limits<int>::min());

    return *moves.pickfirst();
}

Move iterative_deepening(
  TT* tt, Position& pos, int d, Color max, bool usett = true, bool ordermoves = true) {
    auto moves = CustomMovePicker(pos, tt);
    if (moves.size() == 0)
        return Move::none();
    long long tthits       = 0;
    long long visitednodes = 0;
    for (int depth = 1; depth <= d; depth++)
    {
        for (auto& m : moves)
        {
            m.value = minmax(tt, pos, m, depth, 0, max, -VALUE_INFINITE, VALUE_INFINITE, tthits,
                             visitednodes, usett, ordermoves);
        }
        partial_insertion_sort(moves.begin(), moves.end(), std::numeric_limits<int>::min());
    }

    return *moves.pickfirst();
}

struct operation_result {
    std::string title;
    long        duration_us;
    Move        picked;
};

template<typename opertype>
operation_result test_operation(std::string title, opertype oper) {
    Move picked = Move::none();
    std::cout << "-----------------------------------------------------" << std::endl;
    long duration = timeit_us([&]() { picked = oper(); });

    if (picked != Move::none())
    {
        std::cout << title << " : " << picked << "  " << duration / 1000 << " ms" << std::endl;
    }
    return {title, duration, picked};
}

TEST(EvaluationTests, evaluation_minmax_experiment) {
    constexpr auto StartFEN = "1r1r4/8/1h6/2p5/2P5/1HS5/R3R3/1s6 b 0 10";

    Position  pos;
    int       j = 0;
    StateInfo st[100];

    pos.set(StartFEN, &st[j++], true);
    std::cout << pos << std::endl;
    const auto d     = 7;
    int        fails = 0;
    int        sames = 0;
    TT         tt_ordered_itr;
    TT         tt_using_ordered_moves;
    TT         tt_using_not_ordered_moves;
    for (int i = 0; i < 6; ++i)
    {
        auto res1    = test_operation("iterative_deepening", [&]() -> Move {
            return iterative_deepening(&tt_ordered_itr, pos, d, pos.side_to_move(), true, true);
        });
        auto res2    = test_operation("tt_using_ordered_moves", [&]() -> Move {
            return minmax(&tt_using_ordered_moves, pos, d, pos.side_to_move(), true, true);
        });
        auto res3    = test_operation("tt_using_not_ordered_moves", [&]() -> Move {
            return minmax(&tt_using_not_ordered_moves, pos, d, pos.side_to_move(), true, false);
        });
        auto res4    = test_operation("no tt", [&]() -> Move {
            return minmax(nullptr, pos, d, pos.side_to_move(), false, false);
        });

        if (res1.picked == res2.picked && res1.picked == res3.picked && res1.picked == res4.picked)
            sames++;
        else
            fails++;
        std::cout << "-----------------------------------------------------" << std::endl;
        if (res1.picked == Move::none())
        {
            std::cout << pos << std::endl;
            std::cout << "game is over" << std::endl;
            std::cout << "===========================================================" << std::endl;
            break;
        }

        auto p = res4.picked;

        std::cout << "-----------------------------------------------------" << std::endl;
        std::cout << "played: " << p << std::endl;
        pos.do_move(p, st[j++]);
        std::cout << pos << std::endl;
    }
    tt_ordered_itr.dump("tt");
    tt_using_ordered_moves.dump("tt_o");
    tt_using_not_ordered_moves.dump("tt_o_itr");

    std::cout << "fails: " << fails << "  same: " << sames << std::endl;
    EXPECT_EQ(fails, 0);
}

TEST(EvaluationTests, simple_ordermoves_vs_minmax) {
    constexpr auto StartFEN = "r1b1kb1r/ppp1qppp/2n2n2/3pp3/3P4/2NBPN2/PPP2PPP/R1BQK2R w 0 1";

    Position  pos;
    int       j = 0;
    StateInfo st[100];

    pos.set(StartFEN, &st[j++], false);

    TT tt;
    std::cout << pos << std::endl;
    std::cout << "=====================" << std::endl;
    for (int i = 0; i < 1; ++i)
    {
        auto moves = MoveList<LEGAL>(pos);
        for (auto& m : moves)
        {
            StateInfo st;
            pos.do_move(m, st);
            m.value = evaluate(pos, ~pos.side_to_move());
            pos.undo_move(m);
        }
        partial_insertion_sort(moves.begin(), moves.end(), std::numeric_limits<int>::min());
        std::cout << "=====================" << std::endl;
        for (auto& m : moves)
        {
            std::cout << m << " " << m.value << std::endl;
        }
        std::cout << "=====================" << std::endl;
        tt.static_ordering_moves(pos, moves);
        for (auto& m : moves)
        {
            std::cout << m << " " << m.value << std::endl;
        }
        std::cout << "=====================" << std::endl;
        for (auto& m : CustomMovePicker(pos, &tt))
        {
            std::cout << m << " " << m.value << std::endl;
        }
        std::cout << "=====================" << std::endl;
        auto p = *moves.pick();
        std::cout << "played: " << p << " " << p.value << std::endl;
        std::cout << pos << std::endl;
        pos.do_move(p, st[j++]);
        std::cout << "=====================" << std::endl;
    }
}
