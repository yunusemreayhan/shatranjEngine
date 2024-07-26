#include "minimax.h"


namespace Stockfish {

struct Stack {
    Move move;
    // std::string movestr;
};

int minimax(Position&                               position,
            Stack*                                  stack,
            TranspositionTable&                     tt,
            std::unique_ptr<std::deque<StateInfo>>& states,
            const int                               depth,
            const int                               ndepth,
            int                                     alpha,
            int                                     beta,
            Color                                   max_color,
            Move                                    move,
            long long&                              visited) {

    auto [ttHit, ttData, ttWriter] = tt.probe(position.key());
    if (ttHit)
    {
        return ttData.eval;
    }
    StateInfo st;
    states->emplace_back();
    auto& ms = stack[ndepth];
    ms.move  = move;
    // ms.movestr = MoveToStr(move);
    position.do_move(move, st);
    visited++;

    RollbackerRAII toRollback([&]() { position.undo_move(move); });

    auto movelist = MoveList<GenType::LEGAL>(position);
    if (depth == 0 || movelist.size() == 0)
    {
        auto eval = evaluate(position, position.side_to_move(), max_color, movelist.size());
        ttWriter.write(position.key(), eval, false, BOUND_EXACT, depth, move, VALUE_NONE,
                       tt.generation());
        return eval;
    }

    if (position.side_to_move() == max_color)
    {
        auto best = min_val;
        for (auto& move : movelist)
        {
            auto value = minimax(position, stack, tt, states, depth - 1, ndepth + 1, alpha, beta,
                                 max_color, move, visited);
            best       = std::max(best, value);
            alpha      = std::max(alpha, best);
            if (beta <= alpha)
            {
                break;
            }
        }
        return best;
    }
    else
    {
        auto best = max_val;
        for (auto& move : MoveList<GenType::LEGAL>(position))
        {
            auto value = minimax(position, stack, tt, states, depth - 1, ndepth + 1, alpha, beta,
                                 max_color, move, visited);
            best       = std::min(best, value);
            beta       = std::min(beta, best);
            if (beta <= alpha)
            {
                break;
            }
        }
        return best;
    }
}

std::list<std::pair<Move, int>> minimax(TranspositionTable&                     tt,
                                        Position&                               position,
                                        std::unique_ptr<std::deque<StateInfo>>& states,
                                        const int                               depth) {
    tt.new_search();
    Stack     stack[1000] = {};
    long long visited     = 0;

    std::list<std::pair<Move, int>> result;
    for (auto& move : MoveList<GenType::LEGAL>(position))
    {
        auto res = minimax(position, stack, tt, states, depth, 0, min_val, max_val,
                           position.side_to_move(), move, visited);

        result.push_back({move, res});
    }

    std::cout << "total visited node count " << visited << std::endl;
    return result;
}
}
