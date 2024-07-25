#include "minimax.h"

#include <vector>

namespace Stockfish {
double minimax(Position&                               position,
               std::unique_ptr<std::deque<StateInfo>>& states,
               const int                               depth,
               double                                  alpha,
               double                                  beta,
               Color                                   max_color,
               Move                                    move,
               long long&                              visited) {

    StateInfo st;
    states->emplace_back();
    position.do_move(move, st);
    visited++;

    RollbackerRAII toRollback([&]() { position.undo_move(move); });

    auto movelist = MoveList<GenType::LEGAL>(position);
    if (depth == 0 || movelist.size() == 0)
    {
        return evaluate(position, position.side_to_move(), max_color, movelist.size());
    }

    if (position.side_to_move() == max_color)
    {
        double best = min_val;
        for (auto& move : movelist)
        {
            double value =
              minimax(position, states, depth - 1, alpha, beta, max_color, move, visited);
            best  = std::max(best, value);
            alpha = std::max(alpha, best);
            if (beta <= alpha)
            {
                break;
            }
        }
        return best;
    }
    else
    {
        double best = max_val;
        for (auto& move : MoveList<GenType::LEGAL>(position))
        {
            double value =
              minimax(position, states, depth - 1, alpha, beta, max_color, move, visited);
            best = std::min(best, value);
            beta = std::min(beta, best);
            if (beta <= alpha)
            {
                break;
            }
        }
        return best;
    }
}

std::list<std::pair<Move, double>>
minimax(Position& position, std::unique_ptr<std::deque<StateInfo>>& states, const int depth) {
    long long visited = 0;

    std::list<std::pair<Move, double>> result;
    for (auto& move : MoveList<GenType::LEGAL>(position))
    {
        double res = minimax(position, states, depth, min_val, max_val, position.side_to_move(),
                             move, visited);

        result.push_back({move, res});
    }

    std::cout << "total visited node count " << visited << std::endl;

    return result;
}
}
