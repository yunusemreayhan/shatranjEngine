#pragma once

#include "helper.h"
#include "evaluate.h"

#include "../stockfish_position.h"
#include "../movegen.h"
#include "../tt.h"

#include <limits>
#include <map>
#include <memory>
#include <list>
#include <vector>

namespace Stockfish {

constexpr int min_val = std::numeric_limits<int>::min();
constexpr int max_val = std::numeric_limits<int>::max();

std::list<std::pair<Move, int>>
minimax(TranspositionTable& tt, Position& position, const int depth);
}
