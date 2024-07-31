#include "minimax.h"
#include <algorithm>
#include <cstddef>
#include <limits>
#include <sys/types.h>

#include "../movepick.h"
#include "helper.h"

namespace Stockfish {

struct Stack {
    Move move;
    int  ply;
    std::string movestr;
};

std::map<uint64_t, std::map<int, int>> transposition_table_w;
std::map<uint64_t, std::map<int, int>> transposition_table_b;

int minimax(bool                use_ordered,
            Position&           position,
            Stack*              stack,
            TranspositionTable& tt,
            const int           depth,
            const int           ndepth,
            int                 alpha,
            int                 beta,
            Color               max_color,
            Move                move,
            long long&          visited,
            long long&          prunned) {

    StateInfo st;
    auto&     ss = stack[ndepth];
    ss.move      = move;
    ss.movestr   = MoveToStr(move);

    position.do_move(move, st);
    RollbackerRAII toRollback([&]() { position.undo_move(move); });

    auto& transposition_table =
      position.side_to_move() == WHITE ? transposition_table_w : transposition_table_b;
    auto ttres = transposition_table.find(position.key());

    if (ttres != transposition_table.end())
    {
        //std::cout << "tthit " << ttres->second.begin()->first << std::endl;
        int ret = std::numeric_limits<int>::min();
        for (auto rec : ttres->second)
        {
            if (rec.first <= depth)
                ret = std::max(ret, rec.second);
        }
        if (ret != std::numeric_limits<int>::min())
        {
            // std::cout << "tthit" << ret << std::endl;
            if (position.side_to_move() == max_color)
                return ret;
            else
                return -ret;
        }
    }

    visited++;


    if (position.st->previous != nullptr && position.st->previous->previous != nullptr
        && position.st->previous->previous->key == position.st->key)
    {
        return VALUE_DRAW;
    }

    MoveList<LEGAL> movelist = use_ordered
                               ? MoveList<LEGAL>(position) /* minimax(false, tt, position, 1) */
                               : MoveList<LEGAL>(position);


    if (depth == 0 || movelist.size() == 0)
    {
        auto eval = evaluate(position, max_color);
        transposition_table[position.key()][depth] =
          position.side_to_move() == max_color ? eval : -eval;
        return eval;
    }

    if (position.side_to_move() == max_color)
    {
        auto best = min_val;
        for (auto& move : movelist)
        {
            auto value = minimax(use_ordered, position, stack, tt, depth - 1, ndepth + 1, alpha,
                                 beta, max_color, move, visited, prunned);
            best       = std::max(best, value);
            alpha      = std::max(alpha, best);
            if (beta <= alpha)
            {
                prunned++;
                break;
            }
        }
        transposition_table[position.key()][depth] = best;
        return best;
    }
    else
    {
        auto best = max_val;
        for (auto& move : movelist)
        {
            auto value = minimax(use_ordered, position, stack, tt, depth - 1, ndepth + 1, alpha,
                                 beta, max_color, move, visited, prunned);
            best       = std::min(best, value);
            beta       = std::min(beta, best);
            if (beta <= alpha)
            {
                prunned++;
                break;
            }
        }
        transposition_table[position.key()][depth] = -best;
        return best;
    }
}

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

MoveList<LEGAL>
minimax(bool use_ordered, TranspositionTable& tt, Position& position, const int depth) {
    tt.new_search();
    Stack     stack[1000] = {};
    long long visited     = 0;
    long long prunned     = 0;

    auto ret = MoveList<GenType::LEGAL>(position);

    for (auto& move : ret)
    {
        move.value = minimax(use_ordered, position, stack, tt, depth, 0, min_val, max_val,
                             position.side_to_move(), move, visited, prunned);
    }

    //std::cout << "depth:" << depth << "  total visited node count " << visited << std::endl;
    //std::cout << "depth:" << depth << "  prunned count " << prunned << std::endl;
    partial_insertion_sort(ret.begin(), ret.end(), std::numeric_limits<int>::min());
    return ret;
}

MoveList<LEGAL>
iterative_deepening(bool use_ordered, TranspositionTable& tt, Position& position, const int depth) {
    tt.new_search();
    Stack stack[1000] = {};

    auto rootMoveList = MoveList<GenType::LEGAL>(position);
    for (int d = 1; d <= depth; d = d + 2)
    {
        long long visited = 0;
        long long prunned = 0;
        for (auto& move : rootMoveList)
        {
            move.value = minimax(use_ordered, position, stack, tt, d, 0, min_val, max_val,
                                 position.side_to_move(), move, visited, prunned);
        }
        partial_insertion_sort(rootMoveList.begin(), rootMoveList.end(),
                               std::numeric_limits<int>::min());

        //std::cout << "depth:" << d << "  total visited node count " << visited << std::endl;
        //std::cout << "depth:" << d << "  prunned count " << prunned << std::endl;
    }

    return rootMoveList;
}
}
