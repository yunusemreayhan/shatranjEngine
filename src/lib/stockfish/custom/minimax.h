#pragma once

#include "helper.h"
#include "evaluate.h"

#include "../stockfish_position.h"
#include "../movegen.h"

#include <limits>
#include <map>
#include <memory>
#include <list>
#include <vector>

namespace Stockfish {

constexpr int min_val = std::numeric_limits<int>::min();
constexpr int max_val = std::numeric_limits<int>::max();

std::list<std::pair<Move, int>>
minimax(Position& position, std::unique_ptr<std::deque<StateInfo>>& states, const int depth);
}
