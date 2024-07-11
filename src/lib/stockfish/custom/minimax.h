#pragma once

#include "helper.h"
#include "evaluate.h"

#include "../stockfish_position.h"
#include "../movegen.h"

#include <limits>
#include <map>
#include <memory>
#include <list>

namespace Stockfish {

constexpr double min_val = std::numeric_limits<double>::min();
constexpr double max_val = std::numeric_limits<double>::max();

std::list<std::pair<Move, double>> minimax(Position& position, const int depth);

}
