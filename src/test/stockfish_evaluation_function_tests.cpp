#include "helper.h"
#include "minimax.h"
#include "perft.h"

#include "gtest/gtest.h"
#include "shatranj.h"
#include "board.h"
#include "stockfish_position.h"
#include <chrono>
#include <iomanip>
#include <optional>
#include <sys/types.h>

#include "evaluate.h"
#include "types.h"

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

struct TtEntry {
    int8_t depth;
    int8_t ndepth;
    Color  maximizingColor;
    int    value;
};

class TT {
   public:
    void insert(uint64_t key, int8_t depth, int8_t ndepth, int value, Color maximizingColor) {
        tt[key].emplace_back(TtEntry{depth, ndepth, maximizingColor, value});
    }

    std::optional<TtEntry> lookup(uint64_t key, int depth, int ndepth, Color maximizingColor) {
        auto it = tt.find(key);
        if (it == tt.end())
            return std::nullopt;
        int                    maxval = -VALUE_INFINITE;
        std::optional<TtEntry> ret    = std::nullopt;
        for (TtEntry& entry : it->second)
        {
            if (entry.depth >= depth && entry.ndepth <= ndepth)
            {
                if (entry.value > maxval)
                {
                    ret = entry;
                }
            }
        }
        if (
          maximizingColor != ret->maximizingColor
          && ret
               .has_value())  // returning negative for black, since we store white maximization values
            ret->value *= -1;
        return ret;
    }

    std::optional<TtEntry> lookup(uint64_t key, Color maximizingColor) {
        auto it = tt.find(key);
        if (it == tt.end())
            return std::nullopt;
        int                    maxval = -VALUE_INFINITE;
        std::optional<TtEntry> ret    = std::nullopt;
        for (TtEntry& entry : it->second)
        {
            if (entry.value > maxval)
            {
                ret = entry;
            }
        }
        if (
          maximizingColor != ret->maximizingColor
          && ret
               .has_value())  // returning negative for black, since we store white maximization values
            ret->value *= -1;
        return ret;
    }

    template<typename movelisttype>
    void ttordermoves(Position& pos, movelisttype& movelist, Color maximizingColor) {
        for (auto& m : movelist)
        {
            auto key = pos.key_after(m);
            auto res = lookup(key, maximizingColor);
            if (!res)
                m.value = -VALUE_INFINITE;
            else
                m.value = res->value;
        }
        partial_insertion_sort(movelist.begin(), movelist.end(), std::numeric_limits<int>::min());
    }

    void dump(TtEntry& entry, std::string title) {
        std::cout << title << " d:" << int(entry.depth) << " nd:" << int(entry.ndepth)
                  << " c:" << entry.maximizingColor << " v:" << entry.value << std::endl;
    }

    void dump(std::string title) {
        std::cout << "=====================" << std::endl;
        std::cout << title << std::endl;
        TtEntry maxdepthe  = {};
        TtEntry maxndepthe = {};
        long    ecount     = 0;
        for (auto& entries : tt)
        {
            for (auto& entry : entries.second)
            {
                ++ecount;
                if (maxdepthe.depth < entry.depth)
                    maxdepthe = entry;
                if (maxndepthe.ndepth < entry.ndepth)
                    maxndepthe = entry;
            }
        }
        dump(maxdepthe, "maxdepth");
        dump(maxndepthe, "maxndepth");
        std::cout << "entries: " << ecount << std::endl;
        std::cout << "=====================" << std::endl;
    }

   private:
    std::unordered_map<uint64_t, std::list<TtEntry>> tt;
};
int minmax(TT*        tt,
           Position&  pos,
           Move       move,
           int        depth,
           int        ndepth,
           Color      maximizingColor,
           int        alpha,
           int        beta,
           long long& tthits,
           long long& visitednodes,
           bool       debug      = false,
           bool       usett      = true,
           bool       ordermoves = true) {
    if (debug)
    {
        for (int i = 0; i < ndepth; ++i)
            std::cout << "\t";
        std::cout << "minmax depth " << ndepth << " move : " << move;
        if (depth != 0)
            std::cout << std::endl;
    }
    StateInfo st;

    if (usett)
    {
        auto key   = pos.key_after(move);
        auto ttres = tt->lookup(key, depth, ndepth, maximizingColor);
        if (ttres != std::nullopt)
        {
            tthits++;
            if (debug)
            {
                std::cout << " tt result : " << ttres->value << std::endl;
            }
            return ttres->value;
        }
    }
    if (move != Move::none())
    {
        pos.do_move(move, st);
        //std::cout << "play " << move << std::endl;
        //std::cout << pos << std::endl;
    }
    RollbackerRAII undoer([&]() {
        if (move != Move::none())
        {
            pos.undo_move(move);
            if (debug)
            {
                std::cout << "undo " << move << std::endl;
                //std::cout << pos << std::endl;
            }
        }
    });
    visitednodes++;

    if (depth == 0 || MoveList<LEGAL>(pos).size() == 0)
    {
        auto res = evaluate(pos, maximizingColor);
        if (debug)
        {
            std::cout << " eval result : " << res << std::endl;
        }
        if (usett)
        {
            // side to move black
            tt->insert(pos.key(), depth, ndepth, res, maximizingColor);
        }
        return res;
    }

    if (pos.side_to_move() == maximizingColor)
    {
        auto moves = MoveList<LEGAL>(pos);
        if (usett && ordermoves)
        {
            tt->ttordermoves(pos, moves, maximizingColor);
        }
        int best = -VALUE_INFINITE;
        for (auto& m : moves)
        {
            m.value = minmax(tt, pos, m, depth - 1, ndepth + 1, maximizingColor, alpha, beta,
                             tthits, visitednodes, debug, usett, ordermoves);
            best    = std::max(best, m.value);
            alpha   = std::max(alpha, best);
            if (beta <= alpha)
            {
                break;
            }
        }
        partial_insertion_sort(moves.begin(), moves.end(), std::numeric_limits<int>::min());
        if (debug)
        {
            for (int i = 0; i < ndepth; ++i)
                std::cout << "\t";
            for (auto& m : moves)
            {
                std::cout << m << " " << m.value << std::endl;
            }
            std::cout << "eval result : " << alpha << std::endl;
            //pos.dump();
        }

        if (usett)
        {
            tt->insert(pos.key(), depth, ndepth, alpha, maximizingColor);
        }
        return alpha;
    }
    else
    {
        auto moves = MoveList<LEGAL>(pos);
        if (usett && ordermoves)
        {
            tt->ttordermoves(pos, moves, maximizingColor);
        }
        int best = +VALUE_INFINITE;
        for (auto& m : moves)
        {
            m.value = minmax(tt, pos, m, depth - 1, ndepth + 1, maximizingColor, alpha, beta,
                             tthits, visitednodes, debug, usett, ordermoves);
            best    = std::min(best, m.value);
            beta    = std::min(beta, best);
            if (beta <= alpha)
            {
                break;
            }
        }
        partial_insertion_sort(moves.begin(), moves.end(), std::numeric_limits<int>::min());
        if (debug)
        {
            for (int i = 0; i < ndepth; ++i)
                std::cout << "\t";
            for (auto& m : moves)
            {
                std::cout << m << " " << m.value << std::endl;
            }
            std::cout << "eval result : " << beta << std::endl;
        }
        //pos.dump();

        if (usett)
        {
            tt->insert(pos.key(), depth, ndepth, beta, maximizingColor);
        }
        return beta;
    }
}

Move minmax(TT*       tt,
            Position& pos,
            int       depth,
            Color     max,
            bool      debug      = false,
            bool      usett      = true,
            bool      ordermoves = true) {
    auto moves = MoveList<LEGAL>(pos);
    if (moves.size() == 0)
        return Move::none();
    long long tthits       = 0;
    long long visitednodes = 0;
    for (auto& m : moves)
    {
        m.value =
          minmax(tt, pos, m, depth, 0, max, std::numeric_limits<int>::min(),
                 std::numeric_limits<int>::max(), tthits, visitednodes, debug, usett, ordermoves);
    }
    partial_insertion_sort(moves.begin(), moves.end(), std::numeric_limits<int>::min());
    if (debug)
    {
        for (auto& m : moves)
        {
            std::cout << m << " " << m.value << std::endl;
        }
    }
    std::cout << "tthit count : " << tthits << "  %" << 100 * tthits / (tthits + visitednodes)
              << std::endl;
    std::cout << "visited nodes : " << visitednodes << "  %"
              << 100 * visitednodes / (tthits + visitednodes) << std::endl;

    return *moves.pickfirst();
}

Move iterative_deepening(TT*       tt,
                         Position& pos,
                         int       d,
                         Color     max,
                         bool      debug      = false,
                         bool      usett      = true,
                         bool      ordermoves = true) {
    auto moves = MoveList<LEGAL>(pos);
    if (moves.size() == 0)
        return Move::none();
    long long tthits       = 0;
    long long visitednodes = 0;
    for (int depth = 1; depth <= d; depth++)
    {
        for (auto& m : moves)
        {
            m.value = minmax(tt, pos, m, depth, 0, max, std::numeric_limits<int>::min(),
                             std::numeric_limits<int>::max(), tthits, visitednodes, debug, usett,
                             ordermoves);
        }
        partial_insertion_sort(moves.begin(), moves.end(), std::numeric_limits<int>::min());
    }
    if (debug)
    {
        for (auto& m : moves)
        {
            std::cout << m << " " << m.value << std::endl;
        }
    }
    std::cout << "tthit count : " << tthits << "  %" << 100 * tthits / (tthits + visitednodes)
              << std::endl;
    std::cout << "visited nodes : " << visitednodes << "  %"
              << 100 * visitednodes / (tthits + visitednodes) << std::endl;

    return *moves.pickfirst();
}

TEST(EvaluationTests, evaluation_minmax_experiment) {
    constexpr auto StartFEN = "1r1r4/8/1h6/2p5/2P5/1HS5/R3R3/1s6 b 0 10";

    Position  pos;
    int       j = 0;
    StateInfo st[100];

    pos.set(StartFEN, &st[j++], true);
    std::cout << pos << std::endl;
    const auto d     = 5;
    int        fails = 0;
    int        sames = 0;
    TT         tt;
    TT         tt_o;
    TT         tt_o_itr;
    for (int i = 0; i < 6; ++i)
    {
        Move ptto    = Move::none();
        Move ptt     = Move::none();
        Move p       = Move::none();
        Move pttoitr = Move::none();
        std::cout << "-----------------------------------------------------" << std::endl;
        long durationuitr = timeit_us([&]() {
            pttoitr = iterative_deepening(&tt_o_itr, pos, d, pos.side_to_move(), false, true, true);
        });
        std::cout << "-----------------------------------------------------" << std::endl;
        long durationusttorder =
          timeit_us([&]() { ptto = minmax(&tt_o, pos, d, pos.side_to_move(), false, true, true); });
        std::cout << "-----------------------------------------------------" << std::endl;
        long durationustt =
          timeit_us([&]() { ptt = minmax(&tt, pos, d, pos.side_to_move(), false, true, false); });
        std::cout << "-----------------------------------------------------" << std::endl;
        long durationus = timeit_us(
          [&]() { p = minmax(nullptr, pos, d, pos.side_to_move(), false, false, false); });
        std::cout << "-----------------------------------------------------" << std::endl;
        if (ptt == p && p == ptto && p == pttoitr)
            sames++;
        else
            fails++;
        std::cout << "-----------------------------------------------------" << std::endl;
        if (pttoitr == Move::none())
        {
            std::cout << pos << std::endl;
            std::cout << "game is over" << std::endl;
            std::cout << "===========================================================" << std::endl;
            break;
        }
        std::cout << "pttoitr   : " << pttoitr << std::endl;
        std::cout << "ptto: " << ptto << std::endl;
        std::cout << "ptt : " << ptt << std::endl;
        std::cout << "p   : " << p << std::endl;
        std::cout << "-----------------------------------------------------" << std::endl;
        std::cout << "usitr: " << durationuitr / 1000 << "  %"
                  << 100 * durationuitr
                       / (durationus + durationustt + durationusttorder + durationuitr)
                  << std::endl;
        std::cout << "ustto: " << durationusttorder / 1000 << "  %"
                  << 100 * durationusttorder
                       / (durationus + durationustt + durationusttorder + durationuitr)
                  << std::endl;
        std::cout << "ustt: " << durationustt / 1000 << "  %"
                  << 100 * durationustt
                       / (durationus + durationustt + durationusttorder + durationuitr)
                  << std::endl;
        std::cout << "us: " << durationus / 1000 << "  %"
                  << 100 * durationus
                       / (durationus + durationustt + durationusttorder + durationuitr)
                  << std::endl;
        std::cout << "-----------------------------------------------------" << std::endl;
        std::cout << "===========================================================" << std::endl;
        std::cout << "played: " << p << std::endl;
        if (p != Move::none())
            pos.do_move(p, st[j++]);
        else
        {
            std::cout << pos << std::endl;
            std::cout << "game is over" << std::endl;
            std::cout << "===========================================================" << std::endl;
            break;
        }
        std::cout << pos << std::endl;
        std::cout << "===========================================================" << std::endl;
    }
    tt.dump("tt");
    tt_o.dump("tt_o");
    tt_o_itr.dump("tt_o_itr");

    std::cout << "fails: " << fails << "  same: " << sames << std::endl;
    EXPECT_EQ(fails, 0);
}
